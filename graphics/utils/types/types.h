#ifndef TYPES_H
#define TYPES_H

#include <stdint.h>
#include "..\platform_defines.h"

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

#ifndef PLATFORM_EMSCRIPTEN
typedef uint8_t byte;
#else
#include <cstddef>
using byte = std::byte;
#endif

typedef char char8;
typedef wchar_t char16;

typedef size_t sizetype;

template <typename type>
inline bool isequal(const type* a, const type* b, uint l){
	if(a == nullptr || b == nullptr) return false;
	if(a == b) return true;
	for(uint i = 0; i < l; ++i)
		if(a[i] != b[i]) return false;
	return true;
}
inline bool strisequal(const char* a, const char* b){
	if(a == nullptr || b == nullptr) return false;
	if(a == b) return true;
	uint i = 0; 
	for(i = 0; a[i] != 0 && b[i] != 0; ++i)
		if(a[i] != b[i]) return false;
	return (a[i] == b[i]);
}

enum class EValueType {
	int8,
	int16,
	int32,
	int64,
	uint8,
	uint16,
	uint24, //for depth
	uint32,
	uint64,
	float16,
	float24, //for depth
	float32,
	float64,
	char8,//string
	char16,//wstring
};

inline uint sizeInBytes(EValueType t){
	switch(t)
	{
		case EValueType::int8: return 1;
		case EValueType::int16: return 2;
		case EValueType::int32: return 4;
		case EValueType::int64: return 8;
		case EValueType::uint8: return 1;
		case EValueType::uint16: return 2;
		case EValueType::uint24: return 3;
		case EValueType::uint32: return 4;
		case EValueType::uint64: return 8;
		case EValueType::float16: return 2;
		case EValueType::float24: return 3;
		case EValueType::float32: return 4;
		case EValueType::float64: return 8;
		case EValueType::char8: return 1;
		case EValueType::char16: return 2;
		default: break;
	}
	return 0;
}

inline bool isFloatType(EValueType t){
	switch(t)
	{
		case EValueType::float16:
		case EValueType::float24:
		case EValueType::float32:
		case EValueType::float64: return true;
		default: break;
	}
	return false;
}
inline bool isIntType(EValueType t){
	switch(t)
	{
		case EValueType::int8: 
		case EValueType::int16:
		case EValueType::int32:
		case EValueType::int64:
		case EValueType::uint8:
		case EValueType::uint16:
		case EValueType::uint24:
		case EValueType::uint32:
		case EValueType::uint64: return true;
		default: break;
	}
	return false;
}
inline bool isCharType(EValueType t){
	switch(t)
	{
		case EValueType::char8:
		case EValueType::char16: return true;
		default: break;
	}
	return false; 
}

inline const char* tostring(EValueType t){
	switch(t)
	{
		case EValueType::int8: return "int8";
		case EValueType::int16: return "int16";
		case EValueType::int32: return "int32";
		case EValueType::int64: return "int64";
		case EValueType::uint8: return "uint8";
		case EValueType::uint16: return "uint16";
		case EValueType::uint24: return "uint24";
		case EValueType::uint32: return "uint32";
		case EValueType::uint64: return "uint64";
		case EValueType::float16: return "float16";
		case EValueType::float24: return "float24";
		case EValueType::float32: return "float32";
		case EValueType::float64: return "float64";
		case EValueType::char8: return "char8";
		case EValueType::char16: return "char16";
		default: break;
	}
	return "none";
}
inline EValueType toEValueType(const char* str){
	if(str == nullptr) return EValueType::float32;
	if(strisequal(str, "scalar")) return EValueType::float32;
	if(strisequal(str, "float")) return EValueType::float32;
	if(strisequal(str, "vec2")) return EValueType::float32;
	if(strisequal(str, "vec3")) return EValueType::float32;
	if(strisequal(str, "vec4")) return EValueType::float32;
	if(strisequal(str, "mat2x2")) return EValueType::float32;
	if(strisequal(str, "mat3x3")) return EValueType::float32;
	if(strisequal(str, "mat4x4")) return EValueType::float32;
	if(strisequal(str, "mat3x2")) return EValueType::float32;
	if(strisequal(str, "mat2x3")) return EValueType::float32;
	if(strisequal(str, "mat4x3")) return EValueType::float32;
	if(strisequal(str, "mat3x4")) return EValueType::float32;
	if(strisequal(str, "mat4x2")) return EValueType::float32;
	if(strisequal(str, "mat2x4")) return EValueType::float32;
	if(strisequal(str, "int")) return EValueType::int32;
	if(strisequal(str, "ivec2")) return EValueType::int32;
	if(strisequal(str, "ivec3")) return EValueType::int32;
	if(strisequal(str, "ivec4")) return EValueType::int32;
	if(strisequal(str, "uint")) return EValueType::uint32;
	if(strisequal(str, "uivec2")) return EValueType::uint32;
	if(strisequal(str, "uivec3")) return EValueType::uint32;
	if(strisequal(str, "uivec4")) return EValueType::uint32;
	if(strisequal(str, "char")) return EValueType::char8;
	if(strisequal(str, "wchar")) return EValueType::char16;
	return EValueType::float32;
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
		default: break;
	}
	return 0;
}
inline uint componentCount(EValueSize s){ return count(s); }

inline const char* tostring(EValueSize v){
	switch(v)
	{
		case EValueSize::scalar:return "scalar";
		case EValueSize::vec2:return "vec2";
		case EValueSize::vec3:return "vec3";
		case EValueSize::vec4:return "vec4";
		case EValueSize::mat2x2:return "mat2x2";
		case EValueSize::mat3x3:return "mat3x3";
		case EValueSize::mat4x4:return "mat4x4";
		case EValueSize::mat3x2:return "mat3x2";
		case EValueSize::mat2x3:return "mat2x3";
		case EValueSize::mat4x3:return "mat4x3";
		case EValueSize::mat3x4:return "mat3x4";
		case EValueSize::mat4x2:return "mat4x2";
		case EValueSize::mat2x4:return "mat2x4";
		default: break;
	}
	return "none";
}
inline EValueSize toEValueSize(const char* str){
	if(str == nullptr) return EValueSize::scalar;
	if(strisequal(str, "scalar")) return EValueSize::scalar;
	if(strisequal(str, "vec2")) return EValueSize::vec2;
	if(strisequal(str, "vec3")) return EValueSize::vec3;
	if(strisequal(str, "vec4")) return EValueSize::vec4;
	if(strisequal(str, "mat2x2")) return EValueSize::mat2x2;
	if(strisequal(str, "mat3x3")) return EValueSize::mat3x3;
	if(strisequal(str, "mat4x4")) return EValueSize::mat4x4;
	if(strisequal(str, "mat3x2")) return EValueSize::mat3x2;
	if(strisequal(str, "mat2x3")) return EValueSize::mat2x3;
	if(strisequal(str, "mat4x3")) return EValueSize::mat4x3;
	if(strisequal(str, "mat3x4")) return EValueSize::mat3x4;
	if(strisequal(str, "mat4x2")) return EValueSize::mat4x2;
	if(strisequal(str, "mat2x4")) return EValueSize::mat2x4;
	if(strisequal(str, "float")) return EValueSize::scalar;
	if(strisequal(str, "int")) return EValueSize::scalar;
	if(strisequal(str, "uint")) return EValueSize::scalar;
	if(strisequal(str, "ivec2")) return EValueSize::vec2;
	if(strisequal(str, "ivec3")) return EValueSize::vec3;
	if(strisequal(str, "ivec4")) return EValueSize::vec4;
	return EValueSize::scalar;
}
inline EValueSize toEValueSizeVec(uint count){
	switch(count){
		case 1: return EValueSize::scalar;
		case 2: return EValueSize::vec2;
		case 3: return EValueSize::vec3;
		case 4: return EValueSize::vec4;
		default: break;
	}
	return EValueSize::scalar;
}
inline EValueSize toEValueSizeMat(uint count, bool colmajor = false){
	switch(count){
		case 1: return EValueSize::scalar;
		case 4: return EValueSize::mat2x2;
		case 6: return (colmajor)? EValueSize::mat3x2 : EValueSize::mat2x3;
		case 8: return (colmajor)? EValueSize::mat4x2 : EValueSize::mat2x4;
		case 9: return EValueSize::mat3x3;
		case 12: return (colmajor)? EValueSize::mat4x3 : EValueSize::mat3x4;
		case 16: return EValueSize::mat4x4;
		default: break;
	}
	return EValueSize::scalar;
}

template <typename type> bool istypeof(EValueType v, EValueSize s){ return false; }
template<> inline bool istypeof<float>(EValueType v, EValueSize s){ return (v == EValueType::float32) && (s == EValueSize::scalar); }
template<> inline bool istypeof<double>(EValueType v, EValueSize s){ return (v == EValueType::float64) && (s == EValueSize::scalar); }
template<> inline bool istypeof<int64>(EValueType v, EValueSize s){ return (v == EValueType::int64) && (s == EValueSize::scalar); }
template<> inline bool istypeof<uint64>(EValueType v, EValueSize s){ return (v == EValueType::uint64) && (s == EValueSize::scalar); }
template<> inline bool istypeof<int32>(EValueType v, EValueSize s){ return (v == EValueType::int32) && (s == EValueSize::scalar); }
template<> inline bool istypeof<uint32>(EValueType v, EValueSize s){ return (v == EValueType::uint32) && (s == EValueSize::scalar); }
template<> inline bool istypeof<int16>(EValueType v, EValueSize s){ return (v == EValueType::int16) && (s == EValueSize::scalar); }
template<> inline bool istypeof<uint16>(EValueType v, EValueSize s){ return (v == EValueType::uint16) && (s == EValueSize::scalar); }
template<> inline bool istypeof<int8>(EValueType v, EValueSize s){ return (v == EValueType::int8) && (s == EValueSize::scalar); }
template<> inline bool istypeof<uint8>(EValueType v, EValueSize s){ return (v == EValueType::uint8) && (s == EValueSize::scalar); }
template<> inline bool istypeof<char8>(EValueType v, EValueSize s){ return (v == EValueType::char8) && (s == EValueSize::scalar); }
template<> inline bool istypeof<char16>(EValueType v, EValueSize s){ return (v == EValueType::char16) && (s == EValueSize::scalar); }


const uint64 uint64_max = ((uint64)(0xffffffffffffffff));
const int64 int64_max = ((int64)(0x7fffffffffffffff));
const uint32 uint32_max = ((uint32)(0xffffffff));//((uint)-1)
const int32 int32_max = ((int32)(0x7fffffff));
const int16 int16_max = ((int16)(0x7fff));
const uint16 uint16_max = ((uint16)(0xffff));
const int8 int8_max = ((int8)(0x7f));
const uint8 uint8_max = ((uint8)(0xff));
const byte byte_max = ((byte)(0xff));
const float32 float32_infinity = ((float32)((1e+300)*(1e+300)));
const float32 float32_nan = ((float32)(((1e+300)*(1e+300))*0.0));
const float64 float64_infinity = ((float64)((float32)((1e+300)*(1e+300))));
const float64 float64_nan = ((float64)((float32)(((1e+300)*(1e+300))*0.0)));

#endif //TYPES_H