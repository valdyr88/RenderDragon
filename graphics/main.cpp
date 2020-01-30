#include "objects/uniform_buffer.h"
#include "device.h"

int main(){

	struct UBStruct{
		vec4 color;
		float time;
		float intensity;
	};

	SharedPtr<IDevice> dev = std::make_shared<TestDevice>();
	WeakPtr<IDevice> wdev = dev;

	SShaderResourceDesc desc = {
		EShaderResourceType::UniformBuffer,
		EShaderResourceUpdateType::Dynamic,
		0x00000000
	};

	SUniformMap m = { EValueType::float32, EValueSize::scalar, 1, "time" };
	std::vector<SUniformMap> v =
	{
		{ EValueType::float32, EValueSize::vec4, 1, "color"},
		{ EValueType::float32, EValueSize::scalar, 1, "time"},
		{ EValueType::float32, EValueSize::scalar, 1, "intensity"}
	};

	auto ub = CUniformBuffer<UBStruct>(wdev, desc, "UBStruct",
				{
					{ EValueType::float32, EValueSize::vec4, 1, "color"},
					{ EValueType::float32, EValueSize::scalar, 1, "time"},
					{ EValueType::float32, EValueSize::scalar, 1, "intensity"}
				});

	ub->color = vec4(1.0f, 0.0f, 1.0f, 1.0f);
	ub->intensity = 1.25f;
	ub->time = 0.1f;

	ub.setUniform("time", 2.0f);
	ub.setUniform("intensity", 0.1f);
	ub.setUniform("color", vec4(0.1f, 0.7f, 0.8f, 1.0f));
	ub.setUniform("color", 1.2f);

	return 0;
}