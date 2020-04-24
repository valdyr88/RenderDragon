#ifndef RENDERPASS_DESC_H
#define RENDERPASS_DESC_H

#include <vector>
#include "../utils/types/types.h"
#include "../utils/types/vectypes.h"
#include "graphics_enums.h"

struct SAttachmentDesc {
	ETextureFormat format = ETextureFormat::RGBA;
	EValueType valueType = EValueType::uint8;
	uint32 usageFlags = 0x00000000; //ETextureUsage flags
	uint8 sampleCount = 1;
	ELoadStoreOp loadOp = ELoadStoreOp::Clear;
	ELoadStoreOp storeOp = ELoadStoreOp::Store;
	uint32 level = 0;
	uint32 levelCount = 1;

	bool operator == (const SAttachmentDesc& other) const{
		return format == other.format &&
				valueType == other.valueType &&
				usageFlags == other.usageFlags &&
				sampleCount == other.sampleCount &&
				loadOp == other.loadOp &&
				/*level == other.level && //used by framebuffer to specify level attachment. RenderPass comparison should not compare this
				levelCount == other.levelCount &&*/
				storeOp == other.storeOp;
	}
	bool operator != (const SAttachmentDesc& other) const{ return !(*this == other); }

	SAttachmentDesc& operator = (const SAttachmentDesc& other) {
		format = other.format;
		valueType = other.valueType;
		usageFlags = other.usageFlags;
		sampleCount = other.sampleCount;
		loadOp = other.loadOp;
		storeOp = other.storeOp;
		level = other.level;
		levelCount = other.levelCount;

		return *this;
	}
};
struct SDepthStencilAttachmentDesc : public SAttachmentDesc{
	ELoadStoreOp stencilLoadOp = ELoadStoreOp::Clear;
	ELoadStoreOp stencilStoreOp = ELoadStoreOp::Store;

	bool operator == (const SDepthStencilAttachmentDesc& other) const{
		return SAttachmentDesc::operator==(other) &&
			stencilLoadOp == other.stencilLoadOp &&
			stencilStoreOp == other.stencilStoreOp;
	}
	bool operator != (const SDepthStencilAttachmentDesc& other) const{ return !(*this == other); }

	SDepthStencilAttachmentDesc& operator = (const SDepthStencilAttachmentDesc& other) {
		SAttachmentDesc::operator=(other);
		stencilLoadOp = other.stencilLoadOp;
		stencilStoreOp = other.stencilStoreOp;

		return *this;
	}
};

struct SRenderPassDesc {
	uint32 nofAttachments = 0;
	SAttachmentDesc attachments[RD_MAX_RENDER_ATTACHMENTS];
	SDepthStencilAttachmentDesc depthStencil;

	bool operator == (const SRenderPassDesc& other) const{
		if(nofAttachments != other.nofAttachments) return false;
		for(uint i = 0; i < nofAttachments; ++i){
			auto& thisAtt = attachments[i];
			auto& othrAtt = other.attachments[i];
			if(thisAtt != othrAtt) return false;
		}
		if(depthStencil != other.depthStencil) return false;
		return true;
	}
	bool operator != (const SRenderPassDesc& other) const{ return !(*this == other); }

	SRenderPassDesc& operator = (const SRenderPassDesc& other){
		nofAttachments = other.nofAttachments;
		for(uint i = 0; i < nofAttachments; ++i)
			attachments[i] = other.attachments[i];
		depthStencil = other.depthStencil;

		return *this;
	}
};

struct SClearColorValues{
	vec4 color[RD_MAX_RENDER_ATTACHMENTS];
	float depth = 1.0f;
	uint32 stencil = 0x00000000;

	SClearColorValues(std::vector<vec4> colors, float d = 1.0f, uint s = 0x00000000){
		for(uint i = 0; i < RD_MAX_RENDER_ATTACHMENTS; ++i){
			if(i < colors.size())
				color[i] = colors[i];
			else
				color[i] = vec4(0.0f, 0.0f, 0.0f, 0.0f);
		}
		depth = d;
		stencil = s;
	}
	SClearColorValues(vec4 c, float d = 1.0f, uint s = 0x00000000){
		for(uint i = 0; i < RD_MAX_RENDER_ATTACHMENTS; ++i)
			color[i] = vec4(0.0f, 0.0f, 0.0f, 0.0f);
		color[0] = c; depth = d; stencil = s;
	}
	SClearColorValues(float d = 1.0f, uint s = 0x00000000){
		for(uint i = 0; i < RD_MAX_RENDER_ATTACHMENTS; ++i)
			color[i] = vec4(0.0f, 0.0f, 0.0f, 0.0f);
		depth = d; stencil = s;
	}
};

#endif //RENDERPASS_DESC_H
