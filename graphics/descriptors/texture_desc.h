#ifndef TEXTURE_DESC_H
#define TEXTURE_DESC_H

#include "../utils/types/types.h"
#include "../utils/types/vectypes.h"
#include "graphics_enums.h"

struct STextureDesc{
	ETextureFormat format = ETextureFormat::RGBA;
	EValueType type = EValueType::uint8;
	uint32 width = 0, height = 0, depth = 0;
	uint32 usageFlags = 0x00000000;

	bool operator == (const STextureDesc& other){
		return format == other.format &&
			type == other.type &&
			width == other.width &&
			height == other.height &&
			depth == other.depth &&
			usageFlags == other.usageFlags;
	}
	bool operator == (const STextureDesc& other){ !(*this == other); }

	STextureDesc& operator = (const STextureDesc& other){
		format = other.format;
		type = other.type;
		width = other.width;
		height = other.height;
		depth = other.depth;
		usageFlags = other.usageFlags;

		return *this;
	}
};

#endif //TEXTURE_DESC_H