#ifdef RD_API_OPENGL4

#include "device.h"
#include "vertex_buffer.h"

bool CVertexBuffer::Create(SRawData data){
	if(this->device == nullptr) return false;

	device->gl.BufferData(GL_ARRAY_BUFFER, data.size, data.data, GL_STATIC_DRAW);

}

#endif//RD_API_OPENGL4
