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

	GPUDevice() = delete;

	SGPUDeviceContext InitOpenGL(const SWindow& window);
public:

	GPUDevice(const SGPUDeviceDesc& desc) : descriptor(desc){ 
		descriptor.api = EGraphicsAPI::OpenGL4; }

	UniquePtr<CPipelineState> CreatePipelineState(const SPipelineStateDesc& desc){ return nullptr; }
	UniquePtr<CRenderPass> CreateRenderPass(const SRenderPassDesc& desc){ return nullptr; }
	UniquePtr<CBuffer> CreateBuffer(const SBufferDesc& desc){ return nullptr; }
	UniquePtr<CFramebuffer> CreateFramebuffer(const SRenderPassDesc& desc){ return nullptr; }
	//UniquePtr<CShader> CreateShaderModule(const SShaderDesc& desc){ return nullptr; }
	//UniquePtr<CShaderResource> CreateShaderResrouce(const SShaderResourceDesc& desc){ return nullptr; }
	UniquePtr<CSampler> CreateSampler(const SSamplerDesc& desc){ return nullptr; }
	UniquePtr<CVertexBuffer> CreateVertexBuffer(const SVertexFormat& desc, uint32 count){ return nullptr; }
	UniquePtr<CIndexBuffer> CreateIndexBuffer(EValueType type, uint32 count){ return nullptr; }
};

UniquePtr<GPUDevice> CreateGPUDevice(const SGPUDeviceDesc& desc);


#endif //RD_API_OPENGL4
#endif //DEVICE_H