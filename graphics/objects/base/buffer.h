#ifndef BUFFER_H
#define BUFFER_H

#include "../../utils/pointers.h"
#include "../../descriptors/buffer_desc.h"
#include "../../descriptors/graphic_object.h"

class CBuffer : CGraphicObject{
protected:
	SBufferDesc descriptor;
public:
	CBuffer(WeakPtr<GPUDevice>& dev, SBufferDesc& desc) : 
		CGraphicObject(dev), descriptor(desc){}

	EBufferType getType(){ return descriptor.type; }

	void Upload(byte* pData, uint32 size, uint32 offset){};
	bool Map(uint32 start, uint32 size, byte** pOutMappedData){ return false; };
	void Unmap(){};

	virtual ~CBuffer() = default;
};

#endif //BUFFER_H