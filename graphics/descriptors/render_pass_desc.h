#ifndef RENDERPASS_DESC_H
#define RENDERPASS_DESC_H

#include <vector>
#include "../utils/types/types.h"
#include "../utils/types/vectypes.h"
#include "graphics_enums.h"

struct SAttachmentDesc {
	ETextureFormat format = ETextureFormat::RGBA;
	EValueType dataType = EValueType::uint8;
	uint8 sampleCount = 1;
	ELoadStoreOp loadOp = ELoadStoreOp::Clear;
	ELoadStoreOp storeOp = ELoadStoreOp::Store;
	ELoadStoreOp stencilLoadOp = ELoadStoreOp::Clear;
	ELoadStoreOp stencilStoreOp = ELoadStoreOp::Store;

	bool operator == (const SAttachmentDesc& other){
		return format == other.format &&
				dataType == other.dataType &&
				sampleCount == other.sampleCount &&
				loadOp == other.loadOp &&
				storeOp == other.storeOp &&
				stencilLoadOp == other.stencilLoadOp &&
				stencilStoreOp == other.stencilStoreOp;
	}
	bool operator != (const SAttachmentDesc& other){ return !(*this == other); }

	SAttachmentDesc& operator = (const SAttachmentDesc& other) {
		format = other.format;
		dataType = other.dataType;
		sampleCount = other.sampleCount;
		loadOp = other.loadOp;
		storeOp = other.storeOp;
		stencilLoadOp = other.stencilLoadOp;
		stencilStoreOp = other.stencilStoreOp;

		return *this;
	}
};

struct SRenderPassDesc {
	uint32 NofAttachments = 0;
	SAttachmentDesc Attachments[RD_MAX_RENDER_ATTACHMENTS];

	bool operator == (const SRenderPassDesc& other){
		if(NofAttachments != other.NofAttachments) return false;
		for(uint i = 0; i < NofAttachments; ++i){
			auto& thisAtt = Attachments[i];
			auto& othrAtt = other.Attachments[i];
			if(thisAtt != othrAtt) return false;
		}
		return true;
	}
	bool operator != (const SRenderPassDesc& other){ return !(*this == other); }

	SRenderPassDesc& operator = (const SRenderPassDesc& other){
		NofAttachments = other.NofAttachments;
		for(uint i = 0; i < NofAttachments; ++i)
			Attachments[i] = other.Attachments[i];

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
