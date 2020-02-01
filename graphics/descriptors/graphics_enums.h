#ifndef GRAPHICS_ENUMS_H
#define GRAPHICS_ENUMS_H

#define RD_MAX_RENDER_ATTACHMENTS 8
#define RD_MAX_VIEWPORTS RD_MAX_RENDER_ATTACHMENTS

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

//TODO: implement this
enum class ELogicOperation { //not available in GLES2
	NoOp
};

enum class EDepthTestMode {
	None,
	Less,
	LessEqual,
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

enum class ETextureFormat {
	R,
	RG,
	RGB,
	RGBA
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

	NumStages = 12
};

enum class EShaderResourceType{
	Texture,
	Sampler,
	CombinedTexSampler,
	UniformBuffer
};

enum class EShaderResourceUpdateType{
	Static,
	Mutable,
	Dynamic
};

enum class EAttributeLayout{
	Interleaved,
	Contiguous
};

#endif //GRAPHICS_ENUMS_H