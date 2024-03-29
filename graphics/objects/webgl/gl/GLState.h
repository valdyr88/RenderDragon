#ifndef GLSTATE_H
#define GLSTATE_H

#ifdef RD_API_WEBGL

#include "../../../utils/log.h"
#include "../../../utils/types/types.h"
#include "../../../utils/platform_defines.h"
#include "../../../descriptors/graphics_enums.h"

#ifndef forceinline
#define forceinline __inline
#endif

#ifdef PLATFORM_EMSCRIPTEN
#include "gles3/gl3.h"
#endif
#ifdef PLATFORM_LINUX

#include <gl/glx.h>
#include <X11/X.h>
#include <X11/Xlib.h>

#endif
#ifdef PLATFORM_MAC
#endif

const char* glenumToString(GLenum e);
//#define GLState_Static_Class

#ifdef GLState_Static_Class
	#define GLState_prefix_var static
	#define GLState_prefix_func static
#else
	#define GLState_prefix_var
	#define GLState_prefix_func
#endif

class CGLState{
private:
	GLState_prefix_var bool bIsInit = false;

	GLState_prefix_var int MATRIX_MODE;
	GLState_prefix_var int PUSH_MODELVIEW_MATRIX;
	GLState_prefix_var int PUSH_PROJECTION_MATRIX;

	GLState_prefix_var int PUSH_TEXTURE_MATRIX;
	GLState_prefix_var int PUSH_ATTRIB_BITS;
	GLState_prefix_var GLuint ACTIVE_FRAMEBUFFER;
	GLState_prefix_var GLuint BIND_TEXTURE1D_ID;
	GLState_prefix_var GLuint BIND_TEXTURE2D_ID;
	GLState_prefix_var GLuint BIND_TEXTURE3D_ID;
	GLState_prefix_var GLuint ACTIVE_TEXTURE_ID;
	GLState_prefix_var GLenum DEPTH_FUNC;
	GLState_prefix_var GLenum BLEND_SRC, BLEND_DST, BLEND_SRC_ALPHA,BLEND_DST_ALPHA, BLEND_OP, BLEND_OP_ALPHA;
	GLState_prefix_var bool VERTEX_ARRAY_ENABLED,NORMAL_ARRAY_ENABLED,TEXTURE_COORD_ARRAY_ENABLED,COLOR_ARRAY_ENABLED;
	GLState_prefix_var bool LIGHTING_ENABLED,DEPTH_TEST_ENABLED, DEPTH_WRITE_ENABLED, DEPTH_CLAMP_ENABLED,ALPHA_TEST_ENABLED,STENCIL_TEST_ENABLED,BLEND_ENABLED,
		TEXTURE1D_ENABLED,TEXTURE2D_ENABLED,TEXTURE3D_ENABLED,TEXTURECUBE_ENABLED, LINE_SMOOTH_ENABLED,
		MULTISAMPLE_ENABLED,SCISSOR_TEST_ENABLED,SAMPLE_ALPHA_TO_COVERAGE_ENABLED,
		CULL_FACE_ENABLED, SAMPLE_MASK_ENABLED, SAMPLE_COVERAGE_ENABLED, 
		SAMPLE_ALPHA_TO_ONE_ENABLED, TEXTURECUBESEAMLESS_ENABLED,
		POLYGON_OFFSET_FILL_ENABLED, POLYGON_OFFSET_LINE_ENABLED, POLYGON_OFFSET_POINT_ENABLED;

	GLState_prefix_var GLvoid* pVertexBuffer,*pNormalBuffer,*pTexCoordBuffer,*pColorBuffer;
	GLState_prefix_var GLuint program_link, previous_program_link;
	GLState_prefix_var GLuint bound_vertex_array;
	GLState_prefix_var GLuint bound_read_framebuffer, bound_draw_framebuffer;

	GLState_prefix_var bool vertex_attrib_array_enabled[RD_MAX_VERTEX_ATTRIBS];
	GLState_prefix_var GLuint bound_textures[RD_MAX_TEXTURE_BINDINGS];

	class GLStateOnFuctionReturn{
		CGLState* pState; const char* FunctionName;
		void (*_pFunction)(CGLState* pState, const char* FunctionName);
	public:
		GLStateOnFuctionReturn(void(*_pFunct)(CGLState*, const char*), const char* functionname, CGLState* pstate){ FunctionName = functionname; pState = pstate; _pFunction = _pFunct; }
		~GLStateOnFuctionReturn(){ if(_pFunction){ _pFunction(pState, FunctionName); } }
	};

	PFNGLTEXSTORAGE2DPROC pglTexStorage2D = nullptr;
	PFNGLTEXSTORAGE3DPROC pglTexStorage3D = nullptr;
	PFNGLGENVERTEXARRAYSPROC pglGenVertexArrays = nullptr;

#define GLState_LogErrorOnFunctionCall
	//#define GLState_LogFunctionCalls
#define ConsolePrintErrors_PreliminaryNofChecks 4
	
#ifdef GLState_LogErrorOnFunctionCall
	#define GLS_OFR \
		 GLStateOnFuctionReturn ofr( [](CGLState* pState, const char* FunctionName){ if(pState) pState->LogErrors(FunctionName); }, __FUNCTION__, this );
#else
	#define GLS_OFR
#endif

public:
#pragma region enums

	enum EGLError : uint{
		EGL_NO_ERROR = GL_NO_ERROR,
		EGL_INVALID_VALUE = GL_INVALID_VALUE,
		EGL_INVALID_ENUM = GL_INVALID_ENUM,
		EGL_INVALID_OPERATION = GL_INVALID_OPERATION,
		EGL_INVALID_FRAMEBUFFER_OPERATION = GL_INVALID_FRAMEBUFFER_OPERATION,
		//EGL_STACK_OVERFLOW = GL_STACK_OVERFLOW,
		//EGL_STACK_UNDERFLOW = GL_STACK_UNDERFLOW,
		EGL_OUT_OF_MEMORY = GL_OUT_OF_MEMORY,
		EGL_INVALID_INDEX = ((uint)GL_INVALID_INDEX)
	};

	enum EGLTextureTargets{
		EGL_TEXTURE_NONE = -1,
		//EGL_TEXTURE_1D = GL_TEXTURE_1D,
		EGL_TEXTURE_2D_ = GL_TEXTURE_2D,
		EGL_TEXTURE_3D = GL_TEXTURE_3D,
		//EGL_TEXTURE_RECTANGLE = GL_TEXTURE_RECTANGLE,
		//EGL_TEXTURE_BUFFER = GL_TEXTURE_BUFFER,// 	Texture data buffer, definirano u EGLBufferBindingTarget
		EGL_TEXTURE_CUBE_MAP = GL_TEXTURE_CUBE_MAP,
		//EGL_TEXTURE_1D_ARRAY = GL_TEXTURE_1D_ARRAY,
		//EGL_TEXTURE_CUBE_MAP_ARRAY = GL_TEXTURE_CUBE_MAP_ARRAY,
		EGL_TEXTURE_2D_ARRAY = GL_TEXTURE_2D_ARRAY,
		//EGL_TEXTURE_2D_MULTISAMPLE = GL_TEXTURE_2D_MULTISAMPLE,
		//EGL_TEXTURE_2D_MULTISAMPLE_ARRAY = GL_TEXTURE_2D_MULTISAMPLE_ARRAY
	};

	enum EGLTextureFilters{
		EGL_NEAREST = GL_NEAREST,
		EGL_LINEAR = GL_LINEAR,
		EGL_NEAREST_MIPMAP_NEAREST = GL_NEAREST_MIPMAP_NEAREST,
		EGL_LINEAR_MIPMAP_NEAREST = GL_LINEAR_MIPMAP_NEAREST,
		EGL_NEAREST_MIPMAP_LINEAR = GL_NEAREST_MIPMAP_LINEAR,
		EGL_LINEAR_MIPMAP_LINEAR = GL_LINEAR_MIPMAP_LINEAR,
		EGL_TEXTURE_MAG_FILTER = GL_TEXTURE_MAG_FILTER,
		EGL_TEXTURE_MIN_FILTER = GL_TEXTURE_MIN_FILTER,
		EGL_TEXTURE_WRAP_S = GL_TEXTURE_WRAP_S,
		EGL_TEXTURE_WRAP_T = GL_TEXTURE_WRAP_T,
		EGL_TEXTURE_WRAP_R = GL_TEXTURE_WRAP_R,
		//EGL_CLAMP = GL_CLAMP,
		EGL_REPEAT = GL_REPEAT,
		EGL_CLAMP_TO_EDGE = GL_CLAMP_TO_EDGE,

		EGL_TEXTURE_BASE_LEVEL = GL_TEXTURE_BASE_LEVEL,
		EGL_TEXTURE_MAX_LEVEL = GL_TEXTURE_MAX_LEVEL
	};

	enum EGLTextureCubeSide{
		EGL_TEXTURE_CUBE_MAP_POSITIVE_X = GL_TEXTURE_CUBE_MAP_POSITIVE_X, 	//Right
		EGL_TEXTURE_CUBE_MAP_NEGATIVE_X = GL_TEXTURE_CUBE_MAP_NEGATIVE_X, 	//Left
		EGL_TEXTURE_CUBE_MAP_POSITIVE_Y = GL_TEXTURE_CUBE_MAP_POSITIVE_Y, 	//Top
		EGL_TEXTURE_CUBE_MAP_NEGATIVE_Y = GL_TEXTURE_CUBE_MAP_NEGATIVE_Y, 	//Bottom
		EGL_TEXTURE_CUBE_MAP_POSITIVE_Z = GL_TEXTURE_CUBE_MAP_POSITIVE_Z, 	//Back
		EGL_TEXTURE_CUBE_MAP_NEGATIVE_Z = GL_TEXTURE_CUBE_MAP_NEGATIVE_Z 	//Front
	};

	enum EGLValueType{
		EGL_VAR_NONE = -1,
		EGL_BYTE = GL_BYTE,
		EGL_UNSIGNED_BYTE = GL_UNSIGNED_BYTE,
		EGL_SHORT = GL_SHORT,
		EGL_UNSIGNED_SHORT = GL_UNSIGNED_SHORT,
		EGL_INT = GL_INT,
		EGL_UNSIGNED_INT = GL_UNSIGNED_INT,
		EGL_FLOAT = GL_FLOAT,
		//EGL_2_BYTES = GL_2_BYTES,
		//EGL_3_BYTES = GL_3_BYTES,
		//EGL_4_BYTES = GL_4_BYTES,
		//EGL_DOUBLE = GL_DOUBLE,

		//EGL_UNSIGNED_BYTE_3_3_2 = GL_UNSIGNED_BYTE_3_3_2,
		//EGL_UNSIGNED_BYTE_2_3_3_REV = GL_UNSIGNED_BYTE_2_3_3_REV,
		EGL_UNSIGNED_SHORT_5_6_5 = GL_UNSIGNED_SHORT_5_6_5,
		//EGL_UNSIGNED_SHORT_5_6_5_REV = GL_UNSIGNED_SHORT_5_6_5_REV,
		EGL_UNSIGNED_SHORT_4_4_4_4 = GL_UNSIGNED_SHORT_4_4_4_4,
		//EGL_UNSIGNED_SHORT_4_4_4_4_REV = GL_UNSIGNED_SHORT_4_4_4_4_REV,
		EGL_UNSIGNED_SHORT_5_5_5_1 = GL_UNSIGNED_SHORT_5_5_5_1,
		//EGL_UNSIGNED_SHORT_1_5_5_5_REV = GL_UNSIGNED_SHORT_1_5_5_5_REV,
		//EGL_UNSIGNED_INT_8_8_8_8 = GL_UNSIGNED_INT_8_8_8_8,
		//EGL_UNSIGNED_INT_8_8_8_8_REV = GL_UNSIGNED_INT_8_8_8_8_REV,
		//EGL_UNSIGNED_INT_10_10_10_2 = GL_UNSIGNED_INT_10_10_10_2,
		EGL_UNSIGNED_INT_2_10_10_10_REV = GL_UNSIGNED_INT_2_10_10_10_REV
	};

	enum EGLTextureInternalFormat{
		EGL_FORMAT_NONE = -1,
		EGL_DEPTH_COMPONENT = GL_DEPTH_COMPONENT, 	//Depth 	D
		EGL_DEPTH_STENCIL = GL_DEPTH_STENCIL, 	//Depth, Stencil 	D, S
		EGL_RED = GL_RED, 	//Red 	R
		EGL_RG = GL_RG, 	//Red, Green 	R, G
		EGL_RGB = GL_RGB, 	//Red, Green, Blue 	R, G, B
		EGL_RGBA = GL_RGBA, 	//Red, Green, Blue, Alpha 	R, G, B, A
		EGL_R8 = GL_R8, 	// 	GL_RED 	8
		EGL_R8_SNORM = GL_R8_SNORM, 	// 	GL_RED 	s8
		//EGL_R16 = GL_R16, 	//GL_RED 	16
		//EGL_R16_SNORM = GL_R16_SNORM, 	//GL_RED 	s16
		EGL_RG8 = GL_RG8, 	//GL_RG 	8 	8
		EGL_RG8_SNORM = GL_RG8_SNORM, 	//GL_RG 	s8 	s8
		//EGL_RG16 = GL_RG16, 	//GL_RG 	16 	16
		//EGL_RG16_SNORM = GL_RG16_SNORM, 	//GL_RG 	s16 	s16
		//EGL_R3_G3_B2 = GL_R3_G3_B2, 	//GL_RGB 	3 	3 	2
		//EGL_RGB4 = GL_RGB4, 	//GL_RGB 	4 	4 	4
		//EGL_RGB5 = GL_RGB5, 	//GL_RGB 	5 	5 	5
		EGL_RGB8 = GL_RGB8, 	//GL_RGB 	8 	8 	8
		EGL_RGB8_SNORM = GL_RGB8_SNORM, 	//GL_RGB 	s8 	s8 	s8
		//EGL_RGB10 = GL_RGB10, 	//GL_RGB 	10 	10 	10
		//EGL_RGB12 = GL_RGB12, 	//GL_RGB 	12 	12 	12
		//EGL_RGB16_SNORM = GL_RGB16_SNORM, 	//GL_RGB 	16 	16 	16
		//EGL_RGBA2 = GL_RGBA2, 	//GL_RGB 	2 	2 	2 	2
		EGL_RGBA4 = GL_RGBA4, 	//GL_RGB 	4 	4 	4 	4
		EGL_RGB5_A1 = GL_RGB5_A1, 	//GL_RGBA 	5 	5 	5 	1
		EGL_RGBA8 = GL_RGBA8, 	//GL_RGBA 	8 	8 	8 	8
		EGL_RGBA8_SNORM = GL_RGBA8_SNORM, 	//GL_RGBA 	s8 	s8 	s8 	s8
		EGL_RGB10_A2 = GL_RGB10_A2, 	//GL_RGBA 	10 	10 	10 	2
		EGL_RGB10_A2UI = GL_RGB10_A2UI, 	//GL_RGBA 	ui10 	ui10 	ui10 	ui2
		//EGL_RGBA12 = GL_RGBA12, 	//GL_RGBA 	12 	12 	12 	12
		//EGL_RGBA16 = GL_RGBA16, 	//GL_RGBA 	16 	16 	16 	16
		EGL_SRGB8 = GL_SRGB8, 	//GL_RGB 	8 	8 	8
		EGL_SRGB8_ALPHA8 = GL_SRGB8_ALPHA8, 	//GL_RGBA 	8 	8 	8 	8
		EGL_R16F = GL_R16F, 	//GL_RED 	f16
		EGL_RG16F = GL_RG16F, 	//GL_RG 	f16 	f16
		EGL_RGB16F = GL_RGB16F, 	//GL_RGB 	f16 	f16 	f16
		EGL_RGBA16F = GL_RGBA16F, 	//GL_RGBA 	f16 	f16 	f16 	f16
		EGL_R32F = GL_R32F, 	//GL_RED 	f32
		EGL_RG32F = GL_RG32F, 	//GL_RG 	f32 	f32
		EGL_RGB32F = GL_RGB32F, 	//GL_RGB 	f32 	f32 	f32
		EGL_RGBA32F = GL_RGBA32F, 	//GL_RGBA 	f32 	f32 	f32 	f32
		EGL_R11F_G11F_B10F = GL_R11F_G11F_B10F, 	//GL_RGB 	f11 	f11 	f10
		EGL_RGB9_E5 = GL_RGB9_E5, 	//GL_RGB 	9 	9 	9 	  	5
		EGL_R8I = GL_R8I, 	//GL_RED 	i8
		EGL_R8UI = GL_R8UI, 	//GL_RED 	ui8
		EGL_R16I = GL_R16I, 	//GL_RED 	i16
		EGL_R16UI = GL_R16UI, 	//GL_RED 	ui16
		EGL_R32I = GL_R32I, 	//GL_RED 	i32
		EGL_R32UI = GL_R32UI, 	//GL_RED 	ui32
		EGL_RG8I = GL_RG8I, 	//GL_RG 	i8 	i8
		EGL_RG8UI = GL_RG8UI, 	//GL_RG 	ui8 	ui8
		EGL_RG16I = GL_RG16I, 	//GL_RG 	i16 	i16
		EGL_RG16UI = GL_RG16UI, 	//GL_RG 	ui16 	ui16
		EGL_RG32I = GL_RG32I, 	//GL_RG 	i32 	i32
		EGL_RG32UI = GL_RG32UI, 	//GL_RG 	ui32 	ui32
		EGL_RGB8I = GL_RGB8I, 	//GL_RGB 	i8 	i8 	i8
		EGL_RGB8UI = GL_RGB8UI, 	//GL_RGB 	ui8 	ui8 	ui8
		EGL_RGB16I = GL_RGB16I, 	//GL_RGB 	i16 	i16 	i16
		EGL_RGB16UI = GL_RGB16UI, 	//GL_RGB 	ui16 	ui16 	ui16
		EGL_RGB32I = GL_RGB32I, 	//GL_RGB 	i32 	i32 	i32
		EGL_RGB32UI = GL_RGB32UI, 	//GL_RGB 	ui32 	ui32 	ui32
		EGL_RGBA8I = GL_RGBA8I, 	//GL_RGBA 	i8 	i8 	i8 	i8
		EGL_RGBA8UI = GL_RGBA8UI, 	//GL_RGBA 	ui8 	ui8 	ui8 	ui8
		EGL_RGBA16I = GL_RGBA16I, 	//GL_RGBA 	i16 	i16 	i16 	i16
		EGL_RGBA16UI = GL_RGBA16UI, 	//GL_RGBA 	ui16 	ui16 	ui16 	ui16
		EGL_RGBA32I = GL_RGBA32I, 	//GL_RGBA 	i32 	i32 	i32 	i32
		EGL_RGBA32UI = GL_RGBA32UI, 	//GL_RGBA 	ui32 	ui32 	ui32 	ui32
		//EGL_RGBA16_SNORM = GL_RGBA16_SNORM,
		//EGL_COMPRESSED_RED = GL_COMPRESSED_RED, 	//GL_RED 	Generic
		//EGL_COMPRESSED_RG = GL_COMPRESSED_RG, 	//GL_RG 	Generic
		//EGL_COMPRESSED_RGB = GL_COMPRESSED_RGB, 	//GL_RGB 	Generic
		//EGL_COMPRESSED_RGBA = GL_COMPRESSED_RGBA, 	//GL_RGBA 	Generic
		//EGL_COMPRESSED_SRGB = GL_COMPRESSED_SRGB, 	//GL_RGB 	Generic
		//EGL_COMPRESSED_SRGB_ALPHA = GL_COMPRESSED_SRGB_ALPHA, 	//GL_RGBA 	Generic
		//EGL_COMPRESSED_RED_RGTC1 = GL_COMPRESSED_RED_RGTC1, 	//GL_RED 	Specific
		//EGL_COMPRESSED_SIGNED_RED_RGTC1 = GL_COMPRESSED_SIGNED_RED_RGTC1, 	//GL_RED 	Specific
		//EGL_COMPRESSED_RG_RGTC2 = GL_COMPRESSED_RG_RGTC2, 	//GL_RG 	Specific
		//EGL_COMPRESSED_SIGNED_RG_RGTC2 = GL_COMPRESSED_SIGNED_RG_RGTC2, 	//GL_RG 	Specific
		//EGL_COMPRESSED_RGBA_BPTC_UNORM = GL_COMPRESSED_RGBA_BPTC_UNORM, 	//GL_RGBA 	Specific
		//EGL_COMPRESSED_SRGB_ALPHA_BPTC_UNORM = GL_COMPRESSED_SRGB_ALPHA_BPTC_UNORM, 	//GL_RGBA 	Specific
		//EGL_COMPRESSED_RGB_BPTC_SIGNED_FLOAT = GL_COMPRESSED_RGB_BPTC_SIGNED_FLOAT, 	//GL_RGB 	Specific
		//EGL_COMPRESSED_RGB_BPTC_UNSIGNED_FLOAT = GL_COMPRESSED_RGB_BPTC_UNSIGNED_FLOAT 	//GL_RGB 	Specific
	};

	enum EGLSymbolicFormat{
		syGL_SYMBOLIC_NONE = -1,
		syGL_RED = GL_RED,
		syGL_RG = GL_RG,
		syGL_RGB = GL_RGB,
		//syGL_BGR = GL_BGR,
		syGL_RGBA = GL_RGBA,
		//syGL_BGRA = GL_BGRA,
		syGL_RED_INTEGER = GL_RED_INTEGER,
		syGL_RG_INTEGER = GL_RG_INTEGER,
		syGL_RGB_INTEGER = GL_RGB_INTEGER,
		//syGL_BGR_INTEGER = GL_BGR_INTEGER,
		syGL_RGBA_INTEGER = GL_RGBA_INTEGER,
		//syGL_BGRA_INTEGER = GL_BGRA_INTEGER,
		//syGL_STENCIL_INDEX = GL_STENCIL_INDEX,
		syGL_DEPTH_COMPONENT = GL_DEPTH_COMPONENT,
		syGL_DEPTH_STENCIL = GL_DEPTH_STENCIL
	};

	enum EGLBufferUsage{
		EGL_STREAM_DRAW = GL_STREAM_DRAW,
		EGL_STREAM_READ = GL_STREAM_READ,
		EGL_STREAM_COPY = GL_STREAM_COPY,
		EGL_STATIC_DRAW = GL_STATIC_DRAW,
		EGL_STATIC_READ = GL_STATIC_READ,
		EGL_STATIC_COPY = GL_STATIC_COPY,
		EGL_DYNAMIC_DRAW = GL_DYNAMIC_DRAW,
		EGL_DYNAMIC_READ = GL_DYNAMIC_READ,
		EGL_DYNAMIC_COPY = GL_DYNAMIC_COPY
	};

	enum EGLBufferBindingTarget{
		EGL_ARRAY_BUFFER = GL_ARRAY_BUFFER,// 	Vertex attributes
		//EGL_ATOMIC_COUNTER_BUFFER = GL_ATOMIC_COUNTER_BUFFER,// 	Atomic counter storage
		EGL_COPY_READ_BUFFER = GL_COPY_READ_BUFFER,// 	Buffer copy source
		EGL_COPY_WRITE_BUFFER = GL_COPY_WRITE_BUFFER,// 	Buffer copy destination
		//EGL_DISPATCH_INDIRECT_BUFFER = GL_DISPATCH_INDIRECT_BUFFER,// 	Indirect compute dispatch commands
		//EGL_DRAW_INDIRECT_BUFFER = GL_DRAW_INDIRECT_BUFFER,// 	Indirect command arguments
		EGL_ELEMENT_ARRAY_BUFFER = GL_ELEMENT_ARRAY_BUFFER,// 	Vertex array indices (index buffer)
		EGL_PIXEL_PACK_BUFFER = GL_PIXEL_PACK_BUFFER,// 	Pixel read target
		EGL_PIXEL_UNPACK_BUFFER = GL_PIXEL_UNPACK_BUFFER,// 	Texture data source
		//EGL_QUERY_BUFFER = GL_QUERY_BUFFER,// 	Query result buffer
		//EGL_SHADER_STORAGE_BUFFER = GL_SHADER_STORAGE_BUFFER,// 	Read - write storage for shaders
		//EGL_TEXTURE_BUFFER = GL_TEXTURE_BUFFER,// 	Texture data buffer
		//EGL_TRANSFORM_FEEDBACK_BUFFER = GL_TRANSFORM_FEEDBACK_BUFFER,// 	Transform feedback buffer
		EGL_UNIFORM_BUFFER = GL_UNIFORM_BUFFER// 	Uniform block storage
	};

	enum EGLProgramInterface{

		//EGL_UNIFORM = GL_UNIFORM,
		//EGL_UNIFORM_BLOCK = GL_UNIFORM_BLOCK,
		//EGL_PROGRAM_INPUT = GL_PROGRAM_INPUT,
		//EGL_PROGRAM_OUTPUT = GL_PROGRAM_OUTPUT,

		//EGL_VERTEX_SUBROUTINE = GL_VERTEX_SUBROUTINE,
		//EGL_TESS_CONTROL_SUBROUTINE = GL_TESS_CONTROL_SUBROUTINE,
		//EGL_TESS_EVALUATION_SUBROUTINE = GL_TESS_EVALUATION_SUBROUTINE,
		//EGL_GEOMETRY_SUBROUTINE = GL_GEOMETRY_SUBROUTINE,
		//EGL_FRAGMENT_SUBROUTINE = GL_FRAGMENT_SUBROUTINE,
		//EGL_COMPUTE_SUBROUTINE = GL_COMPUTE_SUBROUTINE,

		//EGL_VERTEX_SUBROUTINE_UNIFORM = GL_VERTEX_SUBROUTINE_UNIFORM,
		//EGL_TESS_CONTROL_SUBROUTINE_UNIFORM = GL_TESS_CONTROL_SUBROUTINE_UNIFORM,
		//EGL_TESS_EVALUATION_SUBROUTINE_UNIFORM = GL_TESS_EVALUATION_SUBROUTINE_UNIFORM,
		//EGL_GEOMETRY_SUBROUTINE_UNIFORM = GL_GEOMETRY_SUBROUTINE_UNIFORM,
		//EGL_FRAGMENT_SUBROUTINE_UNIFORM = GL_FRAGMENT_SUBROUTINE_UNIFORM,
		//EGL_COMPUTE_SUBROUTINE_UNIFORM = GL_COMPUTE_SUBROUTINE_UNIFORM,

		//EGL_TRANSFORM_FEEDBACK_VARYING = GL_TRANSFORM_FEEDBACK_VARYING,
		EGL_TRANSFORM_FEEDBACK_BUFFER = GL_TRANSFORM_FEEDBACK_BUFFER,// 	Transform feedback buffer
		//EGL_BUFFER_VARIABLE = GL_BUFFER_VARIABLE,
		//EGL_SHADER_STORAGE_BLOCK = GL_SHADER_STORAGE_BLOCK//
	};

	enum EGLAccess{
		//EGL_READ_ONLY = GL_READ_ONLY,
		//EGL_WRITE_ONLY = GL_WRITE_ONLY,
		//EGL_READ_WRITE = GL_READ_WRITE
	};

	enum EGLDrawMode{
		EGL_POINTS = GL_POINTS,
		EGL_LINES = GL_LINES,
		EGL_LINE_LOOP = GL_LINE_LOOP,
		EGL_LINE_STRIP = GL_LINE_STRIP,
		//EGL_LINES_ADJACENCY = GL_LINES_ADJACENCY,
		//EGL_LINE_STRIP_ADJACENCY = GL_LINE_STRIP_ADJACENCY,
		EGL_TRIANGLES = GL_TRIANGLES,
		EGL_TRIANGLE_STRIP = GL_TRIANGLE_STRIP,
		EGL_TRIANGLE_FAN = GL_TRIANGLE_FAN,
		//EGL_TRIANGLES_ADJACENCY = GL_TRIANGLES_ADJACENCY,
		//EGL_TRIANGLES_STRIP_ADJACENCY = GL_TRIANGLE_STRIP_ADJACENCY,
		//EGL_QUADS = GL_QUADS,
		//EGL_QUAD_STRIP = GL_QUAD_STRIP
	};

	enum EGLShaderStage{
		EGL_VERTEX_SHADER = GL_VERTEX_SHADER,
		//EGL_TESS_CONTROL_SHADER = GL_TESS_CONTROL_SHADER, //how much tessellation to do
		//EGL_TESS_EVALUATION_SHADER = GL_TESS_EVALUATION_SHADER, //vertex shader for tessellated data
		//EGL_GEOMETRY_SHADER = GL_GEOMETRY_SHADER,
		EGL_FRAGMENT_SHADER = GL_FRAGMENT_SHADER,
		STAGE_NUMBER = 5
	};
	
	enum EGLShaderStatus{
		EGL_DELETE_STATUS = GL_DELETE_STATUS,
		EGL_COMPILE_STATUS = GL_COMPILE_STATUS,
		EGL_LINK_STATUS = GL_LINK_STATUS,
		EGL_VALIDATE_STATUS = GL_VALIDATE_STATUS,
		EGL_INFO_LOG_LENGTH = GL_INFO_LOG_LENGTH,
		EGL_ATTACHED_SHADERS = GL_ATTACHED_SHADERS,
		EGL_ACTIVE_UNIFORMS = GL_ACTIVE_UNIFORMS,
		EGL_ACTIVE_UNIFORM_MAX_LENGTH = GL_ACTIVE_UNIFORM_MAX_LENGTH,
		EGL_SHADER_SOURCE_LENGTH = GL_SHADER_SOURCE_LENGTH,
		EGL_ACTIVE_ATTRIBUTES = GL_ACTIVE_ATTRIBUTES,
		EGL_ACTIVE_ATTRIBUTE_MAX_LENGTH = GL_ACTIVE_ATTRIBUTE_MAX_LENGTH,
		EGL_FRAGMENT_SHADER_DERIVATIVE_HINT = GL_FRAGMENT_SHADER_DERIVATIVE_HINT,
		EGL_SHADING_LANGUAGE_VERSION = GL_SHADING_LANGUAGE_VERSION,
		EGL_CURRENT_PROGRAM = GL_CURRENT_PROGRAM
	};


	enum EGLFramebuffer{

		EGL_DRAW_FRAMEBUFFER_BINDING = GL_DRAW_FRAMEBUFFER_BINDING,
		EGL_FRAMEBUFFER_BINDING = GL_FRAMEBUFFER_BINDING,
		EGL_READ_FRAMEBUFFER = GL_READ_FRAMEBUFFER,
		EGL_DRAW_FRAMEBUFFER = GL_DRAW_FRAMEBUFFER,
		EGL_READ_FRAMEBUFFER_BINDING = GL_READ_FRAMEBUFFER_BINDING,
		EGL_FRAMEBUFFER_ATTACHMENT_OBJECT_TYPE = GL_FRAMEBUFFER_ATTACHMENT_OBJECT_TYPE,
		EGL_FRAMEBUFFER_ATTACHMENT_OBJECT_NAME = GL_FRAMEBUFFER_ATTACHMENT_OBJECT_NAME,
		EGL_FRAMEBUFFER_ATTACHMENT_TEXTURE_LEVEL = GL_FRAMEBUFFER_ATTACHMENT_TEXTURE_LEVEL,
		EGL_FRAMEBUFFER_ATTACHMENT_TEXTURE_CUBE_MAP_FACE = GL_FRAMEBUFFER_ATTACHMENT_TEXTURE_CUBE_MAP_FACE,
		EGL_FRAMEBUFFER_ATTACHMENT_TEXTURE_LAYER = GL_FRAMEBUFFER_ATTACHMENT_TEXTURE_LAYER,
		EGL_FRAMEBUFFER_COMPLETE = GL_FRAMEBUFFER_COMPLETE,
		EGL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT = GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT,
		EGL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT = GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT,
		//EGL_FRAMEBUFFER_INCOMPLETE_DRAW_BUFFER = GL_FRAMEBUFFER_INCOMPLETE_DRAW_BUFFER,
		//EGL_FRAMEBUFFER_INCOMPLETE_READ_BUFFER = GL_FRAMEBUFFER_INCOMPLETE_READ_BUFFER,
		EGL_FRAMEBUFFER_UNSUPPORTED = GL_FRAMEBUFFER_UNSUPPORTED,
		EGL_MAX_COLOR_ATTACHMENTS = GL_MAX_COLOR_ATTACHMENTS,
		EGL_COLOR_ATTACHMENT0 = GL_COLOR_ATTACHMENT0,
		EGL_COLOR_ATTACHMENT1 = GL_COLOR_ATTACHMENT1,
		EGL_COLOR_ATTACHMENT2 = GL_COLOR_ATTACHMENT2,
		EGL_COLOR_ATTACHMENT3 = GL_COLOR_ATTACHMENT3,
		EGL_COLOR_ATTACHMENT4 = GL_COLOR_ATTACHMENT4,
		EGL_COLOR_ATTACHMENT5 = GL_COLOR_ATTACHMENT5,
		EGL_COLOR_ATTACHMENT6 = GL_COLOR_ATTACHMENT6,
		EGL_COLOR_ATTACHMENT7 = GL_COLOR_ATTACHMENT7,
		EGL_COLOR_ATTACHMENT8 = GL_COLOR_ATTACHMENT8,
		EGL_COLOR_ATTACHMENT9 = GL_COLOR_ATTACHMENT9,
		EGL_COLOR_ATTACHMENT10 = GL_COLOR_ATTACHMENT10,
		EGL_COLOR_ATTACHMENT11 = GL_COLOR_ATTACHMENT11,
		EGL_COLOR_ATTACHMENT12 = GL_COLOR_ATTACHMENT12,
		EGL_COLOR_ATTACHMENT13 = GL_COLOR_ATTACHMENT13,
		EGL_COLOR_ATTACHMENT14 = GL_COLOR_ATTACHMENT14,
		EGL_COLOR_ATTACHMENT15 = GL_COLOR_ATTACHMENT15,
		EGL_DEPTH_ATTACHMENT = GL_DEPTH_ATTACHMENT,
		EGL_STENCIL_ATTACHMENT = GL_STENCIL_ATTACHMENT,
		EGL_FRAMEBUFFER = GL_FRAMEBUFFER,
		//EGL_STENCIL_INDEX1 = GL_STENCIL_INDEX1,
		//EGL_STENCIL_INDEX4 = GL_STENCIL_INDEX4,
		EGL_STENCIL_INDEX8 = GL_STENCIL_INDEX8,
		//EGL_STENCIL_INDEX16 = GL_STENCIL_INDEX16,
		EGL_FRAMEBUFFER_INCOMPLETE_MULTISAMPLE = GL_FRAMEBUFFER_INCOMPLETE_MULTISAMPLE

	};

	enum EGLRenderbuffer{

		EGL_RENDERBUFFER_BINDING = GL_RENDERBUFFER_BINDING,
		EGL_RENDERBUFFER_SAMPLES = GL_RENDERBUFFER_SAMPLES,
		EGL_RENDERBUFFER = GL_RENDERBUFFER,
		EGL_RENDERBUFFER_WIDTH = GL_RENDERBUFFER_WIDTH,
		EGL_RENDERBUFFER_HEIGHT = GL_RENDERBUFFER_HEIGHT,
		EGL_RENDERBUFFER_INTERNAL_FORMAT = GL_RENDERBUFFER_INTERNAL_FORMAT,
		EGL_RENDERBUFFER_RED_SIZE = GL_RENDERBUFFER_RED_SIZE,
		EGL_RENDERBUFFER_GREEN_SIZE = GL_RENDERBUFFER_GREEN_SIZE,
		EGL_RENDERBUFFER_BLUE_SIZE = GL_RENDERBUFFER_BLUE_SIZE,
		EGL_RENDERBUFFER_ALPHA_SIZE = GL_RENDERBUFFER_ALPHA_SIZE,
		EGL_RENDERBUFFER_DEPTH_SIZE = GL_RENDERBUFFER_DEPTH_SIZE,
		EGL_RENDERBUFFER_STENCIL_SIZE = GL_RENDERBUFFER_STENCIL_SIZE,

	};

#pragma endregion

	CGLState(){
		Init();
	}
	
	GLState_prefix_func void Init();

	GLState_prefix_func void FetchFunctionPointers();

	forceinline GLState_prefix_func GLenum GetError(){
		return glGetError();
	}
	forceinline GLState_prefix_func const char* GetStringFromError(uint error_code){
		switch(error_code){
			case GL_NO_ERROR: return "GL_NO_ERROR"; break;
			case GL_INVALID_VALUE: return "GL_INVALID_VALUE";  break;
			case GL_INVALID_ENUM: return "GL_INVALID_ENUM";  break;
			case GL_INVALID_OPERATION: return "GL_INVALID_OPERATION";  break;
			case GL_INVALID_FRAMEBUFFER_OPERATION: return "GL_INVALID_FRAMEBUFFER_OPERATION"; break;
			//case GL_STACK_OVERFLOW: return "GL_STACK_OVERFLOW";  break;
			//case GL_STACK_UNDERFLOW: return "GL_STACK_UNDERFLOW";  break;
			case GL_OUT_OF_MEMORY: return "GL_OUT_OF_MEMORY";  break;
			default: return "UNKNOWN"; break;
		}
		return "UNKNOWN";
	}
	forceinline GLState_prefix_func const char* GetStringFromError(EGLError error_code){
		uint uerror_code = (uint) error_code; return GetStringFromError(uerror_code);
	}
	forceinline GLState_prefix_func const char* GetErrorString(){
		return GetStringFromError(GetError());
	}
	forceinline GLState_prefix_func void LogErrors(const char* FunctionName = nullptr){
		#ifdef GLState_LogFunctionCalls
			if(FunctionName != nullptr)
				LOG( "%s%s\n"), FunctionName, "()") );
		#endif
		EGLError error = EGLError::EGL_NO_ERROR;

		for(uint i = 0; i < ConsolePrintErrors_PreliminaryNofChecks; ++i){
			error = (EGLError)GetError();
			if(FunctionName != nullptr && error != EGLError::EGL_NO_ERROR){ LOG( "%s%s", FunctionName, "()"); }
			if(error != EGLError::EGL_NO_ERROR){ break; }
		}

		while(error != EGLError::EGL_NO_ERROR){
			LOG("\t""GL error: %s", GetStringFromError(error)); debugbreak;
			error = (EGLError)GetError();
		}
	}

#pragma region gl functions

	/*forceinline GLState_prefix_func void GenerateTextureMipmap(GLuint texture){
		 glGenerateTextureMipmap(texture); GLS_OFR return;
	}*/
	/*forceinline GLState_prefix_func void BlitNamedFramebuffer(GLuint readFramebuffer, GLuint drawFramebuffer, GLint srcX0, GLint srcY0, GLint srcX1, GLint srcY1, GLint dstX0, GLint dstY0, GLint dstX1, GLint dstY1, GLbitfield mask, GLenum filter){
		 glBlitNamedFramebuffer(readFramebuffer, drawFramebuffer, srcX0, srcY0, srcX1, srcY1, dstX0, dstY0, dstX1, dstY1, mask, filter); GLS_OFR return;
	}*/
	forceinline GLState_prefix_func void BlitFramebuffer(GLint srcX0, GLint srcY0, GLint srcX1, GLint srcY1, GLint dstX0, GLint dstY0, GLint dstX1, GLint dstY1, GLbitfield mask, GLenum filter){
		 glBlitFramebuffer(srcX0, srcY0, srcX1, srcY1, dstX0, dstY0, dstX1, dstY1, mask, filter); GLS_OFR return;
	}
	forceinline GLState_prefix_func GLenum CheckFramebufferStatus(GLenum target){
		 auto rtn = glCheckFramebufferStatus(target); GLS_OFR return rtn;
	}
	forceinline GLState_prefix_func void FramebufferTexture2D(GLenum target, GLenum attachment, GLenum textarget, GLuint texture, GLint level){
		 glFramebufferTexture2D(target,attachment,textarget,texture,level); GLS_OFR return;
	}
	forceinline GLState_prefix_func void DetachShader(GLuint program, GLuint shader){
		 glDetachShader(program,shader); GLS_OFR return;
	}
	forceinline GLState_prefix_func void DeleteProgram(GLuint program){
		 glDeleteProgram(program); GLS_OFR return;
	}
	forceinline GLState_prefix_func void DeleteShader(GLuint shader){
		 glDeleteShader(shader); GLS_OFR return;
	}
	forceinline GLState_prefix_func void ShaderSource(GLuint shader, GLsizei count, const GLchar** strings, GLint* lengths){
		 glShaderSource(shader,count,strings,lengths); GLS_OFR return;
	}
	forceinline GLState_prefix_func void CompileShader(GLuint shader){
		 glCompileShader(shader); GLS_OFR return;
	}
	forceinline GLState_prefix_func void GetShaderiv(GLuint shader, GLenum pname, GLint* param){
		 glGetShaderiv(shader,pname,param); GLS_OFR return;
	}
	forceinline GLState_prefix_func void GetShaderInfoLog(GLuint shader, GLsizei bufSize, GLsizei* length, GLchar* infoLog){
		 glGetShaderInfoLog(shader,bufSize,length,infoLog); GLS_OFR return;
	}
	forceinline GLState_prefix_func void AttachShader(GLuint program, GLuint shader){
		 glAttachShader(program,shader); GLS_OFR return;
	}
	forceinline GLState_prefix_func void LinkProgram(GLuint program){
		 glLinkProgram(program); GLS_OFR return;
	}
	forceinline GLState_prefix_func void ValidateProgram(GLuint program){
		 glValidateProgram(program); GLS_OFR return;
	}
	forceinline GLState_prefix_func void GetProgramiv(GLuint program, GLenum pname, GLint* params){
		glGetProgramiv(program, pname, params); GLS_OFR return;
	}
	forceinline GLState_prefix_func void GetProgramInfoLog(GLuint program, GLsizei maxLength, GLsizei* length, GLchar* infoLog){
		glGetProgramInfoLog(program, maxLength, length, infoLog); GLS_OFR return;
	}
	forceinline GLState_prefix_func void UseProgram(GLuint program){
		 glUseProgram(program); GLS_OFR return;
	}
	forceinline GLState_prefix_func void EnableVertexAttribArray(GLuint index){
		 glEnableVertexAttribArray(index);
		 if(index < RD_MAX_VERTEX_ATTRIBS)
			 vertex_attrib_array_enabled[index] = true;
		 GLS_OFR return;
	}
	forceinline GLState_prefix_func void DisableVertexAttribArray(GLuint index){
		 glDisableVertexAttribArray(index);
		 if(index < RD_MAX_VERTEX_ATTRIBS)
			 vertex_attrib_array_enabled[index] = true;
		 GLS_OFR return;
	}
	forceinline GLState_prefix_func bool IsVerexAttribArrayEnabled(GLuint index){
		if(index >= RD_MAX_VERTEX_ATTRIBS) return false;
		return vertex_attrib_array_enabled[index];
	}

	forceinline GLState_prefix_func void DrawElements(GLenum mode,GLsizei count, GLenum type, void* indices){
		 glDrawElements(mode,count,type,indices); GLS_OFR return;
	}
	forceinline GLState_prefix_func void VertexAttribPointer(GLuint index, GLint size, GLenum type, GLboolean normalized, GLsizei stride, GLvoid* pointer){
		 glVertexAttribPointer(index,size,type,normalized,stride,pointer); GLS_OFR return;
	}
	forceinline GLState_prefix_func void VertexAttribIPointer(GLuint index, GLint size, GLenum type, GLsizei stride, GLvoid* pointer){
		 glVertexAttribIPointer(index, size, type, stride, pointer); GLS_OFR return;
	}
	/*forceinline GLState_prefix_func void VertexAttribLPointer(GLuint index, GLint size, GLenum type, GLsizei stride, GLvoid* pointer){
		 glVertexAttribLPointer(index, size, type, stride, pointer); GLS_OFR return;
	}*/
	/*forceinline GLState_prefix_func void VertexAttribFormat(GLuint index, GLint size, GLenum type, GLboolean normalized, GLuint relativeoffset){
		glVertexAttribFormat(index,size,type,normalized,relativeoffset); GLS_OFR return;
	}*/
	/*forceinline GLState_prefix_func void VertexAttribIFormat(GLuint index, GLint size, GLenum type, GLuint relativeoffset){
		glVertexAttribIFormat(index,size,type,relativeoffset); GLS_OFR return;
	}*/
	/*forceinline GLState_prefix_func void VertexAttribLFormat(GLuint index, GLint size, GLenum type, GLuint relativeoffset){
		glVertexAttribLFormat(index,size,type,relativeoffset); GLS_OFR return;
	}*/
	/*forceinline GLState_prefix_func void VertexAttribBinding(GLuint attribindex, GLint bindingindex){
		glVertexAttribBinding(attribindex,bindingindex); GLS_OFR return;
	}*/
	/*forceinline GLState_prefix_func void BindVertexBuffer(GLuint bindingindex, GLuint buffer, GLintptr offset, GLsizei stride){
		glBindVertexBuffer(bindingindex, buffer, offset, stride); GLS_OFR return;
	}*/

	forceinline GLState_prefix_func void VertexAttrib1f(GLuint index, GLfloat x){
		glVertexAttrib1f(index, x); GLS_OFR return;
	}
	forceinline GLState_prefix_func void VertexAttrib1fv(GLuint index, const GLfloat* x){
		glVertexAttrib1fv(index, x); GLS_OFR return;
	}
	forceinline GLState_prefix_func void VertexAttrib2f(GLuint index, GLfloat x, GLfloat y){
		glVertexAttrib2f(index, x, y); GLS_OFR return;
	}
	forceinline GLState_prefix_func void VertexAttrib2fv(GLuint index, const GLfloat* v){
		glVertexAttrib2fv(index, v); GLS_OFR return;
	}
	forceinline GLState_prefix_func void VertexAttrib3f(GLuint index, GLfloat x, GLfloat y, GLfloat z){
		glVertexAttrib3f(index, x, y, z); GLS_OFR return;
	}
	forceinline GLState_prefix_func void VertexAttrib3fv(GLuint index, const GLfloat* v){
		glVertexAttrib3fv(index, v); GLS_OFR return;
	}
	forceinline GLState_prefix_func void VertexAttrib4f(GLuint index, GLfloat x, GLfloat y, GLfloat z, GLfloat w){
		glVertexAttrib4f(index, x, y, z, w); GLS_OFR return;
	}
	forceinline GLState_prefix_func void VertexAttrib4fv(GLuint index, const GLfloat* v){
		glVertexAttrib4fv(index, v); GLS_OFR return;
	}
	forceinline GLState_prefix_func void VertexAttribI4i(GLuint index, GLint v0, GLint v1, GLint v2, GLint v3){
		glVertexAttribI4i(index, v0, v1, v2, v3); GLS_OFR return;
	}
	forceinline GLState_prefix_func void VertexAttribI4ui(GLuint index, GLuint v0, GLuint v1, GLuint v2, GLuint v3){
		glVertexAttribI4ui(index, v0, v1, v2, v3); GLS_OFR return;
	}

	forceinline GLState_prefix_func void Flush(){
		 glFlush(); GLS_OFR return;
	}
	forceinline GLState_prefix_func void Clear(GLbitfield mask){
		 glClear(mask); GLS_OFR return;
	}
	forceinline GLState_prefix_func void ClearColor(GLclampf r, GLclampf g, GLclampf b, GLclampf a){
		 glClearColor(r,g,b,a); GLS_OFR return;
	}
	forceinline GLState_prefix_func void ClearDepthf(GLfloat depth){
		 glClearDepthf(depth); GLS_OFR return;
	}
	forceinline GLState_prefix_func void ClearStencil(GLint stencil){
		glClearStencil(stencil); GLS_OFR return;
	}
	forceinline GLState_prefix_func void ClearBufferiv(GLenum buffer, GLint drawbuffer, const GLint* value){
		glClearBufferiv(buffer, drawbuffer, value); GLS_OFR return;
	}
	forceinline GLState_prefix_func void ClearBufferuiv(GLenum buffer, GLint drawbuffer, const GLuint* value){
		glClearBufferuiv(buffer, drawbuffer, value); GLS_OFR return;
	}
	forceinline GLState_prefix_func void ClearBufferfv(GLenum buffer, GLint drawbuffer, const GLfloat* value){
		glClearBufferfv(buffer, drawbuffer, value); GLS_OFR return;
	}
	forceinline GLState_prefix_func void ClearBufferfi(GLenum buffer, GLint drawbuffer, GLfloat depth, GLint stencil){
		glClearBufferfi(buffer, drawbuffer, depth, stencil); GLS_OFR return;
	}
	forceinline GLState_prefix_func void Viewport(GLint x, GLint y, GLsizei width, GLsizei height){
		 glViewport(x,y,width,height); GLS_OFR return;
	}
	forceinline GLState_prefix_func void Scissor(GLint x, GLint y, GLsizei width, GLsizei height){
		glScissor(x, y, width, height); GLS_OFR return;
	}
	forceinline GLState_prefix_func void Viewport(GLfloat x, GLfloat y, GLfloat width, GLfloat height){
		glViewport((GLint)x, (GLint)y, (GLsizei)width, (GLsizei)height); GLS_OFR return;
	}
	forceinline GLState_prefix_func void Scissor(GLfloat x, GLfloat y, GLfloat width, GLfloat height){
		glScissor((GLint)x, (GLint)y, (GLsizei)width, (GLsizei)height); GLS_OFR return;
	}
	/*forceinline GLState_prefix_func void PointSize(GLfloat size){
		 glPointSize(size); GLS_OFR return;
	}*/
	forceinline GLState_prefix_func void LineWidth(GLfloat width){
		 glLineWidth(width); GLS_OFR return;
	}
	forceinline GLState_prefix_func void Hint(GLenum target, GLenum mode){
		 glHint(target,mode); GLS_OFR return;
	}
	forceinline GLState_prefix_func void CullFace(GLenum mode){
		 glCullFace(mode); GLS_OFR return;
	}
	forceinline GLState_prefix_func void FrontFace(GLenum mode){
		 glFrontFace(mode); GLS_OFR return;
	}
	/*forceinline GLState_prefix_func void PushAttrib(int bits){
		 glPushAttrib(bits); ++PUSH_ATTRIB_BITS; GLS_OFR return;
	}*/
	/*forceinline GLState_prefix_func void PopAttrib(){
		{  glPopAttrib(); --PUSH_ATTRIB_BITS; } GLS_OFR return;
	}*/
	/*forceinline GLState_prefix_func void SampleMaski(GLuint index, GLbitfield mask){
		glSampleMaski(index, mask); GLS_OFR return;
	}*/
	/*forceinline GLState_prefix_func void SampleMask(GLbitfield mask){
		for(uint i = 0; i < RD_MAX_RENDER_ATTACHMENTS; ++i){ glSampleMaski(i, mask); GLS_OFR } return;
	}*/
	forceinline GLState_prefix_func void DepthWriteEnable(){
		glDepthMask(GL_TRUE); DEPTH_WRITE_ENABLED = true; GLS_OFR; return;
	}
	forceinline GLState_prefix_func void DepthWriteDisable(){
		glDepthMask(GL_FALSE); DEPTH_WRITE_ENABLED = false; GLS_OFR; return;
	}
	forceinline GLState_prefix_func void DepthMask(bool enabled){
		glDepthMask(enabled); DEPTH_WRITE_ENABLED = enabled; GLS_OFR; return;
	}
	forceinline GLState_prefix_func void EnableDepthTest(){
		 glEnable(GL_DEPTH_TEST); DEPTH_TEST_ENABLED = true; GLS_OFR return;
	}
	forceinline GLState_prefix_func void DisableDepthTest(){
		{  glDisable(GL_DEPTH_TEST); DEPTH_TEST_ENABLED = false; } GLS_OFR return;
	}
	forceinline GLState_prefix_func void EnableStencilTest(){
		glEnable(GL_STENCIL_TEST); STENCIL_TEST_ENABLED = true; GLS_OFR return;
	}
	forceinline GLState_prefix_func void DisableStencilTest(){
		{ glDisable(GL_STENCIL_TEST); STENCIL_TEST_ENABLED = false; } GLS_OFR return;
	}
	forceinline GLState_prefix_func void EnableBlend(){
		 glEnable(GL_BLEND); BLEND_ENABLED = true; GLS_OFR return;
	}
	forceinline GLState_prefix_func void DisableBlend(){
		{  glDisable(GL_BLEND); BLEND_ENABLED = false; } GLS_OFR return;
	}
	/*forceinline GLState_prefix_func void EnableTexture1D(){
		 glEnable(GL_TEXTURE_1D); TEXTURE1D_ENABLED = true; GLS_OFR return;
	}*/
	/*forceinline GLState_prefix_func void DisableTexture1D(){
		{  glDisable(GL_TEXTURE_1D); TEXTURE1D_ENABLED = false; } GLS_OFR return;
	}*/
	forceinline GLState_prefix_func void EnableTexture2D(){
		 glEnable(GL_TEXTURE_2D); TEXTURE2D_ENABLED = true; GLS_OFR return;
	}
	forceinline GLState_prefix_func void DisableTexture2D(){ return;
		{  glDisable(GL_TEXTURE_2D); TEXTURE2D_ENABLED = false; } GLS_OFR return;
	}
	forceinline GLState_prefix_func void EnableTexture3D(){
		 glEnable(GL_TEXTURE_3D); TEXTURE3D_ENABLED = true; GLS_OFR return;
	}
	forceinline GLState_prefix_func void DisableTexture3D(){
		{  glDisable(GL_TEXTURE_3D); TEXTURE3D_ENABLED = false; } GLS_OFR return;
	}
	forceinline GLState_prefix_func void EnableTextureCube(){
		glEnable(GL_TEXTURE_CUBE_MAP); TEXTURECUBE_ENABLED = true; GLS_OFR return;
	}
	forceinline GLState_prefix_func void DisableTextureCube(){
		{ glDisable(GL_TEXTURE_CUBE_MAP); TEXTURECUBE_ENABLED = false; } GLS_OFR return;
	}
	/*forceinline GLState_prefix_func void EnableTextureCubeSeamless(){
		glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS); TEXTURECUBESEAMLESS_ENABLED = true; GLS_OFR return;
	}*/
	/*forceinline GLState_prefix_func void DisableTextureCubeSeamless(){
		{ glDisable(GL_TEXTURE_CUBE_MAP_SEAMLESS); TEXTURECUBESEAMLESS_ENABLED = false; } GLS_OFR return;
	}*/
	/*forceinline GLState_prefix_func void EnableAlphaTest(){
		glEnable(GL_ALPHA_TEST); ALPHA_TEST_ENABLED = true; GLS_OFR return;
	}*/
	/*forceinline GLState_prefix_func void DisableAlphaTest(){
		{ glDisable(GL_ALPHA_TEST); ALPHA_TEST_ENABLED = false; } GLS_OFR return;
	}*/
	forceinline GLState_prefix_func void EnableCullFace(){
		glEnable(GL_CULL_FACE); CULL_FACE_ENABLED = true; GLS_OFR return;
	}
	forceinline GLState_prefix_func void DisableCullFace(){
		{ glDisable(GL_CULL_FACE); CULL_FACE_ENABLED = false; } GLS_OFR return;
	}
	forceinline GLState_prefix_func void EnableSampleAlphaToCoverage(){
		glEnable(GL_SAMPLE_ALPHA_TO_COVERAGE); SAMPLE_ALPHA_TO_COVERAGE_ENABLED = true; GLS_OFR return;
	}
	forceinline GLState_prefix_func void DisableSampleAlphaToCoverage(){
		{ glDisable(GL_SAMPLE_ALPHA_TO_COVERAGE); SAMPLE_ALPHA_TO_COVERAGE_ENABLED = false; } GLS_OFR return;
	}
	forceinline GLState_prefix_func void EnableScissorTest(){
		glEnable(GL_SCISSOR_TEST); SCISSOR_TEST_ENABLED = true; GLS_OFR return;
	}
	forceinline GLState_prefix_func void DisableScissorTest(){
		{ glDisable(GL_SCISSOR_TEST); SCISSOR_TEST_ENABLED = false; } GLS_OFR return;
	}
	/*forceinline GLState_prefix_func void EnableMultisample(){
		glEnable(GL_MULTISAMPLE); MULTISAMPLE_ENABLED = true; GLS_OFR return;
	}*/
	/*forceinline GLState_prefix_func void DisableMultisample(){
		{ glDisable(GL_MULTISAMPLE); MULTISAMPLE_ENABLED = false; } GLS_OFR return;
	}*/
	/*forceinline GLState_prefix_func void EnableSampleMask(){
		glEnable(GL_SAMPLE_MASK); SAMPLE_MASK_ENABLED = true; GLS_OFR return;
	}*/
	/*forceinline GLState_prefix_func void DisableSampleMask(){
		{ glDisable(GL_SAMPLE_MASK); SAMPLE_MASK_ENABLED = false; } GLS_OFR return;
	}*/
	forceinline GLState_prefix_func void EnableSampleCoverage(){
		glEnable(GL_SAMPLE_COVERAGE); SAMPLE_COVERAGE_ENABLED = true; GLS_OFR return;
	}
	forceinline GLState_prefix_func void DisableSampleCoverage(){
		{ glDisable(GL_SAMPLE_COVERAGE); SAMPLE_COVERAGE_ENABLED = false; } GLS_OFR return;
	}
	/*forceinline GLState_prefix_func void EnableSampleAlphaToOne(){
		glEnable(GL_SAMPLE_ALPHA_TO_ONE); SAMPLE_ALPHA_TO_ONE_ENABLED = true; GLS_OFR return;
	}*/
	/*forceinline GLState_prefix_func void DisableSampleAlphaToOne(){
		{ glDisable(GL_SAMPLE_ALPHA_TO_ONE); SAMPLE_ALPHA_TO_ONE_ENABLED = false; } GLS_OFR return;
	}*/
	/*forceinline GLState_prefix_func void EnableDepthClamp(){
		glEnable(GL_DEPTH_CLAMP); DEPTH_CLAMP_ENABLED = true; GLS_OFR return;
	}*/
	/*forceinline GLState_prefix_func void DisableDepthClamp(){
		{ glDisable(GL_DEPTH_CLAMP); DEPTH_CLAMP_ENABLED = false; } GLS_OFR return;
	}*/
	/*forceinline GLState_prefix_func void EnableLineSmooth(){
		glEnable(GL_LINE_SMOOTH); LINE_SMOOTH_ENABLED = true; GLS_OFR return;
	}*/
	/*forceinline GLState_prefix_func void DisableLineSmooth(){
		{ glDisable(GL_LINE_SMOOTH); LINE_SMOOTH_ENABLED = false; } GLS_OFR return;
	}*/
	forceinline GLState_prefix_func void EnablePolygonOffsetFill(){
		glEnable(GL_POLYGON_OFFSET_FILL); POLYGON_OFFSET_FILL_ENABLED = true; GLS_OFR return;
	}
	forceinline GLState_prefix_func void DisablePolygonOffsetFill(){
		{ glDisable(GL_POLYGON_OFFSET_FILL); POLYGON_OFFSET_FILL_ENABLED = false; } GLS_OFR return;
	}
	/*forceinline GLState_prefix_func void EnablePolygonOffsetLine(){
		glEnable(GL_POLYGON_OFFSET_LINE); POLYGON_OFFSET_LINE_ENABLED = true; GLS_OFR return;
	}*/
	/*forceinline GLState_prefix_func void DisablePolygonOffsetLine(){
		{ glDisable(GL_POLYGON_OFFSET_LINE); POLYGON_OFFSET_LINE_ENABLED = false; } GLS_OFR return;
	}*/
	/*forceinline GLState_prefix_func void EnablePolygonOffsetPoint(){
		glEnable(GL_POLYGON_OFFSET_POINT); POLYGON_OFFSET_POINT_ENABLED = true; GLS_OFR return;
	}*/
	/*forceinline GLState_prefix_func void DisablePolygonOffsetPoint(){
		{ glDisable(GL_POLYGON_OFFSET_POINT); POLYGON_OFFSET_POINT_ENABLED = false; } GLS_OFR return;
	}*/


	forceinline GLState_prefix_func void Enable(GLenum cap){
		switch(cap){
			//case GL_TEXTURE_1D: return EnableTexture1D();
			case GL_TEXTURE_2D: return EnableTexture2D();
			case GL_TEXTURE_3D: return EnableTexture3D();
			case GL_TEXTURE_CUBE_MAP: return EnableTextureCube();
			case GL_BLEND: return EnableBlend();
			case GL_DEPTH_TEST: return EnableDepthTest();
			case GL_STENCIL_TEST: return EnableStencilTest();
			//case GL_ALPHA_TEST: return EnableAlphaTest();
			case GL_CULL_FACE: return EnableCullFace();
			case GL_SCISSOR_TEST: return EnableScissorTest();
			//case GL_MULTISAMPLE: return EnableMultisample();
			//case GL_TEXTURE_CUBE_MAP_SEAMLESS: return EnableTextureCubeSeamless();
			//case GL_SAMPLE_MASK: return EnableSampleMask();
			case GL_SAMPLE_COVERAGE: return EnableSampleCoverage();
			//case GL_SAMPLE_ALPHA_TO_ONE: return EnableSampleAlphaToOne();
			case GL_SAMPLE_ALPHA_TO_COVERAGE: return EnableSampleAlphaToCoverage();
			//case GL_DEPTH_CLAMP: return EnableDepthClamp();
			//case GL_LINE_SMOOTH: return EnableLineSmooth();
			case GL_POLYGON_OFFSET_FILL: return EnablePolygonOffsetFill();
			//case GL_POLYGON_OFFSET_LINE: return EnablePolygonOffsetLine();
			//case GL_POLYGON_OFFSET_POINT: return EnablePolygonOffsetPoint();
		}
		return;
	}
	forceinline GLState_prefix_func void Disable(GLenum cap){
		switch(cap){
			//case GL_TEXTURE_1D: return DisableTexture1D();
			case GL_TEXTURE_2D: return DisableTexture2D();
			case GL_TEXTURE_3D: return DisableTexture3D();
			case GL_TEXTURE_CUBE_MAP: return DisableTextureCube();
			case GL_BLEND: return DisableBlend();
			case GL_DEPTH_TEST: return DisableDepthTest();
			case GL_STENCIL_TEST: return DisableStencilTest();
			//case GL_ALPHA_TEST: return DisableAlphaTest();
			case GL_CULL_FACE: return DisableCullFace();
			case GL_SCISSOR_TEST: return DisableScissorTest();
			//case GL_MULTISAMPLE: return DisableMultisample();
			//case GL_TEXTURE_CUBE_MAP_SEAMLESS: return DisableTextureCubeSeamless();
			//case GL_SAMPLE_MASK: return DisableSampleMask();
			case GL_SAMPLE_COVERAGE: return DisableSampleCoverage();
			//case GL_SAMPLE_ALPHA_TO_ONE: return DisableSampleAlphaToOne();
			case GL_SAMPLE_ALPHA_TO_COVERAGE: return DisableSampleAlphaToCoverage();
			//case GL_DEPTH_CLAMP: return DisableDepthClamp();
			//case GL_LINE_SMOOTH: return DisableLineSmooth();
			case GL_POLYGON_OFFSET_FILL: return DisablePolygonOffsetFill();
			//case GL_POLYGON_OFFSET_LINE: return DisablePolygonOffsetLine();
			//case GL_POLYGON_OFFSET_POINT: return DisablePolygonOffsetPoint();
		}
		return;
	}
	

	/*forceinline GLState_prefix_func void EnableBlend(GLuint i){
		glEnablei(i, GL_BLEND); BLEND_ENABLED = true; GLS_OFR return;
	}*/
	/*forceinline GLState_prefix_func void DisableBlend(GLuint i){
		{ glDisablei(i, GL_BLEND); BLEND_ENABLED = false; } GLS_OFR return;
	}*/
	/*forceinline GLState_prefix_func void EnableScissorTest(GLuint i){
		glEnablei(i, GL_SCISSOR_TEST); SCISSOR_TEST_ENABLED = true; GLS_OFR return;
	}*/
	/*forceinline GLState_prefix_func void DisableScissorTest(GLuint i){
		{ glDisablei(i, GL_SCISSOR_TEST); SCISSOR_TEST_ENABLED = false; } GLS_OFR return;
	}*/

	/*forceinline GLState_prefix_func void Enablei(GLuint i, GLenum cap){
		switch(cap){
			case GL_BLEND: return EnableBlend(i);
			case GL_SCISSOR_TEST: return EnableScissorTest(i);
			default: LOG_WARN("glEnablei(%d): not an indexed enum! <%d>", i, cap);
		}
		return;
	}*/
	/*forceinline GLState_prefix_func void Disablei(GLuint i, GLenum cap){
		switch(cap){
			case GL_BLEND: return DisableBlend(i);
			case GL_SCISSOR_TEST: return DisableScissorTest(i);
			default: LOG_WARN("glDisablei(%d): not an indexed enum! <%d>", i, cap);
		}
		return;
	}*/

	forceinline GLState_prefix_func void BlendFunc(GLenum src, GLenum dst){
		 glBlendFunc(src, dst); BLEND_SRC = src; BLEND_DST = dst; BLEND_SRC_ALPHA = src; BLEND_DST_ALPHA = dst; GLS_OFR return;
	}
	forceinline GLState_prefix_func void BlendFuncSeparate(GLenum src, GLenum dst, GLenum srcA, GLenum dstA){
		glBlendFuncSeparate(src, dst, srcA, dstA); BLEND_SRC = src; BLEND_DST = dst; BLEND_SRC_ALPHA = srcA; BLEND_DST_ALPHA = dstA; GLS_OFR return;
	}
	/*forceinline GLState_prefix_func void BlendFuncSeparatei(GLenum buf, GLenum src, GLenum dst, GLenum srcA, GLenum dstA){
		glBlendFuncSeparatei(buf, src, dst, srcA, dstA); BLEND_SRC = src; BLEND_DST = dst; BLEND_SRC_ALPHA = srcA; BLEND_DST_ALPHA = dstA; GLS_OFR return;
	}*/
	forceinline GLState_prefix_func void BlendEquation(GLenum op){
		glBlendEquation(BLEND_OP); BLEND_OP = op; GLS_OFR return;
	}
	forceinline GLState_prefix_func void BlendEquationSeparate(GLenum op, GLenum opA){
		glBlendEquationSeparate(op, opA); BLEND_OP = op; BLEND_OP_ALPHA = opA; GLS_OFR return;
	}
	/*forceinline GLState_prefix_func void BlendEquationSeparatei(GLenum buf, GLenum op, GLenum opA){
		glBlendEquationSeparatei(buf, op, opA); GLS_OFR return;
	}*/

	forceinline GLState_prefix_func void DepthFunc(GLenum func){
		glDepthFunc(func); DEPTH_FUNC = func; GLS_OFR return;
	}
	
	forceinline GLState_prefix_func void StencilFuncSeparate(GLenum face, GLenum func, uint ref, uint mask){
		glStencilFuncSeparate(face, func, ref, mask); GLS_OFR return;
	}
	forceinline GLState_prefix_func void StencilOpSeparate(GLenum face, GLenum sfail, GLenum dpfail, GLenum dppass){
		glStencilOpSeparate(face, sfail, dpfail, dppass); GLS_OFR return;
	}
	forceinline GLState_prefix_func void StencilMaskSeparate(GLenum face, uint mask){
		glStencilMaskSeparate(face, mask); GLS_OFR return;
	}

	/*forceinline GLState_prefix_func void PolygonMode(GLenum face, GLenum mode){
		glPolygonMode(face, mode); GLS_OFR return;
	}*/
	forceinline GLState_prefix_func void PolygonOffset(GLfloat factor, GLfloat units){
		glPolygonOffset(factor, units); GLS_OFR return;
	}

	/*forceinline GLState_prefix_func void ViewportIndexedf(GLuint i, GLfloat x, GLfloat y, GLfloat w, GLfloat h){
		glViewportIndexedf(i, x, y, w, h); GLS_OFR return;
	}*/
	/*forceinline GLState_prefix_func void ScissorIndexed(GLuint i, GLint x, GLint y, GLsizei w, GLsizei h){
		glScissorIndexed(i, x, y, w, h); GLS_OFR return;
	}*/
	/*forceinline GLState_prefix_func void ScissorIndexed(GLuint i, GLfloat x, GLfloat y, GLfloat w, GLfloat h){
		glScissorIndexed(i, (GLint)x, (GLint)y, (GLsizei)w, (GLsizei)h); GLS_OFR return;
	}*/

	/*forceinline GLState_prefix_func GLvoid* MapBuffer(GLenum target, GLenum access){
		 auto rtn = glMapBuffer(target,access); GLS_OFR return rtn;
	}*/
	forceinline GLState_prefix_func GLvoid* MapBufferRange(GLenum target, GLintptr offset, GLsizei length, GLenum access){
		 auto rtn = glMapBufferRange(target,offset,length,access); GLS_OFR return rtn;
	}
	/*forceinline GLState_prefix_func GLvoid* MapNamedBuffer(GLuint buffer, GLenum access){
		 auto rtn = glMapNamedBuffer(buffer,access); GLS_OFR return rtn;
	}*/
	forceinline GLState_prefix_func GLboolean UnmapBuffer(GLenum target){
		 auto rtn = glUnmapBuffer(target); GLS_OFR return rtn;
	}
	/*forceinline GLState_prefix_func GLboolean UnmapNamedBuffer(GLuint buffer){
		 auto rtn = glUnmapNamedBuffer(buffer); GLS_OFR return rtn;
	}*/
	forceinline GLState_prefix_func void BindBuffer(GLenum target, GLuint buffer){
		 glBindBuffer(target,buffer); GLS_OFR return;
	}
	forceinline GLState_prefix_func void BindAttribLocation(GLuint program, GLuint index, const GLchar* name){
		 glBindAttribLocation(program,index,name); GLS_OFR return;
	}
	/*forceinline GLState_prefix_func void BindAttribLocationARB(GLhandleARB programObj, GLuint index, const GLchar* name){
		 glBindAttribLocationARB(programObj,index,name); GLS_OFR return;
	}*/
	/*forceinline GLState_prefix_func void BindBufferARB(GLenum target, GLuint buffer){
		 glBindBufferARB(target,buffer); GLS_OFR return;
	}*/
	forceinline GLState_prefix_func void BindBufferBase(GLenum target, GLuint index, GLuint buffer){
		 glBindBufferBase(target,index,buffer); GLS_OFR return;
	}
	/*forceinline GLState_prefix_func void BindBufferBaseEXT(GLenum target, GLuint index, GLuint buffer){
		 glBindBufferBaseEXT(target,index,buffer); GLS_OFR return;
	}*/
	/*forceinline GLState_prefix_func void BindBufferOffsetEXT(GLenum target, GLuint index, GLuint buffer, GLintptr offset){
		 glBindBufferOffsetEXT(target,index,buffer,offset); GLS_OFR return;
	}*/
	forceinline GLState_prefix_func void BindBufferRange(GLenum target, GLuint index, GLuint buffer, GLintptr offset, GLsizeiptr size){
		 glBindBufferRange(target,index,buffer,offset,size); GLS_OFR return;
	}
	/*forceinline GLState_prefix_func void BindBufferRangeEXT(GLenum target, GLuint index, GLuint buffer, GLintptr offset, GLsizeiptr size){
		 glBindBufferRangeEXT(target,index,buffer,offset,size); GLS_OFR return;
	}*/
	forceinline GLState_prefix_func void BindFramebuffer(GLenum target, GLuint framebuffer){
		 glBindFramebuffer(target,framebuffer); 
		 switch(target){
			 case GL_DRAW_FRAMEBUFFER: bound_draw_framebuffer = framebuffer; break;
			 case GL_READ_FRAMEBUFFER: bound_read_framebuffer = framebuffer; break;
			 case GL_FRAMEBUFFER: bound_draw_framebuffer = bound_read_framebuffer = framebuffer; break;
		 }
		 GLS_OFR return;
	}
	/*forceinline GLState_prefix_func void BindFramebufferEXT(GLenum mode, GLuint framebuffer){
		 glBindFramebufferEXT(mode,framebuffer); GLS_OFR return;
	}*/
	/*forceinline GLState_prefix_func void BindImageTexture(GLuint unit, GLuint texture, GLint level, GLboolean layered, GLint layer, GLenum access, GLenum format){
		 glBindImageTexture(unit,texture,level,layered,layer,access,format); GLS_OFR return;
	}*/
	/*forceinline GLState_prefix_func void BindImageTextureEXT(GLuint index, GLuint texture, GLint level, GLboolean layered, GLint layer, GLenum access, GLenum format){
		 glBindImageTextureEXT(index,texture,level,layered,layer,access,format); GLS_OFR return;
	}*/
	/*forceinline GLState_prefix_func void BindImageTextures(GLuint first, GLsizei count, GLuint* textures){
		 glBindImageTextures(first,count,textures); GLS_OFR return;
	}*/
	/*forceinline GLState_prefix_func void BindMultiTextureEXT(GLenum texunit, GLenum target, GLuint texture){
		 glBindMultiTextureEXT(texunit,target,texture); GLS_OFR return;
	}*/
	/*forceinline GLState_prefix_func void BindProgramARB(GLenum target, GLuint program){
		 glBindProgramARB(target,program); GLS_OFR return;
	}*/
	/*forceinline GLState_prefix_func void BindBuffersBase(GLenum target, GLuint first, GLsizei count, GLuint* buffers){
		 glBindBuffersBase(target,first,count,buffers); GLS_OFR return;
	}*/
	/*forceinline GLState_prefix_func void BindBuffersRange(GLenum target, GLuint first, GLsizei count, GLuint* buffers, GLintptr* offsets, GLsizeiptr* sizes){
		 glBindBuffersRange(target,first,count,buffers,offsets,sizes); GLS_OFR return;
	}*/
	/*forceinline GLState_prefix_func void BindFragDataLocation(GLuint program, GLuint colorNumber, const GLchar* name){
		 glBindFragDataLocation(program,colorNumber,name); GLS_OFR return;
	}*/
	/*forceinline GLState_prefix_func void BindFragDataLocationEXT(GLuint program, GLuint colorNumber, const GLchar* name){
		 glBindFragDataLocationEXT(program, colorNumber, name); GLS_OFR return;
	}*/
	/*forceinline GLState_prefix_func void BindFragDataLocationIndexed(GLuint program, GLuint colorNumber, GLuint index, const GLchar* name){
		 glBindFragDataLocationIndexed(program,colorNumber,index,name); GLS_OFR return;
	}*/
	/*forceinline GLState_prefix_func GLuint BindParameterEXT(GLenum value){
		 auto rtn = glBindParameterEXT(value); GLS_OFR return rtn;
	}*/
	forceinline GLState_prefix_func void BindRenderbuffer(GLenum target, GLuint renderbuffer){
		 glBindRenderbuffer(target,renderbuffer); GLS_OFR return;
	}
	/*forceinline GLState_prefix_func void BindRenderbufferEXT(GLenum target, GLuint renderbuffer){
		 glBindRenderbufferEXT(target, renderbuffer); GLS_OFR return;
	}*/
	forceinline GLState_prefix_func void BindSampler(GLuint unit, GLuint sampler){
		 glBindSampler(unit, sampler); GLS_OFR return;
	}
	/*forceinline GLState_prefix_func void BindSamplers(GLuint first, GLsizei count, GLuint* samplers){
		 glBindSamplers(first,count,samplers); GLS_OFR return;
	}*/
	/*forceinline GLState_prefix_func GLuint BindTexGenParameterEXT(GLenum unit, GLenum coord, GLenum value){
		 auto rtn = glBindTexGenParameterEXT(unit,coord,value); GLS_OFR return rtn;
	}*/
	forceinline GLState_prefix_func void BindTexture(GLenum target, GLuint texture){
		if((ACTIVE_TEXTURE_ID != (GLuint)-1) && (ACTIVE_TEXTURE_ID - GL_TEXTURE0 < RD_MAX_TEXTURE_BINDINGS)){
			if(bound_textures[ACTIVE_TEXTURE_ID - GL_TEXTURE0] == texture){ return; }
			else{ bound_textures[ACTIVE_TEXTURE_ID - GL_TEXTURE0] = texture; }
		}
		glBindTexture(target,texture); GLS_OFR return;
	}
	/*forceinline GLState_prefix_func void BindTextures(GLenum first, GLsizei count, GLuint* textures){
		 glBindTextures(first,count,textures); GLS_OFR return;
	}*/
	/*forceinline GLState_prefix_func void BindTextureEXT(GLenum target, GLuint texture){
		 glBindTextureEXT(target,texture); GLS_OFR return;
	}*/
	/*forceinline GLState_prefix_func void BindTextureUnit(GLuint unit, GLuint texture){
		 glBindTextureUnit(unit,texture); GLS_OFR return;
	}*/
	/*forceinline GLState_prefix_func GLuint BindTextureUnitParameterEXT(GLenum unit, GLenum value){
		 auto rtn = glBindTextureUnitParameterEXT(unit,value); GLS_OFR return rtn;
	}*/
	/*forceinline GLState_prefix_func void TexStorage1D(GLenum target, GLsizei levels, GLenum internalformat, GLsizei width){
		 glTexStorage1D(target,levels,internalformat,width); GLS_OFR return;
	}*/

	forceinline GLState_prefix_func void TexStorage2D(GLenum target, GLsizei levels, GLenum internalformat, GLsizei width, GLsizei height){
		 pglTexStorage2D(target,levels,internalformat,width,height); GLS_OFR return;
	}
	forceinline GLState_prefix_func void TexStorage3D(GLenum target, GLsizei levels, GLenum internalformat, GLsizei width, GLsizei height, GLsizei depth){
		 pglTexStorage3D(target,levels,internalformat,width,height,depth); GLS_OFR return;
	}

	/*forceinline GLState_prefix_func void TexSubImage1D(GLenum target, GLint level, GLint xoffset, GLsizei width, GLenum format, GLenum type, const void* pixels){
		glTexSubImage1D(target, level, xoffset, width, format, type, pixels); GLS_OFR return;
	}*/
	forceinline GLState_prefix_func void TexSubImage2D(GLenum target, GLint level, GLint xoffset, GLint yoffset, GLsizei width, GLsizei height, GLenum format, GLenum type, const void* pixels){
		glTexSubImage2D(target, level, xoffset, yoffset, width, height, format, type, pixels); GLS_OFR return;
	}
	forceinline GLState_prefix_func void TexSubImage3D(GLenum target, GLint level, GLint xoffset, GLint yoffset, GLint zoffset, GLsizei width, GLsizei height, GLsizei depth, GLenum format, GLenum type, const void* pixels){
		glTexSubImage3D(target, level, xoffset, yoffset, zoffset, width, height, depth, format, type, pixels); GLS_OFR return;
	}
	/*forceinline GLState_prefix_func void CopyImageSubData(GLuint srcName, GLenum srtTarget, GLint srcLevel, GLint srcX, GLint srcY, GLint srcZ, GLuint dstName, GLenum dstTarget, GLint dstLevel, GLint dstX, GLint dstY, GLint dstZ, GLsizei srcWidth, GLsizei srcHeight, GLsizei srcDepth){
		 glCopyImageSubData(srcName, srtTarget, srcLevel, srcX, srcY, srcZ, dstName, dstTarget, dstLevel, dstX, dstY, dstZ, srcWidth, srcHeight, srcDepth); GLS_OFR return;
	}*/
	/*forceinline GLState_prefix_func void DrawBuffer(GLenum mode){
		 glDrawBuffer(mode); GLS_OFR return;
	}*/
	/*forceinline GLState_prefix_func void NamedFramebufferDrawBuffer(GLuint framebuffer, GLenum mode){
		 glNamedFramebufferDrawBuffer(framebuffer, mode); GLS_OFR return;
	}*/
	forceinline GLState_prefix_func void DrawBuffers(GLsizei n, GLenum* buffers){
		 glDrawBuffers(n, buffers); GLS_OFR return;
	}
	/*forceinline GLState_prefix_func void NamedFramebufferDrawBuffers(GLuint framebuffer, GLsizei n, GLenum* buffers){
		 glNamedFramebufferDrawBuffers(framebuffer, n, buffers); GLS_OFR return;
	}*/
	forceinline GLState_prefix_func void ReadBuffer(GLenum mode){
		 glReadBuffer(mode); GLS_OFR return;
	}
	/*forceinline GLState_prefix_func void NamedFramebufferReadBuffer(GLuint framebuffer, GLenum mode){
		 glNamedFramebufferReadBuffer(framebuffer, mode); GLS_OFR return;
	}*/
	/*forceinline GLState_prefix_func void CreateBuffers(GLsizei n, GLuint* buffers){
		 glCreateBuffers(n, buffers); GLS_OFR return;
	}*/
	forceinline GLState_prefix_func void GenBuffers(GLsizei n, GLuint* buffers){
		glGenBuffers(n, buffers); GLS_OFR return;
	}
	forceinline GLState_prefix_func void DeleteBuffers(GLsizei n, GLuint* buffers){
		 glDeleteBuffers(n, buffers); GLS_OFR return;
	}
	forceinline GLState_prefix_func void BufferData(GLenum target, GLsizeiptr size, const void* data, GLenum usage){
		 glBufferData(target, size, data, usage); GLS_OFR return;
	}
	forceinline GLState_prefix_func void BufferSubData(GLenum target, GLintptr offset, GLsizeiptr size, const void* data){
		glBufferSubData(target, offset, size, data); GLS_OFR return;
	}
	/*forceinline GLState_prefix_func void CreateFramebuffers(GLsizei n, GLuint* framebuffers){
		 glCreateFramebuffers(n, framebuffers); GLS_OFR return;
	}*/
	forceinline GLState_prefix_func void GenFramebuffers(GLsizei n, GLuint* framebuffers){
		glGenFramebuffers(n, framebuffers); GLS_OFR return;
	}
	forceinline GLState_prefix_func void GenVertexArrays(GLsizei n, GLuint* arrays){
		pglGenVertexArrays(n,arrays); GLS_OFR return;
	}
	forceinline GLState_prefix_func void BindVertexArray(GLuint array){
		glBindVertexArray(array); bound_vertex_array = array; GLS_OFR return;
	}
	forceinline GLState_prefix_func void DeleteVertexArrays(GLsizei n, GLuint* arrays){
		glDeleteVertexArrays(n, arrays); GLS_OFR return;
	}
	forceinline GLState_prefix_func void DeleteFramebuffers(GLsizei n, GLuint* framebuffers){
		 glDeleteFramebuffers(n, framebuffers); GLS_OFR return;
	}
	forceinline GLState_prefix_func GLuint CreateProgram(){
		 auto rtn = glCreateProgram(); GLS_OFR return rtn;
	}
	/*forceinline GLState_prefix_func GLhandleARB CreateProgramObjectARB(){
		 auto rtn = glCreateProgramObjectARB(); GLS_OFR return rtn;
	}*/
	/*forceinline GLState_prefix_func void CreateRenderbuffers(GLsizei n, GLuint* renderbuffers){
		 glCreateRenderbuffers(n, renderbuffers); GLS_OFR return;
	}*/
	/*forceinline GLState_prefix_func void CreateSamplers(GLsizei n, GLuint* samplers){
		 glCreateSamplers(n, samplers); GLS_OFR return;
	}*/
	forceinline GLState_prefix_func GLuint CreateShader(GLenum type){
		 auto rtn = glCreateShader(type); GLS_OFR return rtn;
	}
	/*forceinline GLState_prefix_func GLhandleARB CreateShaderObjectARB(GLenum type){
		 auto rtn = glCreateShaderObjectARB(type); GLS_OFR return rtn;
	}*/
	/*forceinline GLState_prefix_func GLuint CreateShaderProgramEXT(GLenum type, const GLchar* string){
		 auto rtn = glCreateShaderProgramEXT(type, string); GLS_OFR return rtn;
	}*/
	/*forceinline GLState_prefix_func GLuint CreateShaderProgramv(GLenum type, GLsizei count, const GLchar** strings){
		 auto rtn = glCreateShaderProgramv(type, count, strings); GLS_OFR return rtn;
	}*/
	forceinline GLState_prefix_func GLint GetUniformLocation(GLuint program, const GLchar* name){
		auto rtn = glGetUniformLocation(program, name); GLS_OFR return rtn;
	}
	forceinline GLState_prefix_func GLuint GetUniformBlockIndex(GLuint program, const GLchar* uniformBlockName){
		 auto rtn = glGetUniformBlockIndex(program,uniformBlockName); GLS_OFR return rtn;
	}
	forceinline GLState_prefix_func void GetActiveUniformBlockiv(GLuint program, GLuint uniformBlockIndex, GLenum pname, GLint* params){
		glGetActiveUniformBlockiv(program, uniformBlockIndex, pname, params); GLS_OFR return;
	}
	forceinline GLState_prefix_func void GetActiveUniformBlockName(GLuint program, GLuint uniformBlockIndex, GLsizei bufSize, GLsizei* length, GLchar* uniformBlockName){
		glGetActiveUniformBlockName(program, uniformBlockIndex, bufSize, length, uniformBlockName); GLS_OFR return;
	}
	forceinline GLState_prefix_func void UniformBlockBinding(GLuint program, GLuint uniformBlockIndex, GLuint uniformBlockBinding){
		 glUniformBlockBinding(program, uniformBlockIndex, uniformBlockBinding); GLS_OFR return;
	}
	/*forceinline GLState_prefix_func GLuint GetProgramResourceIndex(GLuint program, GLenum programInterface, const GLchar* name){
		 auto rtn = glGetProgramResourceIndex(program, programInterface, name); GLS_OFR return rtn;
	}*/
	/*forceinline GLState_prefix_func void ShaderStorageBlockBinding(GLuint program, GLuint storageBlockIndex, GLuint storageBlockBinding){
		 glShaderStorageBlockBinding(program, storageBlockIndex, storageBlockBinding); GLS_OFR return;
	}*/
	/*forceinline GLState_prefix_func void CreateVertexArrays(GLsizei n, GLuint* arrays){
		 glCreateVertexArrays(n, arrays); GLS_OFR return;
	}*/
	/*forceinline GLState_prefix_func void CreateTextures(GLenum target, GLsizei n, GLuint* textures){
		 glCreateTextures(target, n, textures); GLS_OFR return;
	}*/
	forceinline GLState_prefix_func void GenTextures(GLsizei n, GLuint* textures){
		 glGenTextures(n, textures); GLS_OFR return;
	}
	forceinline GLState_prefix_func void DeleteTextures(GLsizei n, GLuint* textures){
		glDeleteTextures(n, textures); GLS_OFR return;
	}
	forceinline GLState_prefix_func void GenerateMipmap(GLenum target){
		glGenerateMipmap(target); GLS_OFR return;
	}
	forceinline GLState_prefix_func void CompressedTexImage2D(GLenum target, GLint level, GLenum internalFormat, GLsizei width, GLsizei height, GLint border, GLsizei imageSize, void* data){
		 glCompressedTexImage2D(target,level,internalFormat,width,height,border,imageSize,data); GLS_OFR return;
	}
	/*forceinline GLState_prefix_func void TexImage1D(GLenum target, GLint level, GLint internalFormat, GLsizei width, GLint border, GLenum format, GLenum type, void* data){
		glTexImage1D(target, level, internalFormat, width, border, format, type, data); GLS_OFR return;
	}*/
	forceinline GLState_prefix_func void TexImage2D(GLenum target, GLint level, GLint internalFormat, GLsizei width, GLsizei height, GLint border, GLenum format, GLenum type, void* data){
		LOG("glTexImage2D( %s, %d, %s, %d, %d, %s, %s )", glenumToString(target), level, glenumToString(internalFormat), width, height, glenumToString(format), glenumToString(type));
		glTexImage2D(target,level,internalFormat,width,height,border,format,type,data); GLS_OFR return;
	}
	forceinline GLState_prefix_func void TexImage3D(GLenum target, GLint level, GLint internalFormat, GLsizei width, GLsizei height, GLsizei depth, GLint border, GLenum format, GLenum type, void* data){
		LOG("glTexImage3D( %s, %d, %s, %d, %d, %d, %s, %s )", glenumToString(target), level, glenumToString(internalFormat), width, height, depth, glenumToString(format), glenumToString(type));
		glTexImage3D(target, level, internalFormat, width, height, depth, border, format, type, data); GLS_OFR return;
	}
	forceinline GLState_prefix_func void TexParameteri(GLenum target, GLenum param, GLint val){
		glTexParameteri(target, param, val); GLS_OFR return;
	}
	forceinline GLState_prefix_func void TexParameterf(GLenum target, GLenum param, GLfloat val){
		glTexParameterf(target, param, val); GLS_OFR return;
	}
	/*forceinline GLState_prefix_func void TextureParameteri(GLuint texture, GLenum param, GLint val){
		glTextureParameteri(texture,param,val); GLS_OFR return;
	}*/
	/*forceinline GLState_prefix_func void TextureParameterf(GLuint texture, GLenum param, GLfloat val){
		glTextureParameterf(texture, param, val); GLS_OFR return;
	}*/

	forceinline GLState_prefix_func void GetTexParameterfv(GLenum target, GLenum param, GLfloat* val){
		glGetTexParameterfv(target, param, val); GLS_OFR return;
	}
	forceinline GLState_prefix_func void GetTexParameteriv(GLenum target, GLenum param, GLint* val){
		glGetTexParameteriv(target, param, val); GLS_OFR return;
	}
	/*forceinline GLState_prefix_func void GetTexParameterIiv(GLenum target, GLenum param, GLint* val){
		glGetTexParameterIiv(target, param, val); GLS_OFR return;
	}*/
	/*forceinline GLState_prefix_func void GetTexParameterIuiv(GLenum target, GLenum param, GLuint* val){
		glGetTexParameterIuiv(target, param, val); GLS_OFR return;
	}*/
	/*forceinline GLState_prefix_func void GetTextureParameterfv(GLuint texture, GLenum param, GLfloat* val){
		glGetTextureParameterfv(texture, param, val); GLS_OFR return;
	}*/
	/*forceinline GLState_prefix_func void GetTextureParameteriv(GLuint texture, GLenum param, GLint* val){
		glGetTextureParameteriv(texture, param, val); GLS_OFR return;
	}*/
	/*forceinline GLState_prefix_func void GetTextureParameterIiv(GLuint texture, GLenum param, GLint* val){
		glGetTextureParameterIiv(texture, param, val); GLS_OFR return;
	}*/
	/*forceinline GLState_prefix_func void GetTextureParameterIuiv(GLuint texture, GLenum param, GLuint* val){
		glGetTextureParameterIuiv(texture, param, val); GLS_OFR return;
	}*/

	/*forceinline GLState_prefix_func void TextureView(GLuint texture, GLenum target, GLuint origtexture, GLenum internalformat, GLuint minlevel, GLuint numlevels, GLuint minlayer, GLuint numlayers){
		glTextureView(texture, target, origtexture, internalformat, minlevel, numlevels, minlayer, numlayers); GLS_OFR return;
	}*/
	/*forceinline GLState_prefix_func void UseProgramObjectARB(GLint program){
		{
			if(program == 0){
				glUseProgramObjectARB(0);
				previous_program_link = program_link; program_link = 0;
			}
			else{
				glUseProgramObjectARB(program);
				previous_program_link = program_link; program_link = program;
			}
		}
		GLS_OFR return;
	}*/
	/*forceinline GLState_prefix_func void EnableClientState(GLenum state){
		switch(state){
			case GL_VERTEX_ARRAY:
						glEnableClientState(GL_VERTEX_ARRAY); VERTEX_ARRAY_ENABLED = true;
				break;
			case GL_NORMAL_ARRAY:
						glEnableClientState(GL_NORMAL_ARRAY); NORMAL_ARRAY_ENABLED = true;
				break;
			case GL_TEXTURE_COORD_ARRAY:
						glEnableClientState(GL_TEXTURE_COORD_ARRAY); TEXTURE_COORD_ARRAY_ENABLED = true;
				break;
			case GL_COLOR_ARRAY:
						glEnableClientState(GL_COLOR_ARRAY); COLOR_ARRAY_ENABLED = true;
				break;
		}
		GLS_OFR return;
	}*/
	/*forceinline GLState_prefix_func void DisableClientState(GLenum state){
		switch(state){
			case GL_VERTEX_ARRAY:
				{ glDisableClientState(GL_VERTEX_ARRAY); VERTEX_ARRAY_ENABLED = false; }
				break;
			case GL_NORMAL_ARRAY:
				{ glDisableClientState(GL_NORMAL_ARRAY); NORMAL_ARRAY_ENABLED = false; }
				break;
			case GL_TEXTURE_COORD_ARRAY:
				{ glDisableClientState(GL_TEXTURE_COORD_ARRAY); TEXTURE_COORD_ARRAY_ENABLED = false; }
				break;
			case GL_COLOR_ARRAY:
				{ glDisableClientState(GL_COLOR_ARRAY); COLOR_ARRAY_ENABLED = false; }
				break;
		}
		GLS_OFR return;
	}*/
	/*forceinline GLState_prefix_func void VertexPointer(GLint size, GLenum type, GLint stride,GLvoid* pointer){
		glVertexPointer(size,type,stride,pointer); pVertexBuffer = pointer; GLS_OFR return;
	}*/
	/*forceinline GLState_prefix_func void NormalPointer(GLenum type,int stride,GLvoid* pointer){
		glNormalPointer(type,stride,pointer); pNormalBuffer = pointer; GLS_OFR return;
	}*/
	/*forceinline GLState_prefix_func void TexCoordPointer(GLint size, GLenum type, GLint stride,GLvoid* pointer){
		glTexCoordPointer(size,type,stride,pointer); pTexCoordBuffer = pointer; GLS_OFR return;
	}*/
	/*forceinline GLState_prefix_func void ColorPointer(GLint size,GLenum type, GLint stride,GLvoid* pointer){
		glColorPointer(size,type,stride,pointer); pColorBuffer = pointer; GLS_OFR return;
	}*/
	forceinline GLState_prefix_func void ActiveTextureARB(GLenum id){
		glActiveTexture(id); ACTIVE_TEXTURE_ID = id; GLS_OFR return;
	}
	forceinline GLState_prefix_func void ActiveTexture(GLenum id){
		glActiveTexture(id); ACTIVE_TEXTURE_ID = id; GLS_OFR return;
	}

	forceinline GLState_prefix_func void ResetVertexPointer(){ pVertexBuffer = 0; }
	forceinline GLState_prefix_func void ResetNormalPointer(){ pNormalBuffer = 0; }
	forceinline GLState_prefix_func void ResetTexCoordPointer(){ pTexCoordBuffer = 0; }
	forceinline GLState_prefix_func void ResetColorPointer(){ pColorBuffer = 0; }
	forceinline GLState_prefix_func int  getMatrixMode(){
		return MATRIX_MODE;}
	forceinline GLState_prefix_func int  getBlendSrc(){
		return BLEND_SRC;}
	forceinline GLState_prefix_func int  getBlendDst(){
		return BLEND_DST;}
	forceinline GLState_prefix_func int  getBlendSrcAlpha(){
		return BLEND_SRC_ALPHA;}
	forceinline GLState_prefix_func int  getBlendDstAlpha(){
		return BLEND_DST_ALPHA;}
	forceinline GLState_prefix_func int  getActiveTexture(){
		return ACTIVE_TEXTURE_ID;}
	forceinline GLState_prefix_func int  getBindedTexture(int mode){
		switch(mode){
			//case GL_TEXTURE_1D: return BIND_TEXTURE1D_ID;
			case GL_TEXTURE_2D: return BIND_TEXTURE2D_ID;
			case GL_TEXTURE_3D: return BIND_TEXTURE3D_ID;
			default: return 0;
		}
	}
	forceinline GLState_prefix_func int  getBindedTexture1D(){ return BIND_TEXTURE1D_ID;}
	forceinline GLState_prefix_func int  getBindedTexture2D(){ return BIND_TEXTURE2D_ID;}
	forceinline GLState_prefix_func int  getBindedTexture3D(){ return BIND_TEXTURE3D_ID;}
	forceinline GLState_prefix_func bool isLightingEnabled(){ return LIGHTING_ENABLED;}
	forceinline GLState_prefix_func bool isDepthTestEnabled(){ return DEPTH_TEST_ENABLED;}
	forceinline GLState_prefix_func bool isBlendingEnabled(){ return BLEND_ENABLED;}
	forceinline GLState_prefix_func bool isTexture1DEnabled(){ return TEXTURE1D_ENABLED;}
	forceinline GLState_prefix_func bool isTexture2DEnabled(){ return TEXTURE2D_ENABLED;}
	forceinline GLState_prefix_func bool isTexture3DEnabled(){ return TEXTURE3D_ENABLED;}
	forceinline GLState_prefix_func int  getPreviousProgramObject(){ return previous_program_link;}
	forceinline GLState_prefix_func int  getActiveProgramObject(){ return program_link;}
	
#pragma endregion


};

/*
GLState::EGLTextureInternalFormat GetBaseInternalFormatFromTextureFormat(ETypedTextureFormat fmt);
GLState::EGLSymbolicFormat GetSymbolicFormatFromTextureFormat(ETypedTextureFormat fmt);
GLState::EGLTextureInternalFormat GetBaseInternalFormatFromTextureFormat(ETextureFormat fmt);
GLState::EGLSymbolicFormat GetSymbolicFormatFromTextureFormat(ETextureFormat fmt);
*/

inline CGLState::EGLShaderStage GetShaderStage(const EShaderStage& stage){
	switch(stage)
	{
		case EShaderStage::VertexShader: return CGLState::EGLShaderStage::EGL_VERTEX_SHADER;
		//case EShaderStage::TessControlShader: return CGLState::EGLShaderStage::EGL_TESS_CONTROL_SHADER;
		//case EShaderStage::TessEvaluationShader: return CGLState::EGLShaderStage::EGL_TESS_EVALUATION_SHADER;
		//case EShaderStage::GeometryShader: return CGLState::EGLShaderStage::EGL_GEOMETRY_SHADER;
		case EShaderStage::FragmentShader: return CGLState::EGLShaderStage::EGL_FRAGMENT_SHADER;
		default: break;
	}
	return CGLState::EGLShaderStage::STAGE_NUMBER;
}
inline EShaderStage GetShaderStage(CGLState::EGLShaderStage stage){
	switch(stage)
	{
		case CGLState::EGLShaderStage::EGL_VERTEX_SHADER: return EShaderStage::VertexShader;
		//case CGLState::EGLShaderStage::EGL_TESS_CONTROL_SHADER: return EShaderStage::TessControlShader;
		//case CGLState::EGLShaderStage::EGL_TESS_EVALUATION_SHADER: return EShaderStage::TessEvaluationShader;
		//case CGLState::EGLShaderStage::EGL_GEOMETRY_SHADER: return EShaderStage::GeometryShader;
		case CGLState::EGLShaderStage::EGL_FRAGMENT_SHADER: return EShaderStage::FragmentShader;
		default: break;
	}
	return EShaderStage::NumShaderStages;
}

inline GLenum glenum(const EShaderStage& v){
	switch(v)
	{
		case EShaderStage::VertexShader: return (GLenum)CGLState::EGLShaderStage::EGL_VERTEX_SHADER;
		//case EShaderStage::TessControlShader: return (GLenum)CGLState::EGLShaderStage::EGL_TESS_CONTROL_SHADER;
		//case EShaderStage::TessEvaluationShader: return (GLenum)CGLState::EGLShaderStage::EGL_TESS_EVALUATION_SHADER;
		//case EShaderStage::GeometryShader: return (GLenum)CGLState::EGLShaderStage::EGL_GEOMETRY_SHADER;
		case EShaderStage::FragmentShader: return (GLenum)CGLState::EGLShaderStage::EGL_FRAGMENT_SHADER;
		default: break;
	}
	return (GLenum)GL_NONE;
}
inline GLenum glenum(const EPrimitiveTopology& v){
	switch(v)
	{
		case EPrimitiveTopology::PointList: return (GLenum)CGLState::EGLDrawMode::EGL_POINTS;
		case EPrimitiveTopology::LineList: return (GLenum)CGLState::EGLDrawMode::EGL_LINES;
		case EPrimitiveTopology::LineStrip: return (GLenum)CGLState::EGLDrawMode::EGL_LINE_STRIP;
		case EPrimitiveTopology::LineLoop: return (GLenum)CGLState::EGLDrawMode::EGL_LINE_LOOP;
		//case EPrimitiveTopology::LineListAdjacency: return (GLenum)CGLState::EGLDrawMode::EGL_LINES_ADJACENCY;
		//case EPrimitiveTopology::LineStripAdjacency: return (GLenum)CGLState::EGLDrawMode::EGL_LINE_STRIP_ADJACENCY;
		case EPrimitiveTopology::TriangleList: return (GLenum)CGLState::EGLDrawMode::EGL_TRIANGLES;
		case EPrimitiveTopology::TriangleStrip: return (GLenum)CGLState::EGLDrawMode::EGL_TRIANGLE_STRIP;
		case EPrimitiveTopology::TriangleFan: return (GLenum)CGLState::EGLDrawMode::EGL_TRIANGLE_FAN;
		//case EPrimitiveTopology::TriangleListAdjacency: return (GLenum)CGLState::EGLDrawMode::EGL_TRIANGLES_ADJACENCY;
		//case EPrimitiveTopology::TriangleStripAdjacency: return (GLenum)CGLState::EGLDrawMode::EGL_TRIANGLES_STRIP_ADJACENCY;
		default: return (GLenum)GL_NONE;
	}
}
inline GLenum glenum(const EBlendFactor& v){
	switch(v)
	{
		case EBlendFactor::Zero: return (GLenum)GL_ZERO;
		case EBlendFactor::One: return (GLenum)GL_ONE;
		case EBlendFactor::SrcColor: return (GLenum)GL_SRC_COLOR;
		case EBlendFactor::OneMinusSrcColor: return (GLenum)GL_ONE_MINUS_SRC_COLOR;
		case EBlendFactor::DstColor: return (GLenum)GL_DST_COLOR;
		case EBlendFactor::OneMinusDstColor: return (GLenum)GL_ONE_MINUS_DST_COLOR;
		case EBlendFactor::SrcAlpha: return (GLenum)GL_SRC_ALPHA;
		case EBlendFactor::OneMinusSrcAlpha: return (GLenum)GL_ONE_MINUS_SRC_ALPHA;
		case EBlendFactor::DstAlpha: return (GLenum)GL_DST_ALPHA;
		case EBlendFactor::OneMinusDstAlpha: return (GLenum)GL_ONE_MINUS_DST_ALPHA;
		case EBlendFactor::ConstantColor: return (GLenum)GL_CONSTANT_COLOR;
		case EBlendFactor::OneMinusConstantColor: return (GLenum)GL_ONE_MINUS_CONSTANT_COLOR;
		case EBlendFactor::ConstantAlpha: return (GLenum)GL_CONSTANT_ALPHA;
		case EBlendFactor::OneMinusConstantAlpha: return (GLenum)GL_ONE_MINUS_CONSTANT_ALPHA;
		case EBlendFactor::SrcAlphaSaturate: return (GLenum)GL_SRC_ALPHA_SATURATE;
		default: return (GLenum)GL_NONE;
	}
}
inline GLenum glenum(const EBlendOperation& v){
	switch(v)
	{
		case EBlendOperation::Add: return (GLenum)GL_FUNC_ADD;
		case EBlendOperation::Subtract: return (GLenum)GL_FUNC_SUBTRACT;
		case EBlendOperation::ReverseSubtract: return (GLenum)GL_FUNC_REVERSE_SUBTRACT;
		/*case EBlendOperation::Min: return (GLenum)GL_MIN;
		case EBlendOperation::Max: return (GLenum)GL_MAX;*/
		default: return (GLenum)GL_NONE;
	}
}
inline GLenum glenum(const ELogicOperation& v){
	switch(v)
	{
		case ELogicOperation::NoOp: //return (GLenum)GL_NOOP;
		default: return (GLenum)GL_NONE;
	}
}
inline GLenum glenum(const EDepthTestMode& v){
	switch(v)
	{
		case EDepthTestMode::None: return (GLenum)GL_NONE;
		case EDepthTestMode::Less: return (GLenum)GL_LESS;
		case EDepthTestMode::LessEqual: return (GLenum)GL_LEQUAL;
		case EDepthTestMode::Equal: return (GLenum)GL_EQUAL;
		case EDepthTestMode::Greater: return (GLenum)GL_GREATER;
		case EDepthTestMode::GreaterEqual: return (GLenum)GL_GEQUAL;
		default: return (GLenum)GL_NONE;
	}
}
inline GLenum glenum(const ECullMode& v){
	switch(v)
	{
		case ECullMode::None: return (GLenum)GL_NONE;
		case ECullMode::FrontFaces: return (GLenum)GL_FRONT;
		case ECullMode::BackFaces: return (GLenum)GL_BACK;
		case ECullMode::Both: return (GLenum)GL_FRONT_AND_BACK;
		default: return (GLenum)GL_NONE;
	}
}
inline GLenum glenum(const EFrontFace& v){
	switch(v)
	{
		case EFrontFace::Clockwise: return (GLenum)GL_CW;
		case EFrontFace::CounterClockwise: return (GLenum)GL_CCW;
		default: return (GLenum)GL_NONE;
	}
}
inline GLenum glenum(const EComparisonOp& v){
	switch(v)
	{
		case EComparisonOp::Never: return (GLenum)GL_NEVER;
		case EComparisonOp::Less: return (GLenum)GL_LESS;
		case EComparisonOp::Equal: return (GLenum)GL_EQUAL;
		case EComparisonOp::LessEqual: return (GLenum)GL_LEQUAL;
		case EComparisonOp::Greater: return (GLenum)GL_GREATER;
		case EComparisonOp::NotEqual: return (GLenum)GL_NOTEQUAL;
		case EComparisonOp::GreaterEqual: return (GLenum)GL_GEQUAL;
		case EComparisonOp::Always: return (GLenum)GL_ALWAYS;
		default: return (GLenum)GL_NONE;
	}
}
inline GLenum glenum(const EStencilOp& v){
	switch(v)
	{
		case EStencilOp::Keep: return (GLenum)GL_KEEP;
		case EStencilOp::Zero: return (GLenum)GL_ZERO;
		case EStencilOp::Replace: return (GLenum)GL_REPLACE;
		case EStencilOp::Inc: return (GLenum)GL_INCR;
		case EStencilOp::Dec: return (GLenum)GL_DECR;
		case EStencilOp::Invert: return (GLenum)GL_INVERT;
		case EStencilOp::IncWrap: return (GLenum)GL_INCR_WRAP;
		case EStencilOp::DecWrap: return (GLenum)GL_DECR_WRAP;
		default: return (GLenum)GL_NONE;
	}
}
inline GLenum glenum(const ETypedTextureFormat& v){
	switch(v)
	{
		case ETypedTextureFormat::None: return (GLenum)GL_NONE;
		case ETypedTextureFormat::R32G32B32A32_typeless: return (GLenum)GL_RGBA32UI;
		case ETypedTextureFormat::R32G32B32A32_float: return (GLenum)GL_RGBA32F;
		case ETypedTextureFormat::R32G32B32A32_uint: return (GLenum)GL_RGBA32UI;
		case ETypedTextureFormat::R32G32B32A32_sint: return (GLenum)GL_RGBA32I;
		case ETypedTextureFormat::R32G32B32_typeless: return (GLenum)GL_RGB32UI;
		case ETypedTextureFormat::R32G32B32_float: return (GLenum)GL_RGB32F;
		case ETypedTextureFormat::R32G32B32_uint: return (GLenum)GL_RGB32UI;
		case ETypedTextureFormat::R32G32B32_sint: return (GLenum)GL_RGB32I;
		case ETypedTextureFormat::R16G16B16A16_typeless: return (GLenum)GL_RGBA16UI;
		case ETypedTextureFormat::R16G16B16A16_float: return (GLenum)GL_RGBA16F;
		case ETypedTextureFormat::R16G16B16A16_unorm: return (GLenum)GL_RGBA16F;
		case ETypedTextureFormat::R16G16B16A16_uint: return (GLenum)GL_RGBA16UI;
		case ETypedTextureFormat::R16G16B16A16_snorm: return (GLenum)GL_RGBA16F;
		case ETypedTextureFormat::R16G16B16A16_sint: return (GLenum)GL_RGBA16I;
		case ETypedTextureFormat::R32G32_typeless: return (GLenum)GL_RG32UI;
		case ETypedTextureFormat::R32G32_float: return (GLenum)GL_RG32F;
		case ETypedTextureFormat::R32G32_uint: return (GLenum)GL_RG32UI;
		case ETypedTextureFormat::R32G32_sint: return (GLenum)GL_RG32I;
		case ETypedTextureFormat::R10G10B10A2_typeless: return (GLenum)GL_RGB10_A2UI;
		case ETypedTextureFormat::R10G10B10A2_unorm: return (GLenum)GL_NONE;
		case ETypedTextureFormat::R10G10B10A2_uint: return (GLenum)GL_RGB10_A2UI;
		case ETypedTextureFormat::R11G11B10_float: return (GLenum)GL_R11F_G11F_B10F;
		case ETypedTextureFormat::R8G8B8A8_typeless: return (GLenum)GL_RGBA8UI;
		case ETypedTextureFormat::R8G8B8A8_unorm: return (GLenum)GL_RGBA8UI;
		case ETypedTextureFormat::R8G8B8A8_unorm_sRGB: return (GLenum)GL_SRGB8;
		case ETypedTextureFormat::R8G8B8A8_uint: return (GLenum)GL_RGBA8UI;
		case ETypedTextureFormat::R8G8B8A8_snorm: return (GLenum)GL_RGBA8_SNORM;
		case ETypedTextureFormat::R8G8B8A8_sint: return (GLenum)GL_RGBA8I;
		case ETypedTextureFormat::R16G16_typeless: return (GLenum)GL_RG16UI;
		case ETypedTextureFormat::R16G16_float: return (GLenum)GL_RG16F;
		case ETypedTextureFormat::R16G16_unorm: return (GLenum)GL_RG16F;
		case ETypedTextureFormat::R16G16_uint: return (GLenum)GL_RG16UI;
		//case ETypedTextureFormat::R16G16_snorm: return (GLenum)GL_RG16_SNORM;
		case ETypedTextureFormat::R16G16_sint: return (GLenum)GL_RG16I;
		case ETypedTextureFormat::R32_typeless: return (GLenum)GL_R32UI;
		case ETypedTextureFormat::Depth32_float: return (GLenum)GL_DEPTH32F_STENCIL8;
		case ETypedTextureFormat::R32_float: return (GLenum)GL_R32F;
		case ETypedTextureFormat::R32_uint: return (GLenum)GL_R32UI;
		case ETypedTextureFormat::R32_sint: return (GLenum)GL_R32I;
		case ETypedTextureFormat::Depth24_unorm: return (GLenum)GL_DEPTH24_STENCIL8;
		case ETypedTextureFormat::Depth24_unorm_Stencil8_uint: return (GLenum)GL_DEPTH24_STENCIL8;
		case ETypedTextureFormat::R8G8_typeless: return (GLenum)GL_RG8UI;
		case ETypedTextureFormat::R8G8_unorm: return (GLenum)GL_RG8UI;
		case ETypedTextureFormat::R8G8_uint: return (GLenum)GL_RG8UI;
		case ETypedTextureFormat::R8G8_snorm: return (GLenum)GL_RG8I;
		case ETypedTextureFormat::R8G8_sint: return (GLenum)GL_RG8I;
		case ETypedTextureFormat::R16_typeless: return (GLenum)GL_R16UI;
		case ETypedTextureFormat::R16_float: return (GLenum)GL_R16F;
		case ETypedTextureFormat::Depth16_unorm: return (GLenum)GL_NONE;
		case ETypedTextureFormat::R16_unorm: return (GLenum)GL_R16F;
		case ETypedTextureFormat::R16_uint: return (GLenum)GL_R16UI;
		case ETypedTextureFormat::R16_snorm: return (GLenum)GL_R16F;
		case ETypedTextureFormat::R16_sint: return (GLenum)GL_R16I;
		case ETypedTextureFormat::R8_typeless: return (GLenum)GL_R8UI;
		case ETypedTextureFormat::R8_unorm: return (GLenum)GL_R8UI;
		case ETypedTextureFormat::R8_uint: return (GLenum)GL_R8UI;
		case ETypedTextureFormat::R8_snorm: return (GLenum)GL_R8I;
		case ETypedTextureFormat::R8_sint: return (GLenum)GL_R8I;
		case ETypedTextureFormat::R9G9B9E5_sharedexp: return (GLenum)GL_RGB9_E5;
		//case ETypedTextureFormat::BC1_typeless: return (GLenum)GL_COMPRESSED_RGB_S3TC_DXT1_EXT;
		//case ETypedTextureFormat::BC1_unorm: return (GLenum)GL_COMPRESSED_RGB_S3TC_DXT1_EXT;
		//case ETypedTextureFormat::BC1_unorm_sRGB: return (GLenum)GL_COMPRESSED_SRGB_S3TC_DXT1_EXT;
		//case ETypedTextureFormat::BC2_typeless: return (GLenum)GL_COMPRESSED_RGBA_S3TC_DXT3_EXT;
		//case ETypedTextureFormat::BC2_unorm: return (GLenum)GL_COMPRESSED_RGBA_S3TC_DXT3_EXT;
		case ETypedTextureFormat::BC2_unorm_sRGB: return (GLenum)GL_NONE;
		//case ETypedTextureFormat::BC3_typeless: return (GLenum)GL_COMPRESSED_RGBA_S3TC_DXT5_EXT;
		//case ETypedTextureFormat::BC3_unorm: return (GLenum)GL_COMPRESSED_RGBA_S3TC_DXT5_EXT;
		case ETypedTextureFormat::BC3_unorm_sRGB: return (GLenum)GL_NONE;
		case ETypedTextureFormat::B5G6R5_unorm: return (GLenum)GL_RGB565;
		case ETypedTextureFormat::B5G5R5A1_unorm: return (GLenum)GL_RGB565;
		case ETypedTextureFormat::B8G8R8A8_unorm: return (GLenum)GL_RGBA8UI;
		case ETypedTextureFormat::B8G8R8X8_unorm: return (GLenum)GL_RGBA8UI;
		case ETypedTextureFormat::B8G8R8A8_typeless: return (GLenum)GL_RGBA8UI;
		case ETypedTextureFormat::B8G8R8X8_typeless: return (GLenum)GL_RGBA8UI;
		default: return (GLenum)GL_NONE;
	}
}
inline GLenum glenum(const ETextureFormat& v, bool integer = false){
	switch(v)
	{
		case ETextureFormat::None: return (GLenum)GL_NONE;
		case ETextureFormat::R: return (integer)? (GLenum)GL_RED_INTEGER : (GLenum)GL_RED;
		case ETextureFormat::RG: return (integer)? (GLenum)GL_RG_INTEGER : (GLenum)GL_RG;
		case ETextureFormat::RGB: return (integer)? (GLenum)GL_RGB_INTEGER : (GLenum)GL_RGB;
		case ETextureFormat::RGBA: return (integer)? (GLenum)GL_RGBA_INTEGER : (GLenum)GL_RGBA;
		case ETextureFormat::Depth: return (GLenum)GL_DEPTH_COMPONENT;
		case ETextureFormat::DepthStencil: return (GLenum)GL_DEPTH_STENCIL;
		case ETextureFormat::RGBE: return (integer)? (GLenum)GL_RGBA_INTEGER : (GLenum)GL_RGBA;
		default: return (GLenum)GL_NONE;
	}
}
inline GLenum glenum(const ETextureFormat& f, EValueType& v){
	switch(v)
	{
		case EValueType::int8:
			switch(f)
			{
				case ETextureFormat::None: return (GLenum)GL_NONE;
				case ETextureFormat::R: return (GLenum)GL_R8I;
				case ETextureFormat::RG: return (GLenum)GL_RG8I;
				case ETextureFormat::RGB: return (GLenum)GL_RGB8I;
				case ETextureFormat::RGBA: return (GLenum)GL_RGBA8I;
				case ETextureFormat::Depth: return (GLenum)GL_NONE;
				case ETextureFormat::DepthStencil: return (GLenum)GL_NONE;
				case ETextureFormat::RGBE: return (GLenum)GL_RGBA8I;
				default: return (GLenum)GL_NONE;
			}
			break;
		case EValueType::int16:
			switch(f)
			{
				case ETextureFormat::None: return (GLenum)GL_NONE;
				case ETextureFormat::R: return (GLenum)GL_R16I;
				case ETextureFormat::RG: return (GLenum)GL_RG16I;
				case ETextureFormat::RGB: return (GLenum)GL_RGB16I;
				case ETextureFormat::RGBA: return (GLenum)GL_RGBA16I;
				case ETextureFormat::Depth: return (GLenum)GL_NONE;
				case ETextureFormat::DepthStencil: return (GLenum)GL_NONE;
				case ETextureFormat::RGBE: return (GLenum)GL_RGBA16I;
				default: return (GLenum)GL_NONE;
			}
			break;
		case EValueType::int32:
			switch(f)
			{
				case ETextureFormat::None: return (GLenum)GL_NONE;
				case ETextureFormat::R: return (GLenum)GL_R32I;
				case ETextureFormat::RG: return (GLenum)GL_RG32I;
				case ETextureFormat::RGB: return (GLenum)GL_RGB32I;
				case ETextureFormat::RGBA: return (GLenum)GL_RGBA32I;
				case ETextureFormat::Depth: return (GLenum)GL_NONE;
				case ETextureFormat::DepthStencil: return (GLenum)GL_NONE;
				case ETextureFormat::RGBE: return (GLenum)GL_RGBA32I;
				default: return (GLenum)GL_NONE;
			}
			break;
		case EValueType::uint8:
			switch(f)
			{
				case ETextureFormat::None: return (GLenum)GL_NONE;
				case ETextureFormat::R: return (GLenum)GL_R8UI;
				case ETextureFormat::RG: return (GLenum)GL_RG8UI;
				case ETextureFormat::RGB: return (GLenum)GL_RGB8UI;
				case ETextureFormat::RGBA: return (GLenum)GL_RGBA8UI;
				case ETextureFormat::Depth: return (GLenum)GL_NONE;
				case ETextureFormat::DepthStencil: return (GLenum)GL_NONE;
				case ETextureFormat::RGBE: return (GLenum)GL_RGBA8UI;
				default: return (GLenum)GL_NONE;
			}
			break;
		case EValueType::uint16:
			switch(f)
			{
				case ETextureFormat::None: return (GLenum)GL_NONE;
				case ETextureFormat::R: return (GLenum)GL_R16UI;
				case ETextureFormat::RG: return (GLenum)GL_RG16UI;
				case ETextureFormat::RGB: return (GLenum)GL_RGB16UI;
				case ETextureFormat::RGBA: return (GLenum)GL_RGBA16UI;
				case ETextureFormat::Depth: return (GLenum)GL_NONE;
				case ETextureFormat::DepthStencil: return (GLenum)GL_NONE;
				case ETextureFormat::RGBE: return (GLenum)GL_RGBA16UI;
				default: return (GLenum)GL_NONE;
			}
			break;
		case EValueType::uint24:
			switch(f)
			{
				case ETextureFormat::None: return (GLenum)GL_NONE;
				case ETextureFormat::R: return (GLenum)GL_NONE;
				case ETextureFormat::RG: return (GLenum)GL_NONE;
				case ETextureFormat::RGB: return (GLenum)GL_RGB8UI;
				case ETextureFormat::RGBA: return (GLenum)GL_NONE;
				case ETextureFormat::Depth: return (GLenum)GL_RGB8UI;
				case ETextureFormat::DepthStencil: return (GLenum)GL_NONE;
				case ETextureFormat::RGBE: return (GLenum)GL_NONE;
				default: return (GLenum)GL_NONE;
			}
			break;
		case EValueType::uint32:
			switch(f)
			{
				case ETextureFormat::None: return (GLenum)GL_NONE;
				case ETextureFormat::R: return (GLenum)GL_R32UI;
				case ETextureFormat::RG: return (GLenum)GL_RG32UI;
				case ETextureFormat::RGB: return (GLenum)GL_RGB32UI;
				case ETextureFormat::RGBA: return (GLenum)GL_RGBA32UI;
				case ETextureFormat::Depth: return (GLenum)GL_R32UI;
				case ETextureFormat::DepthStencil: return (GLenum)GL_R32UI;
				case ETextureFormat::RGBE: return (GLenum)GL_RGBA32UI;
				default: return (GLenum)GL_NONE;
			}
			break;
		case EValueType::float16:
			switch(f)
			{
				case ETextureFormat::None: return (GLenum)GL_NONE;
				case ETextureFormat::R: return (GLenum)GL_R16F;
				case ETextureFormat::RG: return (GLenum)GL_RG16F;
				case ETextureFormat::RGB: return (GLenum)GL_RGB16F;
				case ETextureFormat::RGBA: return (GLenum)GL_RGBA16F;
				case ETextureFormat::Depth: return (GLenum)GL_R16F;
				case ETextureFormat::DepthStencil: return (GLenum)GL_NONE;
				case ETextureFormat::RGBE: return (GLenum)GL_RGBA16F;
				default: return (GLenum)GL_NONE;
			}
			break;
		case EValueType::float24:
			switch(f)
			{
				case ETextureFormat::None: return (GLenum)GL_NONE;
				case ETextureFormat::R: return (GLenum)GL_NONE;
				case ETextureFormat::RG: return (GLenum)GL_NONE;
				case ETextureFormat::RGB: return (GLenum)GL_NONE;
				case ETextureFormat::RGBA: return (GLenum)GL_NONE;
				case ETextureFormat::Depth: return (GLenum)GL_RGB8UI;
				case ETextureFormat::DepthStencil: return (GLenum)GL_NONE;
				case ETextureFormat::RGBE: return (GLenum)GL_NONE;
				default: return (GLenum)GL_NONE;
			}
			break;
		case EValueType::float32:
			switch(f)
			{
				case ETextureFormat::None: return (GLenum)GL_NONE;
				case ETextureFormat::R: return (GLenum)GL_R32F;
				case ETextureFormat::RG: return (GLenum)GL_RG32F;
				case ETextureFormat::RGB: return (GLenum)GL_RGB32F;
				case ETextureFormat::RGBA: return (GLenum)GL_RGBA32F;
				case ETextureFormat::Depth: return (GLenum)GL_R32F;
				case ETextureFormat::DepthStencil: return (GLenum)GL_NONE;
				case ETextureFormat::RGBE: return (GLenum)GL_RGBA32F;
				default: return (GLenum)GL_NONE;
			}
			break;
		case EValueType::float64:
			switch(f)
			{
				case ETextureFormat::None: return (GLenum)GL_NONE;
				case ETextureFormat::R: return (GLenum)GL_NONE;
				case ETextureFormat::RG: return (GLenum)GL_NONE;
				case ETextureFormat::RGB: return (GLenum)GL_NONE;
				case ETextureFormat::RGBA: return (GLenum)GL_NONE;
				case ETextureFormat::Depth: return (GLenum)GL_NONE;
				case ETextureFormat::DepthStencil: return (GLenum)GL_NONE;
				case ETextureFormat::RGBE: return (GLenum)GL_NONE;
				default: return (GLenum)GL_NONE;
			}
			break;
		default: return (GLenum)GL_NONE;
	}
	return (GLenum)GL_NONE;
}
inline GLenum glenumTypeless(const ETextureFormat& f, EValueType& v){
	switch(v)
	{
		case EValueType::uint8:
		case EValueType::int8:
			switch(f)
			{
				case ETextureFormat::None: return (GLenum)GL_NONE;
				case ETextureFormat::R: return (GLenum)GL_R8;
				case ETextureFormat::RG: return (GLenum)GL_RG8;
				case ETextureFormat::RGB: return (GLenum)GL_RGB8;
				case ETextureFormat::RGBA: return (GLenum)GL_RGBA8;
				case ETextureFormat::Depth: return (GLenum)GL_NONE;
				case ETextureFormat::DepthStencil: return (GLenum)GL_NONE;
				case ETextureFormat::RGBE: return (GLenum)GL_RGBA8;
				default: return (GLenum)GL_NONE;
			}
			break;
		case EValueType::uint16:
		case EValueType::int16:
			switch(f)
			{
				case ETextureFormat::None: return (GLenum)GL_NONE;
				//case ETextureFormat::R: return (GLenum)GL_R16;
				//case ETextureFormat::RG: return (GLenum)GL_RG16;
				//case ETextureFormat::RGB: return (GLenum)GL_RGB16;
				//case ETextureFormat::RGBA: return (GLenum)GL_RGBA16;
				case ETextureFormat::Depth: return (GLenum)GL_NONE;
				case ETextureFormat::DepthStencil: return (GLenum)GL_NONE;
				//case ETextureFormat::RGBE: return (GLenum)GL_RGBA16;
				default: return (GLenum)GL_NONE;
			}
			break;
		case EValueType::uint32:
		case EValueType::int32:
			switch(f)
			{
				case ETextureFormat::None: return (GLenum)GL_NONE;
				case ETextureFormat::R: return (GLenum)GL_R32UI;
				case ETextureFormat::RG: return (GLenum)GL_RG32UI;
				case ETextureFormat::RGB: return (GLenum)GL_RGB32UI;
				case ETextureFormat::RGBA: return (GLenum)GL_RGBA32UI;
				case ETextureFormat::Depth: return (GLenum)GL_NONE;
				case ETextureFormat::DepthStencil: return (GLenum)GL_NONE;
				case ETextureFormat::RGBE: return (GLenum)GL_RGBA32UI;
				default: return (GLenum)GL_NONE;
			}
			break;
		case EValueType::uint24:
			switch(f)
			{
				case ETextureFormat::None: return (GLenum)GL_NONE;
				case ETextureFormat::RGB: return (GLenum)GL_RGB8;
				case ETextureFormat::Depth: return (GLenum)GL_RGB8;
				default: return (GLenum)GL_NONE;
			}
			break;
		case EValueType::float16:
			switch(f)
			{
				case ETextureFormat::None: return (GLenum)GL_NONE;
				case ETextureFormat::R: return (GLenum)GL_R16F;
				case ETextureFormat::RG: return (GLenum)GL_RG16F;
				case ETextureFormat::RGB: return (GLenum)GL_RGB16F;
				case ETextureFormat::RGBA: return (GLenum)GL_RGBA16F;
				case ETextureFormat::Depth: return (GLenum)GL_R16F;
				case ETextureFormat::DepthStencil: return (GLenum)GL_NONE;
				case ETextureFormat::RGBE: return (GLenum)GL_RGBA16F;
				default: return (GLenum)GL_NONE;
			}
			break;
		case EValueType::float24:
			switch(f)
			{
				case ETextureFormat::None: return (GLenum)GL_NONE;
				case ETextureFormat::Depth: return (GLenum)GL_RGB8UI;
				default: return (GLenum)GL_NONE;
			}
			break;
		case EValueType::float32:
			switch(f)
			{
				case ETextureFormat::None: return (GLenum)GL_NONE;
				case ETextureFormat::R: return (GLenum)GL_R32F;
				case ETextureFormat::RG: return (GLenum)GL_RG32F;
				case ETextureFormat::RGB: return (GLenum)GL_RGB32F;
				case ETextureFormat::RGBA: return (GLenum)GL_RGBA32F;
				case ETextureFormat::Depth: return (GLenum)GL_R32F;
				case ETextureFormat::RGBE: return (GLenum)GL_RGBA32F;
				default: return (GLenum)GL_NONE;
			}
			break;
		case EValueType::float64:
		default: return (GLenum)GL_NONE;
	}
	return (GLenum)GL_NONE;
}
inline GLenum glenum(const EFillMode& v){
	switch(v)
	{
		case EFillMode::Point: return (GLenum)GL_POINTS;
		case EFillMode::Wireframe: return (GLenum)GL_LINES;
		case EFillMode::Solid: return (GLenum)GL_TRIANGLES;
		default: return (GLenum)GL_NONE;
	}
}
inline GLenum glenum(const ETextureType& v){
	switch(v)
	{
		//case ETextureType::Texture1D: return (GLenum)GL_TEXTURE_1D;
		case ETextureType::Texture2D: return (GLenum)GL_TEXTURE_2D;
		case ETextureType::Texture3D: return (GLenum)GL_TEXTURE_3D;
		case ETextureType::TextureCube: return (GLenum)GL_TEXTURE_CUBE_MAP;
		//case ETextureType::Texture1DArray: return (GLenum)GL_TEXTURE_1D_ARRAY;
		case ETextureType::Texture2DArray: return (GLenum)GL_TEXTURE_2D_ARRAY;
		//case ETextureType::TextureCubeArray: return (GLenum)GL_TEXTURE_CUBE_MAP_ARRAY;
		default: return (GLenum)GL_NONE;
	}
}
inline GLenum glenum(const ETextureWrapping& v){
	switch(v)
	{
		case ETextureWrapping::Clamp: return (GLenum)GL_CLAMP_TO_EDGE;
		case ETextureWrapping::Wrap: return (GLenum)GL_REPEAT;
		case ETextureWrapping::Mirror: return (GLenum)GL_MIRRORED_REPEAT;
		default: return (GLenum)GL_NONE;
	}
}
inline GLenum glenum(const ETextureFiltering& f, const ETextureFiltering& mipf){
	switch(f)
	{
		case ETextureFiltering::Nearest: 
			switch(mipf)
			{
				case ETextureFiltering::None:
					return GL_NEAREST;
				case ETextureFiltering::Nearest:
					return GL_NEAREST_MIPMAP_NEAREST;
				case ETextureFiltering::Linear:
					return GL_NEAREST_MIPMAP_LINEAR;
				case ETextureFiltering::Anisotropic:
				default: return (GLenum)GL_NONE;
			}
		case ETextureFiltering::Linear:
			switch(mipf)
			{
				case ETextureFiltering::None:
					return GL_LINEAR;
				case ETextureFiltering::Nearest:
					return GL_LINEAR_MIPMAP_NEAREST;
				case ETextureFiltering::Linear:
					return GL_LINEAR_MIPMAP_LINEAR;
				case ETextureFiltering::Anisotropic:
				default: return (GLenum)GL_NONE;
			}
		case ETextureFiltering::Anisotropic:
			switch(mipf)
			{
				case ETextureFiltering::None:
				case ETextureFiltering::Nearest:
				case ETextureFiltering::Linear:
				case ETextureFiltering::Anisotropic:
				default: return (GLenum)GL_NONE;
			}
		default: return (GLenum)GL_NONE;
	}
}
inline GLenum glenum(const EValueType& v){
	switch(v)
	{
		case EValueType::int8: return (GLenum)GL_BYTE;
		case EValueType::int16: return (GLenum)GL_SHORT;
		case EValueType::int32: return (GLenum)GL_INT;
		case EValueType::uint8: return (GLenum)GL_UNSIGNED_BYTE;
		case EValueType::uint16: return (GLenum)GL_UNSIGNED_SHORT;
		case EValueType::uint24: return (GLenum)GL_UNSIGNED_INT_24_8;
		case EValueType::uint32: return (GLenum)GL_UNSIGNED_INT;
		case EValueType::float16: return (GLenum)GL_HALF_FLOAT;
		case EValueType::float24: return (GLenum)GL_FLOAT_32_UNSIGNED_INT_24_8_REV;
		case EValueType::float32: return (GLenum)GL_FLOAT;
		//case EValueType::float64: return (GLenum)GL_DOUBLE;
		default: return (GLenum)GL_NONE;
	}
}

inline GLenum glenum(const EBufferType& t){
	switch(t)
	{
		case EBufferType::Vertex: return (GLenum)GL_ARRAY_BUFFER;
		case EBufferType::Index: return (GLenum)GL_ELEMENT_ARRAY_BUFFER;
		case EBufferType::Uniform: return (GLenum)GL_UNIFORM_BUFFER;
		case EBufferType::Staging: return (GLenum)GL_NONE;
		default: return (GLenum)GL_NONE;
	}
}

inline GLenum glenumRead(const EShaderResourceUsageType& usage){
	switch(usage)
	{
		case EShaderResourceUsageType::Static: return (GLenum)GL_STATIC_READ;
		case EShaderResourceUsageType::Stream: return (GLenum)GL_STREAM_READ;
		case EShaderResourceUsageType::Dynamic: return (GLenum)GL_DYNAMIC_READ;
		default: return GL_NONE;
	}
}
inline GLenum glenumCopy(const EShaderResourceUsageType& usage){
	switch(usage)
	{
		case EShaderResourceUsageType::Static: return (GLenum)GL_STATIC_COPY;
		case EShaderResourceUsageType::Stream: return (GLenum)GL_STREAM_COPY;
		case EShaderResourceUsageType::Dynamic: return (GLenum)GL_DYNAMIC_COPY;
		default: return GL_NONE;
	}
}
inline GLenum glenumDraw(const EShaderResourceUsageType& usage){
	switch(usage)
	{
		case EShaderResourceUsageType::Static: return (GLenum)GL_STATIC_DRAW;
		case EShaderResourceUsageType::Stream: return (GLenum)GL_STREAM_DRAW;
		case EShaderResourceUsageType::Dynamic: return (GLenum)GL_DYNAMIC_DRAW;
		default: return GL_NONE;
	}
}

inline GLenum glenum(const EShaderResourceUsageType& u, const EShaderResourceAccessType& a){
	switch(a)
	{
		case EShaderResourceAccessType::Read: return glenumRead(u);
		case EShaderResourceAccessType::Copy: return glenumCopy(u);
		case EShaderResourceAccessType::Draw: return glenumDraw(u);
		default: return GL_NONE;
	}
}

inline const char* glenumToString(GLenum e){
	#define rd_case_return_string(x) case x: return #x
	switch(e)
	{
		rd_case_return_string(GL_FALSE);
		rd_case_return_string(GL_TRUE);
		/*rd_case_return_string(GL_POINTS);
		rd_case_return_string(GL_LINES);
		rd_case_return_string(GL_ZERO);
		rd_case_return_string(GL_ONE);
		rd_case_return_string(GL_NO_ERROR);
		rd_case_return_string(GL_NONE);*/
		rd_case_return_string(GL_BLEND_EQUATION);//same as GL_BLEND_EQUATION_RGB
		//rd_case_return_string(GL_BLEND_EQUATION_RGB);
		rd_case_return_string(GL_DEPTH_BUFFER_BIT);
		rd_case_return_string(GL_STENCIL_BUFFER_BIT);
		rd_case_return_string(GL_COLOR_BUFFER_BIT);
		rd_case_return_string(GL_LINE_LOOP);
		rd_case_return_string(GL_LINE_STRIP);
		rd_case_return_string(GL_TRIANGLES);
		rd_case_return_string(GL_TRIANGLE_STRIP);
		rd_case_return_string(GL_TRIANGLE_FAN);
		rd_case_return_string(GL_SRC_COLOR);
		rd_case_return_string(GL_ONE_MINUS_SRC_COLOR);
		rd_case_return_string(GL_SRC_ALPHA);
		rd_case_return_string(GL_ONE_MINUS_SRC_ALPHA);
		rd_case_return_string(GL_DST_ALPHA);
		rd_case_return_string(GL_ONE_MINUS_DST_ALPHA);
		rd_case_return_string(GL_DST_COLOR);
		rd_case_return_string(GL_ONE_MINUS_DST_COLOR);
		rd_case_return_string(GL_SRC_ALPHA_SATURATE);
		rd_case_return_string(GL_FUNC_ADD);
		rd_case_return_string(GL_BLEND_EQUATION_ALPHA);
		rd_case_return_string(GL_FUNC_SUBTRACT);
		rd_case_return_string(GL_FUNC_REVERSE_SUBTRACT);
		rd_case_return_string(GL_BLEND_DST_RGB);
		rd_case_return_string(GL_BLEND_SRC_RGB);
		rd_case_return_string(GL_BLEND_DST_ALPHA);
		rd_case_return_string(GL_BLEND_SRC_ALPHA);
		rd_case_return_string(GL_CONSTANT_COLOR);
		rd_case_return_string(GL_ONE_MINUS_CONSTANT_COLOR);
		rd_case_return_string(GL_CONSTANT_ALPHA);
		rd_case_return_string(GL_ONE_MINUS_CONSTANT_ALPHA);
		rd_case_return_string(GL_BLEND_COLOR);
		rd_case_return_string(GL_ARRAY_BUFFER);
		rd_case_return_string(GL_ELEMENT_ARRAY_BUFFER);
		rd_case_return_string(GL_ARRAY_BUFFER_BINDING);
		rd_case_return_string(GL_ELEMENT_ARRAY_BUFFER_BINDING);
		rd_case_return_string(GL_STREAM_DRAW);
		rd_case_return_string(GL_STATIC_DRAW);
		rd_case_return_string(GL_DYNAMIC_DRAW);
		rd_case_return_string(GL_BUFFER_SIZE);
		rd_case_return_string(GL_BUFFER_USAGE);
		rd_case_return_string(GL_CURRENT_VERTEX_ATTRIB);
		rd_case_return_string(GL_FRONT);
		rd_case_return_string(GL_BACK);
		rd_case_return_string(GL_FRONT_AND_BACK);
		rd_case_return_string(GL_TEXTURE_2D);
		rd_case_return_string(GL_CULL_FACE);
		rd_case_return_string(GL_BLEND);
		rd_case_return_string(GL_DITHER);
		rd_case_return_string(GL_STENCIL_TEST);
		rd_case_return_string(GL_DEPTH_TEST);
		rd_case_return_string(GL_SCISSOR_TEST);
		rd_case_return_string(GL_POLYGON_OFFSET_FILL);
		rd_case_return_string(GL_SAMPLE_ALPHA_TO_COVERAGE);
		rd_case_return_string(GL_SAMPLE_COVERAGE);
		rd_case_return_string(GL_INVALID_ENUM);
		rd_case_return_string(GL_INVALID_VALUE);
		rd_case_return_string(GL_INVALID_OPERATION);
		rd_case_return_string(GL_OUT_OF_MEMORY);
		rd_case_return_string(GL_CW);
		rd_case_return_string(GL_CCW);
		rd_case_return_string(GL_LINE_WIDTH);
		rd_case_return_string(GL_ALIASED_POINT_SIZE_RANGE);
		rd_case_return_string(GL_ALIASED_LINE_WIDTH_RANGE);
		rd_case_return_string(GL_CULL_FACE_MODE);
		rd_case_return_string(GL_FRONT_FACE);
		rd_case_return_string(GL_DEPTH_RANGE);
		rd_case_return_string(GL_DEPTH_WRITEMASK);
		rd_case_return_string(GL_DEPTH_CLEAR_VALUE);
		rd_case_return_string(GL_DEPTH_FUNC);
		rd_case_return_string(GL_STENCIL_CLEAR_VALUE);
		rd_case_return_string(GL_STENCIL_FUNC);
		rd_case_return_string(GL_STENCIL_FAIL);
		rd_case_return_string(GL_STENCIL_PASS_DEPTH_FAIL);
		rd_case_return_string(GL_STENCIL_PASS_DEPTH_PASS);
		rd_case_return_string(GL_STENCIL_REF);
		rd_case_return_string(GL_STENCIL_VALUE_MASK);
		rd_case_return_string(GL_STENCIL_WRITEMASK);
		rd_case_return_string(GL_STENCIL_BACK_FUNC);
		rd_case_return_string(GL_STENCIL_BACK_FAIL);
		rd_case_return_string(GL_STENCIL_BACK_PASS_DEPTH_FAIL);
		rd_case_return_string(GL_STENCIL_BACK_PASS_DEPTH_PASS);
		rd_case_return_string(GL_STENCIL_BACK_REF);
		rd_case_return_string(GL_STENCIL_BACK_VALUE_MASK);
		rd_case_return_string(GL_STENCIL_BACK_WRITEMASK);
		rd_case_return_string(GL_VIEWPORT);
		rd_case_return_string(GL_SCISSOR_BOX);
		rd_case_return_string(GL_COLOR_CLEAR_VALUE);
		rd_case_return_string(GL_COLOR_WRITEMASK);
		rd_case_return_string(GL_UNPACK_ALIGNMENT);
		rd_case_return_string(GL_PACK_ALIGNMENT);
		rd_case_return_string(GL_MAX_TEXTURE_SIZE);
		rd_case_return_string(GL_MAX_VIEWPORT_DIMS);
		rd_case_return_string(GL_SUBPIXEL_BITS);
		rd_case_return_string(GL_RED_BITS);
		rd_case_return_string(GL_GREEN_BITS);
		rd_case_return_string(GL_BLUE_BITS);
		rd_case_return_string(GL_ALPHA_BITS);
		rd_case_return_string(GL_DEPTH_BITS);
		rd_case_return_string(GL_STENCIL_BITS);
		rd_case_return_string(GL_POLYGON_OFFSET_UNITS);
		rd_case_return_string(GL_POLYGON_OFFSET_FACTOR);
		rd_case_return_string(GL_TEXTURE_BINDING_2D);
		rd_case_return_string(GL_SAMPLE_BUFFERS);
		rd_case_return_string(GL_SAMPLES);
		rd_case_return_string(GL_SAMPLE_COVERAGE_VALUE);
		rd_case_return_string(GL_SAMPLE_COVERAGE_INVERT);
		rd_case_return_string(GL_NUM_COMPRESSED_TEXTURE_FORMATS);
		rd_case_return_string(GL_COMPRESSED_TEXTURE_FORMATS);
		rd_case_return_string(GL_DONT_CARE);
		rd_case_return_string(GL_FASTEST);
		rd_case_return_string(GL_NICEST);
		rd_case_return_string(GL_GENERATE_MIPMAP_HINT);
		rd_case_return_string(GL_BYTE);
		rd_case_return_string(GL_UNSIGNED_BYTE);
		rd_case_return_string(GL_SHORT);
		rd_case_return_string(GL_UNSIGNED_SHORT);
		rd_case_return_string(GL_INT);
		rd_case_return_string(GL_UNSIGNED_INT);
		rd_case_return_string(GL_FLOAT);
		rd_case_return_string(GL_FIXED);
		rd_case_return_string(GL_DEPTH_COMPONENT);
		rd_case_return_string(GL_ALPHA);
		rd_case_return_string(GL_RGB);
		rd_case_return_string(GL_RGBA);
		rd_case_return_string(GL_LUMINANCE);
		rd_case_return_string(GL_LUMINANCE_ALPHA);
		rd_case_return_string(GL_UNSIGNED_SHORT_4_4_4_4);
		rd_case_return_string(GL_UNSIGNED_SHORT_5_5_5_1);
		rd_case_return_string(GL_UNSIGNED_SHORT_5_6_5);
		rd_case_return_string(GL_FRAGMENT_SHADER);
		rd_case_return_string(GL_VERTEX_SHADER);
		rd_case_return_string(GL_MAX_VERTEX_ATTRIBS);
		rd_case_return_string(GL_MAX_VERTEX_UNIFORM_VECTORS);
		rd_case_return_string(GL_MAX_VARYING_VECTORS);
		rd_case_return_string(GL_MAX_COMBINED_TEXTURE_IMAGE_UNITS);
		rd_case_return_string(GL_MAX_VERTEX_TEXTURE_IMAGE_UNITS);
		rd_case_return_string(GL_MAX_TEXTURE_IMAGE_UNITS);
		rd_case_return_string(GL_MAX_FRAGMENT_UNIFORM_VECTORS);
		rd_case_return_string(GL_SHADER_TYPE);
		rd_case_return_string(GL_DELETE_STATUS);
		rd_case_return_string(GL_LINK_STATUS);
		rd_case_return_string(GL_VALIDATE_STATUS);
		rd_case_return_string(GL_ATTACHED_SHADERS);
		rd_case_return_string(GL_ACTIVE_UNIFORMS);
		rd_case_return_string(GL_ACTIVE_UNIFORM_MAX_LENGTH);
		rd_case_return_string(GL_ACTIVE_ATTRIBUTES);
		rd_case_return_string(GL_ACTIVE_ATTRIBUTE_MAX_LENGTH);
		rd_case_return_string(GL_SHADING_LANGUAGE_VERSION);
		rd_case_return_string(GL_CURRENT_PROGRAM);
		rd_case_return_string(GL_NEVER);
		rd_case_return_string(GL_LESS);
		rd_case_return_string(GL_EQUAL);
		rd_case_return_string(GL_LEQUAL);
		rd_case_return_string(GL_GREATER);
		rd_case_return_string(GL_NOTEQUAL);
		rd_case_return_string(GL_GEQUAL);
		rd_case_return_string(GL_ALWAYS);
		rd_case_return_string(GL_KEEP);
		rd_case_return_string(GL_REPLACE);
		rd_case_return_string(GL_INCR);
		rd_case_return_string(GL_DECR);
		rd_case_return_string(GL_INVERT);
		rd_case_return_string(GL_INCR_WRAP);
		rd_case_return_string(GL_DECR_WRAP);
		rd_case_return_string(GL_VENDOR);
		rd_case_return_string(GL_RENDERER);
		rd_case_return_string(GL_VERSION);
		rd_case_return_string(GL_EXTENSIONS);
		rd_case_return_string(GL_NEAREST);
		rd_case_return_string(GL_LINEAR);
		rd_case_return_string(GL_NEAREST_MIPMAP_NEAREST);
		rd_case_return_string(GL_LINEAR_MIPMAP_NEAREST);
		rd_case_return_string(GL_NEAREST_MIPMAP_LINEAR);
		rd_case_return_string(GL_LINEAR_MIPMAP_LINEAR);
		rd_case_return_string(GL_TEXTURE_MAG_FILTER);
		rd_case_return_string(GL_TEXTURE_MIN_FILTER);
		rd_case_return_string(GL_TEXTURE_WRAP_S);
		rd_case_return_string(GL_TEXTURE_WRAP_T);
		rd_case_return_string(GL_TEXTURE);
		rd_case_return_string(GL_TEXTURE_CUBE_MAP);
		rd_case_return_string(GL_TEXTURE_BINDING_CUBE_MAP);
		rd_case_return_string(GL_TEXTURE_CUBE_MAP_POSITIVE_X);
		rd_case_return_string(GL_TEXTURE_CUBE_MAP_NEGATIVE_X);
		rd_case_return_string(GL_TEXTURE_CUBE_MAP_POSITIVE_Y);
		rd_case_return_string(GL_TEXTURE_CUBE_MAP_NEGATIVE_Y);
		rd_case_return_string(GL_TEXTURE_CUBE_MAP_POSITIVE_Z);
		rd_case_return_string(GL_TEXTURE_CUBE_MAP_NEGATIVE_Z);
		rd_case_return_string(GL_MAX_CUBE_MAP_TEXTURE_SIZE);
		rd_case_return_string(GL_TEXTURE0);
		rd_case_return_string(GL_TEXTURE1);
		rd_case_return_string(GL_TEXTURE2);
		rd_case_return_string(GL_TEXTURE3);
		rd_case_return_string(GL_TEXTURE4);
		rd_case_return_string(GL_TEXTURE5);
		rd_case_return_string(GL_TEXTURE6);
		rd_case_return_string(GL_TEXTURE7);
		rd_case_return_string(GL_TEXTURE8);
		rd_case_return_string(GL_TEXTURE9);
		rd_case_return_string(GL_TEXTURE10);
		rd_case_return_string(GL_TEXTURE11);
		rd_case_return_string(GL_TEXTURE12);
		rd_case_return_string(GL_TEXTURE13);
		rd_case_return_string(GL_TEXTURE14);
		rd_case_return_string(GL_TEXTURE15);
		rd_case_return_string(GL_TEXTURE16);
		rd_case_return_string(GL_TEXTURE17);
		rd_case_return_string(GL_TEXTURE18);
		rd_case_return_string(GL_TEXTURE19);
		rd_case_return_string(GL_TEXTURE20);
		rd_case_return_string(GL_TEXTURE21);
		rd_case_return_string(GL_TEXTURE22);
		rd_case_return_string(GL_TEXTURE23);
		rd_case_return_string(GL_TEXTURE24);
		rd_case_return_string(GL_TEXTURE25);
		rd_case_return_string(GL_TEXTURE26);
		rd_case_return_string(GL_TEXTURE27);
		rd_case_return_string(GL_TEXTURE28);
		rd_case_return_string(GL_TEXTURE29);
		rd_case_return_string(GL_TEXTURE30);
		rd_case_return_string(GL_TEXTURE31);
		rd_case_return_string(GL_ACTIVE_TEXTURE);
		rd_case_return_string(GL_REPEAT);
		rd_case_return_string(GL_CLAMP_TO_EDGE);
		rd_case_return_string(GL_MIRRORED_REPEAT);
		rd_case_return_string(GL_FLOAT_VEC2);
		rd_case_return_string(GL_FLOAT_VEC3);
		rd_case_return_string(GL_FLOAT_VEC4);
		rd_case_return_string(GL_INT_VEC2);
		rd_case_return_string(GL_INT_VEC3);
		rd_case_return_string(GL_INT_VEC4);
		rd_case_return_string(GL_BOOL);
		rd_case_return_string(GL_BOOL_VEC2);
		rd_case_return_string(GL_BOOL_VEC3);
		rd_case_return_string(GL_BOOL_VEC4);
		rd_case_return_string(GL_FLOAT_MAT2);
		rd_case_return_string(GL_FLOAT_MAT3);
		rd_case_return_string(GL_FLOAT_MAT4);
		rd_case_return_string(GL_SAMPLER_2D);
		rd_case_return_string(GL_SAMPLER_CUBE);
		rd_case_return_string(GL_VERTEX_ATTRIB_ARRAY_ENABLED);
		rd_case_return_string(GL_VERTEX_ATTRIB_ARRAY_SIZE);
		rd_case_return_string(GL_VERTEX_ATTRIB_ARRAY_STRIDE);
		rd_case_return_string(GL_VERTEX_ATTRIB_ARRAY_TYPE);
		rd_case_return_string(GL_VERTEX_ATTRIB_ARRAY_NORMALIZED);
		rd_case_return_string(GL_VERTEX_ATTRIB_ARRAY_POINTER);
		rd_case_return_string(GL_VERTEX_ATTRIB_ARRAY_BUFFER_BINDING);
		rd_case_return_string(GL_IMPLEMENTATION_COLOR_READ_TYPE);
		rd_case_return_string(GL_IMPLEMENTATION_COLOR_READ_FORMAT);
		rd_case_return_string(GL_COMPILE_STATUS);
		rd_case_return_string(GL_INFO_LOG_LENGTH);
		rd_case_return_string(GL_SHADER_SOURCE_LENGTH);
		rd_case_return_string(GL_SHADER_COMPILER);
		rd_case_return_string(GL_SHADER_BINARY_FORMATS);
		rd_case_return_string(GL_NUM_SHADER_BINARY_FORMATS);
		rd_case_return_string(GL_LOW_FLOAT);
		rd_case_return_string(GL_MEDIUM_FLOAT);
		rd_case_return_string(GL_HIGH_FLOAT);
		rd_case_return_string(GL_LOW_INT);
		rd_case_return_string(GL_MEDIUM_INT);
		rd_case_return_string(GL_HIGH_INT);
		rd_case_return_string(GL_FRAMEBUFFER);
		rd_case_return_string(GL_RENDERBUFFER);
		rd_case_return_string(GL_RGBA4);
		rd_case_return_string(GL_RGB5_A1);
		rd_case_return_string(GL_RGB565);
		rd_case_return_string(GL_DEPTH_COMPONENT16);
		rd_case_return_string(GL_STENCIL_INDEX8);
		rd_case_return_string(GL_RENDERBUFFER_WIDTH);
		rd_case_return_string(GL_RENDERBUFFER_HEIGHT);
		rd_case_return_string(GL_RENDERBUFFER_INTERNAL_FORMAT);
		rd_case_return_string(GL_RENDERBUFFER_RED_SIZE);
		rd_case_return_string(GL_RENDERBUFFER_GREEN_SIZE);
		rd_case_return_string(GL_RENDERBUFFER_BLUE_SIZE);
		rd_case_return_string(GL_RENDERBUFFER_ALPHA_SIZE);
		rd_case_return_string(GL_RENDERBUFFER_DEPTH_SIZE);
		rd_case_return_string(GL_RENDERBUFFER_STENCIL_SIZE);
		rd_case_return_string(GL_FRAMEBUFFER_ATTACHMENT_OBJECT_TYPE);
		rd_case_return_string(GL_FRAMEBUFFER_ATTACHMENT_OBJECT_NAME);
		rd_case_return_string(GL_FRAMEBUFFER_ATTACHMENT_TEXTURE_LEVEL);
		rd_case_return_string(GL_FRAMEBUFFER_ATTACHMENT_TEXTURE_CUBE_MAP_FACE);
		rd_case_return_string(GL_COLOR_ATTACHMENT0);
		rd_case_return_string(GL_DEPTH_ATTACHMENT);
		rd_case_return_string(GL_STENCIL_ATTACHMENT);
		rd_case_return_string(GL_FRAMEBUFFER_COMPLETE);
		rd_case_return_string(GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT);
		rd_case_return_string(GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT);
		rd_case_return_string(GL_FRAMEBUFFER_INCOMPLETE_DIMENSIONS);
		rd_case_return_string(GL_FRAMEBUFFER_UNSUPPORTED);
		rd_case_return_string(GL_FRAMEBUFFER_BINDING);
		rd_case_return_string(GL_RENDERBUFFER_BINDING);
		rd_case_return_string(GL_MAX_RENDERBUFFER_SIZE);
		rd_case_return_string(GL_INVALID_FRAMEBUFFER_OPERATION);
		rd_case_return_string(GL_READ_BUFFER);
		rd_case_return_string(GL_UNPACK_ROW_LENGTH);
		rd_case_return_string(GL_UNPACK_SKIP_ROWS);
		rd_case_return_string(GL_UNPACK_SKIP_PIXELS);
		rd_case_return_string(GL_PACK_ROW_LENGTH);
		rd_case_return_string(GL_PACK_SKIP_ROWS);
		rd_case_return_string(GL_PACK_SKIP_PIXELS);
		rd_case_return_string(GL_COLOR);
		rd_case_return_string(GL_DEPTH);
		rd_case_return_string(GL_STENCIL);
		rd_case_return_string(GL_RED);
		rd_case_return_string(GL_RGB8);
		rd_case_return_string(GL_RGBA8);
		rd_case_return_string(GL_RGB10_A2);
		rd_case_return_string(GL_TEXTURE_BINDING_3D);
		rd_case_return_string(GL_UNPACK_SKIP_IMAGES);
		rd_case_return_string(GL_UNPACK_IMAGE_HEIGHT);
		rd_case_return_string(GL_TEXTURE_3D);
		rd_case_return_string(GL_TEXTURE_WRAP_R);
		rd_case_return_string(GL_MAX_3D_TEXTURE_SIZE);
		rd_case_return_string(GL_UNSIGNED_INT_2_10_10_10_REV);
		rd_case_return_string(GL_MAX_ELEMENTS_VERTICES);
		rd_case_return_string(GL_MAX_ELEMENTS_INDICES);
		rd_case_return_string(GL_TEXTURE_MIN_LOD);
		rd_case_return_string(GL_TEXTURE_MAX_LOD);
		rd_case_return_string(GL_TEXTURE_BASE_LEVEL);
		rd_case_return_string(GL_TEXTURE_MAX_LEVEL);
		rd_case_return_string(GL_MIN);
		rd_case_return_string(GL_MAX);
		rd_case_return_string(GL_DEPTH_COMPONENT24);
		rd_case_return_string(GL_MAX_TEXTURE_LOD_BIAS);
		rd_case_return_string(GL_TEXTURE_COMPARE_MODE);
		rd_case_return_string(GL_TEXTURE_COMPARE_FUNC);
		rd_case_return_string(GL_CURRENT_QUERY);
		rd_case_return_string(GL_QUERY_RESULT);
		rd_case_return_string(GL_QUERY_RESULT_AVAILABLE);
		rd_case_return_string(GL_BUFFER_MAPPED);
		rd_case_return_string(GL_BUFFER_MAP_POINTER);
		rd_case_return_string(GL_STREAM_READ);
		rd_case_return_string(GL_STREAM_COPY);
		rd_case_return_string(GL_STATIC_READ);
		rd_case_return_string(GL_STATIC_COPY);
		rd_case_return_string(GL_DYNAMIC_READ);
		rd_case_return_string(GL_DYNAMIC_COPY);
		rd_case_return_string(GL_MAX_DRAW_BUFFERS);
		rd_case_return_string(GL_DRAW_BUFFER0);
		rd_case_return_string(GL_DRAW_BUFFER1);
		rd_case_return_string(GL_DRAW_BUFFER2);
		rd_case_return_string(GL_DRAW_BUFFER3);
		rd_case_return_string(GL_DRAW_BUFFER4);
		rd_case_return_string(GL_DRAW_BUFFER5);
		rd_case_return_string(GL_DRAW_BUFFER6);
		rd_case_return_string(GL_DRAW_BUFFER7);
		rd_case_return_string(GL_DRAW_BUFFER8);
		rd_case_return_string(GL_DRAW_BUFFER9);
		rd_case_return_string(GL_DRAW_BUFFER10);
		rd_case_return_string(GL_DRAW_BUFFER11);
		rd_case_return_string(GL_DRAW_BUFFER12);
		rd_case_return_string(GL_DRAW_BUFFER13);
		rd_case_return_string(GL_DRAW_BUFFER14);
		rd_case_return_string(GL_DRAW_BUFFER15);
		rd_case_return_string(GL_MAX_FRAGMENT_UNIFORM_COMPONENTS);
		rd_case_return_string(GL_MAX_VERTEX_UNIFORM_COMPONENTS);
		rd_case_return_string(GL_SAMPLER_3D);
		rd_case_return_string(GL_SAMPLER_2D_SHADOW);
		rd_case_return_string(GL_FRAGMENT_SHADER_DERIVATIVE_HINT);
		rd_case_return_string(GL_PIXEL_PACK_BUFFER);
		rd_case_return_string(GL_PIXEL_UNPACK_BUFFER);
		rd_case_return_string(GL_PIXEL_PACK_BUFFER_BINDING);
		rd_case_return_string(GL_PIXEL_UNPACK_BUFFER_BINDING);
		rd_case_return_string(GL_FLOAT_MAT2x3);
		rd_case_return_string(GL_FLOAT_MAT2x4);
		rd_case_return_string(GL_FLOAT_MAT3x2);
		rd_case_return_string(GL_FLOAT_MAT3x4);
		rd_case_return_string(GL_FLOAT_MAT4x2);
		rd_case_return_string(GL_FLOAT_MAT4x3);
		rd_case_return_string(GL_SRGB);
		rd_case_return_string(GL_SRGB8);
		rd_case_return_string(GL_SRGB8_ALPHA8);
		rd_case_return_string(GL_COMPARE_REF_TO_TEXTURE);
		rd_case_return_string(GL_MAJOR_VERSION);
		rd_case_return_string(GL_MINOR_VERSION);
		rd_case_return_string(GL_NUM_EXTENSIONS);
		rd_case_return_string(GL_RGBA32F);
		rd_case_return_string(GL_RGB32F);
		rd_case_return_string(GL_RGBA16F);
		rd_case_return_string(GL_RGB16F);
		rd_case_return_string(GL_VERTEX_ATTRIB_ARRAY_INTEGER);
		rd_case_return_string(GL_MAX_ARRAY_TEXTURE_LAYERS);
		rd_case_return_string(GL_MIN_PROGRAM_TEXEL_OFFSET);
		rd_case_return_string(GL_MAX_PROGRAM_TEXEL_OFFSET);
		rd_case_return_string(GL_MAX_VARYING_COMPONENTS);
		rd_case_return_string(GL_TEXTURE_2D_ARRAY);
		rd_case_return_string(GL_TEXTURE_BINDING_2D_ARRAY);
		rd_case_return_string(GL_R11F_G11F_B10F);
		rd_case_return_string(GL_UNSIGNED_INT_10F_11F_11F_REV);
		rd_case_return_string(GL_RGB9_E5);
		rd_case_return_string(GL_UNSIGNED_INT_5_9_9_9_REV);
		rd_case_return_string(GL_TRANSFORM_FEEDBACK_VARYING_MAX_LENGTH);
		rd_case_return_string(GL_TRANSFORM_FEEDBACK_BUFFER_MODE);
		rd_case_return_string(GL_MAX_TRANSFORM_FEEDBACK_SEPARATE_COMPONENTS);
		rd_case_return_string(GL_TRANSFORM_FEEDBACK_VARYINGS);
		rd_case_return_string(GL_TRANSFORM_FEEDBACK_BUFFER_START);
		rd_case_return_string(GL_TRANSFORM_FEEDBACK_BUFFER_SIZE);
		rd_case_return_string(GL_TRANSFORM_FEEDBACK_PRIMITIVES_WRITTEN);
		rd_case_return_string(GL_RASTERIZER_DISCARD);
		rd_case_return_string(GL_MAX_TRANSFORM_FEEDBACK_INTERLEAVED_COMPONENTS);
		rd_case_return_string(GL_MAX_TRANSFORM_FEEDBACK_SEPARATE_ATTRIBS);
		rd_case_return_string(GL_INTERLEAVED_ATTRIBS);
		rd_case_return_string(GL_SEPARATE_ATTRIBS);
		rd_case_return_string(GL_TRANSFORM_FEEDBACK_BUFFER);
		rd_case_return_string(GL_TRANSFORM_FEEDBACK_BUFFER_BINDING);
		rd_case_return_string(GL_RGBA32UI);
		rd_case_return_string(GL_RGB32UI);
		rd_case_return_string(GL_RGBA16UI);
		rd_case_return_string(GL_RGB16UI);
		rd_case_return_string(GL_RGBA8UI);
		rd_case_return_string(GL_RGB8UI);
		rd_case_return_string(GL_RGBA32I);
		rd_case_return_string(GL_RGB32I);
		rd_case_return_string(GL_RGBA16I);
		rd_case_return_string(GL_RGB16I);
		rd_case_return_string(GL_RGBA8I);
		rd_case_return_string(GL_RGB8I);
		rd_case_return_string(GL_RED_INTEGER);
		rd_case_return_string(GL_RGB_INTEGER);
		rd_case_return_string(GL_RGBA_INTEGER);
		rd_case_return_string(GL_SAMPLER_2D_ARRAY);
		rd_case_return_string(GL_SAMPLER_2D_ARRAY_SHADOW);
		rd_case_return_string(GL_SAMPLER_CUBE_SHADOW);
		rd_case_return_string(GL_UNSIGNED_INT_VEC2);
		rd_case_return_string(GL_UNSIGNED_INT_VEC3);
		rd_case_return_string(GL_UNSIGNED_INT_VEC4);
		rd_case_return_string(GL_INT_SAMPLER_2D);
		rd_case_return_string(GL_INT_SAMPLER_3D);
		rd_case_return_string(GL_INT_SAMPLER_CUBE);
		rd_case_return_string(GL_INT_SAMPLER_2D_ARRAY);
		rd_case_return_string(GL_UNSIGNED_INT_SAMPLER_2D);
		rd_case_return_string(GL_UNSIGNED_INT_SAMPLER_3D);
		rd_case_return_string(GL_UNSIGNED_INT_SAMPLER_CUBE);
		rd_case_return_string(GL_UNSIGNED_INT_SAMPLER_2D_ARRAY);
		rd_case_return_string(GL_BUFFER_ACCESS_FLAGS);
		rd_case_return_string(GL_BUFFER_MAP_LENGTH);
		rd_case_return_string(GL_BUFFER_MAP_OFFSET);
		rd_case_return_string(GL_DEPTH_COMPONENT32F);
		rd_case_return_string(GL_DEPTH32F_STENCIL8);
		rd_case_return_string(GL_FLOAT_32_UNSIGNED_INT_24_8_REV);
		rd_case_return_string(GL_FRAMEBUFFER_ATTACHMENT_COLOR_ENCODING);
		rd_case_return_string(GL_FRAMEBUFFER_ATTACHMENT_COMPONENT_TYPE);
		rd_case_return_string(GL_FRAMEBUFFER_ATTACHMENT_RED_SIZE);
		rd_case_return_string(GL_FRAMEBUFFER_ATTACHMENT_GREEN_SIZE);
		rd_case_return_string(GL_FRAMEBUFFER_ATTACHMENT_BLUE_SIZE);
		rd_case_return_string(GL_FRAMEBUFFER_ATTACHMENT_ALPHA_SIZE);
		rd_case_return_string(GL_FRAMEBUFFER_ATTACHMENT_DEPTH_SIZE);
		rd_case_return_string(GL_FRAMEBUFFER_ATTACHMENT_STENCIL_SIZE);
		rd_case_return_string(GL_FRAMEBUFFER_DEFAULT);
		rd_case_return_string(GL_FRAMEBUFFER_UNDEFINED);
		rd_case_return_string(GL_DEPTH_STENCIL_ATTACHMENT);
		rd_case_return_string(GL_DEPTH_STENCIL);
		rd_case_return_string(GL_UNSIGNED_INT_24_8);
		rd_case_return_string(GL_DEPTH24_STENCIL8);
		rd_case_return_string(GL_UNSIGNED_NORMALIZED);
		//rd_case_return_string(GL_DRAW_FRAMEBUFFER_BINDING); //same as GL_FRAMEBUFFER_BINDING
		rd_case_return_string(GL_READ_FRAMEBUFFER);
		rd_case_return_string(GL_DRAW_FRAMEBUFFER);
		rd_case_return_string(GL_READ_FRAMEBUFFER_BINDING);
		rd_case_return_string(GL_RENDERBUFFER_SAMPLES);
		rd_case_return_string(GL_FRAMEBUFFER_ATTACHMENT_TEXTURE_LAYER);
		rd_case_return_string(GL_MAX_COLOR_ATTACHMENTS);
		rd_case_return_string(GL_COLOR_ATTACHMENT1);
		rd_case_return_string(GL_COLOR_ATTACHMENT2);
		rd_case_return_string(GL_COLOR_ATTACHMENT3);
		rd_case_return_string(GL_COLOR_ATTACHMENT4);
		rd_case_return_string(GL_COLOR_ATTACHMENT5);
		rd_case_return_string(GL_COLOR_ATTACHMENT6);
		rd_case_return_string(GL_COLOR_ATTACHMENT7);
		rd_case_return_string(GL_COLOR_ATTACHMENT8);
		rd_case_return_string(GL_COLOR_ATTACHMENT9);
		rd_case_return_string(GL_COLOR_ATTACHMENT10);
		rd_case_return_string(GL_COLOR_ATTACHMENT11);
		rd_case_return_string(GL_COLOR_ATTACHMENT12);
		rd_case_return_string(GL_COLOR_ATTACHMENT13);
		rd_case_return_string(GL_COLOR_ATTACHMENT14);
		rd_case_return_string(GL_COLOR_ATTACHMENT15);
		rd_case_return_string(GL_COLOR_ATTACHMENT16);
		rd_case_return_string(GL_COLOR_ATTACHMENT17);
		rd_case_return_string(GL_COLOR_ATTACHMENT18);
		rd_case_return_string(GL_COLOR_ATTACHMENT19);
		rd_case_return_string(GL_COLOR_ATTACHMENT20);
		rd_case_return_string(GL_COLOR_ATTACHMENT21);
		rd_case_return_string(GL_COLOR_ATTACHMENT22);
		rd_case_return_string(GL_COLOR_ATTACHMENT23);
		rd_case_return_string(GL_COLOR_ATTACHMENT24);
		rd_case_return_string(GL_COLOR_ATTACHMENT25);
		rd_case_return_string(GL_COLOR_ATTACHMENT26);
		rd_case_return_string(GL_COLOR_ATTACHMENT27);
		rd_case_return_string(GL_COLOR_ATTACHMENT28);
		rd_case_return_string(GL_COLOR_ATTACHMENT29);
		rd_case_return_string(GL_COLOR_ATTACHMENT30);
		rd_case_return_string(GL_COLOR_ATTACHMENT31);
		rd_case_return_string(GL_FRAMEBUFFER_INCOMPLETE_MULTISAMPLE);
		rd_case_return_string(GL_MAX_SAMPLES);
		rd_case_return_string(GL_HALF_FLOAT);
		//rd_case_return_string(GL_MAP_READ_BIT);
		//rd_case_return_string(GL_MAP_WRITE_BIT);
		//rd_case_return_string(GL_MAP_INVALIDATE_RANGE_BIT);
		rd_case_return_string(GL_MAP_INVALIDATE_BUFFER_BIT);
		rd_case_return_string(GL_MAP_FLUSH_EXPLICIT_BIT);
		rd_case_return_string(GL_MAP_UNSYNCHRONIZED_BIT);
		rd_case_return_string(GL_RG);
		rd_case_return_string(GL_RG_INTEGER);
		rd_case_return_string(GL_R8);
		rd_case_return_string(GL_RG8);
		rd_case_return_string(GL_R16F);
		rd_case_return_string(GL_R32F);
		rd_case_return_string(GL_RG16F);
		rd_case_return_string(GL_RG32F);
		rd_case_return_string(GL_R8I);
		rd_case_return_string(GL_R8UI);
		rd_case_return_string(GL_R16I);
		rd_case_return_string(GL_R16UI);
		rd_case_return_string(GL_R32I);
		rd_case_return_string(GL_R32UI);
		rd_case_return_string(GL_RG8I);
		rd_case_return_string(GL_RG8UI);
		rd_case_return_string(GL_RG16I);
		rd_case_return_string(GL_RG16UI);
		rd_case_return_string(GL_RG32I);
		rd_case_return_string(GL_RG32UI);
		rd_case_return_string(GL_VERTEX_ARRAY_BINDING);
		rd_case_return_string(GL_R8_SNORM);
		rd_case_return_string(GL_RG8_SNORM);
		rd_case_return_string(GL_RGB8_SNORM);
		rd_case_return_string(GL_RGBA8_SNORM);
		rd_case_return_string(GL_SIGNED_NORMALIZED);
		rd_case_return_string(GL_PRIMITIVE_RESTART_FIXED_INDEX);
		rd_case_return_string(GL_COPY_READ_BUFFER);
		rd_case_return_string(GL_COPY_WRITE_BUFFER);
		//rd_case_return_string(GL_COPY_READ_BUFFER_BINDING); //same as GL_COPY_READ_BUFFER
		//rd_case_return_string(GL_COPY_WRITE_BUFFER_BINDING); //same as GL_COPY_WRITE_BUFFER
		rd_case_return_string(GL_UNIFORM_BUFFER);
		rd_case_return_string(GL_UNIFORM_BUFFER_BINDING);
		rd_case_return_string(GL_UNIFORM_BUFFER_START);
		rd_case_return_string(GL_UNIFORM_BUFFER_SIZE);
		rd_case_return_string(GL_MAX_VERTEX_UNIFORM_BLOCKS);
		rd_case_return_string(GL_MAX_FRAGMENT_UNIFORM_BLOCKS);
		rd_case_return_string(GL_MAX_COMBINED_UNIFORM_BLOCKS);
		rd_case_return_string(GL_MAX_UNIFORM_BUFFER_BINDINGS);
		rd_case_return_string(GL_MAX_UNIFORM_BLOCK_SIZE);
		rd_case_return_string(GL_MAX_COMBINED_VERTEX_UNIFORM_COMPONENTS);
		rd_case_return_string(GL_MAX_COMBINED_FRAGMENT_UNIFORM_COMPONENTS);
		rd_case_return_string(GL_UNIFORM_BUFFER_OFFSET_ALIGNMENT);
		rd_case_return_string(GL_ACTIVE_UNIFORM_BLOCK_MAX_NAME_LENGTH);
		rd_case_return_string(GL_ACTIVE_UNIFORM_BLOCKS);
		rd_case_return_string(GL_UNIFORM_TYPE);
		rd_case_return_string(GL_UNIFORM_SIZE);
		rd_case_return_string(GL_UNIFORM_NAME_LENGTH);
		rd_case_return_string(GL_UNIFORM_BLOCK_INDEX);
		rd_case_return_string(GL_UNIFORM_OFFSET);
		rd_case_return_string(GL_UNIFORM_ARRAY_STRIDE);
		rd_case_return_string(GL_UNIFORM_MATRIX_STRIDE);
		rd_case_return_string(GL_UNIFORM_IS_ROW_MAJOR);
		rd_case_return_string(GL_UNIFORM_BLOCK_BINDING);
		rd_case_return_string(GL_UNIFORM_BLOCK_DATA_SIZE);
		rd_case_return_string(GL_UNIFORM_BLOCK_NAME_LENGTH);
		rd_case_return_string(GL_UNIFORM_BLOCK_ACTIVE_UNIFORMS);
		rd_case_return_string(GL_UNIFORM_BLOCK_ACTIVE_UNIFORM_INDICES);
		rd_case_return_string(GL_UNIFORM_BLOCK_REFERENCED_BY_VERTEX_SHADER);
		rd_case_return_string(GL_UNIFORM_BLOCK_REFERENCED_BY_FRAGMENT_SHADER);
		rd_case_return_string(GL_INVALID_INDEX);
		rd_case_return_string(GL_MAX_VERTEX_OUTPUT_COMPONENTS);
		rd_case_return_string(GL_MAX_FRAGMENT_INPUT_COMPONENTS);
		rd_case_return_string(GL_MAX_SERVER_WAIT_TIMEOUT);
		rd_case_return_string(GL_OBJECT_TYPE);
		rd_case_return_string(GL_SYNC_CONDITION);
		rd_case_return_string(GL_SYNC_STATUS);
		rd_case_return_string(GL_SYNC_FLAGS);
		rd_case_return_string(GL_SYNC_FENCE);
		rd_case_return_string(GL_SYNC_GPU_COMMANDS_COMPLETE);
		rd_case_return_string(GL_UNSIGNALED);
		rd_case_return_string(GL_SIGNALED);
		rd_case_return_string(GL_ALREADY_SIGNALED);
		rd_case_return_string(GL_TIMEOUT_EXPIRED);
		rd_case_return_string(GL_CONDITION_SATISFIED);
		rd_case_return_string(GL_WAIT_FAILED);
		//rd_case_return_string(GL_SYNC_FLUSH_COMMANDS_BIT);
		//rd_case_return_string(GL_TIMEOUT_IGNORED);
		rd_case_return_string(GL_VERTEX_ATTRIB_ARRAY_DIVISOR);
		rd_case_return_string(GL_ANY_SAMPLES_PASSED);
		rd_case_return_string(GL_ANY_SAMPLES_PASSED_CONSERVATIVE);
		rd_case_return_string(GL_SAMPLER_BINDING);
		rd_case_return_string(GL_RGB10_A2UI);
		rd_case_return_string(GL_TEXTURE_SWIZZLE_R);
		rd_case_return_string(GL_TEXTURE_SWIZZLE_G);
		rd_case_return_string(GL_TEXTURE_SWIZZLE_B);
		rd_case_return_string(GL_TEXTURE_SWIZZLE_A);
		rd_case_return_string(GL_GREEN);
		rd_case_return_string(GL_BLUE);
		rd_case_return_string(GL_INT_2_10_10_10_REV);
		rd_case_return_string(GL_TRANSFORM_FEEDBACK);
		rd_case_return_string(GL_TRANSFORM_FEEDBACK_PAUSED);
		rd_case_return_string(GL_TRANSFORM_FEEDBACK_ACTIVE);
		rd_case_return_string(GL_TRANSFORM_FEEDBACK_BINDING);
		rd_case_return_string(GL_PROGRAM_BINARY_RETRIEVABLE_HINT);
		rd_case_return_string(GL_PROGRAM_BINARY_LENGTH);
		rd_case_return_string(GL_NUM_PROGRAM_BINARY_FORMATS);
		rd_case_return_string(GL_PROGRAM_BINARY_FORMATS);
		rd_case_return_string(GL_COMPRESSED_R11_EAC);
		rd_case_return_string(GL_COMPRESSED_SIGNED_R11_EAC);
		rd_case_return_string(GL_COMPRESSED_RG11_EAC);
		rd_case_return_string(GL_COMPRESSED_SIGNED_RG11_EAC);
		rd_case_return_string(GL_COMPRESSED_RGB8_ETC2);
		rd_case_return_string(GL_COMPRESSED_SRGB8_ETC2);
		rd_case_return_string(GL_COMPRESSED_RGB8_PUNCHTHROUGH_ALPHA1_ETC2);
		rd_case_return_string(GL_COMPRESSED_SRGB8_PUNCHTHROUGH_ALPHA1_ETC2);
		rd_case_return_string(GL_COMPRESSED_RGBA8_ETC2_EAC);
		rd_case_return_string(GL_COMPRESSED_SRGB8_ALPHA8_ETC2_EAC);
		rd_case_return_string(GL_TEXTURE_IMMUTABLE_FORMAT);
		rd_case_return_string(GL_MAX_ELEMENT_INDEX);
		rd_case_return_string(GL_NUM_SAMPLE_COUNTS);
		rd_case_return_string(GL_TEXTURE_IMMUTABLE_LEVELS);
		default: break;
	}
	return "UNKNOWN";
	#undef rd_case_return_string
}

#endif //RD_API_OPENGL4
#endif //GLSTATE_H