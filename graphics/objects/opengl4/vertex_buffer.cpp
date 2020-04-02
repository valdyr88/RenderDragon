#ifdef RD_API_OPENGL4

#include "../../utils/types/types.h"
#include "device.h"
#include "vertex_buffer.h"

bool CVertexBuffer::Create(std::vector<SRawData> datas){
	if(this->device == nullptr) return false;

	auto& gl = this->device->gl;
	gl.GenVertexArrays(1, &this->id);
	gl.BindVertexArray(this->id);

	if(this->format.layout == EAttributeLayout::Interleaved) //one buffer
	{
		SRawData data;
		ASSERT(datas.size() == 1 || datas.size() == 0);
		if(datas.size() > 0) data = datas[0];

		SBufferDesc desc;
		{
			desc.type = EBufferType::Vertex;
			desc.usage = EShaderResourceUsageType::Static;
			desc.size = data.size;
		}
		auto abuffer = NewUnique<CBuffer>(CBuffer(this->device, desc, data));
		this->buffers.emplace_back(std::move(abuffer));

		for(uint i = 0; i < this->format.attributes.size(); ++i){
			auto& att = this->format.attributes[i];

			gl.VertexAttribPointer(att.binding, componentCount(att.count), glenum(att.type), att.isNormalized, att.stride, 0);
			gl.EnableVertexAttribArray(att.binding);
		}
	}
	else if(this->format.layout == EAttributeLayout::Contiguous) //multiple buffers
	{
		ASSERT(this->format.attributes.size() == datas.size() || datas.size() == 0);

		for(uint i = 0; i < this->format.attributes.size(); ++i){
			auto& att = this->format.attributes[i];
			uint size = sizeInBytes(att.type)*componentCount(att.count)*count;

			SRawData data;
			if(i < datas.size()) data = datas[i];

			ASSERT(size == data.size || data.size == 0);
			
			SBufferDesc desc;
			{
				desc.type = EBufferType::Vertex;
				desc.usage = EShaderResourceUsageType::Static;
				desc.size = size;
			}
			auto abuffer = NewUnique<CBuffer>(CBuffer(this->device, desc, data));
			this->buffers.emplace_back(std::move(abuffer));

			gl.VertexAttribPointer(att.binding, componentCount(att.count), glenum(att.type), att.isNormalized, att.stride, 0);
			gl.EnableVertexAttribArray(att.binding);
		}
	}
}

void CVertexBuffer::Release(){
	if(this->device == nullptr) return;
	if(id == 0) return;

	device->gl.DeleteVertexArrays(1, &id); id = 0;
}

#endif//RD_API_OPENGL4
