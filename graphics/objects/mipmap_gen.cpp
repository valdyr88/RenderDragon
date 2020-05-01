
#ifdef RD_API_BASE
#include "base/mipmap_gen.cpp"
#endif

#ifdef RD_API_OPENGL4
#include "opengl4/mipmap_gen.cpp"
#endif

#ifdef RD_API_WEBGL
#endif

#ifdef RD_API_VULKAN
#endif

//---------------------------------------------------------------------------------
//common code
//---------------------------------------------------------------------------------

UniquePtr<CVertexBuffer> rdCreateQuadVertexBufferInterleaved(GPUDevice* dev)
{
	SVertexFormat fmt;
	fmt.attributes = {
		{ "vertex", 0, EValueType::float32, EValueSize::vec3 },
		{ "texCoord", 2, EValueType::float32, EValueSize::vec2 }
	};
	fmt.layout = EAttributeLayout::Interleaved;

	struct vertex{
		vec3 position;
		vec2 txCoord;

		vertex(vec3 pos, vec2 tx) : position(pos), txCoord(tx){}
	};

	std::vector<vertex> vertices = { vertex(vec3(-1.0f, -1.0f, 0.01f), vec2(0.0f, 0.0f)),
									 vertex(vec3(-1.0f,  1.0f, 0.01f), vec2(0.0f, 1.0f)),
									 vertex(vec3( 1.0f,  1.0f, 0.01f), vec2(1.0f, 1.0f)),
									 vertex(vec3( 1.0f, -1.0f, 0.01f), vec2(1.0f, 0.0f)) };

	auto vertexBuffer = UniquePtr<CVertexBuffer>(new CVertexBuffer(dev, fmt, (uint)vertices.size(), { SRawData(vertices) }));

	return vertexBuffer;
}
UniquePtr<CIndexBuffer> rdCreateQuadIndexBuffer(GPUDevice* dev)
{
	std::vector<uint16> data = { 0, 1, 2, 2, 3, 0 };
	auto indexBuffer = UniquePtr<CIndexBuffer>(new CIndexBuffer(dev, EValueType::uint16, (uint)data.size(), data));
	return indexBuffer;
}

SharedPtr<CVertexBuffer> CMipMapGen::quadVB = nullptr;
SharedPtr<CIndexBuffer> CMipMapGen::quadIB = nullptr;

std::vector<SUniformMap> CMipMapGen::UBLevelData::desc = {
	{"resolution", EValueType::float32, EValueSize::vec2 },
	{"invResolution", EValueType::float32, EValueSize::vec2 },
	{"level", EValueType::int32, EValueSize::scalar }
};
rdRegisterUniformBufferStructure(CMipMapGen::UBLevelData);

bool CMipMapGen::Create(const SPipelineStateDesc& pipdesc, const SRenderPassDesc& rpdesc){
	if(device == nullptr) return false;

	auto pipedesc = pipdesc;

	renderPass = device->CreateRenderPass(rpdesc);
	pipedesc.renderPass = renderPass;
	pipeline = device->CreatePipelineState(pipedesc);
	dataUB = UniquePtr<CUniformBuffer<UBLevelData>>(new CUniformBuffer<UBLevelData>(device, "data"));

	if(CMipMapGen::quadVB == nullptr)
		CMipMapGen::quadVB = rdCreateQuadVertexBufferInterleaved(device);
	if(CMipMapGen::quadIB == nullptr)
		CMipMapGen::quadIB = rdCreateQuadIndexBuffer(device);

	return true;
}


SPipelineStateDesc CMipMapGen::CreateDefaultPipelineStateDesc(SharedPtr<CShaderProgram> program){
	SPipelineStateDesc psdesc;
	{
		psdesc.shader = program;
		psdesc.blendDesc.alphaToCoverageEnable = false;
		psdesc.blendDesc.attachmentBlends[0].blendEnable = false;
		psdesc.depthDesc.enable = false;
		psdesc.stencilDesc.enable = false;
		psdesc.primitiveTopology = EPrimitiveTopology::TriangleList;
		psdesc.renderPass = nullptr;
		psdesc.viewports.numViewports = 1;
		psdesc.viewports.dynamicViewportEnable = true;
		psdesc.viewports.scissorTest->enable = false;
		psdesc.rasterizerDesc.fillMode = EFillMode::Solid;
		psdesc.rasterizerDesc.cullMode = ECullMode::BackFaces;
		psdesc.rasterizerDesc.frontFace = EFrontFace::Clockwise;
		psdesc.samplingDesc.count = 1;
	}
	return psdesc;
}

SRenderPassDesc CMipMapGen::CreateDefaultRenderPassDesc(STextureFormatDesc format){
	SRenderPassDesc rpdesc;
	{
		rpdesc.nofAttachments = 1;
		rpdesc.attachments[0].format = format.format;
		rpdesc.attachments[0].valueType = format.valueType;
	}
	return rpdesc;
}

//---------------------------------------------------------------------------------