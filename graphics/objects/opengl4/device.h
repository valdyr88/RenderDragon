#ifndef DEVICE_H
#define DEVICE_H

#ifdef RD_API_OPENGL4

#include "../../utils/types/types.h"
#include "../../utils/platform_defines.h"
#include "../../utils/platform.h"
#include "../../descriptors/graphics_enums.h"
#include "../../descriptors/graphic_object.h"
#include "../../descriptors/device_desc.h"
#include "pipeline_state.h"
#include "buffer.h"
#include "framebuffer.h"
#include "render_pass.h"
#include "shader.h"
#include "uniform_buffer.h"
#include "vertex_buffer.h"

#ifdef PLATFORM_WINDOWS

	#define GLEW_STATIC
	#include "gl\glew.h"
	#include "gl\gl.h"
	#include "gl\glext.h"
	#include "gl\wglext.h"

#endif
#ifdef PLATFORM_LINUX

	#include <gl/glx.h>
	#include <X11/X.h>
	#include <X11/Xlib.h>

#endif
#ifdef PLATFORM_MAC
#endif

#include "gl/GLState.h"

struct SGPUDeviceContext{
#ifdef PLATFORM_WINDOWS
	//Windows-specific graphics api context
	HGLRC context = nullptr;
	HMODULE hmoduleOpenGL = nullptr;
#endif
#ifdef PLATFORM_LINUX
	//Linux-specific graphics api context
	GLXContext context;
#endif
#ifdef PLATFORM_MAC
	//MacOS-specific graphics api context
#endif
};

class GPUDevice {
protected:
	SGPUDeviceDesc descriptor;
	SGPUDeviceContext context;
	CGLState gl;
	std::list<SharedPtr<CGraphicObject>> objects;

	GPUDevice() = delete;

	SGPUDeviceContext InitOpenGL(SWindow& window);
public:

	GPUDevice(const SGPUDeviceDesc& desc) : descriptor(desc){
		descriptor.api = EGraphicsAPI::OpenGL4;
	}

	bool InitContextOnWindow(SWindow& window);

	SharedPtr<CPipelineState> CreatePipelineState(const SPipelineStateDesc& desc);
	SharedPtr<CRenderPass> CreateRenderPass(const SRenderPassDesc& desc);
	SharedPtr<CBuffer> CreateBuffer(const SBufferDesc& desc);
	SharedPtr<CFramebuffer> CreateFramebuffer(const SRenderPassDesc& desc, std::vector<SharedPtr<CTexture>> textures, SharedPtr<CTexture> depthStencilTextures = nullptr);
	//SharedPtr<CShader> CreateShaderModule(const SShaderDesc& desc);
	//SharedPtr<CShaderResource> CreateShaderResrouce(const SShaderResourceDesc& desc);
	SharedPtr<CSampler> CreateSampler(const SShaderResourceDesc& sr, const SSamplerDesc& desc);
	SharedPtr<CVertexBuffer> CreateVertexBuffer(const SVertexFormat& desc, uint32 count);
	SharedPtr<CIndexBuffer> CreateIndexBuffer(EValueType type, uint32 count);

	static UniquePtr<GPUDevice> CreateGPUDevice(const SGPUDeviceDesc& desc);
};


#endif //RD_API_OPENGL4
#endif //DEVICE_H