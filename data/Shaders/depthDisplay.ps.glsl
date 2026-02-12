#version 430
#global_defines

#include "include/functions.glsl"
#include "include/defines.glsl"

layout(std140, binding = 2) uniform LightData{
	vec3 position;
	float intensity;
	float time;
	uint channelMask;
} light;

layout(binding = 0) uniform sampler2D tx;

layout(location = 0) in vec2 UV;
layout(location = 1) in vec3 Normal;

layout(location = 0) out vec4 FragColor;

void main(){
	// float r = (textureLod(tx, UV.xy, 0.0f).r+light.time)*light.intensity;
	// FragColor = vec4(r,r,r,1.0f);
	vec4 smpl = textureLod(tx, (UV.xy + light.position.xy)*light.position.z, 0.0f).rgba;
	vec4 color = vec4(0,0,0,0);
	if((light.channelMask & 0x01) != 0x00) color.r = smpl.r;
	if((light.channelMask & 0x02) != 0x00) color.g = smpl.g;
	if((light.channelMask & 0x04) != 0x00) color.b = smpl.b;
	if((light.channelMask & 0x08) != 0x00) color.a = smpl.a;
	if((light.channelMask & 0x10) != 0x00) color = vec4(smpl.r);
	if((light.channelMask & 0x20) != 0x00) color = vec4(smpl.g);
	if((light.channelMask & 0x40) != 0x00) color = vec4(smpl.b);
	if((light.channelMask & 0x80) != 0x00) color = vec4(smpl.a);
	FragColor = (color + light.time)*light.intensity; FragColor.a = 1.0f;
	/* if(r < 0.0f){
		r = saturate(abs(r)); FragColor = vec4(0.1f*r,r,r,1.0f);}
	else{
		r = saturate(r); FragColor = vec4(r,0.5f*r,0.1f*r,1.0f);} */
}
