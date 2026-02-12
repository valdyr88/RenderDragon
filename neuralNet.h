
namespace SharedMemory{ struct SScreenVisibleInfo; }
namespace NeuralNet
{
	void FillInputState(const SharedMemory::SScreenVisibleInfo* TSbuffer, bool resetMapSize = false);
	float CalculateReward(const SharedMemory::SScreenVisibleInfo* TSbuffer);
	float SetReward(float value);
	void Step();
	const float* GetDebugHeatmapData(uint* width, uint* height, bool normalize = true);
	const uint8* GetDebugInputMapData(uint* width, uint* height);
}

int main_tensor();