#version 430
#global_defines

#include "include/functions.glsl"
#include "include/defines.glsl"

layout(std140, binding = 2) uniform LightData{
	vec3 position;
	float intensity;
	float time;
} light;

layout(binding = 0) uniform sampler2D tx;

layout(location = 0) in vec2 UV;
layout(location = 1) in vec3 Normal;

layout(location = 0) out vec4 FragColor;

void main(){
	vec2 d = light.position.xy - UV.xy;
	float v = light.intensity / max(dot(d,d),0.0001f);

	float lod = (cos(10.0f*light.time)*0.5f+0.5f);
	vec4 color = textureLod(tx, UV.xy, lod*10.0f);

	// FragColor = vec4(UV.x, UV.y, 1.0f, 1.0f);
	// FragColor = vec4(lod,lod,lod,1.0f);
	// FragColor = vec4(1.0,1.0,1.0,1.0f);
	FragColor = color;
}
