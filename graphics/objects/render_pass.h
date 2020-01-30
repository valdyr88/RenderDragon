#ifndef RENDERPASS_H
#define RENDERPASS_H

#include "../../utils/pointers.h"
#include "../../utils/types/types.h"
#include "../enum/graphics_enums.h"
#include "graphic_object.h"
#include "render_pass_desc.h"

class IDevice;
class CFramebuffer;

class CRenderPass : public CGraphicObject{
protected:
	SRenderPassDesc descriptor;
	SharedPtr<CFramebuffer> boundFramebuffer;
	SClearColorValues clearColor;

public:
	CRenderPass(WeakPtr<IDevice>& dev, const SRenderPassDesc& desc) :
		CGraphicObject(dev), descriptor(desc) {}

	virtual bool isCompatibleWith(SRenderPassDesc& other) = 0;
	virtual bool isCompatibleWith(CRenderPass& other){ return this->isCompatibleWith(other.descriptor); }

	virtual bool begin(WeakPtr<CFramebuffer> fb, SClearColorValues clear){
		if((boundFramebuffer = fb.lock()) == false) return false;
		clearColor = clear;
	}

	virtual void end() = 0;

	virtual ~CRenderPass() = default;
};

#endif