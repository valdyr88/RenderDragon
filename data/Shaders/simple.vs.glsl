#version 330

layout (location = 0) in vec3 Position;

void main(){
	gl_Position = vec4(0.5f * Position.x, 0.5f * Position.y, Position.z, 1.0f);
}
