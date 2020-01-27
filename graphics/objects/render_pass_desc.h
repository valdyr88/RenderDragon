#ifndef RENDERPASS_DESC_H
#define RENDERPASS_DESC_H

#include <vector>
#include "../../utils/types/types.h"
#include "../../utils/types/vectypes.h"
#include "../enum/graphics_enums.h"

struct AttachmentDesc {
	TextureFormat format = TextureFormat::RGBA;
	ValueType dataType = ValueType::uint8;
	uint8 sampleCount = 1;
	LoadStoreOp loadOp = LoadStoreOp::Clear;
	LoadStoreOp storeOp = LoadStoreOp::Store;
	LoadStoreOp stencilLoadOp = LoadStoreOp::Clear;
	LoadStoreOp stencilStoreOp = LoadStoreOp::Store;

	bool operator == (const AttachmentDesc& other){
		return format == other.format &&
				dataType == other.dataType &&
				sampleCount == other.sampleCount &&
				loadOp == other.loadOp &&
				storeOp == other.storeOp &&
				stencilLoadOp == other.stencilLoadOp &&
				stencilStoreOp == other.stencilStoreOp;
	}
	bool operator != (const AttachmentDesc& other){ return !(*this == other); }

	AttachmentDesc& operator = (const AttachmentDesc& other) {
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

struct RenderPassDesc {
	uint32 NofAttachments = 0;
	AttachmentDesc Attachments[RD_MAX_RENDER_ATTACHMENTS];

	bool operator == (const RenderPassDesc& other){
		if(NofAttachments != other.NofAttachments) return false;
		for(uint i = 0; i < NofAttachments; ++i){
			auto& thisAtt = Attachments[i];
			auto& othrAtt = other.Attachments[i];
			if(thisAtt != othrAtt) return false;
		}
		return true;
	}
	bool operator != (const RenderPassDesc& other){ return !(*this == other); }

	RenderPassDesc& operator = (const RenderPassDesc& other){
		NofAttachments = other.NofAttachments;
		for(uint i = 0; i < NofAttachments; ++i)
			Attachments[i] = other.Attachments[i];

		return *this;
	}
};

struct ClearColorValues{
	vec4 color[RD_MAX_RENDER_ATTACHMENTS];
	float depth = 1.0f;
	uint32 stencil = 0x00000000;

	ClearColorValues(std::vector<vec4> colors, float d = 1.0f, uint s = 0x00000000){
		for(uint i = 0; i < RD_MAX_RENDER_ATTACHMENTS; ++i){
			if(i < colors.size())
				color[i] = colors[i];
			else
				color[i] = vec4(0.0f, 0.0f, 0.0f, 0.0f);
		}
		depth = d;
		stencil = s;
	}
	ClearColorValues(vec4 c, float d = 1.0f, uint s = 0x00000000){
		for(uint i = 0; i < RD_MAX_RENDER_ATTACHMENTS; ++i)
			color[i] = vec4(0.0f, 0.0f, 0.0f, 0.0f);
		color[0] = c; depth = d; stencil = s;
	}
	ClearColorValues(float d = 1.0f, uint s = 0x00000000){
		for(uint i = 0; i < RD_MAX_RENDER_ATTACHMENTS; ++i)
			color[i] = vec4(0.0f, 0.0f, 0.0f, 0.0f);
		depth = d; stencil = s;
	}
};

#endif //RENDERPASS_DESC_H
