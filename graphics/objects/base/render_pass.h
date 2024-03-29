#ifndef RENDERPASS_H
#define RENDERPASS_H

#ifdef RD_API_BASE

#include "../../utils/pointers.h"
#include "../../utils/types/types.h"
#include "../../descriptors/graphics_enums.h"
#include "../../descriptors/graphic_object.h"
#include "../../descriptors/render_pass_desc.h"

class GPUDevice;
class CFramebuffer;

class CRenderPass : public CGraphicObject{
protected:
	SRenderPassDesc descriptor;
	CFramebuffer* boundFramebuffer = nullptr;
	SClearColorValues clearColor;
public:
	CRenderPass(GPUDevice* dev, const SRenderPassDesc& desc) :
		CGraphicObject(dev), descriptor(desc) {}

	const auto& getDescriptor(){ return descriptor; }

	bool isCompatibleWith(SRenderPassDesc& other){ return descriptor == other; }
	bool isCompatibleWith(CRenderPass& other){ return this->isCompatibleWith(other.descriptor); }

	bool Begin(CFramebuffer* fb, SClearColorValues clear = SClearColorValues()){
		boundFramebuffer = fb;
		clearColor = clear;
	}

	void End(){ boundFramebuffer = nullptr; }

	bool DoAttachmentsNeedClearing(){ return false; }

	virtual ~CRenderPass() = default;
};

#endif //RD_API_BASE
#endif