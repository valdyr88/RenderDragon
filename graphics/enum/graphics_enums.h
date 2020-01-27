#ifndef GRAPHICS_ENUMS_H
#define GRAPHICS_ENUMS_H

#define RD_MAX_RENDER_ATTACHMENTS 8
#define RD_MAX_VIEWPORTS RD_MAX_RENDER_ATTACHMENTS

enum class PrimitiveType {
	Points,
	Lines,
	Triangles
};

enum class PrimitiveTopology {
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

enum class BlendFactor {
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

enum class BlendOperation {
	Add,
	Subtract,
	ReverseSubtract,
	Min, //not available in GLES2
	Max
};

//TODO: implement this
enum class LogicOperation { //not available in GLES2
	NoOp
};

enum class DepthTestMode {
	None,
	Less,
	LessEqual,
	Greater,
	GreaterEqual,
};

enum class CullMode {
	None,
	FrontFaces,
	BackFaces,
	Both
};

enum class FrontFace {
	Clockwise,
	CounterClockwise,
};

enum class FillMode {
	Wireframe,
	Solid
};

enum class ComparisonOp {
	Never,
	Less,
	Equal,
	LessEqual,
	Greater,
	NotEqual,
	GreaterEqual,
	Always
};

enum class StencilOp {
	Keep,
	Zero,
	Replace,
	Inc,
	Dec,
	Invert,
	IncWrap,
	DecWrap
};

enum class TextureFormat {
	R,
	RG,
	RGB,
	RGBA
};

enum class TextureUsage{
	ShaderResource,
	ColorAttachment,
	DepthStencilAttachment,
	InputAttachment
};

enum class TextureWrapping{
	Clamp,
	Wrap,
	Mirror
};

enum class TextureFiltering{
	Nearest,
	Linear,
	Anisotropic
};

enum class LoadStoreOp {
	Load,
	Clear,
	Store,
	DontCare
};

enum class BufferType{
	Vertex,
	Index,
	Uniform,
	Staging
};

enum ShaderStage{
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
	MeshShader = 1 << 12
};

enum class ShaderResourceType{
	Texture,
	Sampler,
	CombinedTexSampler,
	UniformBuffer
};

enum class ShaderResourceUpdateType{
	Static,
	Mutable,
	Dynamic
};

#endif //GRAPHICS_ENUMS_H