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

enum class ValueType {
	int8,
	int16,
	int32,
	uint8,
	uint16,
	uint32,
	float16,
	float32,
	float64
};

inline uint sizeInBytes(ValueType t){
	switch(t)
	{
		case ValueType::int8: return 1;
		case ValueType::int16: return 2;
		case ValueType::int32: return 4;
		case ValueType::uint8: return 1;
		case ValueType::uint16: return 2;
		case ValueType::uint32: return 4;
		case ValueType::float16: return 2;
		case ValueType::float32: return 4;
		case ValueType::float64: return 8;
	}
	return 0;
}

enum class ValueSize {
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

inline uint count(ValueSize s){
	switch(s)
	{
		case ValueSize::scalar:return 1;
		case ValueSize::vec2:return 2;
		case ValueSize::vec3:return 3;
		case ValueSize::vec4:return 4;
		case ValueSize::mat2x2:return 4;
		case ValueSize::mat3x3:return 9;
		case ValueSize::mat4x4:return 16;
		case ValueSize::mat3x2:return 6;
		case ValueSize::mat2x3:return 6;
		case ValueSize::mat4x3:return 12;
		case ValueSize::mat3x4:return 12;
		case ValueSize::mat4x2:return 8;
		case ValueSize::mat2x4:return 8;
	}
	return 0;
}

#endif //TYPES_H