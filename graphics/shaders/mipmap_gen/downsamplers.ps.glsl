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


layout(location = 0) in vec2 UV;

layout(location = 0) out type FragColor;

/*

type sample3x3(sampler2D tex, vec2 uv, vec2 duv, float level){
	type sum = type(0.0f);
	for(float x = -1.0f; x <= 1.0f; x += 1.0f)
		for(float y = -1.0f; y <= 1.0f; y += 1.0f)
			sum += textureLod(tex, uv + duv*vec2(x,y), float(level)-1.0f).components;
	return sum/9.0f;
}
type sample3x3kernel(sampler2D tex, vec2 uv, vec2 duv, float level){
	type sum = type(0.0f);
	vec3 kernel[3][3] = {{ vec2(-0.707f, 0.707f), vec2(0.0f,1.0f), vec2(0.707f, 0.707f)},
						 { vec2(-1.0f, 0.0f), vec2(0.0f, 0.0f), vec2(1.0f, 0.0f) },
						 { vec2(-0.707f, -0.707f), vec2(0.0f,-1.0f), vec2(0.707f, -0.707f)}};
	for(int x = 0; x < 3; x++)
		for(int y = 0; y < 3; y++)
			sum += textureLod(tex, uv + duv*kernel[x][y], float(level)-1.0f).components;
	return sum/9.0f;
}

*/

void main(){
	type color = textureLod(tx, UV, float(data.level)-1.0f).components;
	type surround = type(0.0f);
	for(float x = -1.0f; x <= 1.0f; x += 1.0f)
		for(float y = -1.0f; y <= 1.0f; y += 1.0f)
			surround += textureLod(tx, UV + data.invResolution*vec2(x,y), float(data.level)-1.0f);
	surround /= 9.0f;
	FragColor = surround;
	// FragColor = vec4(0.0f,0.7f,0.5f,1.0f);
}
