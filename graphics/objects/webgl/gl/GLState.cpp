#ifdef RD_API_WEBGL

#include "GLState.h"

#ifdef EGL_GL_context
#include "EGL\egl.h"
#endif

#ifdef GLState_Static_Class

int CGLState::MATRIX_MODE = -1;
int CGLState::PUSH_MODELVIEW_MATRIX = -1;
int CGLState::PUSH_PROJECTION_MATRIX = -1;
int CGLState::PUSH_TEXTURE_MATRIX = -1;
int CGLState::PUSH_ATTRIB_BITS = -1;
int CGLState::ACTIVE_FRAMEBUFFER = -1;
int CGLState::BIND_TEXTURE1D_ID = -1;
int CGLState::BIND_TEXTURE2D_ID = -1;
int CGLState::BIND_TEXTURE3D_ID = -1;
int CGLState::ACTIVE_TEXTURE_ID = -1;
int CGLState::BLEND_SRC = -1, GLState::BLEND_DST = -1, GLState::BLEND_SRC_ALPHA = -1, GLState::BLEND_DST_ALPHA = -1, GLState::BLEND_OP = -1, GLState::BLEND_OP_ALPHA = -1;
bool CGLState::VERTEX_ARRAY_ENABLED = false,GLState::NORMAL_ARRAY_ENABLED = false,GLState::TEXTURE_COORD_ARRAY_ENABLED = false,GLState::COLOR_ARRAY_ENABLED = false;
bool CGLState::LIGHTING_ENABLED = false, GLState::DEPTH_TEST_ENABLED = false, GLState::ALPHA_TEST_ENABLED = false, GLState::STENCIL_TEST_ENABLED = false, GLState::BLEND_ENABLED = false, GLState::TEXTURE1D_ENABLED = false, GLState::TEXTURE2D_ENABLED = false, GLState::TEXTURE3D_ENABLED = false, GLState::TEXTURECUBE_ENABLED = false, GLState::MULTISAMPLE_ENABLED = false, GLState::SCISSOR_TEST_ENABLED = false, GLState::SAMPLE_ALPHA_TO_COVERAGE_ENABLED = false, GLState::CULL_FACE_ENABLED = false, GLState::SAMPLE_MASK_ENABLED = false, GLState::SAMPLE_COVERAGE_ENABLED = false, GLState::SAMPLE_ALPHA_TO_ONE_ENABLED = false, GLState::TEXTURECUBESEAMLESS_ENABLED = false;
bool CGLState::init = false;
void* CGLState::pVertexBuffer = 0,*GLState::pNormalBuffer = 0,*GLState::pTexCoordBuffer = 0,*GLState::pColorBuffer = 0;
int CGLState::program_link = 0,GLState::previous_program_link = 0;

#endif

GLState_prefix_func void CGLState::Init(){
	if(bIsInit) return;

	ACTIVE_TEXTURE_ID = -1; ACTIVE_FRAMEBUFFER = -1;
	VERTEX_ARRAY_ENABLED = VERTEX_ARRAY_ENABLED = TEXTURE_COORD_ARRAY_ENABLED = COLOR_ARRAY_ENABLED = false;	
	MATRIX_MODE = -1; BIND_TEXTURE1D_ID = -1; BIND_TEXTURE2D_ID = -1; BIND_TEXTURE3D_ID = -1;
	PUSH_ATTRIB_BITS = PUSH_MODELVIEW_MATRIX = PUSH_PROJECTION_MATRIX = PUSH_TEXTURE_MATRIX = -1;
	LIGHTING_ENABLED = DEPTH_TEST_ENABLED = DEPTH_WRITE_ENABLED = ALPHA_TEST_ENABLED = STENCIL_TEST_ENABLED = BLEND_ENABLED =
	TEXTURE1D_ENABLED = TEXTURE2D_ENABLED = TEXTURE3D_ENABLED = TEXTURECUBE_ENABLED = LINE_SMOOTH_ENABLED =
	MULTISAMPLE_ENABLED = SCISSOR_TEST_ENABLED = SAMPLE_ALPHA_TO_COVERAGE_ENABLED = CULL_FACE_ENABLED = SAMPLE_MASK_ENABLED =
		SAMPLE_COVERAGE_ENABLED = SAMPLE_ALPHA_TO_ONE_ENABLED = TEXTURECUBESEAMLESS_ENABLED = DEPTH_CLAMP_ENABLED =
		POLYGON_OFFSET_FILL_ENABLED = POLYGON_OFFSET_LINE_ENABLED = POLYGON_OFFSET_POINT_ENABLED = false;
	BLEND_DST = BLEND_SRC = BLEND_DST_ALPHA = BLEND_SRC_ALPHA = BLEND_OP = BLEND_OP_ALPHA = -1;
	VERTEX_ARRAY_ENABLED = NORMAL_ARRAY_ENABLED = TEXTURE_COORD_ARRAY_ENABLED = COLOR_ARRAY_ENABLED = false;
	bound_vertex_array = 0;
	program_link = 0; previous_program_link = 0;
	bound_read_framebuffer = 0; bound_draw_framebuffer = 0;

	for(uint i = 0; i < RD_MAX_VERTEX_ATTRIBS; ++i) vertex_attrib_array_enabled[i] = false;
	for(uint i = 0; i < RD_MAX_TEXTURE_BINDINGS; ++i) bound_textures[i] = -1;

	bIsInit = true;
}

GLState_prefix_func void CGLState::FetchFunctionPointers(){
#ifdef EGL_GL_context
	pglTexStorage2D = (PFNGLTEXSTORAGE2DPROC)eglGetProcAddress("glTexStorage2D");
	pglTexStorage3D = (PFNGLTEXSTORAGE3DPROC)eglGetProcAddress("glTexStorage3D");
	pglGenVertexArrays = (PFNGLGENVERTEXARRAYSPROC)eglGetProcAddress("glGenVertexArrays");
#endif
}

#define getProcessAddress(x) wglGetProcAddress(x)//GetProcAddress(hModuleOpenGL,x)//

/*
CGLState::EGLTextureInternalFormat GetBaseInternalFormatFromTextureFormat(ETypedTextureFormat fmt);
CGLState::EGLSymbolicFormat GetSymbolicFormatFromTextureFormat(ETypedTextureFormat fmt);
CGLState::EGLTextureInternalFormat GetBaseInternalFormatFromTextureFormat(ETextureFormat fmt);
CGLState::EGLSymbolicFormat GetSymbolicFormatFromTextureFormat(ETextureFormat fmt);
*/

#endif