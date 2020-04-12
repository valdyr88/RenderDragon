#ifndef VERTEX_BUFFER_DESC_H
#define VERTEX_BUFFER_DESC_H

#include <string>
#include <vector>
#include "../utils/types/types.h"
#include "../utils/types/vectypes.h"
#include "graphics_enums.h"

//-----------------------------------------------------------------------------------

struct SVertexAttributeDesc{
	std::string name = "";
	uint32 binding = 0;

	EValueType type = EValueType::float32;
	EValueSize count = EValueSize::vec4;

	bool bIsNormalized = false;
	uint32 stride = 0;

	SVertexAttributeDesc(std::string n, uint32 b, EValueType t, EValueSize c, bool bNormalized = false, uint32 s = 0) :
		name(n), binding(b), type(t), count(c), bIsNormalized(bNormalized), stride(s){}

	bool operator == (SVertexAttributeDesc& other) const{
		return binding == other.binding &&
			type == other.type &&
			count == other.count &&
			stride == other.stride &&
			bIsNormalized == other.bIsNormalized &&
			name == other.name;
	}
	bool operator != (SVertexAttributeDesc& other) const{ return !(*this == other); }
	SVertexAttributeDesc& operator = (const SVertexAttributeDesc& other){
		binding = other.binding;
		type = other.type;
		count = other.count;
		stride = other.stride;
		bIsNormalized = other.bIsNormalized;
		name = other.name;
		return *this;
	}
};

struct SVertexFormat{
	std::vector<SVertexAttributeDesc> attributes;
	EAttributeLayout layout = EAttributeLayout::Interleaved;
	uint32 stride = 0;
};
//-----------------------------------------------------------------------------------

#endif //VERTEX_BUFFER_DESC_H