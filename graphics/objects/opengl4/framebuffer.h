#ifndef FRAMEBUFFER_H
#define FRAMEBUFFER_H

#ifdef RD_API_OPENGL4

#include "glinclude.h"
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
	
	GLuint id = 0;
	GLuint getId(){ return id; }

	bool CreateAndSetupAttachments();
	virtual void Release() override;

public:
	CFramebuffer(GPUDevice* dev, const SRenderPassDesc& desc, std::vector<SharedPtr<CTexture>> textures, SharedPtr<CTexture> depthStencilTextures = nullptr) :
		CGraphicObject(dev), descriptor(desc) {
		for(uint i = 0; i < textures.size() && i < RD_MAX_RENDER_ATTACHMENTS; ++i){
			this->attachments[i] = textures[i];
		}
		depthStencil = depthStencilTextures;

		CreateAndSetupAttachments();
	}

	const auto& getDescriptor(){ return descriptor; }

	bool isCompatibleWith(SRenderPassDesc& rpdesc){ return this->descriptor == rpdesc; }
	bool isCompatibleWith(CRenderPass& rp){ return rp.isCompatibleWith(this->descriptor); }

	bool hasAttachment(CTexture* tx){
		if(depthStencil.get() == tx) return true;
		for(uint i = 0; i < RD_MAX_RENDER_ATTACHMENTS; ++i)
			if(attachments[i].get() == tx) return true;
		return false;
	}

	virtual ~CFramebuffer() = default;

	friend class GPUDevice;
};

class CGLSwapchainFramebuffer : public CFramebuffer{
public:
	CGLSwapchainFramebuffer(GPUDevice* dev, const SRenderPassDesc& desc) :
		CFramebuffer(dev, desc, {}){
		this->id = 0;
	}

	virtual ~CGLSwapchainFramebuffer() = default;

	friend class GPUDevice;
};

#endif //RD_API_OPENGL4
#endif //FRAMEBUFFER_H