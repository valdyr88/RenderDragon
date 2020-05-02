#ifndef VERTEX_BUFFER_H
#define VERTEX_BUFFER_H

#ifdef RD_API_OPENGL4

#include "glinclude.h"
#include "../../utils/pointers.h"
#include "../../descriptors/vertex_buffer_desc.h"
#include "../../descriptors/shader_desc.h"
#include "buffer.h"

class CVertexBuffer : public CGraphicObject{
protected:
	SVertexFormat format;
	uint32 count;
	GLuint id = 0;

	std::vector<UniquePtr<CBuffer>> buffers;

	bool Create(std::vector<SRawData> data);
public:

	CVertexBuffer(GPUDevice* dev, const SVertexFormat& fmt, uint32 c, std::vector<SRawData> data = std::vector<SRawData>()) :
		CGraphicObject(dev), format(fmt), count(c){
		Create(data);
	}

	const SVertexFormat& getVertexFormat(){ return format; }

	GLuint getId(){ return id; }
	void Release();
	virtual ~CVertexBuffer() override{ Release(); }

	bool Bind();
};

class CIndexBuffer : public CGraphicObject{
protected:
	EValueType type;
	uint32 count;

	UniquePtr<CBuffer> buffer;

	bool Create(SRawData data);
public:

	CIndexBuffer(GPUDevice* dev, EValueType t, uint32 c, SRawData data = SRawData()) :
		CGraphicObject(dev), type(t), count(c){
		Create(data);
	}

	GLuint getId(){ return (buffer != nullptr)? buffer->getId() : 0; }
	void Release();
	virtual ~CIndexBuffer() override{ Release(); }

	uint32 getCount(){ return count; }
	EValueType getType(){ return type; }

	bool Bind();
};

#endif //RD_API_OPENGL4
#endif //VERTEX_BUFFER_H