#ifndef TYPES_H
#define TYPES_H

#include <stdint.h>

typedef uint8_t uint8;
typedef uint16_t uint16;
typedef uint32_t uint32;
typedef uint64_t uint64;

typedef int8_t int8;
typedef int16_t int16;
typedef int32_t int32;
typedef int64_t int64;

typedef float float32;
typedef double float64;

typedef unsigned int uint;
typedef uint8_t byte;

enum class EValueType {
	int8,
	int16,
	int32,
	uint8,
	uint16,
	uint24, //for depth
	uint32,
	float16,
	float24, //for depth
	float32,
	float64
};

inline uint sizeInBytes(EValueType t){
	switch(t)
	{
		case EValueType::int8: return 1;
		case EValueType::int16: return 2;
		case EValueType::int32: return 4;
		case EValueType::uint8: return 1;
		case EValueType::uint16: return 2;
		case EValueType::uint24: return 3;
		case EValueType::uint32: return 4;
		case EValueType::float16: return 2;
		case EValueType::float24: return 3;
		case EValueType::float32: return 4;
		case EValueType::float64: return 8;
	}
	return 0;
}

enum class EValueSize {
	scalar,
	vec2,
	vec3,
	vec4,
	mat2x2,
	mat3x3,
	mat4x4,
	mat3x2,
	mat2x3,
	mat4x3,
	mat3x4,
	mat4x2,
	mat2x4
};

inline uint count(EValueSize s){
	switch(s)
	{
		case EValueSize::scalar:return 1;
		case EValueSize::vec2:return 2;
		case EValueSize::vec3:return 3;
		case EValueSize::vec4:return 4;
		case EValueSize::mat2x2:return 4;
		case EValueSize::mat3x3:return 9;
		case EValueSize::mat4x4:return 16;
		case EValueSize::mat3x2:return 6;
		case EValueSize::mat2x3:return 6;
		case EValueSize::mat4x3:return 12;
		case EValueSize::mat3x4:return 12;
		case EValueSize::mat4x2:return 8;
		case EValueSize::mat2x4:return 8;
	}
	return 0;
}

const uint64 uint64_max = ((uint64)(0xffffffffffffffff));
const int64 int64_max = ((int64)(0x7fffffffffffffff));
const uint32 uint32_max = ((uint32)(0xffffffff));//((uint)-1)
const int32 int32_max = ((int32)(0x7fffffff));
const int16 int16_max = ((int16)(0x7fff));
const uint16 uint16_max = ((uint16)(0xffff));
const int8 int8_max = ((int8)(0x7f));
const uint8 uint8_max = ((uint8)(0xff));
const byte byte_max = ((byte)(0xff));

#endif //TYPES_H