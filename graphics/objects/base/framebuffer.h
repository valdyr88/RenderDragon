#ifndef FRAMEBUFFER_H
#define FRAMEBUFFER_H

#ifdef RD_API_BASE

#include "../../utils/pointers.h"
#include "../../utils/types/types.h"
#include "../../descriptors/graphics_enums.h"
#include "../../descriptors/graphic_object.h"
#include "../../descriptors/render_pass_desc.h"
#include "render_pass.h"

class GPUDevice;
class CTexture;

class CFramebuffer : public CGraphicObject{
protected:
	SRenderPassDesc descriptor;
	SharedPtr<CTexture> attachments[RD_MAX_RENDER_ATTACHMENTS];
	SharedPtr<CTexture> depthStencil;
	
public:
	CFramebuffer(GPUDevice* dev, const SRenderPassDesc desc, std::vector<SharedPtr<CTexture>> textures, SharedPtr<CTexture> depthStencilTextures = nullptr) :
		CGraphicObject(dev), descriptor(desc) {
		for(uint i = 0; i < textures.size() && i < RD_MAX_RENDER_ATTACHMENTS; ++i){
			this->attachments[i] = textures[i];
		}
		depthStencil = depthStencilTextures;
	}

	const auto& getDescriptor(){ return descriptor; }

	bool isCompatibleWith(SRenderPassDesc& rpdesc){ return this->descriptor == rpdesc; }
	bool isCompatibleWith(CRenderPass& rp){ return rp.isCompatibleWith(this->descriptor); }

	virtual ~CFramebuffer() = default;
};

#endif //RD_API_BASE
#endif //FRAMEBUFFER_H