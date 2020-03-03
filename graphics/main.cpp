#include "include.h"
#include "descriptors/shader_parser.h"

const char* include_list[] = 
{
	"Shaders/include/functions.glsl",
	"Shaders/include/defines.glsl",
	"Shaders/error_shader.glsl",
	nullptr
};

struct CNekiSingletonData{
	int b = 0;
};

int main(){

	CShaderFileSource* srcList = CSingleton<CShaderFileSource>::get();
	for(uint i = 0; include_list[i] != nullptr; ++i)
		srcList->add(include_list[i], getFileStringContents(include_list[i]));

	CNekiSingletonData* nekisingle = CSingleton<CNekiSingletonData>::get();
	nekisingle->b = 1;


	CShaderDefines* globalDefines = CSingleton<CShaderDefines>::get();
	globalDefines->add("DEBUG", "1");
	globalDefines->add("HDR_ENABLE", "true");
	globalDefines->add("NEKIDEFINE", "0x001");

	TestIncludes("Shaders/simple.ps.glsl");





	SGPUDeviceDesc devdesc;
	devdesc.swapchain.depthFormat = ETextureFormat::DepthStencil;
	devdesc.swapchain.width = 1280;
	devdesc.swapchain.height = 860;

	SWindow window;
	window.CreateProgramWindow("Prozor", 200, 200, 20, 20, window.flags, true);

	UniquePtr<GPUDevice> dev = GPUDevice::CreateGPUDevice(devdesc);

	dev->InitContextOnWindow(window);
	
	SShaderResourceBindingDesc desc = {
		0, 0, "name",
		EShaderResourceType::UniformBuffer,
		0x00000000,
		EShaderResourceUsageType::Dynamic,
		0
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

	auto shub = CUniformBuffer<UBStruct>::CreateUniformBuffer(dev.get(), "UBStruct",
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

	SShaderResourceBindingDesc srdesc(0, 0, "name", EShaderResourceType::Texture, EShaderStage::FragmentShader);
	{
		srdesc.shaderStages = EShaderStage::VertexShader | EShaderStage::FragmentShader;
		srdesc.type = EShaderResourceType::Texture;
		srdesc.usage = EShaderResourceUsageType::Static;
	};
	SharedPtr<CShaderResource> sr = SharedPtr<CShaderResource>(new CShaderResource(dev.get(), EShaderResourceType::Texture));

	SShaderDesc fsdesc;
	{
		fsdesc.source = "neki fragment source";
		fsdesc.stage = EShaderStage::FragmentShader;
		fsdesc.bindings = {
			{0, 1, "texUniformName", EShaderResourceType::Texture, EShaderStage::FragmentShader, EShaderResourceUsageType::Static, 1 },
			{0, 2, "ubUniformName", EShaderResourceType::UniformBuffer, EShaderStage::FragmentShader, EShaderResourceUsageType::Static, 1 },
			{0, 3, "samplerUniformName", EShaderResourceType::Sampler, EShaderStage::VertexShader | EShaderStage::FragmentShader, EShaderResourceUsageType::Static, 1 }
		};
	};

	SShaderDesc vsdesc;
	{
		vsdesc.source = "neki vertex source";
		vsdesc.stage = EShaderStage::VertexShader;
		vsdesc.bindings = {
			{0, 0, "vertexUniform", EShaderResourceType::UniformBuffer, EShaderStage::VertexShader, EShaderResourceUsageType::Static, 1 },
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
	
	auto srm = dev->GetShaderResourceManager();
	auto reset = srm.GetResourceSetDesc({
		{0, 0, "texUniformName", EShaderResourceType::Texture, EShaderStage::FragmentShader, EShaderResourceUsageType::Static, 1 },
		{0, 1, "ubUniformName", EShaderResourceType::UniformBuffer, EShaderStage::FragmentShader, EShaderResourceUsageType::Static, 1 },
		{0, 2, "samplerUniformName", EShaderResourceType::Sampler, EShaderStage::VertexShader | EShaderStage::FragmentShader, EShaderResourceUsageType::Static, 1 },
	});

	auto reset2 = srm.GetResourceSetDesc({
		{1, 0, "texUniformName", EShaderResourceType::Texture, EShaderStage::FragmentShader, EShaderResourceUsageType::Static, 1 },
		{1, 1, "ubUniformName", EShaderResourceType::UniformBuffer, EShaderStage::FragmentShader, EShaderResourceUsageType::Static, 1 },
		{1, 2, "samplerUniformName", EShaderResourceType::Sampler, EShaderStage::VertexShader | EShaderStage::FragmentShader, EShaderResourceUsageType::Static, 1 },
	});

	//merge test
	auto descSetA = std::vector<SShaderResourceBindingDesc>{
		{ 0, 0, "texUniformName", EShaderResourceType::Texture, EShaderStage::FragmentShader, EShaderResourceUsageType::Static, 1 },
		{ 0, 4, "ubUniformName", EShaderResourceType::UniformBuffer, EShaderStage::FragmentShader, EShaderResourceUsageType::Static, 1 },
		{ 0, 5, "samplerUniformName", EShaderResourceType::Sampler, EShaderStage::VertexShader | EShaderStage::FragmentShader, EShaderResourceUsageType::Static, 1 },
	};
	auto descSetB = std::vector<SShaderResourceBindingDesc>{
		{ 0, 0, "texUniformName", EShaderResourceType::Texture, EShaderStage::VertexShader, EShaderResourceUsageType::Static, 1 },
		{ 0, 4, "ubUniformName", EShaderResourceType::UniformBuffer, EShaderStage::FragmentShader, EShaderResourceUsageType::Static, 1 },
		{ 0, 3, "sampler", EShaderResourceType::Sampler, EShaderStage::VertexShader | EShaderStage::FragmentShader, EShaderResourceUsageType::Static, 1 },
	};
	auto descSetC = std::vector<SShaderResourceBindingDesc>{
		{ 0, 0, "texUniformName", EShaderResourceType::Texture, EShaderStage::FragmentShader, EShaderResourceUsageType::Static, 1 },
		{ 0, 1, "nes", EShaderResourceType::UniformBuffer, EShaderStage::FragmentShader, EShaderResourceUsageType::Static, 1 },
		{ 0, 2, "snes", EShaderResourceType::Sampler, EShaderStage::VertexShader | EShaderStage::FragmentShader, EShaderResourceUsageType::Static, 1 },
	};

	auto merged = CShaderResourceSetDesc::merge(descSetA, descSetB);
	merged = CShaderResourceSetDesc::merge(merged, descSetC);

	auto mergedSetDesc = srm.GetResourceSetDesc(merged);

	ASSERT(reset.get() == reset2.get());

	CTextureView tv(dev.get(), texture);
	shub;
	SSamplerDesc smpldsc;
	CSampler sm(dev.get(), smpldsc);

	auto resrc = reset->GetResourceSetWith({ &tv, shub.get(), &sm });
	mergedSetDesc->GetResourceSetWith({ &tv, shub.get(), &sm });


	resrc->getDescriptor();

	MainPlatformLoop();
	
	return 0;
}