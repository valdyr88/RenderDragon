#version 430

#global_defines

#include "data/Shaders/include/functions.glsl"
#include "data/Shaders/include/defines.glsl"
#include "data/Shaders/include/pbr.glsl"

layout(std140, binding = 4) uniform ubLight{
	SLight light;
} light;

layout(binding = 1) uniform sampler2D txDiffuse;
layout(binding = 2) uniform sampler2D txNormal;
layout(binding = 3) uniform sampler2D txAoRS;

in vec3 WorldPosition;
in vec3 Normal;
in vec3 Tangent;
in vec3 Bitangent;
in vec2 UV;
in vec3 ViewDir;
in vec3 ViewPosition;
flat in uint Flags;

out vec4 FragColor;

float D_my(float r, float dotNL){
	r = (1.0f-r);
	r = pow(r,8.0f)+1.0f;
	return 2.0f*pow(0.5f*r,2.0f)*gauss(pow(r,r),20.0f*dotNL);
}

void main(){
/**/
	// vec3 LightPosition = 1.0f*vec3(-1.0f,2.0f,-1.5f);
	// vec3 LightPosition = 1.0f*vec3(-1.0f,0.35f,-0.05f) + 0.15f*vec3(sin(light.time),0.0f,cos(light.time));
	vec3 LightPosition = light.light.position.xyz;
	highp vec3 toLight = LightPosition - WorldPosition;
	vec3 L = normalize(toLight);
	vec3 N = normalize(Normal);
	vec3 V = normalize(ViewDir);
	
	float distoLight = length(toLight) + 0.0001f;
	
	vec4 color = texture(txDiffuse, UV.xy);
	vec4 AoRS = texture(txAoRS, UV.xy);
	vec4 normal = texture(txNormal, UV.xy);
	normal.xyz = vec3(1.0f,-1.0f,1.0f)*normalize(2.0f*normal.xyz - vec3(1.0f));
	
	mat3 normalMatrix = transpose(mat3(Tangent,Bitangent,Normal));
	N = normalize(normal.xyz*normalMatrix);
	
	float3 H = normalize(halfvec(V,L));
	
	float dotNL = dot(L,N);
	float dotNH = dot(N,H);
	
	float roughness = clamp(1.0f*AoRS.y+0.5f,0.0f,1.0f);
	
	float shade = 0.5f*dotNL+0.5f;
	// shade = 0.1f / pow(distoLight,2.0f);
	shade = 4.0f*D_GGX(roughness, dotNH)*dotNL;

	FragColor = color*vec4(vec3(shade),1.0f);
	// FragColor = AoRS;
	// FragColor.xyz = 0.5f*N+0.5f;
	// FragColor.xyz = 0.5f*normal.xyz+0.5f;
	// FragColor.xyz = 0.5f*Normal+0.5f;
	// FragColor.xyz = 0.5f*Tangent+0.5f;
	// FragColor.xyz = 0.5f*Bitangent+0.5f;
	// FragColor.xyz = vec3(fract(UV.x),fract(UV.y),1.0f);
	FragColor.a = 1.0f;
	
	// FragColor.xyz = vec3(shade);
	
	// if(Flags != 0)
		// FragColor.xyz = vec3(0.0f);
	// if((Flags & VertexFlags_UVisCCW) != 0)
		// FragColor.xyz += vec3(0.0f,0.0f,0.5f);
	// if((Flags & VertexFlags_TangentHandedness) != 0)
		// FragColor.xyz += vec3(0.0f,0.5f,0.0f);
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
