#ifndef TEXTURE_DESC_H
#define TEXTURE_DESC_H

#include "../utils/types/glm/common.hpp"
#include "../utils/types/types.h"
#include "../utils/types/vectypes.h"
#include "../utils/pointers.h"
#include "graphics_enums.h"

inline uint CalcNumberOfMips(uint w, uint h, uint d){
	uint v = glm::max(w, h); v = glm::max(v, d);
	uint num = ((uint)glm::ceil(log2(v))) + 1;
	return num;
}

inline ivec3 CalcMipDimension(uint level, uint baseW, uint baseH, uint baseD){
	float divisor = pow(2.0f, (float)level);
	vec3 dim = vec3((float)baseW,(float)baseH,(float)baseD) / divisor;
	dim = glm::max(dim, glm::vec3(1.0f));
	return ivec3((uint)dim.x, (uint)dim.y, (uint)dim.z);
}

struct STextureDesc{
	ETextureType type = ETextureType::Texture2D;
	ETextureFormat format = ETextureFormat::RGBA;
	EValueType valueType = EValueType::uint8;
	uint32 width = 0, height = 0, depth = 0;
	uint32 usageFlags = 0x00000000;
	bool bGenMipmaps = true; //no comparing

	bool operator == (const STextureDesc& other) const{
		return valueType == other.valueType &&
			format == other.format &&
			type == other.type &&
			width == other.width &&
			height == other.height &&
			depth == other.depth &&
			usageFlags == other.usageFlags;
	}
	bool operator != (const STextureDesc& other) const{ return !(*this == other); }

	STextureDesc& operator = (const STextureDesc& other){
		type = other.type;
		format = other.format;
		valueType = other.valueType;
		width = other.width;
		height = other.height;
		depth = other.depth;
		usageFlags = other.usageFlags;
		bGenMipmaps = other.bGenMipmaps;

		return *this;
	}
};

struct STextureSliceRawData : public SRawData{
	uint mipLevel = 0;
	uint side = 0;
};

struct STextureRawData{
	uint numSlices = 0;
	uint numMips = 1;
	STextureSliceRawData* slices = nullptr;

	STextureRawData(uint w, uint h, uint d, uint sides, bool hasMips = true){
		numMips = (hasMips)? CalcNumberOfMips(w, h, d) : 1;
		numSlices = numMips*sides;
		slices = __new STextureSliceRawData[numSlices];
	}

	~STextureRawData(){
		__release_array(slices);
	}

private:
	STextureRawData() = default;
};

#endif //TEXTURE_DESC_H