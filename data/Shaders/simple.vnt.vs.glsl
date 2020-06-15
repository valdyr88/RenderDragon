#version 430

layout (location = 0) in vec3 vertex;
layout (location = 1) in vec3 normal;
layout (location = 2) in vec2 texCoord;

layout(location = 0) out vec2 UV;
layout(location = 1) out vec3 Normal;

void main(){
	gl_Position = vec4(vertex.xyz, 1.0f);
	UV = texCoord;
	Normal = normal;
}
