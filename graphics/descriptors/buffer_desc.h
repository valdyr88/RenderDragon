#ifndef BUFFER_DESC_H
#define BUFFER_DESC_H

#include "../utils/types/types.h"
#include "graphics_enums.h"

struct SBufferDesc{
	EBufferType type = EBufferType::None;
	EShaderResourceUsageType usage = EShaderResourceUsageType::Static;
	EShaderResourceAccessType access = EShaderResourceAccessType::Draw;

	sizetype size = 0;

	bool operator == (const SBufferDesc& other) const{
		return type == other.type &&
			usage == other.usage &&
			size == other.size;
	}
	bool operator != (const SBufferDesc& other) const{ return !(*this == other); }

	SBufferDesc operator =(const SBufferDesc& other){
		type = other.type;
		usage = other.usage;
		size = other.size;
		return *this;
	}
};

#endif //BUFFER_DESC_H