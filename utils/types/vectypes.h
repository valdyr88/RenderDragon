#ifndef VECTYPES_H
#define VECTYPES_H

#include "glm/glm.hpp"

typedef float scalar;
typedef glm::vec2 vec2;
typedef glm::vec3 vec3;
typedef glm::vec4 vec4;
typedef glm::ivec2 ivec2;
typedef glm::ivec3 ivec3;
typedef glm::ivec4 ivec4;
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

#endif //VECTYPES_H