#ifndef VERTEX_BUFFER_H
#define VERTEX_BUFFER_H

#ifdef RD_API_OPENGL4

#include "../../utils/pointers.h"
#include "../../descriptors/vertex_buffer_desc.h"
#include "../../descriptors/shader_desc.h"
#include "buffer.h"

class CVertexBuffer : public CGraphicObject{
protected:
	SVertexFormat format;
	UniquePtr<CBuffer> buffer;
	uint32 count;
public:

	CVertexBuffer(GPUDevice* dev, const SVertexFormat& fmt, uint32 s) :
		CGraphicObject(dev), format(fmt), count(s){}

	virtual ~CVertexBuffer() = default;
};

class CIndexBuffer : public CGraphicObject{
protected:
	EValueType type;
	uint32 count;
public:

	CIndexBuffer(GPUDevice* dev, EValueType t, uint32 s) :
		CGraphicObject(dev), type(t), count(s){}

	virtual ~CIndexBuffer() = default;
};

#endif //RD_API_OPENGL4
#endif //VERTEX_BUFFER_H