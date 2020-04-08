#ifndef VERTEX_BUFFER_DESC_H
#define VERTEX_BUFFER_DESC_H

#include <string>
#include <vector>
#include "../utils/types/types.h"
#include "../utils/types/vectypes.h"
#include "graphics_enums.h"

//-----------------------------------------------------------------------------------

//ToDo: finish this
struct SVertexAttributeDesc{
	std::string name = "";
	uint32 binding = 0; //shader location && if separate VertexPointer, NormalPointer, TexCoordPointer etc

	EValueType type = EValueType::float32;
	EValueSize count = EValueSize::vec4;

	uint32 stride = 0; //if struct has Vertex/Normal/TexCoord then at what offset
	bool bIsNormalized = false;

	SVertexAttributeDesc(std::string n, uint32 b, EValueType t, EValueSize c, uint32 s = 0, bool bNormalized = false) :
		name(n), binding(b), type(t), count(c), stride(s), bIsNormalized(bNormalized){}

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
/*
struct SVertexInputDesc{
	std::vector<SVertexAttributeDesc> attributes;
}; */
//-----------------------------------------------------------------------------------
/*
struct SVertexAttribute{
	std::string name = "";
	EValueType type = EValueType::float32;
	EValueSize count = EValueSize::vec4;
	bool bIsNormalized = false;

	SVertexAttribute(std::string n, EValueType t, EValueSize c, bool bNormalized = false) :
		name(n), type(t), count(c), bIsNormalized(bNormalized){}

	bool operator == (SVertexAttribute& other) const{
		return type == other.type &&
			count == other.count &&
			bIsNormalized == other.bIsNormalized &&
			name == other.name;
	}
	bool operator != (SVertexAttribute& other) const{ return !(*this == other); }

	SVertexAttribute& operator = (const SVertexAttribute& other){
		type = other.type;
		count = other.count;
		name = other.name;
		bIsNormalized = other.bIsNormalized;
		return *this;
	}
};*/

struct SVertexFormat{
	std::vector<SVertexAttributeDesc> attributes;
	EAttributeLayout layout = EAttributeLayout::Interleaved;
	uint32 stride = 0;
};

#endif //VERTEX_BUFFER_DESC_H