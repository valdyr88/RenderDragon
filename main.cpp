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

int main_old(){

	SGPUDeviceDesc devdesc;
	devdesc.swapchain.depthFormat = ETextureFormat::DepthStencil;
	devdesc.swapchain.width = 1280;
	devdesc.swapchain.height = 860;

	UniquePtr<GPUDevice> dev = GPUDevice::CreateGPUDevice(devdesc);

	SWindow window;
	window.CreateProgramWindow("Prozor", 200, 200, 20, 20, window.flags, true);

	dev->InitContextOnWindow(window);
		
	SShaderResourceBindingDesc desc = {
		0, 0, "name",
		EShaderResourceType::UniformBuffer,
		0x00000000,
		EShaderResourceUsageType::Dynamic,
		0
	};

	SUniformMap m = { "time", EValueType::float32, EValueSize::scalar, 1 };
	std::vector<SUniformMap> v =
	{
		{ "color", EValueType::float32, EValueSize::vec4, 1},
		{ "time", EValueType::float32, EValueSize::scalar, 1},
		{ "intensity", EValueType::float32, EValueSize::scalar, 1}
	};
	
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
		fsdesc.bindingSetsDesc = {
			{
				{0, 1, "texUniformName", EShaderResourceType::Texture, EShaderStage::FragmentShader, EShaderResourceUsageType::Static, 1 },
				{0, 2, "ubUniformName", EShaderResourceType::UniformBuffer, EShaderStage::FragmentShader, EShaderResourceUsageType::Static, 1 },
				{0, 3, "samplerUniformName", EShaderResourceType::Sampler, EShaderStage::VertexShader | EShaderStage::FragmentShader, EShaderResourceUsageType::Static, 1 }
			} 
		};
	};

	SShaderDesc vsdesc;
	{
		vsdesc.source = "neki vertex source";
		vsdesc.stage = EShaderStage::VertexShader;
		vsdesc.bindingSetsDesc = {
			{
				{0, 0, "vertexUniform", EShaderResourceType::UniformBuffer, EShaderStage::VertexShader, EShaderResourceUsageType::Static, 1 },
			}
		};
	};

	auto fshader = SharedPtr<CShader>(new CShader(dev.get(), fsdesc));
	auto vshader = SharedPtr<CShader>(new CShader(dev.get(), vsdesc));

	auto shaderProgram = SharedPtr<CShaderProgram>(new CShaderProgram(dev.get(), "s", { vshader,fshader }));
	
	SPipelineStateDesc psdesc;
	{
		psdesc.shader = shaderProgram;
		psdesc.blendDesc.alphaToCoverageEnable = true;
	}
	auto pipeline = dev->CreatePipelineState(psdesc);
	pipeline->Bind();


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
		rpdesc.nofAttachments = 1;
		rpdesc.attachments[0].valueType = EValueType::uint8;
		rpdesc.attachments[0].format = ETextureFormat::RGBA;
		rpdesc.attachments[0].loadOp = ELoadStoreOp::Load;
		rpdesc.attachments[0].storeOp = ELoadStoreOp::Store;
		rpdesc.attachments[0].usageFlags = ETextureUsage::ColorAttachment | ETextureUsage::ShaderResource;
		rpdesc.attachments[0].sampleCount = 1;
	}

	auto framebuffer = dev->CreateFramebuffer(rpdesc, { texture });
	
	auto srm = dev->GetShaderResourceManager();
	auto reset = srm->GetResourceSetDesc({
		{0, 0, "texUniformName", EShaderResourceType::Texture, EShaderStage::FragmentShader, EShaderResourceUsageType::Static, 1 },
		{0, 1, "ubUniformName", EShaderResourceType::UniformBuffer, EShaderStage::FragmentShader, EShaderResourceUsageType::Static, 1 },
		{0, 2, "samplerUniformName", EShaderResourceType::Sampler, EShaderStage::VertexShader | EShaderStage::FragmentShader, EShaderResourceUsageType::Static, 1 },
	});

	auto reset2 = srm->GetResourceSetDesc({
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

	auto mergedSetDesc = srm->GetResourceSetDesc(merged);

	ASSERT(reset.get() == reset2.get());
	
	STextureViewDesc viewdesc; viewdesc = texture->getDescriptor();
	CTextureView tv(dev.get(), viewdesc, texture);
	SSamplerDesc smpldsc;
	CSampler sm(dev.get(), smpldsc);
	
	//shader creation test
	vsdesc.bindingSetsDesc = {
		{
			{0, 0, "A", EShaderResourceType::UniformBuffer, EShaderStage::VertexShader},
			{0, 1, "B", EShaderResourceType::Texture, EShaderStage::VertexShader},
			{0, 2, "C", EShaderResourceType::Sampler, EShaderStage::VertexShader}
		},
		{
			{1, 0, "E", EShaderResourceType::UniformBuffer, EShaderStage::VertexShader },
			{1, 1, "F", EShaderResourceType::Texture, EShaderStage::VertexShader},
			{1, 2, "G", EShaderResourceType::Sampler, EShaderStage::VertexShader}
		}
	};
	fsdesc.bindingSetsDesc = {
		{
			{0, 1, "B", EShaderResourceType::Texture, EShaderStage::FragmentShader},
			{0, 2, "C", EShaderResourceType::Sampler, EShaderStage::FragmentShader},
			{0, 3, "D", EShaderResourceType::Texture, EShaderStage::FragmentShader}
		},
		{
			{1, 3, "H", EShaderResourceType::UniformBuffer, EShaderStage::VertexShader },
			{1, 4, "I", EShaderResourceType::Texture, EShaderStage::VertexShader},
			{1, 5, "J", EShaderResourceType::Sampler, EShaderStage::VertexShader}
		}
	};

	SharedPtr<CShader> VShader = NewShared<CShader>(dev.get(), vsdesc);
	SharedPtr<CShader> FShader = NewShared<CShader>(dev.get(), fsdesc);
	
	CShaderProgram program(dev.get(), "s", {VShader, FShader});

	MainPlatformLoop();
	
	return 0;
}

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

	static std::vector<SUniformMap> desc;
};
std::vector<SUniformMap> LightData::desc = {
	{"position", EValueType::float32, EValueSize::vec3 },
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
	
	auto source300es = transfGLSLtoGLSL(source, 300, true);
	printContentsToFile("graphics/shaders/mipmap_gen/downsamplers.ps.v300es.glsl", source300es.c_str(), (uint)source300es.length());
		 source300es = transfGLSLtoGLSL(vsSource, 300, true);
	printContentsToFile("graphics/shaders/mipmap_gen/simple.vnt.vs.v300es.glsl", source300es.c_str(), (uint)source300es.length());

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
														\t\t\t<param name=\"position\" value=\"1.0,2.0,3.0\" type=\"vec3\"></param>\r\n \
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
	auto mi = material->CreateInstance(dev, {}, {"ublight"});

	ub = nullptr;
	return mi;
}

#include "graphics/ext/model/mesh.h"
auto testMeshXML(GPUDevice* dev){
	SModelDesc mdlDesc;
	mdlDesc.name = "nejm";
	mdlDesc.path = "data/Meshes/Sponza/glTF/Sponza.gltf";

	CModel mesh = CModel(dev, mdlDesc);

	return mesh;
}
#include <conio.h>

int chipschallenge_main();
#define LOG_TO_CONSOLE(format, ...)		{ printf_s(format "\n", __VA_ARGS__); }

int main_assimp_load()
{
	SGPUDeviceDesc devdesc;
	devdesc.swapchain.depthFormat = ETextureFormat::DepthStencil;
	devdesc.swapchain.width = 1600;
	devdesc.swapchain.height = 970;

	UniquePtr<GPUDevice> device = GPUDevice::CreateGPUDevice(devdesc);

	SWindow window;
	window.CreateProgramWindow("Prozor", devdesc.swapchain.width, devdesc.swapchain.height, 20, 20, window.flags, true);

	device->InitContextOnWindow(window);

	auto sponza = testMeshXML(device.get());

	auto vsubTransform = device->CreateUniformBuffer<TransformMatrices>("transform");
	auto& vsub = *vsubTransform.get();

	auto ubLight = device->CreateUniformBuffer<LightData>("light");
	auto& ub = *ubLight.get();

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

	auto source = TestIncludes("data/Shaders/simple_shading.ps.glsl");
	source = globalDefines->InsertInto(source);

	//printContentsToFile("data/Shaders/simple.ps.glsl.processed.glsl", source.c_str(), (uint)source.length());
	{
		CFile file; file.Open("data/Shaders/simple.ps.glsl.processed.glsl", CFile::EFileMode::WriteBinary);
		file.Write(source.c_str(), source.size());
		file.Close();
	}

	auto vsSource = TestIncludes("data/Shaders/simple.vntt.vs.glsl");
	vsSource = globalDefines->InsertInto(vsSource);

	SShaderDesc fsdesc(EShaderStage::FragmentShader, "simple_shading.ps.glsl", source, "data/Shaders/simple_shading.ps.glsl", {{
			{0, 1, "txDiffuse", EShaderResourceType::Texture, EShaderStage::FragmentShader},
			{0, 2, "txNormal", EShaderResourceType::Texture, EShaderStage::FragmentShader},
			{0, 3, "txAoRS", EShaderResourceType::Texture, EShaderStage::FragmentShader}
		}});
	SShaderDesc vsdesc(EShaderStage::VertexShader, "simple.vntt.vs.glsl", vsSource, "data/Shaders/simple.vntt.vs.glsl", {{
			{0, 0, "transform", EShaderResourceType::UniformBuffer, EShaderStage::VertexShader},
			{0, 4, "light", EShaderResourceType::UniformBuffer, EShaderStage::FragmentShader}
		}});

	vsdesc.vertexFormat = (*sponza.begin())->Mesh()->getVertexBuffer()->getVertexFormat();

	auto FShader = CSingleton<CShaderModuleManager>::get()->CreateShaderModule(fsdesc);
	auto VShader = CSingleton<CShaderModuleManager>::get()->CreateShaderModule(vsdesc);
	auto program = CSingleton<CShaderProgramManager>::get()->CreateShaderProgram("simple_shading.glsl", { VShader,FShader });
	
	auto renderPass = device->GetSwapchainRenderPass();
	auto framebuffer = device->GetActiveSwapchainFramebuffer();

	SPipelineStateDesc psdesc;
	{
		psdesc.shader = program;
		psdesc.blendDesc.alphaToCoverageEnable = false;
		psdesc.blendDesc.attachmentBlends[0].blendEnable = false;
		psdesc.depthDesc.enable = true;
		psdesc.depthDesc.depthFunc = EComparisonOp::Less;
		psdesc.depthDesc.depthWriteEnable = true;
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
		psdesc.rasterizerDesc.frontFace = EFrontFace::CounterClockwise;
	}
	auto pipeline = device->CreatePipelineState(psdesc);

	program->getNofStages();
	auto shader = pipeline->getShaderProgram();

	float aspect = ((float)devdesc.swapchain.width) / ((float)devdesc.swapchain.height);

	float possitionHeight = 0.2f;
	vec3 position = vec3(1.0f,0.0f,0.0f);
	vec3 lookPt = vec3(0.0f, possitionHeight,0.0f);

	float angle = 12.75f;
	float dTime = 0.01f;
	float time = 0.0f;
	float averageFPS = 30.0f;
	float averageDTime = 0.01f;

	uint64 frameNo = 0; 
	double startTime = getTime_s();
	time = getTime_s();

	bool bRotate = false;

	while(true)
	{
		PlatfromLoopUpdate();
		if(_kbhit() == true){
			char c = _getch();
			if(c == 's' || c == 'S'){
				program->Recompile();
				LOG("Recompiled shader: <%s>", program->getName())
			}
			if(c == 'r' || c == 'R')
				bRotate = !bRotate;
			if(c == 'p' || c == 'P'){
				LOG("Paused;");
				_getch();
				LOG("Resumed;");
			}
		}

		double frameStartTime = getTime_s();

		//----------------------------
		renderPass->Begin(framebuffer.get(), SClearColorValues(vec4(0.5f, 0.7f, 1.0f, 1.0f), 1.0f));
		pipeline->Bind();

		if(bRotate == true) angle = angle + 0.25f * dTime;

		position = 0.15f * vec3(cosf(angle), possitionHeight/0.15f, sinf(angle));

			vsub->projection = glm::perspective(75.0f, aspect, 0.01f, 100.0f);
			vsub->view = glm::identity<mat4>();
			vsub->view = glm::lookAt(position, lookPt, vec3(0.0f, -1.0f, 0.0f));
			vsub->world = glm::identity<mat4>();
			vsub->normal = vsub->world;
			vsub->world = glm::scale(vsub->world, vec3(0.001f));
		vsub.Upload();

			ub->position = vec3(0.5f, 0.5f, 0.1f) + 0.1f * vec3(sinf(angle), cosf(angle), 0.0f);
			ub->intensity = 0.01f * (cosf(angle) * 0.5f + 0.5f);
			ub->time = time;
		ub.Upload();

		//ToDo: napravit provjeru shader sourcea i resourceSetDesc. postoje li svi resoursi u shaderu, ili shader ima neke koji nema resourceSetDesc i obratno?
		shader->setUniformBuffer("transform", &vsub);
		shader->setUniformBuffer("light", &ub);

		for(auto meshT : sponza.AllMeshAndMaterials()){
			shader->setTexture("txDiffuse", meshT.material->getTexture(0));
			shader->setTexture("txNormal", meshT.material->getTexture(2));
			shader->setTexture("txAoRS", meshT.material->getTexture(3));

			device->BindVertexBuffer(meshT.mesh->Mesh()->getVertexBuffer());
			device->BindIndexBuffer(meshT.mesh->Mesh()->getIndexBuffer());

			device->DrawIndexed();
		}
		renderPass->End();
		//----------------------------
		float renderpassTime = getTime_s();

		device->PresentFrame();
		
		averageFPS = lerp(averageFPS, 1.0f/dTime, 0.05f);
		averageDTime = lerp(averageDTime, (float)(renderpassTime - frameStartTime), 0.05f);

		++frameNo;

		if(false&&(frameNo & 0x7f) == 0){
			system("cls");
			LOG_TO_CONSOLE("time: %f", time);
			LOG_TO_CONSOLE("FPS avg: %f", averageFPS);
			LOG_TO_CONSOLE("FPS fc/dr: %f", ((float)(frameNo)) / (time - startTime));
			LOG_TO_CONSOLE("frame no: %d", (int32)frameNo);
			LOG_TO_CONSOLE("dTime avg: %f", averageDTime);
		}
		sleep_ms(25);

		time = getTime_s();
		dTime = time - frameStartTime;
	}

	return 0;
}

int main_mipmapgen()
{
	SGPUDeviceDesc devdesc;
	devdesc.swapchain.depthFormat = ETextureFormat::DepthStencil;
	devdesc.swapchain.width = 512;
	devdesc.swapchain.height = 288;

	UniquePtr<GPUDevice> device = GPUDevice::CreateGPUDevice(devdesc);

	SWindow window;
	window.CreateProgramWindow("Prozor", devdesc.swapchain.width, devdesc.swapchain.height, 20, 20, window.flags, true);

	device->InitContextOnWindow(window);

	auto materialInstance = testXML(device.get());
	//IUniformBuffer::CreateUniformBufferType["LightData"] = CUniformBuffer<LightData>::CreateUniformBuffer;
	//SharedPtr<IUniformBuffer> iub = SharedPtr<CUniformBuffer<LightData>>(new CUniformBuffer<LightData>(device.get(), "light"));

	auto iubTyped = IUniformBuffer::CreateUniformBufferType["LightData"](device.get(), "lightub");
	if(IUniformBuffer::CreateUniformBufferType["LightData2"] != nullptr)
		auto iubTyped2 = IUniformBuffer::CreateUniformBufferType["LightData2"](device.get(), "light2ub");

	auto shpUniformBuffer = CreateUniformBuffer(device.get());
	auto& ub = *shpUniformBuffer.get();

	auto vertexBuffer = CreateVertexBufferInterleaved(device.get());
	auto indexBuffer = CreateIndexBuffer(device.get());

	//STextureDesc txdesc;
	//SharedPtr<CTexture> texture = NewShared<CTexture>(device.get(), txdesc, "data/Textures/TLWJP_p.jpg");

	SSamplerDesc smpldesc;
	/*smpldesc.magFilter = ETextureFiltering::Nearest;
	smpldesc.minFilter = ETextureFiltering::Nearest;
	smpldesc.mipFilter = ETextureFiltering::Nearest;*/
	smpldesc.uWrapping = ETextureWrapping::Clamp;
	smpldesc.vWrapping = ETextureWrapping::Clamp;
	smpldesc.wWrapping = ETextureWrapping::Clamp;
	SharedPtr<CSampler> sampler = NewShared<CSampler>(device.get(), smpldesc);
	
	//STextureViewDesc viewdesc; viewdesc = texture->getDescriptor();
	//CTextureView txView(device.get(), viewdesc, texture, sampler);

	auto txView = materialInstance->getTexture(0);
	SharedPtr<CTexture> texture = device->FindSharedPtr<CTexture>(txView->getActualTexture());

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

	auto source = TestIncludes("data/Shaders/simple.ps.glsl");
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

	STextureFormatDesc txfmt;
	{
		txfmt.format = ETextureFormat::RGBA;
		txfmt.valueType = EValueType::uint8;
	}
	auto mipshader = CreateMipmapShader(device.get(), CMipMapGen::getVertexFormat());
	CMipMapGen mipmap(device.get(), mipshader, txfmt);

	mipmap.Generate(texture);
	
	while(true)
	{
		PlatfromLoopUpdate();

		//----------------------------
		renderPass->Begin(framebuffer.get(), SClearColorValues(vec4(0.5f,0.7f,1.0f,1.0f)));
			pipeline->Bind();

			angle = 10.0f * time;

			ub->position = vec3(0.5f,0.5f,0.1f) + 0.1f*vec3(sinf(angle), cosf(angle),0.0f);
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

		time += dTime;
		sleep_ms(1); dTime = 1.0f / 1000.0f;
	}

	return 0;
}

//---------------------------------------------------------------------------------

int main(){
	//return main_assimp_load();
	return main_mipmapgen();
}