#ifndef RENDERPASS_H
#define RENDERPASS_H

#include "../../utils/pointers.h"
#include "../../utils/types/types.h"
#include "../enum/graphics_enums.h"
#include "graphic_object.h"
#include "render_pass_desc.h"

class Device;
class Framebuffer;

class RenderPass : public GraphicObject{
protected:
	RenderPassDesc descriptor;
	SharedPtr<Framebuffer> boundFramebuffer;
	ClearColorValues clearColor;

public:
	RenderPass(WeakPtr<Device>& dev, const RenderPassDesc& desc) :
		GraphicObject(dev), descriptor(desc) {}

	virtual bool isCompatibleWith(RenderPassDesc& other) = 0;
	virtual bool isCompatibleWith(RenderPass& other){ return this->isCompatibleWith(other.descriptor); }

	virtual bool begin(WeakPtr<Framebuffer> fb, ClearColorValues clear){
		if((boundFramebuffer = fb.lock()) == false) return false;
		clearColor = clear;
	}

	virtual void end() = 0;

	virtual ~RenderPass() = default;
};

#endif