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
	sizetype count;
	GLuint id = 0;
	bool firstBind = true;
	
	std::vector<UniquePtr<CBuffer>> buffers;

	bool Create(std::vector<SRawData> data);

	virtual void Release() override;
public:

	CVertexBuffer(GPUDevice* dev, const SVertexFormat& fmt, sizetype c, std::vector<SRawData> data = std::vector<SRawData>()) :
		CGraphicObject(dev), format(fmt), count(c){
		Create(data);
	}

	const SVertexFormat& getVertexFormat(){ return format; }

	GLuint getId(){ return id; }

	bool Bind();

	virtual ~CVertexBuffer() override{ Release(); }

	static std::vector<byte> InterleaveVertexData(const SVertexFormat& fmt, const std::vector<byte*>& data, sizetype vertexCount);
};

class CIndexBuffer : public CGraphicObject{
protected:
	EValueType type;
	uint32 count;

	UniquePtr<CBuffer> buffer;

	bool Create(SRawData data);

	virtual void Release() override;
public:

	CIndexBuffer(GPUDevice* dev, EValueType t, uint32 c, SRawData data = SRawData()) :
		CGraphicObject(dev), type(t), count(c){
		Create(data);
	}

	GLuint getId(){ return (buffer != nullptr)? buffer->getId() : 0; }

	uint32 getCount(){ return count; }
	EValueType getType(){ return type; }

	bool Bind();

	virtual ~CIndexBuffer() override{ Release(); }
};

#endif //RD_API_OPENGL4
#endif //VERTEX_BUFFER_H