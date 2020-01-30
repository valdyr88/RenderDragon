#ifndef DEVICE_H
#define DEVICE_H

#include "../utils/types/types.h"
#include "enum/graphics_enums.h"
#include "objects/graphic_object.h"
#include "objects/pipeline_state.h"
#include "objects/buffer.h"
#include "objects/framebuffer.h"
#include "objects/render_pass.h"
#include "objects/shader.h"
#include "objects/uniform_buffer.h"
#include "objects/vertex_buffer.h"

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