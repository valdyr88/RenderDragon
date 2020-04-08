#ifndef DEVICE_H
#define DEVICE_H

#ifdef RD_API_BASE

#include "../../utils/types/types.h"
#include "../../utils/platform_defines.h"
#include "../../utils/platform.h"
#include "../../descriptors/graphics_enums.h"
#include "../../descriptors/graphic_object.h"
#include "../../descriptors/device_desc.h"
#include "../../descriptors/shader_desc.h"
#include "pipeline_state.h"
#include "buffer.h"
#include "framebuffer.h"
#include "render_pass.h"
#include "shader.h"
#include "texture.h"
#include "uniform_buffer.h"
#include "vertex_buffer.h"
#include "shader_resource_manager.h"

class GPUDevice {
protected:
	SGPUDeviceDesc descriptor;

	CShaderResourceSetManager shaderResourceSetManager;

	GPUDevice() = delete;
public:

	GPUDevice(const SGPUDeviceDesc& desc) : descriptor(desc), shaderResourceSetManager(this){}

	bool InitContextOnWindow(const SWindow& window){ return false; }

	UniquePtr<CPipelineState> CreatePipelineState(const SPipelineStateDesc& desc){ return nullptr; }
	UniquePtr<CRenderPass> CreateRenderPass(const SRenderPassDesc& desc){ return nullptr; }
	UniquePtr<CBuffer> CreateBuffer(const SBufferDesc& desc){ return nullptr; }
	SharedPtr<CFramebuffer> CreateFramebuffer(const SRenderPassDesc& desc, std::vector<SharedPtr<CTexture>> textures, SharedPtr<CTexture> depthStencilTextures = nullptr){ return nullptr; }
	//UniquePtr<CShader> CreateShaderModule(const SShaderDesc& desc){ return nullptr; }
	UniquePtr<CSampler> CreateSampler(const SSamplerDesc& desc){ return nullptr; }
	UniquePtr<CVertexBuffer> CreateVertexBuffer(const SVertexFormat& desc, uint32 count){ return nullptr; }
	UniquePtr<CIndexBuffer> CreateIndexBuffer(EValueType type, uint32 count){ return nullptr; }
	SharedPtr<CTexture> CreateTexture(const STextureDesc& desc, const STextureRawData& data){ return nullptr; }

	void BindVertexBuffer(CVertexBuffer* vb){ return; }
	void BindIndexBuffer(CIndexBuffer* ib){ return; }

	bool DrawIndexed(uint count = 0);

	void ClearAttachments(CRenderPass* rp, CFramebuffer* fb, SClearColorValues clear){}

	bool PresentFrame();

	UniquePtr<CShaderResourceBinding> CreateShaderResourceBinding(const SShaderResourceBindingDesc& desc, CShaderResource* resource);
	UniquePtr<CShaderResourceSetDesc> CreateShaderResourceSetDesc(const std::vector<SShaderResourceBindingDesc>& binds);
	UniquePtr<CShaderResourceSet> CreateShaderResourceSet(const CShaderResourceSetDesc* desc, const std::vector<CShaderResource*>& rers);

	auto& getShaderResourceManager(){ return shaderResourceSetManager; }

	static UniquePtr<GPUDevice> CreateGPUDevice(const SGPUDeviceDesc& desc);
};


#endif //RD_API_BASE
#endif //DEVICE_H