#ifndef UNIFORM_BUFFER_H
#define UNIFORM_BUFFER_H

#ifdef RD_API_OPENGL4

#include <string>
#include <map>
#include <vector>
#include "../../utils/pointers.h"
#include "../../descriptors/uniform_buffer_desc.h"
#include "../../descriptors/shader_desc.h"
#include "buffer.h"

template <typename Type> class CUniformBuffer : public IUniformBuffer{
protected:
	Type data;
	std::map<std::string, SUniformMap> mapping;
	std::string name;
	UniquePtr<CBuffer> buffer;

	template <typename UniformType> bool setUniform(const char* name, EValueType type, EValueSize size, uint32 count, UniformType& value);

	void CreateMapping(const std::vector<SUniformMap> maps);
	void CreateBuffer(uint32 size);
public:

	CUniformBuffer(GPUDevice* dev, const SShaderResourceDesc& sr, const char* bufferName, const std::vector<SUniformMap> maps) :
		IUniformBuffer(dev, sr), name(bufferName){
		this->CreateMapping(maps);
		this->CreateBuffer(sizeof(Type));
	}
	CUniformBuffer() = delete;

	virtual bool setUniform(const char* name, float value) override;
	virtual bool setUniform(const char* name, vec2 value) override;
	virtual bool setUniform(const char* name, vec3 value) override;
	virtual bool setUniform(const char* name, vec4 value) override;

	virtual bool setUniform(const char* name, mat2 value) override;
	virtual bool setUniform(const char* name, mat3 value) override;
	virtual bool setUniform(const char* name, mat4 value) override;

	virtual bool setUniform(const char* name, mat2x3 value) override;
	virtual bool setUniform(const char* name, mat3x2 value) override;
	virtual bool setUniform(const char* name, mat4x3 value) override;
	virtual bool setUniform(const char* name, mat3x4 value) override;
	virtual bool setUniform(const char* name, mat4x2 value) override;
	virtual bool setUniform(const char* name, mat2x4 value) override;

	virtual bool setUniform(const char* name, int value) override;
	virtual bool setUniform(const char* name, ivec2 value) override;
	virtual bool setUniform(const char* name, ivec3 value) override;
	virtual bool setUniform(const char* name, ivec4 value) override;

	/*virtual bool setUniform(const char* name, uint count, float* value) override;
	virtual bool setUniform(const char* name, uint count, vec2* value) override;
	virtual bool setUniform(const char* name, uint count, vec3* value) override;
	virtual bool setUniform(const char* name, uint count, vec4* value) override;

	virtual bool setUniform(const char* name, uint count, mat2* value) override;
	virtual bool setUniform(const char* name, uint count, mat3* value) override;
	virtual bool setUniform(const char* name, uint count, mat4* value) override;

	virtual bool setUniform(const char* name, uint count, mat2x3* value) override;
	virtual bool setUniform(const char* name, uint count, mat3x2* value) override;
	virtual bool setUniform(const char* name, uint count, mat4x3* value) override;
	virtual bool setUniform(const char* name, uint count, mat3x4* value) override;
	virtual bool setUniform(const char* name, uint count, mat4x2* value) override;
	virtual bool setUniform(const char* name, uint count, mat2x4* value) override;

	virtual bool setUniform(const char* name, uint count, int* value) override;
	virtual bool setUniform(const char* name, uint count, ivec2* value) override;
	virtual bool setUniform(const char* name, uint count, ivec3* value) override;
	virtual bool setUniform(const char* name, uint count, ivec4* value) override;*/

	virtual void Upload() override;
	virtual bool isShared() override;

	Type* operator->(){ return &data; }

	static SharedPtr<CUniformBuffer<Type>> CreateUniformBuffer(GPUDevice* dev, const SShaderResourceDesc& sr, const char* bufferName, const std::vector<SUniformMap>& maps);

	virtual ~CUniformBuffer() = default;
};

template<typename Type> template<typename UniformType>
bool CUniformBuffer<Type>::setUniform(const char* name, EValueType type, EValueSize size, uint32 count, UniformType& value){
	auto& map = this->mapping[name];
	if(map.count != count) return false;
	if(map.size != size) return false;
	if(map.type != type) return false;

	if(count == 1){
		UniformType& loc = *( UniformType*) ((( byte*) &this->data) + map.offset);
		loc = value;
	}
	else{
		for(uint i = 0; i < count; ++i){
			UniformType& loc = *((( UniformType*) ((( byte*) &this->data) + map.offset)) + i);
			loc = *((&value)+i);
		}
	}

	return true;
}

template<typename Type> bool CUniformBuffer<Type>::setUniform(const char* name, float value){
	return setUniform<float>(name, EValueType::float32, EValueSize::scalar, 1, value);
}
template<typename Type> bool CUniformBuffer<Type>::setUniform(const char* name, vec2 value){
	return setUniform<vec2>(name, EValueType::float32, EValueSize::vec2, 1, value);
}
template<typename Type> bool CUniformBuffer<Type>::setUniform(const char* name, vec3 value){
	return setUniform<vec3>(name, EValueType::float32, EValueSize::vec3, 1, value);
}
template<typename Type> bool CUniformBuffer<Type>::setUniform(const char* name, vec4 value){
	return setUniform<vec4>(name, EValueType::float32, EValueSize::vec4, 1, value);
}
template<typename Type> bool CUniformBuffer<Type>::setUniform(const char* name, mat2 value){
	return setUniform<mat2x2>(name, EValueType::float32, EValueSize::mat2x2, 1, value);
}
template<typename Type> bool CUniformBuffer<Type>::setUniform(const char* name, mat3 value){
	return setUniform<mat3x3>(name, EValueType::float32, EValueSize::mat3x3, 1, value);
}
template<typename Type> bool CUniformBuffer<Type>::setUniform(const char* name, mat4 value){
	return setUniform<mat4x4>(name, EValueType::float32, EValueSize::mat4x4, 1, value);
}
template<typename Type> bool CUniformBuffer<Type>::setUniform(const char* name, mat2x3 value){
	return setUniform<mat2x3>(name, EValueType::float32, EValueSize::mat2x3, 1, value);
}
template<typename Type> bool CUniformBuffer<Type>::setUniform(const char* name, mat3x2 value){
	return setUniform<mat3x2>(name, EValueType::float32, EValueSize::mat3x2, 1, value);
}
template<typename Type> bool CUniformBuffer<Type>::setUniform(const char* name, mat4x3 value){
	return setUniform<mat4x3>(name, EValueType::float32, EValueSize::mat4x3, 1, value);
}
template<typename Type> bool CUniformBuffer<Type>::setUniform(const char* name, mat3x4 value){
	return setUniform<mat3x4>(name, EValueType::float32, EValueSize::mat3x4, 1, value);
}
template<typename Type> bool CUniformBuffer<Type>::setUniform(const char* name, mat4x2 value){
	return setUniform<mat4x2>(name, EValueType::float32, EValueSize::mat4x2, 1, value);
}
template<typename Type> bool CUniformBuffer<Type>::setUniform(const char* name, mat2x4 value){
	return setUniform<mat2x4>(name, EValueType::float32, EValueSize::mat2x4, 1, value);
}
template<typename Type> bool CUniformBuffer<Type>::setUniform(const char* name, int value){
	return setUniform<int>(name, EValueType::int32, EValueSize::scalar, 1, value);
}
template<typename Type> bool CUniformBuffer<Type>::setUniform(const char* name, ivec2 value){
	return setUniform<ivec2>(name, EValueType::int32, EValueSize::vec2, 1, value);
}
template<typename Type> bool CUniformBuffer<Type>::setUniform(const char* name, ivec3 value){
	return setUniform<ivec3>(name, EValueType::int32, EValueSize::vec3, 1, value);
}
template<typename Type> bool CUniformBuffer<Type>::setUniform(const char* name, ivec4 value){
	return setUniform<ivec4>(name, EValueType::int32, EValueSize::vec4, 1, value);
}

template<typename Type> bool CUniformBuffer<Type>::isShared(){ return false; }
template<typename Type> void CUniformBuffer<Type>::Upload(){}

template<typename Type> void CUniformBuffer<Type>::CreateMapping(const std::vector<SUniformMap> maps){
	
	uint offset = 0;
	for(auto it = maps.begin(); it != maps.end(); ++it){
		auto map = *it;
		map.offset = offset;

		mapping.emplace(it->name, map);

		int bytesize = sizeInBytes(it->type);
		bytesize = (bytesize < 4) ? 4 : bytesize;

		offset += it->count * count(it->size) * bytesize;
	}
}

template<typename Type> void CUniformBuffer<Type>::CreateBuffer(uint32 size){
	if(device != nullptr){
		SBufferDesc desc;
		{
			desc.type = EBufferType::Uniform;
			desc.size = size;
		};
		//this->buffer = device->CreateBuffer(desc);
	}
}

template<typename Type> SharedPtr<CUniformBuffer<Type>> CUniformBuffer<Type>::CreateUniformBuffer(GPUDevice* dev, const SShaderResourceDesc& sr, const char* bufferName, const std::vector<SUniformMap>& maps){
	return SharedPtr<CUniformBuffer<Type>>(new CUniformBuffer<Type>(dev, sr, bufferName, maps));
}


#endif //RD_API_OPENGL4
#endif //UNIFORM_BUFFER_H