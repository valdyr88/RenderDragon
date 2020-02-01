#ifndef VERTEX_BUFFER_DESC_H
#define VERTEX_BUFFER_DESC_H

#include <string>
#include <vector>
#include "../utils/types/types.h"
#include "../utils/types/vectypes.h"
#include "graphics_enums.h"

struct SVertexAttribute{
	EValueType type = EValueType::float32;
	EValueSize count = EValueSize::vec4;
	std::string name = "";
	bool bIsNormalized = false;

	bool operator == (SVertexAttribute& other){
		return type == other.type &&
			count == other.count &&
			bIsNormalized == other.bIsNormalized &&
			name == other.name;
	}
	bool operator != (SVertexAttribute& other){ return !(*this == other); }

	SVertexAttribute& operator = (const SVertexAttribute& other){
		type = other.type;
		count = other.count;
		name = other.name;
		bIsNormalized = other.bIsNormalized;
		return *this;
	}
};

struct SVertexFormat{
	std::vector<SVertexAttribute> attributes;
	EAttributeLayout layout = EAttributeLayout::Interleaved;
};

#endif //VERTEX_BUFFER_DESC_H