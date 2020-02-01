#ifndef VERTEX_BUFFER_H
#define VERTEX_BUFFER_H

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
};

class CIndexBuffer : public CGraphicObject{
protected:
	EValueType type;
	uint32 count;
};

#endif //VERTEX_BUFFER_H