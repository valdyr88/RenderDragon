#ifdef RD_API_WEBGL

#include "device.h"
#include "buffer.h"

GLenum usageGLenum(const EShaderResourceUsageType& t){
	switch(t)
	{
		case EShaderResourceUsageType::Static: return GL_STATIC_DRAW;
		case EShaderResourceUsageType::Stream: return GL_STREAM_DRAW;
		case EShaderResourceUsageType::Dynamic: return GL_DYNAMIC_DRAW;
		default: return GL_STATIC_DRAW;
	}
}

bool CBuffer::Create(SRawData data){
	if(this->device == nullptr) return false;
	Release();

	auto& gl = device->gl;

	if(data.data == nullptr)
		data.size = descriptor.size;
	else if(data.size == 0)
		LOG_ERR("data != nullptr but size == 0");

	gl.GenBuffers(1, &id);

	gl.BindBuffer(glenum(descriptor.type), id);
	gl.BufferData(glenum(descriptor.type), data.size, data.data, glenum(descriptor.usage, descriptor.access));

	return true;
}

bool CBuffer::Upload(byte* pData, uint32 size, uint32 offset){
	if(this->device == nullptr) return false;

	auto& gl = device->gl;

	gl.BindBuffer(glenum(descriptor.type), id);
	gl.BufferSubData(glenum(descriptor.type), offset, size, pData);

	return true;
}

bool CBuffer::Bind(){
	if(this->device == nullptr) return false;
	auto& gl = device->gl;
	gl.BindBuffer(glenum(descriptor.type), id);
	return true;
}

void CBuffer::Release(){
	if(this->device == nullptr) return;
	if(id == 0) return;

	device->gl.DeleteBuffers(1, &id); id = 0;
}

#endif//RD_API_OPENGL4
