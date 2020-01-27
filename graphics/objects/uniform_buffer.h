#ifndef UNIFORM_BUFFER_H
#define UNIFORM_BUFFER_H

#include <string>
#include <map>
#include <vector>
#include "../../utils/pointers.h"
#include "../../utils/types/types.h"
#include "../../utils/types/vectypes.h"
#include "../enum/graphics_enums.h"
#include "shader_desc.h"

class Device;

struct UniformMap
{
	ValueType type;
	ValueSize size;
	uint32 count;
	std::string name;
	uint32 offset;

	UniformMap(ValueType t, ValueSize s, uint32 c, const char* n) :
		type(t), size(s), count(c), name(n) { offset = 0; }

	UniformMap(){ type = ValueType::float32; size = ValueSize::scalar; count = 1; name = ""; offset = 0; };
};

template <typename Type> class UniformBuffer : public IUniformBuffer{
protected:
	Type data;
	std::map<std::string, UniformMap> mapping;
	std::string name;

	template <typename UniformType> bool setUniform(const char* name, ValueType type, ValueSize size, uint32 count, UniformType& value);

	void CreateMapping(const std::vector<UniformMap> maps);
public:

	UniformBuffer(WeakPtr<Device>& dev, const ShaderResourceDesc& sr, const char* bufferName, const std::vector<UniformMap> maps) :
		IUniformBuffer(dev, sr), name(bufferName){
		this->CreateMapping(maps);
	}
	UniformBuffer() = delete;

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

	virtual void upload() override;
	virtual bool isShared() override;

	Type* operator->(){ return &data; }
};

template<typename Type> template<typename UniformType>
bool UniformBuffer<Type>::setUniform(const char* name, ValueType type, ValueSize size, uint32 count, UniformType& value){
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

template<typename Type> bool UniformBuffer<Type>::setUniform(const char* name, float value){
	return setUniform<float>(name, ValueType::float32, ValueSize::scalar, 1, value);
}
template<typename Type> bool UniformBuffer<Type>::setUniform(const char* name, vec2 value){
	return setUniform<vec2>(name, ValueType::float32, ValueSize::vec2, 1, value);
}
template<typename Type> bool UniformBuffer<Type>::setUniform(const char* name, vec3 value){
	return setUniform<vec3>(name, ValueType::float32, ValueSize::vec3, 1, value);
}
template<typename Type> bool UniformBuffer<Type>::setUniform(const char* name, vec4 value){
	return setUniform<vec4>(name, ValueType::float32, ValueSize::vec4, 1, value);
}
template<typename Type> bool UniformBuffer<Type>::setUniform(const char* name, mat2 value){
	return setUniform<mat2x2>(name, ValueType::float32, ValueSize::mat2x2, 1, value);
}
template<typename Type> bool UniformBuffer<Type>::setUniform(const char* name, mat3 value){
	return setUniform<mat3x3>(name, ValueType::float32, ValueSize::mat3x3, 1, value);
}
template<typename Type> bool UniformBuffer<Type>::setUniform(const char* name, mat4 value){
	return setUniform<mat4x4>(name, ValueType::float32, ValueSize::mat4x4, 1, value);
}
template<typename Type> bool UniformBuffer<Type>::setUniform(const char* name, mat2x3 value){
	return setUniform<mat2x3>(name, ValueType::float32, ValueSize::mat2x3, 1, value);
}
template<typename Type> bool UniformBuffer<Type>::setUniform(const char* name, mat3x2 value){
	return setUniform<mat3x2>(name, ValueType::float32, ValueSize::mat3x2, 1, value);
}
template<typename Type> bool UniformBuffer<Type>::setUniform(const char* name, mat4x3 value){
	return setUniform<mat4x3>(name, ValueType::float32, ValueSize::mat4x3, 1, value);
}
template<typename Type> bool UniformBuffer<Type>::setUniform(const char* name, mat3x4 value){
	return setUniform<mat3x4>(name, ValueType::float32, ValueSize::mat3x4, 1, value);
}
template<typename Type> bool UniformBuffer<Type>::setUniform(const char* name, mat4x2 value){
	return setUniform<mat4x2>(name, ValueType::float32, ValueSize::mat4x2, 1, value);
}
template<typename Type> bool UniformBuffer<Type>::setUniform(const char* name, mat2x4 value){
	return setUniform<mat2x4>(name, ValueType::float32, ValueSize::mat2x4, 1, value);
}
template<typename Type> bool UniformBuffer<Type>::setUniform(const char* name, int value){
	return setUniform<int>(name, ValueType::int32, ValueSize::scalar, 1, value);
}
template<typename Type> bool UniformBuffer<Type>::setUniform(const char* name, ivec2 value){
	return setUniform<ivec2>(name, ValueType::int32, ValueSize::vec2, 1, value);
}
template<typename Type> bool UniformBuffer<Type>::setUniform(const char* name, ivec3 value){
	return setUniform<ivec3>(name, ValueType::int32, ValueSize::vec3, 1, value);
}
template<typename Type> bool UniformBuffer<Type>::setUniform(const char* name, ivec4 value){
	return setUniform<ivec4>(name, ValueType::int32, ValueSize::vec4, 1, value);
}

template<typename Type> bool UniformBuffer<Type>::isShared(){ return false; }
template<typename Type> void UniformBuffer<Type>::upload(){}

template<typename Type> void UniformBuffer<Type>::CreateMapping(const std::vector<UniformMap> maps){
	
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

#endif //UNIFORM_BUFFER_H