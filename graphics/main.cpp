#include "include.h"

int main(){

	SGPUDeviceDesc devdesc;
	devdesc.swapchain.depthFormat = ETextureFormat::DepthStencil;
	devdesc.swapchain.width = 1280;
	devdesc.swapchain.height = 860;

	SWindow window;
	window.CreateProgramWindow("Prozor", 200, 200, 20, 20, window.flags, true);

	UniquePtr<GPUDevice> dev = GPUDevice::CreateGPUDevice(devdesc);

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

	auto shub = CUniformBuffer<UBStruct>::CreateUniformBuffer(dev.get(), desc, "UBStruct",
				{
					{ EValueType::float32, EValueSize::vec4, 1, "color"},
					{ EValueType::float32, EValueSize::scalar, 1, "time"},
					{ EValueType::float32, EValueSize::scalar, 1, "intensity"}
				} );
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
	SharedPtr<CShaderResource> sr = SharedPtr<CShaderResource>(new CShaderResource(dev.get(), srdesc));

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

	auto fshader = SharedPtr<CShader>(new CShader(dev.get(), fsdesc));
	auto vshader = SharedPtr<CShader>(new CShader(dev.get(), vsdesc));

	auto shaderProgram = SharedPtr<CShaderProgram>(new CShaderProgram(dev.get(), { vshader,fshader }));


	SPipelineStateDesc psdesc;
	{
		psdesc.shader = shaderProgram;
		psdesc.blendDesc.alphaToCoverageEnable = true;
	}
	auto pipeline = dev->CreatePipelineState(psdesc);
	pipeline->bind();


	STextureDesc txdesc;
	{
		txdesc.bGenMipmaps = true;
		txdesc.width = 128;
		txdesc.height = 128;
		txdesc.depth = 1;
		txdesc.type = ETextureType::Texture2D;
		txdesc.format = ETextureFormat::RGBA;
		txdesc.valueType = EValueType::uint8;
		txdesc.usageFlags = ETextureUsage::ColorAttachment | ETextureUsage::ShaderResource;
	}

	STextureRawData txdata(txdesc.width, txdesc.height, 1, 1, true);
	uint width = txdesc.width; uint height = txdesc.height;

	for(uint i = 0; i < txdata.numMips; ++i){
		uint32* dataptr = new uint32[width*height];
		txdata.slices[i].data = (uint8*)dataptr;
		txdata.slices[i].mipLevel = i;
		txdata.slices[i].size = width*height*sizeof(uint32);

		for(uint d = 0; d < width * height; ++d){
			dataptr[d] = 0x2299ffff; //RGBA
		}
		width /= 2; height /= 2;
	}

	auto texture = dev->CreateTexture(txdesc, txdata);

	SRenderPassDesc rpdesc;
	{
		rpdesc.NofAttachments = 1;
		rpdesc.Attachments[0].valueType = EValueType::uint8;
		rpdesc.Attachments[0].format = ETextureFormat::RGBA;
		rpdesc.Attachments[0].loadOp = ELoadStoreOp::Load;
		rpdesc.Attachments[0].storeOp = ELoadStoreOp::Store;
		rpdesc.Attachments[0].usageFlags = ETextureUsage::ColorAttachment | ETextureUsage::ShaderResource;
		rpdesc.Attachments[0].sampleCount = 1;
	}

	auto framebuffer = dev->CreateFramebuffer(rpdesc, { texture });

	MainPlatformLoop();
	
	return 0;
}