#version 430

#global_defines

#include "Shaders/include/functions.glsl"
#include "Shaders/include/defines.glsl"

layout(std140, binding = 2) uniform LightData{
	vec3 position;
	float intensity;
	float time;
} light;

layout(binding = 1) uniform sampler2D txDiffuse;
layout(binding = 2) uniform sampler2D txNormal;

in vec3 WorldPosition;
in vec3 Normal;
in vec3 Tangent;
in vec3 Bitangent;
in vec2 UV;
flat in uint Flags;

out vec4 FragColor;

void main(){
/**/
	vec3 LightPosition = 1.0f*vec3(-1.0,-0.0,-0.1);
	highp vec3 toLight = LightPosition - WorldPosition;
	vec3 L = normalize(toLight);
	vec3 N = normalize(Normal);
	
	float distoLight = length(toLight) + 0.0001f;
	
	vec4 color = texture(txDiffuse, UV.xy);
	vec4 normal = texture(txNormal, UV.xy);
	normal.xyz = normalize(2.0f*normal.xyz - vec3(1.0f));
	
	mat3 normalMatrix = mat3(Normal,Tangent,Bitangent);
	N = normalize(normal.xyz*normalMatrix);	
	
	float shade = dot(L,N)*0.5f + 0.5f;
	// shade = 0.1f / pow(distoLight,2.0f);

	FragColor = vec4(vec3(shade),1.0f);
	// FragColor.xyz = 0.5f*N+0.5f;
	// FragColor.xyz = 0.5f*normal.xyz+0.5f;
	// FragColor.xyz = 0.5f*Normal+0.5f;
	// FragColor.xyz = 0.5f*Tangent+0.5f;
	// FragColor.xyz = 0.5f*Bitangent+0.5f;
	// FragColor.xyz = vec3(fract(UV.x),fract(UV.y),1.0f);
	FragColor.a = 1.0f;
	
	// if(Flags != 0)
		// FragColor.xyz = vec3(0.0f);
	// if((Flags & VertexFlags_UVisCCW) != 0)
		// FragColor.xyz += vec3(0.0f,0.0f,0.5f);
	if((Flags & VertexFlags_TangentHandedness) != 0)
		FragColor.xyz += vec3(0.0f,0.5f,0.0f);
	// if((Flags & VertexFlags_UVWindIsDifferent) != 0)
		// FragColor.xyz += vec3(0.5f,0.0f,0.0f);
		
	// if((Flags) > 0)
		// FragColor.xyz += vec3(0.0f,0.5f,0.0f);
	// else
		// FragColor.xyz = vec3(shade);
	
	
	// if(dFdx(UV.x) < 0.0f)
		// FragColor.xyz = vec3(1.0f-fract(UV.x),fract(UV.y),1.0f);
	
	// FragColor = texture(txDiffuse, UV.xy);
		
	// FragColor = vec4(1.0f,0.0f,1.0f,1.0f);
}
