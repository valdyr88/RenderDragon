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

class GPUDevice {
protected:
public:
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

#endif //DEVICE_H