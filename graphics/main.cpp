#include "objects/uniform_buffer.h"
#include "device.h"

int main(){

	struct UBStruct{
		vec4 color;
		float time;
		float intensity;
	};

	SharedPtr<Device> dev = std::make_shared<Device>();
	WeakPtr<Device> wdev = dev;

	ShaderResourceDesc desc = {
		ShaderResourceType::UniformBuffer,
		ShaderResourceUpdateType::Dynamic,
		0x00000000
	};

	UniformMap m = { ValueType::float32, ValueSize::scalar, 1, "time" };
	std::vector<UniformMap> v =
	{
		{ ValueType::float32, ValueSize::vec4, 1, "color"},
		{ ValueType::float32, ValueSize::scalar, 1, "time"},
		{ ValueType::float32, ValueSize::scalar, 1, "intensity"}
	};

	auto ub = UniformBuffer<UBStruct>(wdev, desc, "UBStruct",
				{
					{ ValueType::float32, ValueSize::vec4, 1, "color"},
					{ ValueType::float32, ValueSize::scalar, 1, "time"},
					{ ValueType::float32, ValueSize::scalar, 1, "intensity"}
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