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
	uint32 location = 0; //shader location
	std::string name = "";

	EValueType type = EValueType::float32;
	EValueSize count = EValueSize::vec4;

	uint32 binding = 0; //if separate VertexPointer, NormalPointer, TexCoordPointer etc
	uint32 stride = 0; //if struct has Vertex/Normal/TexCoord then at what offset
	bool isNormalized = false;

	bool operator == (SVertexAttributeDesc& other) const{}
	bool operator != (SVertexAttributeDesc& other) const{ return !(*this == other); }
	SVertexAttributeDesc& operator = (const SVertexAttributeDesc& other){}
};

struct SVertexInputDesc{
	std::vector<SVertexAttributeDesc> attributes;
};
//-----------------------------------------------------------------------------------

struct SVertexAttribute{
	EValueType type = EValueType::float32;
	EValueSize count = EValueSize::vec4;
	std::string name = "";
	bool bIsNormalized = false;

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
};

struct SVertexFormat{
	std::vector<SVertexAttributeDesc> attributes;
	EAttributeLayout layout = EAttributeLayout::Interleaved;
};

#endif //VERTEX_BUFFER_DESC_H