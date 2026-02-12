#ifndef GRAPHICS_ENUMS_H
#define GRAPHICS_ENUMS_H

#define RD_MAX_RENDER_ATTACHMENTS 8
#define RD_MAX_VIEWPORTS RD_MAX_RENDER_ATTACHMENTS
#define RD_MAX_VERTEX_ATTRIBS 32
#define RD_MAX_UNIFORM_BUFFER_BINDINGS 32
#define RD_MAX_UV_CHANNELS 8
#define RD_MAX_TEXTURE_BINDINGS 32

enum class EGraphicsAPI{
	Base,
	OpenGL3,
	OpenGL4,
	Vulkan,
	WebGL2
};

enum class EPrimitiveType {
	Points,
	Lines,
	Triangles
};

enum class EPrimitiveTopology {
	PointList,
	LineList,
	LineStrip,
	LineLoop,
	LineListAdjacency,
	LineStripAdjacency,
	TriangleList,
	TriangleStrip,
	TriangleFan,
	TriangleListAdjacency,
	TriangleStripAdjacency
};

enum class EBlendFactor {
	Zero,
	One,
	SrcColor,
	OneMinusSrcColor,
	DstColor,
	OneMinusDstColor,
	SrcAlpha,
	OneMinusSrcAlpha,
	DstAlpha,
	OneMinusDstAlpha,
	ConstantColor,
	OneMinusConstantColor,
	ConstantAlpha,
	OneMinusConstantAlpha,
	SrcAlphaSaturate
};

enum class EBlendOperation {
	Add,
	Subtract,
	ReverseSubtract,
	Min, //not available in GLES2
	Max
};

//ToDo: implement this
enum class ELogicOperation { //not available in GLES2
	NoOp
};

enum class EDepthTestMode {
	None,
	Less,
	LessEqual,
	Equal,
	Greater,
	GreaterEqual,
};

enum class ECullMode {
	None,
	FrontFaces,
	BackFaces,
	Both
};

enum class EFrontFace {
	Clockwise,
	CounterClockwise,
};

enum class EFillMode {
	Point,
	Wireframe,
	Solid
};

enum class EComparisonOp {
	Never,
	Less,
	Equal,
	LessEqual,
	Greater,
	NotEqual,
	GreaterEqual,
	Always
};

enum class EStencilOp {
	Keep,
	Zero,
	Replace,
	Inc,
	Dec,
	Invert,
	IncWrap,
	DecWrap
};

enum class ESwapchainBuffer{
	SingleBuffer,
	DoubleBuffer,
	TripleBuffer
};

enum class ETextureFormat {
	None,
	R,
	RG,
	RGB,
	RGBA,
	Depth,
	DepthStencil,
	RGBE,
};
inline bool isDepthFormat(const ETextureFormat& format){
	return format == ETextureFormat::Depth ||
		format == ETextureFormat::DepthStencil; }
inline bool isStencilFormat(const ETextureFormat& format){
	return format == ETextureFormat::DepthStencil; }
inline uint8 count(const ETextureFormat& format){
	switch(format)
	{
		case ETextureFormat::None: return 0;
		case ETextureFormat::R: return 1;
		case ETextureFormat::RG:return 2;
		case ETextureFormat::RGB:return 3;
		case ETextureFormat::RGBA:return 4;
		case ETextureFormat::RGBE:return 4;
		case ETextureFormat::Depth:return 1;
		case ETextureFormat::DepthStencil:return 2;
		default: break;
	}
	return 0;
}
inline const char* tostring(const ETextureFormat& format)
{
	switch(format)
	{
		case ETextureFormat::R: return "R";
		case ETextureFormat::RG:return "RG";
		case ETextureFormat::RGB:return "RGB";
		case ETextureFormat::RGBA:return "RGBA";
		case ETextureFormat::RGBE:return "RGBE";
		case ETextureFormat::Depth:return "Depth";
		case ETextureFormat::DepthStencil:return "DepthStencil";
		default: break;
	}
	return "None";
}

enum class ETypedTextureFormat {

	None = 0,
	R32G32B32A32_typeless,
	R32G32B32A32_float,
	R32G32B32A32_uint,
	R32G32B32A32_sint,
	R32G32B32_typeless,
	R32G32B32_float,
	R32G32B32_uint,
	R32G32B32_sint,
	R16G16B16A16_typeless,
	R16G16B16A16_float,
	R16G16B16A16_unorm,
	R16G16B16A16_uint,
	R16G16B16A16_snorm,
	R16G16B16A16_sint,
	R32G32_typeless,
	R32G32_float,
	R32G32_uint,
	R32G32_sint,

	R10G10B10A2_typeless,
	R10G10B10A2_unorm,
	R10G10B10A2_uint,
	R11G11B10_float,
	R8G8B8A8_typeless,
	R8G8B8A8_unorm,
	R8G8B8A8_unorm_sRGB,
	R8G8B8A8_uint,
	R8G8B8A8_snorm,
	R8G8B8A8_sint,
	R16G16_typeless,
	R16G16_float,
	R16G16_unorm,
	R16G16_uint,
	R16G16_snorm,
	R16G16_sint,
	R32_typeless,
	Depth32_float,
	R32_float,
	R32_uint,
	R32_sint,
	Depth24_unorm,

	Depth24_unorm_Stencil8_uint,

	R8G8_typeless,
	R8G8_unorm,
	R8G8_uint,
	R8G8_snorm,
	R8G8_sint,
	R16_typeless,
	R16_float,
	Depth16_unorm,
	R16_unorm,
	R16_uint,
	R16_snorm,
	R16_sint,
	R8_typeless,
	R8_unorm,
	R8_uint,
	R8_snorm,
	R8_sint,

	R9G9B9E5_sharedexp,

	BC1_typeless,
	BC1_unorm,
	BC1_unorm_sRGB,
	BC2_typeless,
	BC2_unorm,
	BC2_unorm_sRGB,
	BC3_typeless,
	BC3_unorm,
	BC3_unorm_sRGB,

	B5G6R5_unorm,
	B5G5R5A1_unorm,

	B8G8R8A8_unorm,
	B8G8R8X8_unorm,
	B8G8R8A8_typeless,
	B8G8R8X8_typeless,
};
inline ETextureFormat getFormat(const ETypedTextureFormat& format){
	switch(format)
	{
		case ETypedTextureFormat::None: return ETextureFormat::None;
		case ETypedTextureFormat::R32G32B32A32_typeless: return ETextureFormat::RGBA;
		case ETypedTextureFormat::R32G32B32A32_float: return ETextureFormat::RGBA;
		case ETypedTextureFormat::R32G32B32A32_uint: return ETextureFormat::RGBA;
		case ETypedTextureFormat::R32G32B32A32_sint: return ETextureFormat::RGBA;
		case ETypedTextureFormat::R32G32B32_typeless: return ETextureFormat::RGB;
		case ETypedTextureFormat::R32G32B32_float: return ETextureFormat::RGB;
		case ETypedTextureFormat::R32G32B32_uint: return ETextureFormat::RGB;
		case ETypedTextureFormat::R32G32B32_sint: return ETextureFormat::RGB;
		case ETypedTextureFormat::R16G16B16A16_typeless: return ETextureFormat::RGBA;
		case ETypedTextureFormat::R16G16B16A16_float: return ETextureFormat::RGBA;
		case ETypedTextureFormat::R16G16B16A16_unorm: return ETextureFormat::RGBA;
		case ETypedTextureFormat::R16G16B16A16_uint: return ETextureFormat::RGBA;
		case ETypedTextureFormat::R16G16B16A16_snorm: return ETextureFormat::RGBA;
		case ETypedTextureFormat::R16G16B16A16_sint: return ETextureFormat::RGBA;
		case ETypedTextureFormat::R32G32_typeless: return ETextureFormat::RG;
		case ETypedTextureFormat::R32G32_float: return ETextureFormat::RG;
		case ETypedTextureFormat::R32G32_uint: return ETextureFormat::RG;
		case ETypedTextureFormat::R32G32_sint: return ETextureFormat::RG;
		case ETypedTextureFormat::R10G10B10A2_typeless: return ETextureFormat::RGBA;
		case ETypedTextureFormat::R10G10B10A2_unorm: return ETextureFormat::RGBA;
		case ETypedTextureFormat::R10G10B10A2_uint: return ETextureFormat::RGBA;
		case ETypedTextureFormat::R11G11B10_float: return ETextureFormat::RGB;
		case ETypedTextureFormat::R8G8B8A8_typeless: return ETextureFormat::RGBA;
		case ETypedTextureFormat::R8G8B8A8_unorm: return ETextureFormat::RGBA;
		case ETypedTextureFormat::R8G8B8A8_unorm_sRGB: return ETextureFormat::RGBA;
		case ETypedTextureFormat::R8G8B8A8_uint: return ETextureFormat::RGBA;
		case ETypedTextureFormat::R8G8B8A8_snorm: return ETextureFormat::RGBA;
		case ETypedTextureFormat::R8G8B8A8_sint: return ETextureFormat::RGBA;
		case ETypedTextureFormat::R16G16_typeless: return ETextureFormat::RG;
		case ETypedTextureFormat::R16G16_float: return ETextureFormat::RG;
		case ETypedTextureFormat::R16G16_unorm: return ETextureFormat::RG;
		case ETypedTextureFormat::R16G16_uint: return ETextureFormat::RG;
		case ETypedTextureFormat::R16G16_snorm: return ETextureFormat::RG;
		case ETypedTextureFormat::R16G16_sint: return ETextureFormat::RG;
		case ETypedTextureFormat::R32_typeless: return ETextureFormat::R;
		case ETypedTextureFormat::Depth32_float: return ETextureFormat::Depth;
		case ETypedTextureFormat::R32_float: return ETextureFormat::R;
		case ETypedTextureFormat::R32_uint: return ETextureFormat::R;
		case ETypedTextureFormat::R32_sint: return ETextureFormat::R;
		case ETypedTextureFormat::Depth24_unorm: return ETextureFormat::Depth;
		case ETypedTextureFormat::Depth24_unorm_Stencil8_uint: return ETextureFormat::DepthStencil;
		case ETypedTextureFormat::R8G8_typeless: return ETextureFormat::RG;
		case ETypedTextureFormat::R8G8_unorm: return ETextureFormat::RG;
		case ETypedTextureFormat::R8G8_uint: return ETextureFormat::RG;
		case ETypedTextureFormat::R8G8_snorm: return ETextureFormat::RG;
		case ETypedTextureFormat::R8G8_sint: return ETextureFormat::RG;
		case ETypedTextureFormat::R16_typeless: return ETextureFormat::R;
		case ETypedTextureFormat::R16_float: return ETextureFormat::R;
		case ETypedTextureFormat::Depth16_unorm: return ETextureFormat::Depth;
		case ETypedTextureFormat::R16_unorm: return ETextureFormat::R;
		case ETypedTextureFormat::R16_uint: return ETextureFormat::R;
		case ETypedTextureFormat::R16_snorm: return ETextureFormat::R;
		case ETypedTextureFormat::R16_sint: return ETextureFormat::R;
		case ETypedTextureFormat::R8_typeless: return ETextureFormat::R;
		case ETypedTextureFormat::R8_unorm: return ETextureFormat::R;
		case ETypedTextureFormat::R8_uint: return ETextureFormat::R;
		case ETypedTextureFormat::R8_snorm: return ETextureFormat::R;
		case ETypedTextureFormat::R8_sint: return ETextureFormat::R;
		case ETypedTextureFormat::R9G9B9E5_sharedexp: return ETextureFormat::RGBE;
		case ETypedTextureFormat::BC1_typeless: return ETextureFormat::RGB;
		case ETypedTextureFormat::BC1_unorm: return ETextureFormat::RGB;
		case ETypedTextureFormat::BC1_unorm_sRGB: return ETextureFormat::RGB;
		case ETypedTextureFormat::BC2_typeless: return ETextureFormat::RGBA;
		case ETypedTextureFormat::BC2_unorm: return ETextureFormat::RGBA;
		case ETypedTextureFormat::BC2_unorm_sRGB: return ETextureFormat::RGBA;
		case ETypedTextureFormat::BC3_typeless: return ETextureFormat::RGBA;
		case ETypedTextureFormat::BC3_unorm: return ETextureFormat::RGBA;
		case ETypedTextureFormat::BC3_unorm_sRGB: return ETextureFormat::RGBA;
		case ETypedTextureFormat::B5G6R5_unorm: return ETextureFormat::RGB;
		case ETypedTextureFormat::B5G5R5A1_unorm: return ETextureFormat::RGBA;
		case ETypedTextureFormat::B8G8R8A8_unorm: return ETextureFormat::RGBA;
		case ETypedTextureFormat::B8G8R8X8_unorm: return ETextureFormat::RGBA;
		case ETypedTextureFormat::B8G8R8A8_typeless: return ETextureFormat::RGBA;
		case ETypedTextureFormat::B8G8R8X8_typeless: return ETextureFormat::RGBA;
		default: return ETextureFormat::None;
	}
	return ETextureFormat::None;
}
inline uint sizeOf(const ETypedTextureFormat& format){
	switch(format)
	{
		case ETypedTextureFormat::None: return 0;
		case ETypedTextureFormat::R32G32B32A32_typeless: return 16;
		case ETypedTextureFormat::R32G32B32A32_float: return 16;
		case ETypedTextureFormat::R32G32B32A32_uint: return 16;
		case ETypedTextureFormat::R32G32B32A32_sint: return 16;
		case ETypedTextureFormat::R32G32B32_typeless: return 12;
		case ETypedTextureFormat::R32G32B32_float: return 12;
		case ETypedTextureFormat::R32G32B32_uint: return 12;
		case ETypedTextureFormat::R32G32B32_sint: return 12;
		case ETypedTextureFormat::R16G16B16A16_typeless: return 8;
		case ETypedTextureFormat::R16G16B16A16_float: return 8;
		case ETypedTextureFormat::R16G16B16A16_unorm: return 8;
		case ETypedTextureFormat::R16G16B16A16_uint: return 8;
		case ETypedTextureFormat::R16G16B16A16_snorm: return 8;
		case ETypedTextureFormat::R16G16B16A16_sint: return 8;
		case ETypedTextureFormat::R32G32_typeless: return 8;
		case ETypedTextureFormat::R32G32_float: return 8;
		case ETypedTextureFormat::R32G32_uint: return 8;
		case ETypedTextureFormat::R32G32_sint: return 8;
		case ETypedTextureFormat::R10G10B10A2_typeless: return 4;
		case ETypedTextureFormat::R10G10B10A2_unorm: return 4;
		case ETypedTextureFormat::R10G10B10A2_uint: return 4;
		case ETypedTextureFormat::R11G11B10_float: return 4;
		case ETypedTextureFormat::R8G8B8A8_typeless: return 4;
		case ETypedTextureFormat::R8G8B8A8_unorm: return 4;
		case ETypedTextureFormat::R8G8B8A8_unorm_sRGB: return 4;
		case ETypedTextureFormat::R8G8B8A8_uint: return 4;
		case ETypedTextureFormat::R8G8B8A8_snorm: return 4;
		case ETypedTextureFormat::R8G8B8A8_sint: return 4;
		case ETypedTextureFormat::R16G16_typeless: return 4;
		case ETypedTextureFormat::R16G16_float: return 4;
		case ETypedTextureFormat::R16G16_unorm: return 4;
		case ETypedTextureFormat::R16G16_uint: return 4;
		case ETypedTextureFormat::R16G16_snorm: return 4;
		case ETypedTextureFormat::R16G16_sint: return 4;
		case ETypedTextureFormat::R32_typeless: return 4;
		case ETypedTextureFormat::Depth32_float: return 4;
		case ETypedTextureFormat::R32_float: return 4;
		case ETypedTextureFormat::R32_uint: return 4;
		case ETypedTextureFormat::R32_sint: return 4;
		case ETypedTextureFormat::Depth24_unorm: return 3;
		case ETypedTextureFormat::Depth24_unorm_Stencil8_uint: return 4;
		case ETypedTextureFormat::R8G8_typeless: return 2;
		case ETypedTextureFormat::R8G8_unorm: return 2;
		case ETypedTextureFormat::R8G8_uint: return 2;
		case ETypedTextureFormat::R8G8_snorm: return 2;
		case ETypedTextureFormat::R8G8_sint: return 2;
		case ETypedTextureFormat::R16_typeless: return 2;
		case ETypedTextureFormat::R16_float: return 2;
		case ETypedTextureFormat::Depth16_unorm: return 2;
		case ETypedTextureFormat::R16_unorm: return 2;
		case ETypedTextureFormat::R16_uint: return 2;
		case ETypedTextureFormat::R16_snorm: return 2;
		case ETypedTextureFormat::R16_sint: return 2;
		case ETypedTextureFormat::R8_typeless: return 1;
		case ETypedTextureFormat::R8_unorm: return 1;
		case ETypedTextureFormat::R8_uint: return 1;
		case ETypedTextureFormat::R8_snorm: return 1;
		case ETypedTextureFormat::R8_sint: return 1;
		case ETypedTextureFormat::R9G9B9E5_sharedexp: return 4;
		case ETypedTextureFormat::BC1_typeless: return 8;
		case ETypedTextureFormat::BC1_unorm: return 8;
		case ETypedTextureFormat::BC1_unorm_sRGB: return 8;
		case ETypedTextureFormat::BC2_typeless: return 16;
		case ETypedTextureFormat::BC2_unorm: return 16;
		case ETypedTextureFormat::BC2_unorm_sRGB: return 16;
		case ETypedTextureFormat::BC3_typeless: return 16;
		case ETypedTextureFormat::BC3_unorm: return 16;
		case ETypedTextureFormat::BC3_unorm_sRGB: return 16;
		case ETypedTextureFormat::B5G6R5_unorm: return 2;
		case ETypedTextureFormat::B5G5R5A1_unorm: return 2;
		case ETypedTextureFormat::B8G8R8A8_unorm: return 4;
		case ETypedTextureFormat::B8G8R8X8_unorm: return 4;
		case ETypedTextureFormat::B8G8R8A8_typeless: return 4;
		case ETypedTextureFormat::B8G8R8X8_typeless: return 4;
		default: return 0;
	}
	return 0;
}
inline bool isDepthFormat(const ETypedTextureFormat& format){
	return isDepthFormat(getFormat(format)); }
inline bool isStencilFormat(const ETypedTextureFormat& format){
	return isStencilFormat(getFormat(format)); }

enum class ETextureType{
	Texture1D,
	Texture2D,
	Texture3D,
	TextureCube,
	Texture1DArray,
	Texture2DArray,
	TextureCubeArray,
};

enum ETextureUsage{
	ShaderResource = 1 << 0,
	ColorAttachment = 1 << 1,
	DepthStencilAttachment = 1 << 2,
	InputAttachment = 1 << 3
};

enum class ETextureWrapping{
	Clamp,
	Wrap,
	Mirror
};

enum class ETextureFiltering{
	None,
	Nearest,
	Linear,
	Anisotropic
};

enum class ELoadStoreOp {
	Load,
	Clear,
	Store,
	DontCare
};

enum class EBufferType{
	None,
	Vertex,
	Index,
	Uniform,
	Staging
};

enum EShaderStage{
	VertexShader = 1 << 0,
	FragmentShader = 1 << 1,
	TessControlShader = 1 << 2,
	TessEvaluationShader = 1 << 3,
	GeometryShader = 1 << 4,

	ComputeShader = 1 << 5,

	RayGenerationShader = 1 << 6,
	RayIntersectionShader = 1 << 7,
	RayMissShader = 1 << 8,
	RayClosestHitShader = 1 << 9,
	RayAnyHitShader = 1 << 10,

	TaskShader = 1 << 11,
	MeshShader = 1 << 12,

	NumShaderStages = 13
};
inline uint getStageNumber(const EShaderStage t){
	auto log2i = [](uint32 i){ uint32 r = 0; while((i >>= 1) != 0) ++r; return r; };

	switch(t)
	{
		case EShaderStage::NumShaderStages: return (uint)EShaderStage::NumShaderStages;
		default:
			return (uint)log2i((uint32)t);
	}
}
inline uint32 operator|(const EShaderStage a, const EShaderStage b){
	return ((uint32)a) | ((uint32)b); }
inline uint32 operator|(const uint32 a, const EShaderStage b){
	return a | ((uint32)b); }
inline uint32 operator|(const EShaderStage a, const uint32 b){
	return ((uint32)a) | b; }
inline uint32 operator|=(uint32& a, const EShaderStage b){
	a = a | b;	return a; }

enum class EShaderResourceType{
	Texture,
	Sampler,
	CombinedTexSampler,
	UniformBuffer
};

enum class EShaderResourceUsageType{
	Static, //The data store contents will be modified once and used many times.
	Stream, //The data store contents will be modified once and used at most a few times.
	Dynamic //The data store contents will be modified repeatedlyand used many times.
};

enum class EShaderResourceAccessType{
	Read, //The data store contents are modified by reading data from the GPU, and used to return that data when queried by the application. Means the data will be read by the client's application (GPU to CPU)
	Copy, //The data store contents are modified by reading data from the GPU, and used as the source for GPU drawing and image specification commands. Means the data will be used both drawing and reading (GPU to GPU)
	Draw //The data store contents are modified by the application, and used as the source for GPU drawing and image specification commands. Means the data will be sent to GPU in order to draw (CPU to GPU)
};

enum class EAttributeLayout{
	Interleaved,
	Contiguous
};

#endif //GRAPHICS_ENUMS_H