#ifndef BUFFER_H
#define BUFFER_H

#include "../../utils/pointers.h"
#include "../../descriptors/buffer_desc.h"
#include "../../descriptors/graphic_object.h"

class CBuffer : CGraphicObject{
protected:
	SBufferDesc descriptor;
public:
	CBuffer(WeakPtr<IDevice>& dev, SBufferDesc& desc) : 
		CGraphicObject(dev), descriptor(desc){}

	EBufferType getType(){ return descriptor.type; }

	virtual void Upload(byte* pData, uint32 size, uint32 offset) = 0;
	virtual bool Map(uint32 start, uint32 size, byte** pOutMappedData) = 0;
	virtual void Unmap() = 0;
};

#endif //BUFFER_H