#ifndef DEVICE_H
#define DEVICE_H

#include "../../utils/types/types.h"
#include "../../descriptors/graphics_enums.h"
#include "../../descriptors/graphic_object.h"
#include "pipeline_state.h"
#include "buffer.h"
#include "framebuffer.h"
#include "render_pass.h"
#include "shader.h"
#include "uniform_buffer.h"
#include "vertex_buffer.h"

class IDevice {
protected:
public:
	virtual UniquePtr<CPipelineState> CreatePipelineState(const SPipelineStateDesc& desc) = 0;
	virtual UniquePtr<CRenderPass> CreateRenderPass(const SRenderPassDesc& desc) = 0;
	virtual UniquePtr<CBuffer> CreateBuffer(const SBufferDesc& desc) = 0;
	virtual UniquePtr<CFramebuffer> CreateFramebuffer(const SRenderPassDesc& desc) = 0;
	//virtual UniquePtr<CShader> CreateShaderModule(const SShaderDesc& desc) = 0;
	//virtual UniquePtr<CShaderResource> CreateShaderResrouce(const SShaderResourceDesc& desc) = 0;
	virtual UniquePtr<CSampler> CreateSampler(const SSamplerDesc& desc) = 0;
	virtual UniquePtr<CVertexBuffer> CreateVertexBuffer(const SVertexFormat& desc, uint32 count) = 0;
	virtual UniquePtr<CIndexBuffer> CreateIndexBuffer(EValueType type, uint32 count) = 0;
};

//Just a test class for object creation
class TestDevice : public IDevice {
protected:
public:
	virtual UniquePtr<CPipelineState> CreatePipelineState(const SPipelineStateDesc& desc){ return nullptr; }
	virtual UniquePtr<CRenderPass> CreateRenderPass(const SRenderPassDesc& desc){ return nullptr; }
	virtual UniquePtr<CBuffer> CreateBuffer(const SBufferDesc& desc){ return nullptr; }
	virtual UniquePtr<CFramebuffer> CreateFramebuffer(const SRenderPassDesc& desc){ return nullptr; }
	//virtual UniquePtr<CShader> CreateShaderModule(const SShaderDesc& desc){ return nullptr; }
	//virtual UniquePtr<CShaderResource> CreateShaderResrouce(const SShaderResourceDesc& desc){ return nullptr; }
	virtual UniquePtr<CSampler> CreateSampler(const SSamplerDesc& desc){ return nullptr; }
	virtual UniquePtr<CVertexBuffer> CreateVertexBuffer(const SVertexFormat& desc, uint32 count){ return nullptr; }
	virtual UniquePtr<CIndexBuffer> CreateIndexBuffer(EValueType type, uint32 count){ return nullptr; }
};

#endif //DEVICE_H