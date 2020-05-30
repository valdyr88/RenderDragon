#ifndef VECTYPES_H
#define VECTYPES_H

#define GLM_FORCE_XYZW_ONLY
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"

typedef float scalar;
typedef glm::vec2 vec2;
typedef glm::vec3 vec3;
typedef glm::vec4 vec4;
typedef glm::ivec2 ivec2;
typedef glm::ivec3 ivec3;
typedef glm::ivec4 ivec4;
typedef glm::uvec2 uivec2;
typedef glm::uvec3 uivec3;
typedef glm::uvec4 uivec4;
typedef glm::mat2 mat2;
typedef glm::mat3 mat3;
typedef glm::mat4 mat4;
typedef glm::mat2 mat2x2;
typedef glm::mat3 mat3x3;
typedef glm::mat4 mat4x4;
typedef glm::mat<2, 3, glm::f32, glm::defaultp> mat2x3;
typedef glm::mat<3, 2, glm::f32, glm::defaultp> mat3x2;
typedef glm::mat<2, 4, glm::f32, glm::defaultp> mat2x4;
typedef glm::mat<4, 2, glm::f32, glm::defaultp> mat4x2;
typedef glm::mat<3, 4, glm::f32, glm::defaultp> mat3x4;
typedef glm::mat<4, 3, glm::f32, glm::defaultp> mat4x3;

#include "types.h"

template<> inline bool istypeof<vec2>(EValueType v, EValueSize s){ return (v == EValueType::float32) && (s == EValueSize::vec2); }
template<> inline bool istypeof<vec3>(EValueType v, EValueSize s){ return (v == EValueType::float32) && (s == EValueSize::vec3); }
template<> inline bool istypeof<vec4>(EValueType v, EValueSize s){ return (v == EValueType::float32) && (s == EValueSize::vec4); }
template<> inline bool istypeof<ivec2>(EValueType v, EValueSize s){ return (v == EValueType::int32) && (s == EValueSize::vec2); }
template<> inline bool istypeof<ivec3>(EValueType v, EValueSize s){ return (v == EValueType::int32) && (s == EValueSize::vec3); }
template<> inline bool istypeof<ivec4>(EValueType v, EValueSize s){ return (v == EValueType::int32) && (s == EValueSize::vec4); }
template<> inline bool istypeof<uivec2>(EValueType v, EValueSize s){ return (v == EValueType::uint32) && (s == EValueSize::vec2); }
template<> inline bool istypeof<uivec3>(EValueType v, EValueSize s){ return (v == EValueType::uint32) && (s == EValueSize::vec3); }
template<> inline bool istypeof<uivec4>(EValueType v, EValueSize s){ return (v == EValueType::uint32) && (s == EValueSize::vec4); }
template<> inline bool istypeof<mat2x2>(EValueType v, EValueSize s){ return (v == EValueType::float32) && (s == EValueSize::mat2x2); }
template<> inline bool istypeof<mat3x3>(EValueType v, EValueSize s){ return (v == EValueType::float32) && (s == EValueSize::mat3x3); }
template<> inline bool istypeof<mat4x4>(EValueType v, EValueSize s){ return (v == EValueType::float32) && (s == EValueSize::mat4x4); }
template<> inline bool istypeof<mat2x3>(EValueType v, EValueSize s){ return (v == EValueType::float32) && (s == EValueSize::mat2x3); }
template<> inline bool istypeof<mat3x2>(EValueType v, EValueSize s){ return (v == EValueType::float32) && (s == EValueSize::mat3x2); }
template<> inline bool istypeof<mat2x4>(EValueType v, EValueSize s){ return (v == EValueType::float32) && (s == EValueSize::mat2x4); }
template<> inline bool istypeof<mat4x2>(EValueType v, EValueSize s){ return (v == EValueType::float32) && (s == EValueSize::mat4x2); }
template<> inline bool istypeof<mat3x4>(EValueType v, EValueSize s){ return (v == EValueType::float32) && (s == EValueSize::mat3x4); }
template<> inline bool istypeof<mat4x3>(EValueType v, EValueSize s){ return (v == EValueType::float32) && (s == EValueSize::mat4x3); }

template<typename type, typename ttype>
inline type lerp(type a, type b, ttype t){ return (ttype(1.0)-t)*a + t*b; }

inline bool isClockwise(vec3 a, vec3 b, vec3 c, vec3 N){
	vec3 v13 = glm::normalize(a - c);
	vec3 v23 = glm::normalize(b - c);

	vec3 n = glm::cross(v13, v23);

	if(glm::dot(N,n) < 0.0f) return true; //clockwise
	else return false; //counterclockways
}
inline bool isClockwise(vec2 a, vec2 b, vec2 c){
	return isClockwise(vec3(a.x, a.y, 0.0f), vec3(b.x, b.y, 0.0f), vec3(c.x, c.y, 0.0f), vec3(0.0f,0.0f,1.0f)); }

inline vec3 calcNormal(vec3 a, vec3 b, vec3 c){
	vec3 v13 = glm::normalize(a - c);
	vec3 v23 = glm::normalize(b - c);
	return glm::normalize(glm::cross(v13, v23));
}

#endif //VECTYPES_H