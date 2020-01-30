#ifndef TEXTURE_H
#define TEXTURE_H

#include "../../utils/log.h"
#include "../../utils/pointers.h"
#include "../../utils/types/types.h"
#include "../../utils/types/vectypes.h"
#include "../enum/graphics_enums.h"
#include "graphic_object.h"
#include "shader_desc.h"

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

class CTexture : public CGraphicObject{
protected:
	STextureDesc description;
	UniquePtr<CTextureView> view;
public:
	CTexture(WeakPtr<IDevice>& dev, const STextureDesc& desc, const SRawData& data) :
		CGraphicObject(dev), description(desc){
	}
};

#endif //TEXTURE_H