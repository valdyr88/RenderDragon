#include <torch/torch.h>
#include <iostream>
#include <random>
#include <cuda.h>
#include <cuda_runtime_api.h>
#include "sharedmem\shared_memory.h"

#pragma push_macro("min")
#pragma push_macro("max")
#undef min
#undef max

namespace NeuralNet
{
constexpr torch::DeviceType Device = torch::kCPU; //torch::kCUDA; //

struct uint2d
{
	uint x = 0; uint y = 0;
	uint2d() { }
	uint2d(uint X, uint Y){ x = X; y = Y; }
	uint2d operator*(float v) const { return uint2d(v*x, v*y); }
	friend uint2d operator*(float a, uint2d b){ return b*a; }
};
struct float2 { float x; float y; };
struct float3 { float x; float y; float z; };

template<typename type>
type min(type a, type b){ return (a < b)? a : b; }
template<typename type>
type max(type a, type b){ return (a > b)? a : b; }

struct SpatialAttention : torch::nn::Module
{
	uint convInputSize = 0;
	torch::nn::Conv2d conv = nullptr;

	torch::Tensor mask;
	float* rawMask = nullptr;

	//add this to loss if penalize sharp attention maps is needed
	torch::Tensor entropyLoss;

	SpatialAttention() = default;
	SpatialAttention(uint numChannels, bool heatmapDebug = false){
		create(numChannels, heatmapDebug);
	}
	
	void create(uint numChannels, bool heatmapDebug = false)
	{  
		convInputSize = numChannels;
		conv = torch::nn::Conv2d(torch::nn::Conv2dOptions(convInputSize, 1, 1));// 1x1 conv -> attention score
		conv->to(Device);
		register_module("conv", conv);
		outputDebugHeatmap = heatmapDebug;

		initializeWeights();//
	}

	void initializeWeights()
	{
		for(auto& param : named_parameters()){
			if(param.key().find("weight") != std::string::npos && param.value().dim() >= 2)
				torch::nn::init::kaiming_normal_(param.value());
			if(param.key().find("bias") != std::string::npos)
				torch::nn::init::constant_(param.value(), 0);
		}
	}

	void printModel()
	{
		for(auto& param : named_parameters())
			std::cout << param.key() << "\n";
	}

	void setMapMask(float* data, int w, int h)
	{
		/*torch::Tensor mask512 = torch::from_blob(data, {1, w, h, 1}, torch::TensorOptions(torch::kFloat32));
		mask512 = mask512.permute({ 0, 3, 1, 2 }); //.resize_({1, 1, (uint)(((float)w)/((float)h))*convInputSize, convInputSize }).to(Device);
		mask = torch::resize(mask512, {1, 1, (uint)(((float)w)/((float)h))*convInputSize, convInputSize }).to(Device);*/

		uint2d maskWH = { (uint)(((float)w)/((float)h))*convInputSize, convInputSize };
		uint2d scaleDown = uint2d(w/maskWH.x, h/maskWH.y);

		if(rawMask != nullptr) delete[] rawMask;
		rawMask = new float[maskWH.x*maskWH.y];
		
		for(uint y = 0; y < maskWH.y; ++y)
			for(uint x = 0; x < maskWH.x; ++x)
			{
				float sum = 0.0f;
				for(uint j = 0; j < scaleDown.y; ++j)
					for(uint i = 0; i < scaleDown.x; ++i)
					{
						uint a = x*scaleDown.x + i;
						uint b = y*scaleDown.y + j;
						sum += data[b*w + a];
					}
				sum /= scaleDown.x*scaleDown.y;

				rawMask[y*maskWH.x + x] = sum;
			}

		mask = torch::from_blob(rawMask, { 1, maskWH.x, maskWH.y, 1 }, torch::TensorOptions(torch::kFloat32)).clone();
		mask = mask.permute({ 0, 3, 1, 2 }).to(Device).log();
	}

	static torch::Tensor softmax(torch::Tensor x)
	{
		x = torch::exp(x);
		return x / (torch::abs(torch::sum(x)) + 1e-5f);
	}

	torch::Tensor forward(torch::Tensor x)
	{
		static volatile float temperature = 1.5f;
		static volatile bool heatmapBeforeSoftmax = true;
		static volatile bool debugPrintAttentionCenter = true;

		torch::Tensor scores = conv->forward(x) + mask; // [B, 1, H, W]
		int64 B = scores.size(0), H = scores.size(2), W = scores.size(3);
		outHeatmapSize.x = W; outHeatmapSize.y = H;

		if(outputDebugHeatmap && heatmapBeforeSoftmax) outHeatmap = scores.detach();
		
		////torch::Tensor softmax = SpatialAttention::softmax(scores / temperature);
		// 2D softmax over height and width
		torch::Tensor softmax = torch::softmax(scores / temperature, 2);
		softmax = torch::softmax(softmax, 3); // [B, 1, H, W]
		
		if(outputDebugHeatmap && heatmapBeforeSoftmax == false) outHeatmap = softmax.detach();//.permute({ 0,2,3,1 });

		// Optional: print mean attention center (for debug)
		if(debugPrintAttentionCenter)
		{
			torch::Tensor y = torch::arange(H, x.options());
			torch::Tensor x_ = torch::arange(W, x.options());
			auto grid_y = y.view({ 1, 1, H, 1 }).expand({ B, 1, H, W });
			auto grid_x = x_.view({ 1, 1, 1, W }).expand({ B, 1, H, W });
			auto norm = softmax.sum({ 2, 3 }).unsqueeze(-1).unsqueeze(-1) + 1e-5;
			auto mean_x = (softmax * grid_x).sum({ 2, 3 }) / norm.squeeze();
			auto mean_y = (softmax * grid_y).sum({ 2, 3 }) / norm.squeeze();
			std::cout << "Mean attention center (x,y): "
				<< mean_x.mean().item<float>() << ", "
				<< mean_y.mean().item<float>() << "\n";
		}

		torch::Tensor flat = softmax.view({ softmax.size(0), -1 }); // [B, H*W]
		torch::Tensor entropy = -(flat * (flat + 1e-5).log()).sum(1); // [B]
		entropyLoss = entropy.mean(); // scalar

		// Attention-weighted sum + optional residual connection
		return (x * softmax + x).sum({ 2, 3 }); // [B, C]
	}

	const float* getDebugHeatmapData(uint* width, uint* height, bool normalize = true) const
	{
		/**width = mask.size(2);
		*height = mask.size(3);
		return rawMask;

		torch::Tensor filter = mask.detach().squeeze();
		if(normalize)
			filter = (filter - filter.min()) / (filter.max() - filter.min() + 1e-5);
		filter = filter.to(torch::kCPU);

		*width = mask.size(2);
		*height = mask.size(3);

		return filter.const_data_ptr<float>();	*/

		if(outputDebugHeatmap == false) return nullptr;
		torch::Tensor heatmap = outHeatmap.squeeze();  // [H, W]
		if(normalize)
			heatmap = (heatmap - heatmap.min()) / (heatmap.max() - heatmap.min() + 1e-5);

		cudaDeviceSynchronize();
		//torch::Device device(Device);//
		heatmap = heatmap.contiguous().to(torch::kCPU).detach();

		*width = outHeatmapSize.x;
		*height = outHeatmapSize.y;

		return heatmap.contiguous().const_data_ptr<float>();
	}

	~SpatialAttention(){ delete[] rawMask; rawMask = nullptr; }

private:
	torch::Tensor outHeatmap;
	bool outputDebugHeatmap = false;
	uint2d outHeatmapSize;
};

// Policy network that outputs a 2D control vector (mean of Gaussian)
struct NeuralNetwork : torch::nn::Module
{
	torch::nn::Sequential cnn = nullptr;
	std::shared_ptr<SpatialAttention> attention;
	torch::nn::Linear fc = nullptr;
	torch::nn::Linear valueHead = nullptr;
	float* rawState = nullptr;
	uint8* rawStateDebugRGBA = nullptr;
	float* mapMask = nullptr;
	torch::Tensor inputState;

	NeuralNetwork(){}
	NeuralNetwork(uint inSize, uint hiddenSizeScale, uint outSize, bool useAttention = false){
		create(inSize, hiddenSizeScale, outSize, useAttention);
	}
	void create(uint inSize, uint hiddenSizeScale, uint outSize, bool useAttention = false)
	{
		const uint32 hiddenSize = hiddenSizeScale * std::pow(2, (std::ceil(std::log2(inSize))));

		if(useAttention)
			cnn = torch::nn::Sequential(
				torch::nn::Conv2d(torch::nn::Conv2dOptions(inSize, hiddenSize, 3).padding(1)),
				torch::nn::BatchNorm2d(hiddenSize),
				torch::nn::ReLU(),

				torch::nn::Conv2d(torch::nn::Conv2dOptions(hiddenSize, hiddenSize, 3).padding(1)),
				torch::nn::BatchNorm2d(hiddenSize),
				torch::nn::ReLU(),

				torch::nn::MaxPool2d(2),
				torch::nn::Conv2d(torch::nn::Conv2dOptions(hiddenSize, hiddenSizeScale*hiddenSize, 3).padding(1)),
				torch::nn::BatchNorm2d(hiddenSizeScale*hiddenSize),
				torch::nn::ReLU(),
				torch::nn::MaxPool2d(2)
			);
		else
			cnn = torch::nn::Sequential(
				torch::nn::Conv2d(torch::nn::Conv2dOptions(inSize, hiddenSize, 3).padding(1)),
				torch::nn::BatchNorm2d(hiddenSize),
				torch::nn::ReLU(),
				torch::nn::Conv2d(torch::nn::Conv2dOptions(hiddenSize, hiddenSize, 3).padding(1)),
				torch::nn::BatchNorm2d(hiddenSize),
				torch::nn::ReLU(),
				torch::nn::MaxPool2d(2),
				torch::nn::Conv2d(torch::nn::Conv2dOptions(hiddenSize, hiddenSizeScale*hiddenSize, 3).padding(1)),
				torch::nn::BatchNorm2d(hiddenSizeScale*hiddenSize),
				torch::nn::ReLU(),
				torch::nn::MaxPool2d(2)
			);

		if(useAttention){
			attention = std::make_shared<SpatialAttention>(hiddenSizeScale*hiddenSize, true);
			attention->to(Device); //torch::kCUDA
			fc = torch::nn::Linear(hiddenSizeScale*hiddenSize, outSize);
		}
		else
			fc = torch::nn::Linear(std::pow(hiddenSizeScale*hiddenSize, 3), outSize);

		cnn->to(Device);
		fc->to(Device);

		if(attention != nullptr) register_module("attention", attention);
		register_module("cnn", cnn);
		register_module("fc", fc);

		initializeWeights();
	}

	void initializeWeights()
	{
		for(auto& param : named_parameters()){
			if(param.key().find("weight") != std::string::npos && param.value().dim() >= 2)
				torch::nn::init::kaiming_normal_(param.value());
			if(param.key().find("bias") != std::string::npos)
				torch::nn::init::constant_(param.value(), 0);
		}
	}

	void printModel()
	{
		for(auto& param : named_parameters())
			std::cout << param.key() << "\n";
	}

	torch::Tensor forward(torch::Tensor x)
	{
		x = cnn->forward(x);
		if(attention != nullptr)
			x = attention->forward(x);
		else
			//x = x.view({x.size(0), -1});
			x = x.reshape({x.size(0), -1});
		x = fc->forward(x);
		return x;
	}

	SpatialAttention* getSpatialAttention() const { return attention.get(); }

	uint8* getRawStateDebugRGBA(uint* width, uint* height) const
	{
		*width = *height = 512;
		return rawStateDebugRGBA;
	}

	static int sampleFromProbs(torch::Tensor probs)
	{
		//Get data back to cpu (if is on gpu)
		torch::Tensor probsCpu = probs.squeeze().clone().to(torch::kCPU);
		const float* data = probsCpu.const_data_ptr<float>();
		const int N = probsCpu.size(0);

		//Compute cumulative distribution
		std::vector<float> cumulative(N);
		cumulative[0] = data[0];
		float2 minmax = {cumulative[0], cumulative[0]};
		for(int i = 1; i < N; ++i){
			cumulative[i] = cumulative[i-1] + data[i];
			minmax.x = min<float>(minmax.x, cumulative[i]);
			minmax.y = max<float>(minmax.y, cumulative[i]);
		}
		for(int i = 0; i < N; ++i)
			cumulative[i] = (cumulative[i] - minmax.x) / (minmax.y - minmax.x);

		//Random number between 0.0f, and 1.0f
		static std::random_device rd;
		static std::mt19937 gen(rd());
		std::uniform_real_distribution<> dis(0.0f,1.0f);
		float r = dis(gen);

		//Find where random number falls into
		for(int i = 0; i < N; ++i)
			if(r < cumulative[i])
				return i;
		return -1;
	}

	~NeuralNetwork(){ 
		if(rawState != nullptr) delete[] rawState; rawState = nullptr;
		if(mapMask != nullptr) delete[] mapMask; mapMask = nullptr;
	}
};

NeuralNetwork neuralNet; //ucit cemo jednu mrezu po programu

struct ObjectMap
{
	uint32 get(int8 HouseID, int32 ID)
	{
		assert(HouseID < 9);
		uint32 i = map.size()+1;
		i = (i << 4) | (HouseID & 0x0f);

		if (auto found = map.find(i); found != map.end())
			return found->first;
		
		map.insert(std::pair<uint32, int32>(i, ID));
		return i;
	}
	int32 get(uint32 i)
	{
		if(auto found = map.find(i); found != map.end())
			return found->second;
		return -1;
	}

private:
	std::map<uint32, int32> map; //<HouseID/seq broj, ID>
};
ObjectMap objectIDMap;

void FillInputState(const SharedMemory::SScreenVisibleInfo* TSbuffer, bool resetMapSize/* = false*/)
{
	//const int64 numCells = TSbuffer->MapWidth*TSbuffer->MapHeight;
	const int64 numCells = TSbuffer->MapNumElements;
	assert(numCells == TSbuffer->NumCellsX*TSbuffer->NumCellsY);

	struct Object{ float Type = 0; float ID = 0; float Health = 0; };
	struct Queued{ float Type = 0; float Cost = 0; float NumQueued = 0; };
	
	struct HouseDescription
	{
		float Credits = 0;
		float PowerAvailable = 0;
		float PowerRequired = 0;
		Queued SidebarBuildings[SharedMemory::SSidebarItem<SharedMemory::EBuildingType>::SidebarColumnNumItems];
		Queued SidebarUnits[SharedMemory::SSidebarItem<SharedMemory::EUnitType>::SidebarColumnNumItems];
	};
	struct CellDescription
	{
		float LandType = 0;
		float TiberiumValue = 0;
		Object BuildingType;
		Object UnitTypeOnLand[SharedMemory::SCellDescription::MaxUnitsPerCell];
		Object UnitTypeInFlight[SharedMemory::SCellDescription::MaxUnitsInFlight];

		static void Copy(const SharedMemory::SObjectDescription<SharedMemory::EUnitType>& obj, Object& outObj)
		{
			outObj.Type = obj.Type;
			outObj.ID = objectIDMap.get(obj.HouseID, obj.ID);
			outObj.Health = obj.Health;
		}
		void Copy(const SharedMemory::SObjectDescription<SharedMemory::EBuildingType>& obj){
			CellDescription::Copy(obj, BuildingType);
		}
		void Copy(const SharedMemory::SObjectDescription<SharedMemory::EUnitType>& obj, int32 i, bool inflight = false){
			if(inflight == false)
				CellDescription::Copy(obj, UnitTypeOnLand[i]);
			else
				CellDescription::Copy(obj, UnitTypeInFlight[i]);
		}
		void Copy(const SharedMemory::SCellDescription& smcell)
		{
			Copy(smcell.BuildingOnCell);
			for(int i = 0; i < SharedMemory::SCellDescription::MaxUnitsPerCell; ++i)
				Copy(smcell.UnitsOnLand[i], i);
			for(int i = 0; i < SharedMemory::SCellDescription::MaxUnitsInFlight; ++i)
				Copy(smcell.UnitsInFlight[i], i, true);

			TiberiumValue = CellDescription::CombineTiberiumTypeValue(smcell.TiberiumValue, smcell.TiberiumType);
			LandType = CellDescription::CombineLandType(smcell.LandType, smcell.TileType, smcell.PassabilityType);
		}

		static uint32 CombineLandType(SharedMemory::ELandType land, SharedMemory::EIsoTileType tile, SharedMemory::EPassabilityType pass){
			return (uint32(tile) << 7) | (uint32(land) << 3) | uint32(pass);
		}
		static uint32 CombineTiberiumTypeValue(int32 value, SharedMemory::ETiberiumType type){
			return (value < 3) | uint32(type);
		}

		bool IsZero() const{
			const float* fvalues = (float*)this;
			for(uint i = 0; i < sizeof(CellDescription); ++i)
				if(fvalues[i] != 0.0f) return false;
			return true;
		}
	};

	const int32 rawStateBufferSizeBytes = sizeof(HouseDescription) + numCells*sizeof(CellDescription);
	const int32 mapFilterBufferSizeBytes = numCells*sizeof(float);
	if(neuralNet.rawState == nullptr)
	{
		const uint numElementsInCell = (sizeof(CellDescription) / sizeof(float));
		const uint2d numElements = { numElementsInCell*TSbuffer->NumCellsX, TSbuffer->NumCellsY };
		const uint2d outputElements = {7, 1};

		neuralNet.rawState = new float[rawStateBufferSizeBytes/sizeof(float)];
		neuralNet.mapMask = new float[mapFilterBufferSizeBytes/sizeof(float)];
		neuralNet.rawStateDebugRGBA = new uint8[4*numCells];
		neuralNet.inputState = torch::from_blob(neuralNet.rawState, {1, TSbuffer->NumCellsX, TSbuffer->NumCellsY, numElementsInCell}, torch::TensorOptions(torch::kFloat32));
		
		neuralNet.create(numElementsInCell, 2, 7, true);
		
		neuralNet.to(Device); //torch::kCUDA

		ZeroMemory(neuralNet.rawState, rawStateBufferSizeBytes);
		ZeroMemory(neuralNet.rawStateDebugRGBA, 4*numCells*sizeof(uint8));
		resetMapSize = true; //initialize first time

		neuralNet.printModel();
		if(auto att = neuralNet.getSpatialAttention())
			att->printModel();
	}

	if(resetMapSize)
		for(uint i = 0; i < numCells; ++i) neuralNet.mapMask[i] = 1e-9f;

	//copy new data
#if 0
	HouseDescription* houseDescription = (HouseDescription*)neuralNet.rawState;
	CellDescription* cellDescriptions = (CellDescription*)(((byte*)neuralNet.rawState) + sizeof(HouseDescription));
	{
		houseDescription->Credits = TSbuffer->Player.Credits;
		houseDescription->PowerAvailable = TSbuffer->Player.PowerAvailable;
		houseDescription->PowerRequired = TSbuffer->Player.PowerUsed;

		for(int i = 0; i < SharedMemory::SSidebarItem<SharedMemory::EBuildingType>::SidebarColumnNumItems; ++i){
			houseDescription->SidebarBuildings[i].Type = TSbuffer->Player.SidebarBuildings[i].Type;
			houseDescription->SidebarBuildings[i].Cost = TSbuffer->Player.SidebarBuildings[i].Cost;
			houseDescription->SidebarBuildings[i].NumQueued = TSbuffer->Player.SidebarBuildings[i].NumQueued;
		}
		for(int i = 0; i < SharedMemory::SSidebarItem<SharedMemory::EUnitType>::SidebarColumnNumItems; ++i){
			houseDescription->SidebarUnits[i].Type = TSbuffer->Player.SidebarUnits[i].Type;
			houseDescription->SidebarUnits[i].Cost = TSbuffer->Player.SidebarUnits[i].Cost;
			houseDescription->SidebarUnits[i].NumQueued = TSbuffer->Player.SidebarUnits[i].NumQueued;
		}
	}
#else
	CellDescription* cellDescriptions = (CellDescription*)neuralNet.rawState;
#endif
	
	for(uint i = 0; i < numCells; ++i)
	{
		const SharedMemory::SCellDescription& smcell = TSbuffer->GetMap()[i];
		uint id = smcell.CellID.y*TSbuffer->NumCellsX + smcell.CellID.x;
		assert(id < numCells);
		CellDescription& cell = cellDescriptions[id];
		//if(cell.IsZero() == false && id != 0) __debugbreak();
		cell.Copy(smcell);

		neuralNet.rawStateDebugRGBA[id*4 + 0] = 255*smcell.BuildingOnCell.Health + 0.5f;
		neuralNet.rawStateDebugRGBA[id*4 + 1] = 255*(smcell.TiberiumValue / 100.0f) + 0.5f;
		neuralNet.rawStateDebugRGBA[id*4 + 2] = 255*sqrt(smcell.LandType / float(SharedMemory::ELandType::LandCount)) + 0.5f;
		neuralNet.rawStateDebugRGBA[id*4 + 3] = 255*1.0f;
		
		if(resetMapSize)
			neuralNet.mapMask[id] = 1.0f;
	}
	
	if(resetMapSize)
		if(auto attention = neuralNet.getSpatialAttention())
			attention->setMapMask(neuralNet.mapMask, TSbuffer->NumCellsX, TSbuffer->NumCellsY);
}

float CalculateReward(const SharedMemory::SScreenVisibleInfo* TSbuffer)
{
	static volatile float rewardScale = 1.0f/1.0f;

	static float buildingValues[] = {
		0.0f,
		#include "sharedmem\buildingTypeValues.h"
		0.0f
	};
	static float unitValues[] = {
		0.0f,
		#include "sharedmem\unitTypeValues.h"
		0.0f
	};

	static SharedMemory::SObjectStatistics objectStatistics;
	static SharedMemory::SPlayerInfo playerStatistics;

	SharedMemory::SObjectStatistics deltaObj = TSbuffer->ObjectStatistics - objectStatistics;
	SharedMemory::SPlayerInfo deltaPlayer = TSbuffer->Player - playerStatistics;
	objectStatistics = TSbuffer->ObjectStatistics;
	playerStatistics = TSbuffer->Player;

	float valueSum = 0.0f;
	static volatile float enemyValueScale = 100.0f;
	for(int i = 0; i < SharedMemory::SObjectStatistics::NumBuildingTypes; ++i)
	{
		valueSum -= enemyValueScale * deltaObj.EnemyBuildingTypesCount[i] * buildingValues[i];
		valueSum += deltaObj.PlayerBuildingTypesCount[i] * buildingValues[i];
	}
	for(int i = 0; i < SharedMemory::SObjectStatistics::NumUnitTypes; ++i)
	{
		valueSum -= enemyValueScale * deltaObj.EnemyUnitTypesCount[i] * unitValues[i];
		valueSum += deltaObj.PlayerUnitTypesCount[i] * unitValues[i];
 	}

	static volatile float creditValueScale = 1.0f/1000.0f;
	valueSum += creditValueScale*deltaPlayer.Credits;
	static volatile float powerBonusValueScale = 1.0f/100.0f;
	valueSum += (TSbuffer->Player.PowerAvailable < TSbuffer->Player.PowerUsed)? -1.0f : powerBonusValueScale*min<float>(float(TSbuffer->Player.PowerAvailable - TSbuffer->Player.PowerUsed) / max<float>(TSbuffer->Player.PowerAvailable,1.0f), 0.5f);

	return rewardScale*valueSum;
}

float SetReward(float value){
	static float reward = 0.0f;
	float oldValue = reward;
	reward = value;
	return oldValue;
}
float GetReward(){
	return SetReward(0.0f);
}

void Step()
{
	cudaDeviceSynchronize();
	static torch::optim::Adam optimizer(neuralNet.parameters(), torch::optim::AdamOptions(1e-4));

	// Reshape from NHWC to NCHW format: [batch, channels, height, width]
	torch::Tensor input = neuralNet.inputState.permute({0, 3, 1, 2}).to(Device);//torch::kCUDA.clone();

	/*torch::Tensor logits = neuralNet.forward(input);
	torch::Tensor probs = torch::softmax(logits, 1);*/
	//auto dist = torch::distributions::Categorical(probs);
	//int action = dist.sample().item<int>();
	//assert(action != -1);

	neuralNet.train();
	torch::Tensor logits = neuralNet.forward(input);          // [1, action_dim]
	torch::Tensor probs = torch::softmax(logits, 1);      // policy distribution

	if(((logits.max() < INFINITY) & (logits.min() >= 0)).item().to<bool>() == false)
	{
		auto maxValue = logits.max();
		auto minValue = logits.min();
		printf("logits:\n\t");
		torch::print(maxValue); printf("\n\t");
		torch::print(minValue);	printf("\n");
	}

	int action = NeuralNetwork::sampleFromProbs(logits); assert(action != -1);
	torch::Tensor log_probs = torch::log_softmax(logits, 1);

	// Log prob of the action taken
	torch::Tensor log_prob = log_probs[0][action];  // scalar

	// Entropy bonus (to avoid overconfidence)
	torch::Tensor entropy = -(probs * log_probs).sum(1).mean();
	// Entropy from attention
	entropy += neuralNet.getSpatialAttention()->entropyLoss;

	const float reward = GetReward();

	// Optional: Normalize reward or subtract baseline
	float baseline = 0.0f;
	float advantage = reward - baseline;

	// Final loss
	torch::Tensor loss = -log_prob * advantage - 0.01 * entropy;

	optimizer.zero_grad();
	loss.backward();

	// Gradient clipping
	torch::nn::utils::clip_grad_norm_(neuralNet.parameters(), 1.0);

	optimizer.step();
}

const float* GetDebugHeatmapData(uint* width, uint* height, bool normalize/* = true*/)
{
	if(neuralNet.attention != nullptr)
		return neuralNet.attention->getDebugHeatmapData(width, height, normalize);
	return nullptr;
}
const uint8* GetDebugInputMapData(uint* width, uint* height)
{
	if(neuralNet.rawStateDebugRGBA != nullptr)
		return neuralNet.getRawStateDebugRGBA(width, height);
	return nullptr;
}
}


//---------------------------------------------------------------------------------
int main_tensor()
{
	//neuralNet.create({148481, 148481, 148481, 148481, 148481, 4192, 256, 16});

	torch::Tensor tensor = torch::rand({ 2, 3 });
	if(torch::cuda::is_available())
	{
		std::cout << "CUDA is available! Training on GPU" << std::endl;
		auto tensor_cuda = tensor.cuda();
		std::cout << tensor_cuda << std::endl;
	}
	else
	{
		std::cout << "CUDA is not available! Training on CPU" << std::endl;
		std::cout << tensor << std::endl;
	}

	//std::cin.get();
	return 0;
}
//---------------------------------------------------------------------------------

#pragma pop_macro("max")
#pragma pop_macro("min")