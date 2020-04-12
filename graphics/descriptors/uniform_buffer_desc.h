#ifndef UNIFORM_BUFFER_DESC_H
#define UNIFORM_BUFFER_DESC_H

#include <string>
#include "../utils/types/types.h"
#include "../utils/types/vectypes.h"
#include "graphics_enums.h"

struct SUniformMap
{
	std::string name = "";
	EValueType type = EValueType::float32;
	EValueSize size = EValueSize::vec4;
	uint32 count = 1;
	uint32 offset = 0;

	SUniformMap(const char* n, EValueType t, EValueSize s, uint32 c = 1) :
		type(t), size(s), count(c), name(n) {
		offset = 0;
	}

	SUniformMap(){ type = EValueType::float32; size = EValueSize::vec4; count = 1; name = ""; offset = 0; }

	bool operator ==(const SUniformMap& other) const{
		return type == other.type &&
			size == other.size &&
			count == other.count &&
			name == other.name;
	}
	bool operator !=(const SUniformMap& other) const{ return !(*this == other); }

	SUniformMap& operator =(const SUniformMap& other){
		type = other.type;
		size = other.size;
		count = other.count;
		name = other.name;
		offset = other.offset;
		return *this;
	}
};

#endif //UNIFORM_BUFFER_DESC_H