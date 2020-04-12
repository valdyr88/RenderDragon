#ifndef BUFFER_H
#define BUFFER_H

#ifdef RD_API_OPENGL4

#include "glinclude.h"
#include "../../utils/pointers.h"
#include "../../descriptors/buffer_desc.h"
#include "../../descriptors/graphic_object.h"

class CBuffer : public CGraphicObject{
protected:
	SBufferDesc descriptor;
	GLuint id = 0;

	bool Create(SRawData data);

public:
	CBuffer(GPUDevice* dev, const SBufferDesc& desc, SRawData data = SRawData()) :
		CGraphicObject(dev), descriptor(desc){
		Create(data);
	}

	GLuint getId(){ return id; }

	const auto& getDescriptor(){ return descriptor; }

	EBufferType getType(){ return descriptor.type; }

	bool Upload(byte* pData, uint32 size, uint32 offset = 0);
	bool Map(uint32 start, uint32 size, byte** pOutMappedData){ return false; };
	void Unmap(){};

	bool Bind();

	void Release();

	virtual ~CBuffer() = default;
};

#endif //RD_API_OPENGL4
#endif //BUFFER_H