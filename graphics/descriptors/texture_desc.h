#ifndef TEXTURE_DESC_H
#define TEXTURE_DESC_H

#include "../utils/types/glm/common.hpp"
#include "../utils/types/types.h"
#include "../utils/types/vectypes.h"
#include "../utils/pointers.h"
#include "graphics_enums.h"

inline uint rdCalcNumberOfMips(uint w, uint h, uint d){
	uint v = glm::max(glm::max(w, h), d);
	uint num = ((uint)glm::floor(log2(v))) + 1; //was std::ceil
	return num;
}

inline ivec3 rdCalcMipDimension(uint level, uint baseW, uint baseH, uint baseD){
	float divisor = pow(2.0f, (float)level);
	vec3 dim = vec3((float)baseW,(float)baseH,(float)baseD) / divisor;
	dim = glm::max(dim, glm::vec3(1.0f));
	return ivec3((uint)dim.x, (uint)dim.y, (uint)dim.z);
}

struct STextureFormatDesc{
	ETextureType type = ETextureType::Texture2D;
	ETextureFormat format = ETextureFormat::RGBA;
	EValueType valueType = EValueType::uint8;

	bool operator ==(const STextureFormatDesc& other) const{
		return valueType == other.valueType &&
			format == other.format &&
			type == other.type;
	}
	bool operator != (const STextureFormatDesc& other) const{ return !(*this == other); }

	STextureFormatDesc& operator = (const STextureFormatDesc& other){
		type = other.type;
		format = other.format;
		valueType = other.valueType;

		return *this;
	}
};

struct STextureDesc : public STextureFormatDesc{
	uint32 width = 0, height = 0, depth = 0;
	uint32 usageFlags = 0x00000000;
	bool bGenMipmaps = true; //no comparing
	std::string name; //no comparing

	bool operator == (const STextureDesc& other) const{
		return STextureFormatDesc::operator==(other) &&
			width == other.width &&
			height == other.height &&
			depth == other.depth &&
			usageFlags == other.usageFlags;
	}
	bool operator != (const STextureDesc& other) const{ return !(*this == other); }

	STextureDesc& operator = (const STextureDesc& other){
		STextureFormatDesc::operator=(other);
		width = other.width;
		height = other.height;
		depth = other.depth;
		usageFlags = other.usageFlags;
		bGenMipmaps = other.bGenMipmaps;
		name = other.name;

		return *this;
	}
	STextureDesc& operator =(const STextureFormatDesc& other){
		STextureFormatDesc::operator=(other);
		return *this;
	}
};

struct STextureViewDesc : public STextureDesc{
	uint32 mipLevelBase = 0;
	uint32 numMipLevels = 0xffffffff;
	uint32 layerBase = 0;
	uint32 numLayers = 0xffffffff;
	
	bool operator == (const STextureViewDesc& other) const{
		return STextureDesc::operator==(other) &&
			mipLevelBase == other.mipLevelBase &&
			numMipLevels == other.numMipLevels &&
			layerBase == other.layerBase &&
			numLayers == other.numLayers;
	}
	bool operator != (const STextureViewDesc& other) const{ return !(*this == other); }

	STextureViewDesc& operator = (const STextureViewDesc& other){
		STextureDesc::operator=(other);
		mipLevelBase = other.mipLevelBase;
		numMipLevels = other.numMipLevels;
		layerBase = other.layerBase;
		numLayers = other.numLayers;

		return *this;
	}

	STextureViewDesc& operator =(const STextureDesc& other){
		STextureDesc::operator=(other);
		return *this;
	}
	STextureViewDesc& operator =(const STextureFormatDesc& other){
		STextureFormatDesc::operator=(other);
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
		numMips = (hasMips)? rdCalcNumberOfMips(w, h, d) : 1;
		numSlices = numMips*sides;
		slices = __rd_new STextureSliceRawData[numSlices];
	}
	STextureRawData(){}

	~STextureRawData(){
		__rd_release_array(slices);
	}
};

#endif //TEXTURE_DESC_H