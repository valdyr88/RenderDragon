#ifdef RD_API_OPENGL4

#include "device.h"
#include "buffer.h"

GLenum usageGLenum(const EShaderResourceUsageType& t){
	switch(t)
	{
		case EShaderResourceUsageType::Static: return GL_STATIC_DRAW;
		case EShaderResourceUsageType::Mutable: return GL_DYNAMIC_DRAW;
		case EShaderResourceUsageType::Dynamic: return GL_DYNAMIC_DRAW;
		default: return GL_STATIC_DRAW;
	}
}

bool CBuffer::Create(SRawData data){
	if(this->device == nullptr) return false;
	Release();

	auto& gl = device->gl;

	gl.GenBuffers(1, &id);

	gl.BindBuffer(glenum(descriptor.type), id);
	gl.BufferData(glenum(descriptor.type), data.size, data.data, usageGLenum(descriptor.usage));
}

void CBuffer::Release(){
	if(this->device == nullptr) return;
	if(id == 0) return;

	device->gl.DeleteBuffers(1, &id); id = 0;
}

#endif//RD_API_OPENGL4
