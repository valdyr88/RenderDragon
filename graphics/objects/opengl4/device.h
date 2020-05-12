#ifndef DEVICE_H
#define DEVICE_H

#ifdef RD_API_OPENGL4

#include <vector>
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
	SWindow window;
	CGLState gl;

	struct CreatedObjects{
		stdex::container<WeakPtr<CGraphicObject>> objects;

		stdex::container<WeakPtr<CShader>> shaders;
		stdex::container<WeakPtr<CShaderProgram>> programs;
		stdex::container<WeakPtr<CTexture>> textures;
	} created;

	template <typename type>
	void addTrackedObject(SharedPtr<type>& obj);
	template <typename type>
	SharedPtr<type> getTrackedObject(type* ptr);

	SharedPtr<CRenderPass> swapchainRenderPass;
	SharedPtr<CFramebuffer> swapchainFramebuffer;
	SPipelineStateDesc pipelineState;
	CFramebuffer* boundFramebuffer = nullptr;
	CVertexBuffer* boundVertexBuffer = nullptr;
	CIndexBuffer* boundIndexBuffer = nullptr;

	//CShaderResourceSetManager shaderResourceSetManager;

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

	void initPipelineState();

public:

	GPUDevice(const SGPUDeviceDesc& desc) : descriptor(desc){
		descriptor.api = EGraphicsAPI::OpenGL4;
	}

	const auto& getDescriptor(){ return descriptor; }

	bool InitContextOnWindow(SWindow& window);

	bool setDynamicViewports(const SViewports& mode);
	bool setDynamicViewport(uint i, const SViewport& mode);

	SharedPtr<CPipelineState> CreatePipelineState(const SPipelineStateDesc& desc);
	SharedPtr<CRenderPass> CreateRenderPass(const SRenderPassDesc& desc);
	SharedPtr<CBuffer> CreateBuffer(const SBufferDesc& desc);
	SharedPtr<CFramebuffer> CreateFramebuffer(const SRenderPassDesc& desc, std::vector<SharedPtr<CTexture>> textures, SharedPtr<CTexture> depthStencilTexture = nullptr);
	SharedPtr<CShader> CreateShaderModule(const SShaderDesc& desc);
	//SharedPtr<CShaderResource> CreateShaderResrouce(const SShaderResourceDesc& desc);
	SharedPtr<CShaderProgram> CreateShaderProgram(std::string uniquename, std::vector<SharedPtr<CShader>> shaders);
	SharedPtr<CSampler> CreateSampler(const SSamplerDesc& desc);
	SharedPtr<CVertexBuffer> CreateVertexBuffer(const SVertexFormat& desc, uint32 count, std::vector<SRawData> data = std::vector<SRawData>());
	SharedPtr<CIndexBuffer> CreateIndexBuffer(EValueType type, uint32 count, SRawData data = SRawData());
	SharedPtr<CTexture> CreateTexture(const STextureDesc& desc, const STextureRawData& data = STextureRawData());
	SharedPtr<CTexture> CreateTexture(const STextureDesc& desc, std::string fileName);
	SharedPtr<CTextureView> CreateTextureView(const STextureViewDesc& desc, SharedPtr<CTexture> tx);

	void ClearAttachments(CRenderPass* rp, CFramebuffer* fb, SClearColorValues clear);
	void BindVertexBuffer(CVertexBuffer* vb){ boundVertexBuffer = vb; if(vb) vb->Bind(); }
	void BindIndexBuffer(CIndexBuffer* ib){ boundIndexBuffer = ib; if(ib) ib->Bind(); }

	bool DrawIndexed(uint count = 0);

	bool PresentFrame();

	SharedPtr<CRenderPass> GetSwapchainRenderPass(){ return swapchainRenderPass; }
	SharedPtr<CFramebuffer> GetActiveSwapchainFramebuffer(){ return swapchainFramebuffer; }

	SharedPtr<CShaderResourceBinding> CreateShaderResourceBinding(const SShaderResourceBindingDesc& desc, CShaderResource* resource);
	SharedPtr<CShaderResourceSetDesc> CreateShaderResourceSetDesc(const std::vector<SShaderResourceBindingDesc>& binds);
	SharedPtr<CShaderResourceSet> CreateShaderResourceSet(const CShaderResourceSetDesc* desc, const std::vector<CShaderResource*>& rers);

	auto GetShaderResourceManager(){ return CSingleton<CShaderResourceSetManager>::get(); }
	auto GetShaderProgramManager(){ return CSingleton<CShaderProgramManager>::get(); }
	auto GetShaderModuleManager(){ return CSingleton<CShaderModuleManager>::get(); }
	auto GetTextureManager(){ return CSingleton<CTextureManager>::get(); }

	template <typename type>
	SharedPtr<type> FindSharedPtr(CGraphicObject* ptr);

	static UniquePtr<GPUDevice> CreateGPUDevice(const SGPUDeviceDesc& desc);

	friend class CPipelineState;
	friend class CRenderPass;
	friend class CBuffer;
	friend class CFramebuffer;
	friend class CSampler;
	friend class CTextureView;
	friend class CVertexBuffer;
	friend class CIndexBuffer;
	friend class CTexture;
	friend class CShader;
	friend class CShaderProgram;
	friend class CShaderResource;
	friend class IUniformBuffer;
	
	friend class CShaderResourceSetManager;
	friend class CShaderModuleManager;
	friend class CShaderProgramManager;
	friend class CTextureManager;
};

SharedPtr<CBuffer> rdDeviceCreateBuffer(GPUDevice* device, const SBufferDesc& desc);

bool rdSetupDeviceForGlobalObjects(GPUDevice* device);

#endif //RD_API_OPENGL4
#endif //DEVICE_H