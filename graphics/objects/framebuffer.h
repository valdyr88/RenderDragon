#ifndef FRAMEBUFFER_H
#define FRAMEBUFFER_H

#include "../../utils/pointers.h"
#include "../../utils/types/types.h"
#include "../enum/graphics_enums.h"
#include "graphic_object.h"
#include "render_pass_desc.h"
#include "render_pass.h"

class Device;
class Texture;

class Framebuffer : public GraphicObject{
protected:
	RenderPassDesc descriptor;
	SharedPtr<Texture> Attachments[RD_MAX_RENDER_ATTACHMENTS];
	SharedPtr<Texture> DepthStencil;
	
public:
	Framebuffer(WeakPtr<Device>& dev, const RenderPassDesc desc, std::vector<SharedPtr<Texture>> textures, SharedPtr<Texture> depthStencilTextures = nullptr) :
		GraphicObject(dev), descriptor(desc) {
		for(uint i = 0; i < textures.size() && i < RD_MAX_RENDER_ATTACHMENTS; ++i){
			this->Attachments[i] = textures[i];
		}
		DepthStencil = depthStencilTextures;
	}

	bool isCompatibleWith(RenderPassDesc& rpdesc){ return this->descriptor == rpdesc; }
	bool isCompatibleWith(RenderPass& rp){ return rp.isCompatibleWith(this->descriptor); }
};

#endif //FRAMEBUFFER_H