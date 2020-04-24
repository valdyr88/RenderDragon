#version 430

#global_defines

#ifndef type
	#define type vec4
	#define components rgba
#endif

layout(binding = 0) uniform sampler2D tx;

layout(std140, binding = 1) uniform LevelData{
	vec2 resolution;
	vec2 invResolution;
	int level;
} data;


in vec2 UV;

out type FragColor;

void main(){
	type color = textureLod(tx, UV, float(data.level)-1.0f).components;
	FragColor = color;
	// FragColor = vec4(0.0f,0.7f,0.5f,1.0f);
}
