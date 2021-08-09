#include "graphics/include.h"

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
	vec4 position;
	float intensity;
	float time;

	static std::vector<SUniformMap> desc;
};
std::vector<SUniformMap> LightData::desc = {
	{"position", EValueType::float32, EValueSize::vec4 },
	{"intensity", EValueType::float32, EValueSize::scalar },
	{"time", EValueType::float32, EValueSize::scalar },
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

SharedPtr<CShaderProgram> CreateMipmapShader(GPUDevice* device, SVertexFormat vertexformat){
	
	CShaderDefines defines;
	defines.add("type", "vec4");
	defines.add("components", "rgba");
	
	auto source = CShader::ReadSouceFromFile("graphics/shaders/mipmap_gen/downsamplers.ps.glsl", &defines);
	auto vsSource = CShader::ReadSouceFromFile("graphics/shaders/simple.vnt.vs.glsl", &defines);

	printContentsToFile("graphics/shaders/mipmap_gen/downsamplers.ps.glsl.processed.glsl", source.c_str(), (uint)source.length());

	SShaderDesc fsdesc(EShaderStage::FragmentShader, "downsamplers.ps.glsl", source, 
							{ {
								{ 0, 0, "tx", EShaderResourceType::Texture, EShaderStage::FragmentShader },
								{ 0, 1, "data", EShaderResourceType::UniformBuffer, EShaderStage::FragmentShader }
							} });
	SShaderDesc vsdesc(EShaderStage::VertexShader, "simple.vnt.vs.glsl", vsSource, {});
	vsdesc.vertexFormat = vertexformat;

	auto VShader = device->GetShaderModuleManager()->CreateShaderModule(vsdesc);
	auto FShader = device->GetShaderModuleManager()->CreateShaderModule(fsdesc);
	auto program = device->GetShaderProgramManager()->CreateShaderProgram("downsamplers.ps.glsl", { VShader, FShader });
	
	return program;
}

#include "graphics/ext/material/material.h"
auto testXML(GPUDevice* dev){
	auto desc = CMaterial::CreateMaterialDescFromXML(/*"  \t\r\n \
														\t<material id=\"hull_surface\" shader=\"deferred_BcNAoRSMt\">\r\n \
														\t\t<paramgroup ubstruct=\"LightData\">\r\n \
														\t\t\t<param name=\"emissionMult\" value=\"1.0\" type=\"int\"></param>\r\n \
														\t\t\t<param name=\"roughnessScaleOffsetPower\" value=\"0.2,0.8,1.0\" type=\"vec3\"></param>\r\n \
														\t\t\t<param name=\"txDiffuse_gamma_value\" value=\"2.2\" type=\"float\"></param>\r\n \
														\t\t\t<param name=\"txAoRS_gamma_value\" type=\"float\"></param>\r\n \
														\t\t\t<param name=\"txNormal_gamma_value\" value=\"1.0\" type=\"float\"></param>\r\n \
														\t\t\t<param name=\"mat2\" value=\"1.0, 0.0, 0.0, 1.0\" type=\"mat2x2\"></param>\r\n \
														\t\t\t<param name=\"mat4\" value=\"1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,\" type=\"mat4x4\"></param>\r\n \
														\t\t\t<param name=\"mat4x2\" value=\"1,2,3,4,5,6,7,8\" type=\"mat4x2\"></param>\r\n \
														\t\t\t<param name=\"mat4x3\" value=\"1,2,3,4,5,6,7,8,9,10,11,12\" type=\"mat4x3\"></param>\r\n \
														\t\t\t<param name=\"mat3x4\" value=\"1,2,3,4,5,6,7,8,9,10,11,12\" type=\"mat3x4\"></param>\r\n \
														\t\t</paramgroup>\r\n \
														\t</material>"*/
													 "\t\r\n \
														\t <material id = \"hull_surface\" shader=\"deferred_BcNAoRSMt\">\r\n \
														\t\t<paramgroup ubstruct=\"LightData\">\r\n \
														\t\t\t<param name=\"position\" value=\"1.0,2.0,3.0,0.0\" type=\"vec4\"></param>\r\n \
														\t\t\t<param name=\"intensity\" value=\"1.0\" type=\"float\"></param>\r\n \
														\t\t\t<param name=\"time\" value=\"1.0\" type=\"float\"></param>\r\n \
														\t\t</paramgroup>\r\n \
														\t\t<texture name=\"tx\" path=\"data/Textures/TLWJP_p.jpg\" type=\"AoRSMt\"></texture>\r\n \
														\t</material>"
	);

	auto manager = CSingleton<CMaterialManager>::get();
	auto material = manager->CreateMaterial(desc);
	material = manager->CreateMaterial(desc);

	auto ub = CreateUniformBuffer(dev);
	auto mi = material->CreateInstance(dev, {}, { "ublight" });

	ub = nullptr;
	return mi;
}

struct SGlobalState{
	UniquePtr<GPUDevice> device;
	UniquePtr<SWindow> window;
	SharedPtr<CMaterialInstance> materialInstance;
	SharedPtr<CUniformBuffer<LightData>> shpUniformBuffer;
	UniquePtr<CVertexBuffer> vertexBuffer;
	UniquePtr<CIndexBuffer> indexBuffer;
	SharedPtr<CSampler> sampler;
	SharedPtr<CTexture> texture;
	SharedPtr<CShaderProgram> program;
	SharedPtr<CShaderProgram> mipshader;
	SharedPtr<CRenderPass> renderPass;
	SharedPtr<CFramebuffer> framebuffer;
	SharedPtr<CPipelineState> pipeline;
	float angle = 0.0f;
	float dTime = 0.0f;
	float time = 0.0f;
	uint width = 0;
	uint height = 0;
	bool bIsInit = false;
};

#ifdef PLATFORM_EMSCRIPTEN
	typedef int iter_rtn_type;
	const iter_rtn_type iter_rtn_true = 1;
	const iter_rtn_type iter_rtn_false = 0;
#else
	typedef int iter_rtn_type;
	const iter_rtn_type iter_rtn_true = 1;
	const iter_rtn_type iter_rtn_false = 0;
#endif

iter_rtn_type one_iter(double in_time, void* in_userData)
{
	SGlobalState& gs = *((SGlobalState*)in_userData);

	auto& device = gs.device;
	auto& window = gs.window;
	auto& materialInstance = gs.materialInstance;
	auto& shpUniformBuffer = gs.shpUniformBuffer;
	auto& vertexBuffer = gs.vertexBuffer;
	auto& indexBuffer = gs.indexBuffer;
	auto& sampler = gs.sampler;
	auto& texture = gs.texture;
	auto& program = gs.program;
	auto& renderPass = gs.renderPass;
	auto& framebuffer = gs.framebuffer;
	auto& pipeline = gs.pipeline;
	auto& mipshader = gs.mipshader;
	auto& angle = gs.angle;
	auto& dTime = gs.dTime;
	auto& time = gs.time;
	dTime = in_time - time;
	time = in_time;

	if(gs.bIsInit == false)
	{
		gs.bIsInit = true;

		LOG("Init of global state");

		SGPUDeviceDesc devdesc;
		devdesc.swapchain.depthFormat = ETextureFormat::DepthStencil;
		devdesc.swapchain.width = gs.width;
		devdesc.swapchain.height = gs.height;

		device = GPUDevice::CreateGPUDevice(devdesc);

		window = UniquePtr<SWindow>(new SWindow());
		window->CreateProgramWindow("Prozor", devdesc.swapchain.width, devdesc.swapchain.height, 20, 20, window->flags, true);

		device->InitContextOnWindow(*window.get());

		materialInstance = testXML(device.get());
		//IUniformBuffer::CreateUniformBufferType["LightData"] = CUniformBuffer<LightData>::CreateUniformBuffer;
		//SharedPtr<IUniformBuffer> iub = SharedPtr<CUniformBuffer<LightData>>(new CUniformBuffer<LightData>(device.get(), "light"));

		shpUniformBuffer = CreateUniformBuffer(device.get());

		vertexBuffer = CreateVertexBufferInterleaved(device.get());
		indexBuffer = CreateIndexBuffer(device.get());

		//STextureDesc txdesc;
		//SharedPtr<CTexture> texture = NewShared<CTexture>(device.get(), txdesc, "data/Textures/TLWJP_p.jpg");

		SSamplerDesc smpldesc;
		/*smpldesc.magFilter = ETextureFiltering::Nearest;
		smpldesc.minFilter = ETextureFiltering::Nearest;
		smpldesc.mipFilter = ETextureFiltering::Nearest;*/
		smpldesc.uWrapping = ETextureWrapping::Clamp;
		smpldesc.vWrapping = ETextureWrapping::Clamp;
		smpldesc.wWrapping = ETextureWrapping::Clamp;
		sampler = NewShared<CSampler>(device.get(), smpldesc);

		//STextureViewDesc viewdesc; viewdesc = texture->getDescriptor();
		//CTextureView txView(device.get(), viewdesc, texture, sampler);

		auto txView = materialInstance->getTexture(0);
		texture = device->FindSharedPtr<CTexture>(txView->getActualTexture());

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

		//auto source = TestIncludes("data/Shaders/simple.ps.glsl");
		//auto source = transfSPIRVtoGLSL("data/Shaders/simple.ps.spv", 300, true);
		//source = globalDefines->InsertInto(source);
		auto source = CShader::ReadSouceFromFile("data/Shaders/simple.ps.glsl", globalDefines);

		printContentsToFile("data/Shaders/simple.ps.glsl.processed.wasm.glsl", source.c_str(), (uint)source.length());

		//auto vsSource = TestIncludes("data/Shaders/simple.vnt.vs.glsl");
		//auto vsSource = transfSPIRVtoGLSL("data/Shaders/simple.vnt.vs.spv", 300, true);
		//vsSource = globalDefines->InsertInto(vsSource);
		auto vsSource = CShader::ReadSouceFromFile("data/Shaders/simple.vnt.vs.glsl", globalDefines);

		SShaderDesc fsdesc(EShaderStage::FragmentShader, "simple.ps.glsl", source, { {
				{0, 0, "tx", EShaderResourceType::Texture, EShaderStage::FragmentShader},
				{0, 2, "light", EShaderResourceType::UniformBuffer, EShaderStage::FragmentShader}
			} });
		SShaderDesc vsdesc(EShaderStage::VertexShader, "simple.vntt.vs.glsl", vsSource, {});
		vsdesc.vertexFormat = vertexBuffer->getVertexFormat();

		auto VShader = CSingleton<CShaderModuleManager>::get()->CreateShaderModule(vsdesc);
		auto FShader = CSingleton<CShaderModuleManager>::get()->CreateShaderModule(fsdesc);
		program = CSingleton<CShaderProgramManager>::get()->CreateShaderProgram("simple.ps.glsl", { VShader,FShader });

		renderPass = device->GetSwapchainRenderPass();
		framebuffer = device->GetActiveSwapchainFramebuffer();

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
		pipeline = device->CreatePipelineState(psdesc);

		program->getNofStages();

		STextureFormatDesc txfmt;
		{
			txfmt.format = ETextureFormat::RGBA;
			txfmt.valueType = EValueType::uint8;
		}
		mipshader = CreateMipmapShader(device.get(), CMipMapGen::getVertexFormat());
		CMipMapGen mipmap(device.get(), mipshader, txfmt);

		mipmap.Generate(texture);
		LOG("End init of global state");
	}
	
	//one iter
	{
		LOG("Main loop");
		PlatfromLoopUpdate();
		auto shader = pipeline->getShaderProgram();
		auto txView = materialInstance->getTexture(0);
		auto& ub = *shpUniformBuffer.get();

		//----------------------------
		renderPass->Begin(framebuffer.get(), SClearColorValues(vec4(0.5f,0.7f,1.0f,1.0f)));
			pipeline->Bind();

			angle = 10.0f * time;

			ub->position = vec4(vec3(0.5f,0.5f,0.1f) + 0.1f*vec3(sinf(angle), cosf(angle),0.0f), 1.0f);
			ub->intensity = 0.01f*(cosf(angle) * 0.5f + 0.5f);
			ub->time = time;
			ub.Upload();
			//ToDo: napravit provjeru shader sourcea i resourceSetDesc. postoje li svi resoursi u shaderu, ili shader ima neke koji nema resourceSetDesc i obratno?
			shader->setUniformBuffer("light", &ub);
			shader->setTexture("tx", txView);
			//shader->setTexture(0, 0, &txView);
			//shader->setUniformBuffer(0, 2, &ub);

			device->BindVertexBuffer(vertexBuffer.get());
			device->BindIndexBuffer(indexBuffer.get());

			device->DrawIndexed();
		renderPass->End();
		//----------------------------
		device->PresentFrame();
	}

	return iter_rtn_true;
}

#ifdef PLATFORM_EMSCRIPTEN
void emsc_one_iter(void* in_userData){
	one_iter(getTime_s(), in_userData); }
#endif

SGlobalState globalState;

int main(){
	globalState.width = 512;
	globalState.height = 288;
	globalState.bIsInit = false;

#ifdef PLATFORM_EMSCRIPTEN
	emscripten_set_main_loop_arg(emsc_one_iter, (void*)&globalState, 2, 0);
#else
	while(true){
		one_iter(getTime_s(), &globalState);
		sleep_ms(1);
	}
#endif
}

//---------------------------------------------------------------------------------
