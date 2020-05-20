#version 430

#include "Shaders/include/functions.glsl"
#include "Shaders/include/defines.glsl"

layout(std140, binding = 2) uniform LightData{
	vec3 position;
	float intensity;
	float time;
} light;

// layout(binding = 0) uniform sampler2D tx;

in vec3 WorldPosition;
in vec3 Normal;
in vec3 Tangent;
in vec2 UV;

out vec4 FragColor;

void main(){

	vec3 LightPosition = 1000.0f*vec3(0.1,0.5,1.0);
	vec3 toLight = LightPosition - WorldPosition;
	vec3 L = normalize(toLight);
	vec3 N = normalize(Normal);
	
	float shade = dot(L,N)*0.5f + 0.5f;

	FragColor = vec4(shade,shade,shade,1.0f);
	// FragColor.xyz = 0.5f*N+0.5f;
	FragColor.a = 1.0f;
	
	// FragColor = vec4(1.0f,0.0f,1.0f,1.0f);
}
