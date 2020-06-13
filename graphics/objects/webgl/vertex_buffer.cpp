#ifdef RD_API_WEBGL

#include "../../utils/types/types.h"
#include "device.h"
#include "vertex_buffer.h"

//----------------------------------------------------------------------------------------------
//	CVertexBuffer
//----------------------------------------------------------------------------------------------

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
		auto abuffer = UniquePtr<CBuffer>(__rd_new CBuffer(this->device, desc, data));
		abuffer->Bind();

		uint32 stride = 0;

		for(sizetype i = 0; i < this->format.attributes.size(); ++i){
			auto& att = this->format.attributes[i];
			if(att.stride == 0){
				att.stride = stride;
				stride += componentCount(att.count)* sizeInBytes(att.type);
			}

			gl.EnableVertexAttribArray(att.binding);
			if(isFloatType(att.type))
				gl.VertexAttribPointer(att.binding, componentCount(att.count), glenum(att.type), att.bIsNormalized, att.stride, 0);
			else if(isIntType(att.type))
				gl.VertexAttribIPointer(att.binding, componentCount(att.count), glenum(att.type), att.stride, 0);
		}

		this->format.stride = stride;

		this->buffers.emplace_back(std::move(abuffer));
	}
	else if(this->format.layout == EAttributeLayout::Contiguous) //multiple buffers
	{
		ASSERT(this->format.attributes.size() == datas.size() || datas.size() == 0);

		for(uint i = 0; i < this->format.attributes.size(); ++i){
			auto& att = this->format.attributes[i];
			sizetype size = (sizetype)sizeInBytes(att.type)*componentCount(att.count)*count;
			if(att.stride == 0){
				att.stride = componentCount(att.count)*sizeInBytes(att.type); }

			SRawData data;
			if(i < datas.size()) data = datas[i];

			ASSERT(size == data.size || data.size == 0);
			
			SBufferDesc desc;
			{
				desc.type = EBufferType::Vertex;
				desc.usage = EShaderResourceUsageType::Static;
				desc.size = size;
			}
			auto abuffer = UniquePtr<CBuffer>(__rd_new CBuffer(this->device, desc, data));
			abuffer->Bind();

			gl.EnableVertexAttribArray(att.binding);
			if(isFloatType(att.type))
				gl.VertexAttribPointer(att.binding, componentCount(att.count), glenum(att.type), att.bIsNormalized, att.stride, 0);
			else if(isIntType(att.type))
				gl.VertexAttribIPointer(att.binding, componentCount(att.count), glenum(att.type), att.stride, 0);

			this->buffers.emplace_back(std::move(abuffer));
		}
	}

	gl.BindVertexArray(0);
	return true;
}

bool CVertexBuffer::Bind(){
	if(this->device == nullptr) return false;
	auto& gl = this->device->gl;
	
	gl.BindVertexArray(this->id);

	if(firstBind){
		if(this->format.layout == EAttributeLayout::Interleaved) //one buffer
		{
			auto& buffer = this->buffers[0];
			gl.BindBuffer(GL_ARRAY_BUFFER, buffer->getId());

			for(uint i = 0; i < this->format.attributes.size(); ++i){
				auto& att = this->format.attributes[i];
				if(gl.IsVerexAttribArrayEnabled(att.binding) == false)
					gl.EnableVertexAttribArray(att.binding);
			}
		}
		else if(this->format.layout == EAttributeLayout::Contiguous) //multiple buffers
		{
			ASSERT(this->format.attributes.size() == buffers.size());

			for(uint i = 0; i < this->format.attributes.size(); ++i){
				auto& att = this->format.attributes[i];
				auto& buffer = this->buffers[i];

				gl.BindBuffer(GL_ARRAY_BUFFER, buffer->getId());

				if(gl.IsVerexAttribArrayEnabled(att.binding) == false)
					gl.EnableVertexAttribArray(att.binding);
			}
		}
	}

	firstBind = false;
	return true;
}

void CVertexBuffer::Release(){
	if(this->device == nullptr) return;
	if(id != 0)
		device->gl.DeleteVertexArrays(1, &id); id = 0;
	for(auto it = buffers.begin(); it != buffers.end(); ++it){
		auto& buffer = *it;
		if(buffer != nullptr)
			buffer->Release();
		buffer.reset();
	}
}

//----------------------------------------------------------------------------------------------
//	CIndexBuffer
//----------------------------------------------------------------------------------------------

bool CIndexBuffer::Create(SRawData data){
	if(this->device == nullptr) return false;

	auto& gl = this->device->gl;

	SBufferDesc desc;
	{
		desc.type = EBufferType::Index;
		desc.usage = EShaderResourceUsageType::Static;
		desc.size = data.size;
	}
	buffer = UniquePtr<CBuffer>(__rd_new CBuffer(this->device, desc, data));
	return true;
}

void CIndexBuffer::Release(){
	if(this->device == nullptr) return;
	if(buffer == nullptr) return;
	buffer->Release();
	buffer = nullptr;
}

bool CIndexBuffer::Bind(){
	if(this->buffer == nullptr) return false;
	return this->buffer->Bind();
}

//----------------------------------------------------------------------------------------------

#endif//RD_API_OPENGL4
