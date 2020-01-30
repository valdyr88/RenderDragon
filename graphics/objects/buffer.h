#ifndef BUFFER_H
#define BUFFER_H

#include "../../utils/pointers.h"
#include "../../utils/types/types.h"
#include "../enum/graphics_enums.h"
#include "graphic_object.h"

struct SBufferDesc{
	EBufferType type;
	uint32 size;

	bool operator == (const SBufferDesc& other){
		return type == other.type &&
			size == other.size;
	}
	bool operator != (const SBufferDesc& other){ return !(*this == other); }

	SBufferDesc operator =(const SBufferDesc& other){
		type = other.type;
		size = other.size;
		return *this;
	}
};

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