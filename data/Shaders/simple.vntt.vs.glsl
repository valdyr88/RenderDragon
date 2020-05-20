#version 430

//vertex, normal, tangent, texCoord
layout(location = 0) in vec3 vertex;
layout(location = 1) in vec3 normal;
layout(location = 2) in vec3 tangent;
layout(location = 3) in vec2 texCoord;

layout(std140, row_major, binding = 0) uniform TransformMatrices{
	mat4 world;
	mat4 view;
	mat4 projection;
} transform;

out vec3 WorldPosition;
out vec3 Normal;
out vec3 Tangent;
out vec2 UV;

void main(){
	vec4 wPosition = vec4(vertex,1.0) * transform.world;
	gl_Position = (wPosition*transform.view)*transform.projection;
	WorldPosition = wPosition.xyz;
	Normal = normal;
	Tangent = tangent;
	UV = texCoord;
}
