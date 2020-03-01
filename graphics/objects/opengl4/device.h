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
#include "texture.h"
#include "uniform_buffer.h"
#include "vertex_buffer.h"
#include "shader_resource_manager.h"

#include "glinclude.h"

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

	SPipelineStateDesc pipelineState;
	CFramebuffer* boundFramebuffer = nullptr;

	GPUDevice() = delete;

	SGPUDeviceContext InitOpenGL(SWindow& window);

	bool setBlendState(const SBlendStateDesc& mode);
	bool setDepthState(const SDepthStateDesc& mode);
	bool setStencilState(const SStencilStateDesc& mode);
	bool setPrimitiveTopology(const EPrimitiveTopology& mode);
	bool setRasterizerState(const SRasterizerStateDesc& mode);
	bool setSampleState(const SSampleDesc& mode);
	bool setViewports(const SViewports& mode);

	bool bindShaderProgram(SharedPtr<CShaderProgram>& shader);

	void bindFramebuffer(CFramebuffer* framebuffer);

public:

	GPUDevice(const SGPUDeviceDesc& desc) : descriptor(desc){
		descriptor.api = EGraphicsAPI::OpenGL4;
	}

	bool InitContextOnWindow(SWindow& window);

	SharedPtr<CPipelineState> CreatePipelineState(const SPipelineStateDesc& desc);
	SharedPtr<CRenderPass> CreateRenderPass(const SRenderPassDesc& desc);
	SharedPtr<CBuffer> CreateBuffer(const SBufferDesc& desc);
	SharedPtr<CFramebuffer> CreateFramebuffer(const SRenderPassDesc& desc, std::vector<SharedPtr<CTexture>> textures, SharedPtr<CTexture> depthStencilTexture = nullptr);
	//SharedPtr<CShader> CreateShaderModule(const SShaderDesc& desc);
	//SharedPtr<CShaderResource> CreateShaderResrouce(const SShaderResourceDesc& desc);
	SharedPtr<CSampler> CreateSampler(const SSamplerDesc& desc);
	SharedPtr<CVertexBuffer> CreateVertexBuffer(const SVertexFormat& desc, uint32 count);
	SharedPtr<CIndexBuffer> CreateIndexBuffer(EValueType type, uint32 count);
	SharedPtr<CTexture> CreateTexture(const STextureDesc& desc, const STextureRawData& data);

	void ClearAttachments(CRenderPass* rp, CFramebuffer* fb, SClearColorValues clear);

	static UniquePtr<GPUDevice> CreateGPUDevice(const SGPUDeviceDesc& desc);

	friend class CPipelineState;
	friend class CRenderPass;
	friend class CBuffer;
	friend class CFramebuffer;
	friend class CSampler;
	friend class CVertexBuffer;
	friend class CIndexBuffer;
	friend class CTexture;
	friend class CShader;
	friend class CShaderResource;
};

SharedPtr<CBuffer> rdDeviceCreateBuffer(GPUDevice* device, const SBufferDesc& desc);

#endif //RD_API_OPENGL4
#endif //DEVICE_H