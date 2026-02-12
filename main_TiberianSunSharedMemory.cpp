#include "graphics/include.h"

#include <conio.h>
#define LOG_TO_CONSOLE(format, ...)		{ printf_s(format "\n", __VA_ARGS__); }

struct OnFunctionReturn
{
	OnFunctionReturn(std::function<void()> fn) : callback(fn) {}
	~OnFunctionReturn(){ callback(); }
private:
	std::function<void()> callback;
};

struct UBStruct{
	vec4 color;
	float time;
	float intensity;

	static std::vector<SUniformMap> desc;
};

std::vector<SUniformMap> UBStruct::desc =
	{
		{ "color", EValueType::float32, EValueSize::vec4, 1},
		{ "time", EValueType::float32, EValueSize::scalar, 1},
		{ "intensity", EValueType::float32, EValueSize::scalar, 1}
	};

//---------------------------------------------------------------------------------
#include "graphics/descriptors/shader_parser.h"
#include "graphics/objects/mipmap_gen.h"

const char* include_list[] =
{
	"data/Shaders/include/functions.glsl",
	"data/Shaders/include/defines.glsl",
	"data/Shaders/include/pbr.glsl",
	"data/Shaders/error_shader.glsl",
	nullptr
};

UniquePtr<CVertexBuffer> CreateVertexBuffer(GPUDevice* dev)
{
	SVertexFormat fmt;
	fmt.attributes = {
		{ "vertex", 0, EValueType::float32, EValueSize::vec3 },
		{ "normal", 1, EValueType::float32, EValueSize::vec3, 0, true },
		{ "texCoord", 2, EValueType::float32, EValueSize::vec2 }
	};
	fmt.layout = EAttributeLayout::Contiguous;

	std::vector<vec3> vertex = { vec3(-1.0f, -1.0f, 0.01f), vec3(-1.0f, 1.0f, 0.01f), vec3(1.0f, 1.0f, 0.01f), vec3(1.0f, -1.0f, 0.01f) };
	std::vector<vec3> normal = { vec3(0.0f, 1.0f, 0.0f), vec3(0.0f, 1.0f, 0.0f), vec3(0.0f, 1.0f, 0.0f), vec3(0.0f, 1.0f, 0.0f) };
	std::vector<vec2> txCoord = { vec2(0.0f, 0.0f), vec2(0.0f, 1.0f), vec2(1.0f, 1.0f), vec2(1.0f, 0.0f) };

	auto vertexBuffer = UniquePtr<CVertexBuffer>(new CVertexBuffer(dev, fmt, (uint)vertex.size(), { SRawData(vertex), SRawData(normal), SRawData(txCoord) }));

	return vertexBuffer;
}
UniquePtr<CVertexBuffer> CreateVertexBufferInterleaved(GPUDevice* dev)
{
	SVertexFormat fmt;
	fmt.attributes = {
		{ "vertex", 0, EValueType::float32, EValueSize::vec3 },
		{ "normal", 1, EValueType::float32, EValueSize::vec3, true },
		{ "texCoord", 2, EValueType::float32, EValueSize::vec2 }
	};
	fmt.layout = EAttributeLayout::Interleaved;

	struct vertex{
		vec3 position;
		vec3 normal;
		vec2 txCoord;

		vertex(vec3 pos, vec3 norm, vec2 tx) : position(pos), normal(norm), txCoord(tx){}
	};

	std::vector<vertex> vertices = { vertex(vec3(-1.0f, -1.0f, 0.01f), vec3(0.0f, 1.0f, 0.0f), vec2(0.0f, 0.0f)),
									 vertex(vec3(-1.0f,  1.0f, 0.01f), vec3(0.0f, 1.0f, 0.0f), vec2(0.0f, 1.0f)),
									 vertex(vec3( 1.0f,  1.0f, 0.01f), vec3(0.0f, 1.0f, 0.0f), vec2(1.0f, 1.0f)),
									 vertex(vec3( 1.0f, -1.0f, 0.01f), vec3(0.0f, 1.0f, 0.0f), vec2(1.0f, 0.0f)) };

	auto vertexBuffer = UniquePtr<CVertexBuffer>(new CVertexBuffer(dev, fmt, (uint)vertices.size(), { SRawData(vertices) }));

	return vertexBuffer;
}
UniquePtr<CIndexBuffer> CreateIndexBuffer(GPUDevice* dev)
{
	std::vector<uint16> data = { 0, 1, 2, 2, 3, 0 };
	auto indexBuffer = UniquePtr<CIndexBuffer>(new CIndexBuffer(dev, EValueType::uint16, (uint)data.size(), data));
	return indexBuffer;
}

struct LightData{
	vec3 position;
	float intensity;
	float time;
	uint channelMask;

	static std::vector<SUniformMap> desc;
};
std::vector<SUniformMap> LightData::desc = {
	{"position", EValueType::float32, EValueSize::vec3 },
	{"intensity", EValueType::float32, EValueSize::scalar },
	{"time", EValueType::float32, EValueSize::scalar },
	{"channelMask", EValueType::uint32, EValueSize::scalar }
};
rdRegisterUniformBufferStructure(LightData);


struct TransformMatrices{
	mat4 world;
	mat4 view;
	mat4 projection;
	mat4 normal;

	static std::vector<SUniformMap> desc;
};
std::vector<SUniformMap> TransformMatrices::desc = {
	{"world", EValueType::float32, EValueSize::mat4x4 },
	{"view", EValueType::float32, EValueSize::mat4x4 },
	{"projection", EValueType::float32, EValueSize::mat4x4 },
	{"normal", EValueType::float32, EValueSize::mat4x4 }
};
rdRegisterUniformBufferStructure(TransformMatrices);

SharedPtr<CUniformBuffer<LightData>> CreateUniformBuffer(GPUDevice* dev){
	return SharedPtr<CUniformBuffer<LightData>>(new CUniformBuffer<LightData>(dev, "light"));
}
//---------------------------------------------------------------------------------
#include "neuralNet.h"

//---------------------------------------------------------------------------------
#include "sharedmem\shared_memory.h"

byte* ConvertTSBufferToTextureData(const SharedMemory::SScreenVisibleInfo* TSbuffer, byte* textureDataBuffer)
{
	if(TSbuffer == nullptr) return nullptr;
	const int txDataSize = TSbuffer->MapHeight*TSbuffer->MapWidth*sizeof(uint32);
	//assert(TSbuffer->MapHeight * TSbuffer->MapWidth <= TSbuffer->MapNumElements);

	if(textureDataBuffer == nullptr){
		textureDataBuffer = __rd_new byte[txDataSize];
		ZeroMemory(textureDataBuffer, txDataSize);
	}
#ifdef RD_DEBUG
	ZeroMemory(textureDataBuffer, txDataSize);
#endif

	const double time = getTime_s();

	auto asInt = [](double r, double g, double b, double a){
		return (uint32(a*255) << 24) | (uint32(b*255) << 16) | (uint32(g*255) << 8) | uint32(r*255);
	};

	auto asRGBA = [](uint32 color)
	{
		double r = ((color & 0x000000ff) >> 0) / 255.0;
		double g = ((color & 0x0000ff00) >> 8) / 255.0;
		double b = ((color & 0x00ff0000) >> 16) / 255.0;
		double a = ((color & 0xff000000) >> 24) / 255.0;

		return vec4(r,g,b,a);
	};

	auto mult = [&](uint32 color, double r, double g, double b, double a){
		vec4 rgba = asRGBA(color);
		rgba *= vec4(r,g,b,a);

		return asInt(rgba.x, rgba.y, rgba.z, rgba.w);
	};

	auto rotate45 = [](glm::ivec2 x, glm::ivec2 wh)
	{
		constexpr float cos45 = 0.70710678118654752440084436210485f, sin45 = cos45;
		constexpr float cosn45 = 0.70710678118654752440084436210485f, sinn45 = -cos45;
		constexpr float sqrt2 = 1.4142135623730950488016887242097f;

		/*[cos, -sin]
		  [sin,  cos]*/

		//struct float2 { float x = 0; float y = 0; float2(float x, float y) : x(x), y(y) {} };
		//float2 offset = float2(cosn45 * (wh.x), cosn45 * (wh.y)); //{mul({w,0},R), mul({0,h},R)}
		//float2 r = float2(x.x - 0.5f * offset.x, x.y - 0.5f * offset.y);
		//r = float2(cos45 * r.x + sin45 * r.y, -sin45 * r.x + cos45 * r.y);
		//r = float2(r.x + 0.5f * wh.x, r.y + 0.5f * wh.y);
		//return glm::ivec2(r.x + 0.5f, r.y + 0.5f);

		//static glm::mat2x2 R(cos45, sin45, -sin45, cos45);
		//static glm::vec2 offset = glm::vec2((glm::vec2(wh.x, 0)*R).x, (glm::vec2(0, wh.y)*R).y);
		//static glm::vec2 add = 0.5f*glm::vec2(wh);
		//glm::vec2 r = (glm::vec2(x) + glm::vec2(0.5f) - 0.5f * offset) * R;
		//glm::ivec2 c = r + add; // offset = {83.0, 83.0}, add = {150.5, 58.5};
		//return c;

		//nece radit, nemoze se mapirat iz 45° rotirano u 0°.

		if(x.x == 0 && x.y == 0) return glm::ivec2(0,0);
		static glm::vec2 offset = { 58.0, 58.0 }, add = { 150.5, 58.5 };
		glm::vec2 r = glm::vec2(x.x, x.y) - offset;
		r = glm::vec2((r.x - r.y) / sqrt2, (r.x + r.y) / sqrt2);
		r = r + add;
		return glm::ivec2(r.x, r.y);
	};
	
	float c1 = 0.5f*glm::cos(time*0.5f + 0.0f)+0.5f;
	float c2 = 0.5f*glm::cos(time*0.5f + 3.14f/4)+0.5f;

	int* textureData = (int*)textureDataBuffer;
	const SharedMemory::SCellDescription* Map = TSbuffer->GetMap();
	for(uint32 i = 0; i < TSbuffer->MapNumElements; ++i)
	{
		const SharedMemory::SCellDescription* cell = &Map[i];

		if((TSbuffer->MapHeight - cell->CellID.y) < 0)__debugbreak();
		if(cell->CellID.x < 0)__debugbreak();
		int t = (TSbuffer->MapHeight - cell->CellID.y)*TSbuffer->MapWidth + cell->CellID.x;
		//assert(t < TSbuffer->MapNumElements);
		if(t > txDataSize/4) __debugbreak();
		if(t < 0){ __debugbreak(); continue; }

		uint32 value = 0;
		if(cell->IsVisible)
		{
			switch(cell->TileType)
			{
				case SharedMemory::EIsoTileType::Clear:
					value = asInt(0.5,0.5,0.2,1.0); break;
				case SharedMemory::EIsoTileType::Green:
					value = asInt(0.25, 0.75, 0.25, 1.0); break;
				case SharedMemory::EIsoTileType::Blue:
					value = asInt(0.0, 0.25, 0.75, 1.0); break;
				case SharedMemory::EIsoTileType::Sand:
					value = asInt(0.9, 0.75, 0.5, 1.0); break;
				case SharedMemory::EIsoTileType::Cliff:
					value = asInt(0.5, 0.4, 0.25, 1.0); break;
				case SharedMemory::EIsoTileType::DesroyableCliff:
					value = asInt(0.55, 0.45, 0.3, 1.0); break;
				case SharedMemory::EIsoTileType::Rough:
					value = asInt(0.4, 0.4, 0.25, 1.0); break;
				case SharedMemory::EIsoTileType::Rock:
					value = asInt(0.3, 0.3, 0.25, 1.0); break;
				case SharedMemory::EIsoTileType::Crystal:
					value = asInt(0.25, 0.75, 0.25, 1.0); break;
				case SharedMemory::EIsoTileType::Ramp:
					value = asInt(1.0, 0.0, 1.0, 1.0); break;
				case SharedMemory::EIsoTileType::Shore:
					value = asInt(0.25, 0.75, 0.9, 1.0); break;
				case SharedMemory::EIsoTileType::Water:
					value = asInt(0.0, 0.5, 1.0, 1.0); break;
				case SharedMemory::EIsoTileType::Waterfall:
					value = asInt(0.25, 0.6, 1.0, 1.0); break;
				case SharedMemory::EIsoTileType::IceShore:
					value = asInt(0.55, 0.75, 0.95, 1.0); break;
				case SharedMemory::EIsoTileType::Ice:
					value = asInt(0.55, 0.85, 0.95, 1.0); break;
				case SharedMemory::EIsoTileType::MiscPavement:
					value = asInt(0.55, 0.55, 0.55, 1.0); break;
				case SharedMemory::EIsoTileType::Pavement:
					value = asInt(0.55, 0.55, 0.55, 1.0); break;
				case SharedMemory::EIsoTileType::DirtRoad:
					value = asInt(0.8, 0.7, 0.5, 1.0); break;
				case SharedMemory::EIsoTileType::DirtRoadSlope:
					value = asInt(0.77, 0.65, 0.5, 1.0); break;
				case SharedMemory::EIsoTileType::PavedRoad:
					value = asInt(0.35, 0.35, 0.35, 1.0); break;
				case SharedMemory::EIsoTileType::Tunnel:
					value = asInt(0.15, 0.15, 0.15, 1.0); break;
				case SharedMemory::EIsoTileType::Bridge:
					value = asInt(0.45, 0.45, 0.45, 1.0); break;
				case SharedMemory::EIsoTileType::BridgeMiddle:
					value = asInt(0.45, 0.45, 0.45, 1.0); break;
				case SharedMemory::EIsoTileType::TrainBridge:
					value = asInt(0.45, 0.45, 0.45, 1.0); break;
				case SharedMemory::EIsoTileType::TrainBridgeMiddle:
					value = asInt(0.45, 0.45, 0.45, 1.0); break;
				case SharedMemory::EIsoTileType::TrainSlope:
					value = asInt(0.45, 0.45, 0.45, 1.0); break;
				case SharedMemory::EIsoTileType::Train:
					value = asInt(0.35, 0.45, 0.35, 1.0); break;
				case SharedMemory::EIsoTileType::Slope:
					value = asInt(0.15, 0.25, 0.35, 1.0); break;
				case SharedMemory::EIsoTileType::HeightBase:
					value = asInt(0.5, 0.0, 0.5, 1.0); break;
				case SharedMemory::EIsoTileType::Black:
					value = asInt(0.0, 0.0, 0.0, 1.0); break;
				default:
					value = asInt(1.0, 0.5, 1.0, 1.0); break;
			}

			switch(cell->LandType)
			{
				/*case SharedMemory::ELandType::LandNone:
					value = asInt(0.0, 0.0, 0.0, 1.0); break;*/
				case SharedMemory::ELandType::LandClear:
					value = asInt(0.7, 0.5, 0.3, 1.0); break;
				case SharedMemory::ELandType::LandRoad:
					value = asInt(0.3, 0.3, 0.3, 1.0); break;
				case SharedMemory::ELandType::LandWater:
					value = asInt(0.0, 0.5, 1.0, 1.0); break;
				case SharedMemory::ELandType::LandRock:
					value = asInt(0.43, 0.33, 0.33, 1.0); break;
				case SharedMemory::ELandType::LandWall:
					value = asInt(0.44, 0.44, 0.44, 1.0); break;
				case SharedMemory::ELandType::LandTiberium:
					value = asInt(0.0, 0.75, 0.0, 1.0); break;
				case SharedMemory::ELandType::LandBeach:
					value = asInt(0.5, 0.5, 1.0, 1.0); break;
				case SharedMemory::ELandType::LandRough:
					value = asInt(0.5, 0.3, 0.2, 1.0); break;
				case SharedMemory::ELandType::LandIce:
					value = asInt(0.75, 0.75, 0.9, 1.0); break;
				case SharedMemory::ELandType::LandRailroad:
					value = asInt(0.5, 0.5, 0.5, 1.0); break;
				case SharedMemory::ELandType::LandTunnel:
					value = asInt(0.2, 0.2, 0.2, 1.0); break;
				case SharedMemory::ELandType::LandWeeds:
					value = asInt(0.9, 0.8, 0.0, 1.0); break;
				case SharedMemory::ELandType::LandCount:
					break;
				default:
					break;
			}

			float tiberium = glm::min((cell->TiberiumValue / 500.0f), 0.99f);
			switch(cell->TiberiumType)
			{
				case SharedMemory::TiberiumRiparius:
					value = (value & 0xfff0f0f0) | asInt(0, 0.5f*tiberium, 0, 0);
					break;
				case SharedMemory::TiberiumCruentus:
					break;
				case SharedMemory::TiberiumAboreus:
				case SharedMemory::TiberiumVinifera:
					value = (value & 0xff000000) | asInt(0, 0, tiberium, 0);
					break;
					break;
				default:
				case SharedMemory::TiberiumNone:
				case SharedMemory::TiberiumCount:
					break;
			}

			float height = (cell->CellElevation / 10.0f) * 0.5f + 0.5f;
			value = mult(value, height, height, height, 1.0);

			for(int i = 0; i < SharedMemory::SCellDescription::MaxUnitsPerCell; ++i){
				const SharedMemory::SObjectDescription<SharedMemory::EUnitType>& unitOnLand = cell->UnitsOnLand[i];
				if(unitOnLand.ID > 0)
					if(unitOnLand.HouseID == TSbuffer->Player.HouseID)
						value = asInt(0.5f, 0.75f, 1.0f, 1.0f);
					else
						value = (unitOnLand.GetCloakedStatus())? asInt(1.0f, c1*0.75f, c1*0.5f, 1.0f) : asInt(1.0f, 0.75f, 0.5f, 1.0f);
			}
			for(int i = 0; i < SharedMemory::SCellDescription::MaxUnitsInFlight; ++i){
				const SharedMemory::SObjectDescription<SharedMemory::EUnitType>& unitInFlight = cell->UnitsInFlight[i];
				if(unitInFlight.ID > 0)
					if(unitInFlight.HouseID == TSbuffer->Player.HouseID)
						value = asInt(0.5f, 0.75f, 1.0f, 1.0f);
					else
						value = (unitInFlight.GetCloakedStatus())? asInt(1.0f, c1*0.875f, c1*0.75f, 1.0f) : asInt(1.0f, 0.875f, 0.75f, 1.0f);
			}
			
			if(cell->BuildingOnCell.Type != 0)
				if(cell->BuildingOnCell.HouseID == TSbuffer->Player.HouseID)
					value = asInt(0.8,0.9,1.0,1.0);
				else
					value = (cell->BuildingOnCell.GetCloakedStatus())? asInt(1.0f, c1*0.9f, c1*0.8f, 1.0f) : asInt(1.0f, 0.9f, 0.8f, 1.0f);
					
		}

		textureData[t] = value;
	}

	return textureDataBuffer;
}

void WriteCommandToBuffer(SharedMemory::SCommand* Command, SharedMemory::ECommand commandID, SharedMemory::EUnitType buildUnitType, SharedMemory::EBuildingType buildBuildingType, int objectID, SharedMemory::SIntCoord2D cellID)
{
	assert(commandID > SharedMemory::ECommand::None && commandID < SharedMemory::ECommand::NumCommands);

	if(commandID == SharedMemory::ECommand::BuildBuilding || commandID == SharedMemory::ECommand::SuspendBuildBuilding || commandID == SharedMemory::ECommand::AbandonBuildBuilding || commandID == SharedMemory::ECommand::PlaceBuilding)
		assert(buildBuildingType > 0 && buildBuildingType <= SharedMemory::GetNumBuildingTypes());
	if(commandID == SharedMemory::ECommand::BuildUnit || commandID == SharedMemory::ECommand::SuspendBuildUnit || commandID == SharedMemory::ECommand::AbandonBuildUnit)
		assert(buildUnitType > 0 && buildUnitType <= SharedMemory::GetNumUnitTypes());

	if(commandID == SharedMemory::ECommand::PlaceBuilding)
		assert(cellID.x >= 0 && cellID.y >= 0);

	Command->LockForWrite();
	Command->IncreaseFrameID();

		Command->BuildBuildingType = buildBuildingType;
		Command->BuildUnitType = buildUnitType;
		Command->CellID = cellID;
		Command->CommandID = commandID;
		Command->ObjectID = objectID;

	Command->Unlock();
}

void DecideCommand()
{
	volatile SharedMemory::ECommand commandID = SharedMemory::ECommand::None;
	SharedMemory::EUnitType buildUnitType = 0;
	SharedMemory::EBuildingType buildBuildingType = 0;
	int objectID = -1;
	SharedMemory::SIntCoord2D cellID;

	switch(commandID)
	{
		case SharedMemory::None:
			return;
		case SharedMemory::Select:
			break;
		case SharedMemory::SelectAllSameUnits:
			break;
		case SharedMemory::SelectAllUnits:
			break;
		case SharedMemory::Unselect:
			break;
		case SharedMemory::MoveTo:
			break;
		case SharedMemory::AttackMoveTo:
			break;
		case SharedMemory::BuildBuilding:
			break;
		case SharedMemory::BuildUnit:
			break;
		case SharedMemory::SuspendBuildBuilding:
			break;
		case SharedMemory::SuspendBuildUnit:
			break;
		case SharedMemory::AbandonBuildBuilding:
			break;
		case SharedMemory::AbandonBuildUnit:
			break;
		case SharedMemory::PlaceBuilding:
			break;
		case SharedMemory::RepairAllBuildings:
			break;
		case SharedMemory::NumCommands:
			break;
		default:
			break;
	}

	WriteCommandToBuffer(SharedMemory::Command, commandID, buildUnitType, buildBuildingType, objectID, cellID);
}

int main_TSDisplay()
{
	main_tensor();

	while(SharedMemory::ptrSharedMemory == nullptr)
	{
		SharedMemory::AllocateSScreenVisibleInfoBuffers();
		Sleep(1000);
	}

	SharedMemory::SScreenVisibleInfo* TSbuffer = SharedMemory::WaitForBufferLock();
	int windowHeight = TSbuffer->MapHeight;
	int windowWidth = TSbuffer->MapWidth;
	windowHeight = 512;
	windowWidth =  512;
	constexpr int windowScale = 1;

	LOG("Window width: %d, height: %d", windowWidth, windowHeight);

	SGPUDeviceDesc devdesc;
	devdesc.swapchain.depthFormat = ETextureFormat::DepthStencil;
	devdesc.swapchain.width = windowScale*windowWidth;//512;
	devdesc.swapchain.height = windowScale*windowHeight;//288;

	UniquePtr<GPUDevice> device = GPUDevice::CreateGPUDevice(devdesc);

	SWindow window;
	window.CreateProgramWindow("Prozor", devdesc.swapchain.width, devdesc.swapchain.height, 20, 20, window.flags, true);

	device->InitContextOnWindow(window);

	//auto materialInstance = testXML(device.get());
	//IUniformBuffer::CreateUniformBufferType["LightData"] = CUniformBuffer<LightData>::CreateUniformBuffer;
	//SharedPtr<IUniformBuffer> iub = SharedPtr<CUniformBuffer<LightData>>(new CUniformBuffer<LightData>(device.get(), "light"));

	auto iubTyped = IUniformBuffer::CreateUniformBufferType["LightData"](device.get(), "lightub");
	if(IUniformBuffer::CreateUniformBufferType["LightData2"] != nullptr)
		auto iubTyped2 = IUniformBuffer::CreateUniformBufferType["LightData2"](device.get(), "light2ub");

	auto shpUniformBuffer = CreateUniformBuffer(device.get());
	auto& ub = *shpUniformBuffer.get();

	auto vertexBuffer = CreateVertexBufferInterleaved(device.get());
	auto indexBuffer = CreateIndexBuffer(device.get());

	STextureDesc txdesc;
	//txdesc.format = ETextureFormat::R;
	txdesc.format = ETextureFormat::RGBA;
	txdesc.type = ETextureType::Texture2D;
	//txdesc.valueType = EValueType::uint32;
	txdesc.valueType = EValueType::uint8;
	txdesc.width = TSbuffer->MapHeight;
	txdesc.height = TSbuffer->MapWidth;
	txdesc.bGenMipmaps = false;
	txdesc.name = "tsBufferData";
	STextureRawData textureRawData(txdesc.width, txdesc.height, 1, 1, false);
	byte* rawTSbufferTextureData = ConvertTSBufferToTextureData(TSbuffer, nullptr);
	TSbuffer->Unlock();
	textureRawData.slices[0].data = rawTSbufferTextureData;
	textureRawData.slices[0].size = txdesc.height*txdesc.width*sizeof(uint32);
	SharedPtr<CTexture> texture = NewShared<CTexture>(device.get(), txdesc, textureRawData);

	texture->UpdateLevelData(textureRawData.slices[0]);

	txdesc.format = ETextureFormat::R;
	txdesc.valueType = EValueType::float32;
	txdesc.width = 128;
	txdesc.height = 128;
	txdesc.name = "heatmapDebug";
	STextureRawData textureDebugRawData(txdesc.width, txdesc.height, 1, 1, false);
	textureDebugRawData.slices[0].data = new byte[txdesc.width*txdesc.height*4];
	textureDebugRawData.slices[0].size = txdesc.width*txdesc.height*sizeof(uint32);
	SharedPtr<CTexture> debug128x128f32Texture = NewShared<CTexture>(device.get(), txdesc, textureDebugRawData);
	
	debug128x128f32Texture->UpdateLevelData(textureDebugRawData.slices[0]);
	delete[] textureDebugRawData.slices[0].data;
	textureDebugRawData.slices[0].data = nullptr;

	txdesc.format = ETextureFormat::RGBA;
	txdesc.valueType = EValueType::uint8;
	txdesc.width = 512;
	txdesc.height = 512;
	txdesc.name = "heatmapDebug";
	STextureRawData textureDebugRawData2(txdesc.width, txdesc.height, 1, 1, false);
	textureDebugRawData2.slices[0].data = new byte[txdesc.width*txdesc.height*4];
	textureDebugRawData2.slices[0].size = txdesc.width*txdesc.height*sizeof(uint32);
	SharedPtr<CTexture> debug512x512i8RGBATexture = NewShared<CTexture>(device.get(), txdesc, textureDebugRawData2);

	debug512x512i8RGBATexture->UpdateLevelData(textureDebugRawData2.slices[0]);
	delete[] textureDebugRawData2.slices[0].data;
	textureDebugRawData2.slices[0].data = nullptr;

	SSamplerDesc smpldesc;
	smpldesc.magFilter = ETextureFiltering::Nearest;
	smpldesc.minFilter = ETextureFiltering::Nearest;
	smpldesc.mipFilter = ETextureFiltering::Nearest;
	smpldesc.uWrapping = ETextureWrapping::Clamp;
	smpldesc.vWrapping = ETextureWrapping::Clamp;
	smpldesc.wWrapping = ETextureWrapping::Clamp;
	SharedPtr<CSampler> sampler = NewShared<CSampler>(device.get(), smpldesc);
	
	STextureViewDesc viewdesc; 
	viewdesc = texture->getDescriptor(); 
	viewdesc.bGenMipmaps = false; 
	viewdesc.usageFlags = ETextureUsage::ShaderResource;
	viewdesc.format = ETextureFormat::RGBA;
	viewdesc.type = ETextureType::Texture2D;
	viewdesc.mipLevelBase = 0;
	viewdesc.numMipLevels = 1;
	viewdesc.valueType = EValueType::uint8;
	CTextureView txView(device.get(), viewdesc, texture, sampler);

	viewdesc = debug128x128f32Texture->getDescriptor();
	viewdesc.format = ETextureFormat::R;
	viewdesc.valueType = EValueType::float32;
	CTextureView txDebug128x128f32View(device.get(), viewdesc, debug128x128f32Texture, sampler);

	viewdesc = debug512x512i8RGBATexture->getDescriptor();
	viewdesc.format = ETextureFormat::RGBA;
	viewdesc.valueType = EValueType::uint8;
	CTextureView txDebug512x512i8View(device.get(), viewdesc, debug512x512i8RGBATexture, sampler);

	//auto txView = materialInstance->getTexture(0);
	//SharedPtr<CTexture> texture = device->FindSharedPtr<CTexture>(txView->getActualTexture());

	CShaderFileSource* srcList = CSingleton<CShaderFileSource>::get();
	for(uint i = 0; include_list[i] != nullptr; ++i)
		srcList->add(include_list[i], getFileStringContents(include_list[i]));

	CShaderDefines* globalDefines = CSingleton<CShaderDefines>::get();
	globalDefines->add("DEBUG", "1");
	globalDefines->add("HDR_ENABLE", "true");
	globalDefines->add("NEKIDEFINE", "0x001");

	CShaderDefines defines2;
	defines2.add("DEBUG_BUG", "123");
	defines2.add("BLABLA_TADA", "DEBUG_BUG");
	defines2.add("T", "1");

	*globalDefines += defines2;

	std::string source;
	static volatile bool useNeuralNet = false;
	if(useNeuralNet)
		source = TestIncludes("data/Shaders/depthDisplay.ps.glsl");
	else
		source = TestIncludes("data/Shaders/simple.ps.glsl");
	//auto source = transfSPIRVtoGLSL("data/Shaders/simple.ps.spv",430,false);
	source = globalDefines->InsertInto(source);

	printContentsToFile("data/Shaders/simple.ps.glsl.processed.glsl", source.c_str(), (uint)source.length());

	auto vsSource = TestIncludes("data/Shaders/simple.vnt.vs.glsl");
	//auto vsSource = transfSPIRVtoGLSL("data/Shaders/simple.vnt.vs.spv",430,false);
	vsSource = globalDefines->InsertInto(vsSource);
	
	SShaderDesc fsdesc(EShaderStage::FragmentShader, "simple_shading.ps.glsl", source, { {
			{0, 0, "tx", EShaderResourceType::Texture, EShaderStage::FragmentShader},
			{0, 2, "light", EShaderResourceType::UniformBuffer, EShaderStage::FragmentShader}
		}});
	SShaderDesc vsdesc(EShaderStage::VertexShader, "simple.vntt.vs.glsl", vsSource, {});
	vsdesc.vertexFormat = vertexBuffer->getVertexFormat();

	auto VShader = CSingleton<CShaderModuleManager>::get()->CreateShaderModule(vsdesc);
	auto FShader = CSingleton<CShaderModuleManager>::get()->CreateShaderModule(fsdesc);
	auto program = CSingleton<CShaderProgramManager>::get()->CreateShaderProgram("simple.ps.glsl", { VShader,FShader });

	auto renderPass = device->GetSwapchainRenderPass();
	auto framebuffer = device->GetActiveSwapchainFramebuffer();
	
	SPipelineStateDesc psdesc;
	{
		psdesc.shader = program;
		psdesc.blendDesc.alphaToCoverageEnable = false;
		psdesc.blendDesc.attachmentBlends[0].blendEnable = false;
		psdesc.depthDesc.enable = false;
		psdesc.primitiveTopology = EPrimitiveTopology::TriangleList;
		psdesc.renderPass = renderPass;
		psdesc.viewports.numViewports = 1;
		psdesc.viewports.viewports[0].width = (float)device->getDescriptor().swapchain.width;
		psdesc.viewports.viewports[0].height = (float)device->getDescriptor().swapchain.height;
		psdesc.viewports.viewports[0].x = 0;
		psdesc.viewports.viewports[0].y = 0;
		psdesc.viewports.scissorTest->enable = false;
		psdesc.rasterizerDesc.fillMode = EFillMode::Solid;
		/*psdesc.rasterizerDesc.cullMode = ECullMode::FrontFaces;
		psdesc.rasterizerDesc.frontFace = EFrontFace::CounterClockwise;*/
		psdesc.rasterizerDesc.cullMode = ECullMode::BackFaces;
		psdesc.rasterizerDesc.frontFace = EFrontFace::Clockwise;
	}
	auto pipeline = device->CreatePipelineState(psdesc);
	
	program->getNofStages();
	auto shader = pipeline->getShaderProgram();

	float angle = 0.0f;
	float dTime = 0.0f;
	float time = 0.0f;

	OnFunctionReturn ofrtn([=]()
	{
		TSbuffer->Unlock();
	});

	static volatile bool showHeatmapDebugTexture = false;
	static volatile bool showInputMapDebugTexture = false;
	while(true)
	{
		TSbuffer = SharedMemory::WaitForBufferLock();
		textureRawData.slices[0].data = ConvertTSBufferToTextureData(TSbuffer, rawTSbufferTextureData);
		if(useNeuralNet) NeuralNet::FillInputState(TSbuffer);
		float reward = (useNeuralNet)? NeuralNet::CalculateReward(TSbuffer) : 0.0f;
		TSbuffer->Unlock();

		if(useNeuralNet) NeuralNet::SetReward(reward);
		if(useNeuralNet) NeuralNet::Step();

		texture->UpdateLevelData(textureRawData.slices[0]);

		CTextureView* pTxView = &txView;
		if(showHeatmapDebugTexture && useNeuralNet)
		{
			uint width = 0, height = 0;
			if(const float* heatmapData = NeuralNet::GetDebugHeatmapData(&width, &height, false))
			{
				textureDebugRawData.slices[0].data = (byte*)heatmapData;
				textureDebugRawData.slices[0].size = height*width*sizeof(float);

				assert(width == height && width == 128);
				debug128x128f32Texture->UpdateLevelData(textureDebugRawData.slices[0]);
				pTxView = &txDebug128x128f32View;
			}
		}
		if(showInputMapDebugTexture && useNeuralNet)
		{
			uint width = 0, height = 0;
			if(const uint8* inputMapData = NeuralNet::GetDebugInputMapData(&width, &height))
			{
				textureDebugRawData2.slices[0].data = (byte*)inputMapData;
				textureDebugRawData2.slices[0].size = height*width*4*sizeof(uint8);

				assert(width == height && width == 512);
				debug512x512i8RGBATexture->UpdateLevelData(textureDebugRawData2.slices[0]);
				pTxView = &txDebug512x512i8View;
			}
		}

		PlatfromLoopUpdate();

		//----------------------------
		renderPass->Begin(framebuffer.get(), SClearColorValues(vec4(0.5f,0.7f,1.0f,1.0f)));
			pipeline->Bind();

			angle = 10000.0f * time;

			static volatile float offset = 0.0f;
			static volatile float intensity = 1.0f;
			static volatile vec3 position = vec3(0,0,0.25f);
			static volatile uint channelMask = 0x10;
			ub->position = vec3(position.x, position.y, position.z);
			ub->intensity = intensity;//0.01f*(cosf(angle) * 0.5f + 0.5f);
			ub->time = offset;//time;
			ub->channelMask = channelMask;
			ub.Upload();
			//ToDo: napravit provjeru shader sourcea i resourceSetDesc. postoje li svi resoursi u shaderu, ili shader ima neke koji nema resourceSetDesc i obratno?
			shader->setUniformBuffer("light", &ub);
			shader->setTexture("tx", pTxView);

			device->BindVertexBuffer(vertexBuffer.get());
			device->BindIndexBuffer(indexBuffer.get());

			device->DrawIndexed();
		renderPass->End();
		//----------------------------
		device->PresentFrame();

		time += dTime;
		sleep_ms(1); dTime = 1.0f / 1000.0f;
	}

	return 0;
}

//---------------------------------------------------------------------------------

int main(){
	//return main_assimp_load();
	//return main_old();
	return main_TSDisplay();
}