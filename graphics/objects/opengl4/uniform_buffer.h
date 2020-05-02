#ifndef UNIFORM_BUFFER_H
#define UNIFORM_BUFFER_H

#ifdef RD_API_OPENGL4

#include <string>
#include <map>
#include <vector>
#include <functional>
#include "../../utils/pointers.h"
#include "../../descriptors/uniform_buffer_desc.h"
#include "../../descriptors/shader_desc.h"
#include "buffer.h"

SharedPtr<CBuffer> rdDeviceCreateBuffer(GPUDevice* device, const SBufferDesc& desc);
	
//-----------------------------------------------------------------------------------------------------------------------------------

class IUniformBuffer : public CShaderResource{
protected:
	std::string name;
	std::map<std::string, SUniformMap> mapping;
	SharedPtr<CBuffer> buffer;

	uint set = 0;
	uint binding = 0;
	
	void CreateMapping(const std::vector<SUniformMap> maps);
	bool CreateBuffer(uint32 size);
	bool Upload(byte* pData, uint32 size, uint32 offset = 0);
	bool Bind(uint set, uint binding);

	IUniformBuffer() = delete;
public:
	IUniformBuffer(GPUDevice* dev, const char* bufferName, const std::vector<SUniformMap> maps)
		: CShaderResource(dev, EShaderResourceType::UniformBuffer), name(bufferName)
	{
		CreateMapping(maps);
	}

	virtual bool setUniform(const char* name, float value){ LOG_ERR("not implemented!"); return false; }
	virtual bool setUniform(const char* name, vec2 value){ LOG_ERR("not implemented!"); return false; }
	virtual bool setUniform(const char* name, vec3 value){ LOG_ERR("not implemented!"); return false; }
	virtual bool setUniform(const char* name, vec4 value){ LOG_ERR("not implemented!"); return false; }

	virtual bool setUniform(const char* name, mat2 value){ LOG_ERR("not implemented!"); return false; }
	virtual bool setUniform(const char* name, mat3 value){ LOG_ERR("not implemented!"); return false; }
	virtual bool setUniform(const char* name, mat4 value){ LOG_ERR("not implemented!"); return false; }

	virtual bool setUniform(const char* name, mat2x3 value){ LOG_ERR("not implemented!"); return false; }
	virtual bool setUniform(const char* name, mat3x2 value){ LOG_ERR("not implemented!"); return false; }
	virtual bool setUniform(const char* name, mat4x3 value){ LOG_ERR("not implemented!"); return false; }
	virtual bool setUniform(const char* name, mat3x4 value){ LOG_ERR("not implemented!"); return false; }
	virtual bool setUniform(const char* name, mat4x2 value){ LOG_ERR("not implemented!"); return false; }
	virtual bool setUniform(const char* name, mat2x4 value){ LOG_ERR("not implemented!"); return false; }

	virtual bool setUniform(const char* name, int value){ LOG_ERR("not implemented!"); return false; }
	virtual bool setUniform(const char* name, ivec2 value){ LOG_ERR("not implemented!"); return false; }
	virtual bool setUniform(const char* name, ivec3 value){ LOG_ERR("not implemented!"); return false; }
	virtual bool setUniform(const char* name, ivec4 value){ LOG_ERR("not implemented!"); return false; }

	virtual bool setUniform(const char* name, uint count, float* value){ LOG_ERR("not implemented!"); return false; }
	virtual bool setUniform(const char* name, uint count, vec2* value){ LOG_ERR("not implemented!"); return false; }
	virtual bool setUniform(const char* name, uint count, vec3* value){ LOG_ERR("not implemented!"); return false; }
	virtual bool setUniform(const char* name, uint count, vec4* value){ LOG_ERR("not implemented!"); return false; }

	virtual bool setUniform(const char* name, uint count, mat2* value){ LOG_ERR("not implemented!"); return false; }
	virtual bool setUniform(const char* name, uint count, mat3* value){ LOG_ERR("not implemented!"); return false; }
	virtual bool setUniform(const char* name, uint count, mat4* value){ LOG_ERR("not implemented!"); return false; }

	virtual bool setUniform(const char* name, uint count, mat2x3* value){ LOG_ERR("not implemented!"); return false; }
	virtual bool setUniform(const char* name, uint count, mat3x2* value){ LOG_ERR("not implemented!"); return false; }
	virtual bool setUniform(const char* name, uint count, mat4x3* value){ LOG_ERR("not implemented!"); return false; }
	virtual bool setUniform(const char* name, uint count, mat3x4* value){ LOG_ERR("not implemented!"); return false; }
	virtual bool setUniform(const char* name, uint count, mat4x2* value){ LOG_ERR("not implemented!"); return false; }
	virtual bool setUniform(const char* name, uint count, mat2x4* value){ LOG_ERR("not implemented!"); return false; }

	virtual bool setUniform(const char* name, uint count, int* value){ LOG_ERR("not implemented!"); return false; }
	virtual bool setUniform(const char* name, uint count, ivec2* value){ LOG_ERR("not implemented!"); return false; }
	virtual bool setUniform(const char* name, uint count, ivec3* value){ LOG_ERR("not implemented!"); return false; }
	virtual bool setUniform(const char* name, uint count, ivec4* value){ LOG_ERR("not implemented!"); return false; }

	virtual void Upload() = 0;
	virtual bool isShared() = 0;

	virtual const char* getUBStructTypeName(){ return ""; }
	virtual const std::vector<SUniformMap>& getUBStructDesc(){ return {}; }

	//static std::map<std::string, SharedPtr<IUniformBuffer>(*)(GPUDevice*, const char*)> CreateUniformBufferType;
	static std::map<std::string, std::function< SharedPtr<IUniformBuffer>(GPUDevice*, const char*) >> CreateUniformBufferType;
	
	virtual void Release() override;
	virtual ~IUniformBuffer() override{ Release(); }

	friend class CShaderProgram;
};

//-----------------------------------------------------------------------------------------------------------------------------------

template <typename Type> class CUniformBuffer : public IUniformBuffer{
protected:
	Type data;

	static const char* structTypeName;
	template <typename UniformType> bool setUniform(const char* name, EValueType type, EValueSize size, uint32 count, UniformType& value);
public:
	CUniformBuffer(GPUDevice* dev, const char* bufferName, const std::vector<SUniformMap> maps = Type::desc)
		: IUniformBuffer(dev, bufferName, maps)
	{
		CreateBuffer(sizeof(Type));
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

	virtual const char* getUBStructTypeName() override{ return structTypeName; }
	virtual const std::vector<SUniformMap>& getUBStructDesc() override{ return Type::desc; }

	static SharedPtr<CUniformBuffer<Type>> CreateUniformBuffer(GPUDevice* dev, const char* bufferName);
	//static SharedPtr<CUniformBuffer<Type>> CreateUniformBuffer(GPUDevice* dev, const char* bufferName, const std::vector<SUniformMap>& maps);

	virtual ~CUniformBuffer() override{ Release(); }
};

template<typename Type> template<typename UniformType>
bool CUniformBuffer<Type>::setUniform(const char* name, EValueType type, EValueSize size, uint32 count, UniformType& value){
	auto& map = this->mapping[name];
	if(map.count != count) return false;
	if(map.size != size) return false;
	if(map.type != type) return false;

	if(count == 1){
		UniformType& loc = *(UniformType*) (((byte*) &this->data) + map.offset);
		loc = value;
	}
	else{
		for(uint i = 0; i < count; ++i){
			UniformType& loc = *(((UniformType*) (((byte*) &this->data) + map.offset)) + i);
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

template<typename Type> void CUniformBuffer<Type>::Upload(){
	IUniformBuffer::Upload((byte*)&this->data, sizeof(Type));
}
//-----------------------------------------------------------------------------------------------------------------------------------

template<typename Type> SharedPtr<CUniformBuffer<Type>> CUniformBuffer<Type>::CreateUniformBuffer(GPUDevice* dev, const char* bufferName){
	return SharedPtr<CUniformBuffer<Type>>(new CUniformBuffer<Type>(dev, bufferName, Type::desc));
}

//template<typename Type> SharedPtr<CUniformBuffer<Type>> CUniformBuffer<Type>::CreateUniformBuffer(GPUDevice* dev, const char* bufferName, const std::vector<SUniformMap>& maps){
//	return SharedPtr<CUniformBuffer<Type>>(new CUniformBuffer<Type>(dev, bufferName, maps));
//}

size_t internal_rdAppendRegisterUniformBufferStructureFunctionCall(void (*func)());
void internal_rdCallRegisterUniformBufferStructureFunctionCall();

#define RD_MACRO_CONCAT_IMPL(x,y) x##y
#define RD_MACRO_CONCAT(x,y) RD_MACRO_CONCAT_IMPL(x,y)

#define rdRegisterUniformBufferStructure(Type) \
	template class CUniformBuffer<Type>; \
	const char* CUniformBuffer<Type>::structTypeName = #Type; \
namespace RD_MACRO_CONCAT(RD_MACRO_CONCAT(registerUBnamespace,__COUNTER__), __LINE__){ \
	void internal_rdRegisterUniformBufferStructure(){ \
		IUniformBuffer::CreateUniformBufferType[#Type] = &CUniformBuffer<Type>::CreateUniformBuffer; \
	} \
	size_t internalvar_registeredUBStructure = internal_rdAppendRegisterUniformBufferStructureFunctionCall( &internal_rdRegisterUniformBufferStructure ); \
}

#define rdInitUniformBufferStructreRegistry internal_rdCallRegisterUniformBufferStructureFunctionCall
//-----------------------------------------------------------------------------------------------------------------------------------

#endif //RD_API_OPENGL4
#endif //UNIFORM_BUFFER_H