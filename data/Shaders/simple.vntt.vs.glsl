#version 430

#global_defines

//vertex, normal, tangent, texCoord
layout(location = 0) in vec3 vertex;
layout(location = 1) in vec3 normal;
layout(location = 2) in vec3 tangent;
layout(location = 3) in uint flags;
layout(location = 4) in vec2 texCoord;

layout(std140, row_major, binding = 0) uniform TransformMatrices{
	mat4 world;
	mat4 view;
	mat4 projection;
	mat4 normal;
} transform;

out vec3 WorldPosition;
out vec3 Normal;
out vec3 Tangent;
out vec3 Bitangent;
out vec2 UV;
flat out uint Flags;

void main(){
	vec4 wPosition = vec4(vertex,1.0) * transform.world;
	gl_Position = (wPosition*transform.view)*transform.projection;
	WorldPosition = wPosition.xyz;
	Normal = normalize(normal*mat3(transform.normal));
	Tangent = normalize(tangent);
	
	Flags = flags;
	
	// if(Flags != 0){
		// Tangent = -normalize(tangent);
		// Bitangent = normalize(cross(Tangent, Normal));
	// }
	// else{
		// Tangent = normalize(tangent);
		// Bitangent = normalize(cross(Tangent, Normal));
	// }
	
	if((Flags & VertexFlags_TangentHandedness) != 0)
		Bitangent = -normalize(cross(Normal, Tangent));
	else
		Bitangent = normalize(cross(Normal, Tangent));
	
	UV = texCoord;
}
