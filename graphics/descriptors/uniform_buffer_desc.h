#ifndef UNIFORM_BUFFER_DESC_H
#define UNIFORM_BUFFER_DESC_H

#include <string>
#include "../utils/types/types.h"
#include "../utils/types/vectypes.h"
#include "graphics_enums.h"

struct SUniformMap
{
	EValueType type;
	EValueSize size;
	uint32 count;
	std::string name;
	uint32 offset;

	SUniformMap(EValueType t, EValueSize s, uint32 c, const char* n) :
		type(t), size(s), count(c), name(n) {
		offset = 0;
	}

	SUniformMap(){ type = EValueType::float32; size = EValueSize::scalar; count = 1; name = ""; offset = 0; };

	bool operator ==(const SUniformMap& other){
		return type == other.type &&
			size == other.size &&
			count == other.count &&
			name == other.name;
	}
	bool operator !=(const SUniformMap& other){ return !(*this == other); }

	SUniformMap& operator =(SUniformMap& other){
		type = other.type;
		size = other.size;
		count = other.count;
		name = other.name;
		offset = other.offset;
		return *this;
	}
};

#endif //UNIFORM_BUFFER_DESC_H