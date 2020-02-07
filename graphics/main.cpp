#include "include.h"

int main(){

	SGPUDeviceDesc devdesc;
	devdesc.swapchain.depthFormat = ETextureFormat::DepthStencil;
	devdesc.swapchain.width = 1280;
	devdesc.swapchain.height = 860;

	SWindow window;
	window.CreateProgramWindow("Prozor", 200, 200, 20, 20, window.flags, true);

	SharedPtr<GPUDevice> dev = rdCreateGPUDevice(devdesc);
	WeakPtr<GPUDevice> wdev = dev;

	dev->InitContextOnWindow(window);
	
	SShaderResourceDesc desc = {
		EShaderResourceType::UniformBuffer,
		EShaderResourceUpdateType::Dynamic,
		0x00000000
	};

	struct UBStruct{
		vec4 color;
		float time;
		float intensity;
	};

	SUniformMap m = { EValueType::float32, EValueSize::scalar, 1, "time" };
	std::vector<SUniformMap> v =
	{
		{ EValueType::float32, EValueSize::vec4, 1, "color"},
		{ EValueType::float32, EValueSize::scalar, 1, "time"},
		{ EValueType::float32, EValueSize::scalar, 1, "intensity"}
	};

	auto shub = SharedPtr<CUniformBuffer<UBStruct>>(new CUniformBuffer<UBStruct>(wdev, desc, "UBStruct",
				{
					{ EValueType::float32, EValueSize::vec4, 1, "color"},
					{ EValueType::float32, EValueSize::scalar, 1, "time"},
					{ EValueType::float32, EValueSize::scalar, 1, "intensity"}
				}) );
	auto& ub = *shub.get();

	ub->color = vec4(1.0f, 0.0f, 1.0f, 1.0f);
	ub->intensity = 1.25f;
	ub->time = 0.1f;

	ub.setUniform("time", 2.0f);
	ub.setUniform("intensity", 0.1f);
	ub.setUniform("color", vec4(0.1f, 0.7f, 0.8f, 1.0f));
	ub.setUniform("color", 1.2f);


	SShaderResourceDesc srdesc(EShaderResourceType::Texture);
	{
		srdesc.stages = EShaderStage::VertexShader | EShaderStage::FragmentShader;
		srdesc.type = EShaderResourceType::Texture;
		srdesc.updateType = EShaderResourceUpdateType::Static;
	};
	SharedPtr<CShaderResource> sr = SharedPtr<CShaderResource>(new CShaderResource(wdev, srdesc));

	SShaderDesc fsdesc;
	{
		fsdesc.source = "neki fragment source";
		fsdesc.stage = EShaderStage::FragmentShader;
		fsdesc.resources = {
			{0, sr },
			{1, shub },
			{2, sr }
		};
	};

	SShaderDesc vsdesc;
	{
		vsdesc.source = "neki vertex source";
		vsdesc.stage = EShaderStage::VertexShader;
		vsdesc.resources = {
			{0, sr },
			{1, shub },
			{2, sr }
		};
	};

	auto fshader = SharedPtr<CShader>(new CShader(wdev, fsdesc));
	auto vshader = SharedPtr<CShader>(new CShader(wdev, vsdesc));

	auto shaderProgram = CShaderProgram(wdev, { vshader,fshader });

	MainPlatformLoop();
	
	return 0;
}