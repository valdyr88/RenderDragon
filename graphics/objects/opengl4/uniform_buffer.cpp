#include "uniform_buffer.h"
#include "device.h"

#ifdef RD_API_OPENGL4

bool IUniformBuffer::CreateBuffer(uint32 size){
	if(this->device == nullptr) return false;

	auto& gl = this->device->gl;

	SBufferDesc desc;
	{
		desc.type = EBufferType::Uniform;
		desc.size = size;
	};
	this->buffer = device->CreateBuffer(desc);
	//this->buffer = rdDeviceCreateBuffer(device, desc);
	return (this->buffer != nullptr);	
}

bool IUniformBuffer::Upload(byte* pData, uint32 size, uint32 offset){
	if(buffer == nullptr) return false;
	return buffer->Upload(pData, size, offset);
}

bool IUniformBuffer::Bind(uint s, uint b){
	if(this->device == nullptr) return false;
	if(buffer == nullptr) return false;
	auto& gl = this->device->gl;
	set = s; binding = b;
	
	gl.BindBufferBase(GL_UNIFORM_BUFFER, binding, buffer->getId());

	return true;
}

#endif //RD_API_OPENGL4