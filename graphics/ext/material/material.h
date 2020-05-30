#ifndef MATERIAL_H
#define MATERIAL_H

#include <string>
#include <vector>
#include "../../utils/pointers.h"
#include "../../utils/containers.h"
#include "../../utils/singleton.h"
#include "../../descriptors/uniform_buffer_desc.h"

//----------------------------------------------------------------------------------------------

struct SMaterialParam : public SUniformMap{
private:
	void* value = nullptr;
	uint valueSizeInBytes = 0;
	bool bIsValueSet = false;

	void Release(){
		if(value != nullptr)
			__release_array(value); 
		valueSizeInBytes = 0; bIsValueSet = false;
	}
public:


	void Reserve()
	{ 
		Release();
		if(value != nullptr) return;
		valueSizeInBytes = sizeInBytes(type) * componentCount(size) * count;
		if(valueSizeInBytes > 0) value = __new byte[valueSizeInBytes]; bIsValueSet = false;
	}

	SMaterialParam(const char* n, EValueType t, EValueSize s, uint32 c = 1) :
		SUniformMap(n, t, s, c)
	{
		Reserve();
	}

	SMaterialParam(const SMaterialParam& other){
		this->operator=(other);
	}

	SMaterialParam() : SUniformMap("", EValueType::float32, EValueSize::scalar, 1)
	{
		valueSizeInBytes = 0;
		value = nullptr; bIsValueSet = false;
	}

	SMaterialParam& operator = (const SMaterialParam& other){
		SUniformMap::operator=(other);
		if(other.value != nullptr)
		{
			this->Reserve();
			for(uint i = 0; i < this->valueSizeInBytes; ++i){
				((byte*)this->value)[i] = ((byte*)other.value)[i];
			}
		}
		bIsValueSet = other.bIsValueSet;
		
		return *this;
	}

	bool isValueSet(){ return bIsValueSet; }

	template <typename Type>
	bool isOfType(){ return (valueSizeInBytes == sizeof(Type) && istypeof<Type>(type, size)); }
	template <typename Type>
	bool isOfType(uint c){ return (valueSizeInBytes == c*sizeof(Type) && istypeof<Type>(type, size) && c == count); }

	template <typename Type>
	Type getAs(){ return (this->isOfType<Type>() && bIsValueSet == true)? *(Type*)value : Type(0); }
	template <typename Type>
	const Type* getAs(uint c){ return (this->isOfType<Type>(c) && bIsValueSet == true)? (Type*)value : nullptr; }

	template <typename Type>
	bool setAs(const Type v){ if(value != nullptr && this->isOfType<Type>()){ (*(Type*)value) = v; bIsValueSet = true; return true; } return false; }
	template <typename Type>
	bool setAs(const Type* v, uint c){ if(value != nullptr && this->isOfType<Type>(c)){ memcpy(value, v, valueSizeInBytes); bIsValueSet = true; return true; } return false; }

	~SMaterialParam(){
		Release();
	}
};

struct SMaterialParamsGroup{
	std::string ubstruct;
	std::vector<SMaterialParam> params;

	SMaterialParamsGroup(std::string n, const std::vector<SMaterialParam>& p) : ubstruct(n), params(p) { }
	SMaterialParamsGroup(std::string n) : ubstruct(n) { }
	SMaterialParamsGroup() : ubstruct("") { }

	bool operator == (const SMaterialParamsGroup& other) const{
		if(ubstruct != other.ubstruct) return false;
		if(params.size() != other.params.size()) return false;
		for(uint i = 0; i < params.size(); ++i)
			if(params[i] != other.params[i]) return false;
		return true;
	}
	bool operator !=(const SMaterialParamsGroup& other) const{ return !((*this) == other); }

	SMaterialParamsGroup& operator = (const SMaterialParamsGroup& other){
		ubstruct = other.ubstruct;
		params.resize(other.params.size());
		for(uint i = 0; i < other.params.size(); ++i)
			params[i] = other.params[i];
		return *this;
	}
};

enum class EMaterialTextureType{
	None = 0,
	Albedo = 1<<0,
	Normal = 1<<1,
	Roughness = 1<<2,
	Specular = 1<<3,
	AmbientOcclusion = 1<<4,
	Metalness = 1<<5,
	Emissive = 1<<6,
	AmbientMap = 1<<7,
	Height = 1<<8,
	Opacity = 1<<9
};
inline uint operator | (const EMaterialTextureType& a, const EMaterialTextureType& b){	return ((uint)a) | ((uint)b); }
inline uint operator | (const uint& a, const EMaterialTextureType& b){ return ((uint)a) | ((uint)b); }
inline uint operator | (const EMaterialTextureType& a, const uint& b){ return ((uint)a) | ((uint)b); }
inline uint& operator |= (uint& a, const EMaterialTextureType& b){ a = ((uint)a) | ((uint)b); return a; }

uint getMaterialTextureTypeFromString(const char* cstr);

struct SMaterialTexture{
	std::string name;
	std::string path;
	uint type = 0; //EMaterialTextureType

	bool operator == (const SMaterialTexture& other) const{
		return type == other.type &&
			name == other.name &&
			path == other.path;
	}
	bool operator != (const SMaterialTexture& other) const{ return !(*this == other); }

	SMaterialTexture& operator = (const SMaterialTexture& other){
		type = other.type;
		name = other.name;
		path = other.path;
		return *this;
	}
};

struct SMaterialDesc{
	std::string name;
	std::string shader;
	std::vector<SMaterialParamsGroup> paramGroups;
	std::vector<SMaterialTexture> textures;

	bool operator == (const SMaterialDesc& other) const{
		if(name != other.name) return false;
		if(shader != other.shader) return false;
		if(paramGroups.size() != other.paramGroups.size()) return false;
		for(uint i = 0; i < paramGroups.size(); ++i){
			if(paramGroups[i] != other.paramGroups[i]) return false;}
		for(uint i = 0; i < textures.size(); ++i){
			if(textures[i] != other.textures[i]) return false;}
		return true;
	}
	bool operator !=(const SMaterialDesc& other) const{ return !((*this) == other); }

	SMaterialDesc& operator = (const SMaterialDesc& other){
		name = other.name;
		shader = other.shader;
		paramGroups.resize(other.paramGroups.size());
		for(uint i = 0; i < other.paramGroups.size(); ++i)
			paramGroups[i] = other.paramGroups[i];
		textures.resize(other.textures.size());
		for(uint i = 0; i < other.textures.size(); ++i)
			textures[i] = other.textures[i];
		return *this;
	}
};

//----------------------------------------------------------------------------------------------
class CMaterial;
class IUniformBuffer;
class CTextureView;
class CShaderProgram;

class CMaterialInstance{
protected:
	CMaterial* material = nullptr;
	stdex::container<SharedPtr<IUniformBuffer>> uniformBuffers;
	stdex::container<SharedPtr<CTextureView>> textures;
	SharedPtr<CShaderProgram> shader;

	CMaterialInstance(CMaterial* m) : material(m){}
public:

	CTextureView* getTexture(uint i){ if(i >= textures.size()) return nullptr; return textures[i].get(); }
	IUniformBuffer* getUniformBuffer(uint i){ if(i >= uniformBuffers.size()) return nullptr; return uniformBuffers[i].get(); }

	CShaderProgram* getShader(){ return shader.get(); }
	bool hasShader(){ return shader != nullptr; }

	~CMaterialInstance();

	friend class CMaterial;
};

//----------------------------------------------------------------------------------------------
class GPUDevice;

class CMaterial{
protected:
	SMaterialDesc descriptor;
	stdex::container<SharedPtr<CMaterialInstance>> materialInstances;

	bool Create();
public:
	CMaterial(const SMaterialDesc& desc) : descriptor(desc){
		Create(); }

	SharedPtr<CMaterialInstance> CreateInstance(GPUDevice* dev, std::vector<SharedPtr<IUniformBuffer>> ubs, std::vector<std::string> ubnames = {});

	static SMaterialDesc CreateMaterialDescFromXML(std::string xml);
	static SMaterialDesc CreateMaterialDescFromXML(void* xmlobject);

	static bool CheckIfParamsGroupAndUBAreEqual(const std::vector<SMaterialParam>& mpg, const std::vector<SUniformMap>& ub);
	static bool CheckIfParamsGroupAndUBAreEqual(const SMaterialParamsGroup& mpg, const IUniformBuffer& ub);

	friend class CMaterialManager;
};

//----------------------------------------------------------------------------------------------

class CMaterialManager{
protected:
	stdex::container<SharedPtr<CMaterial>> materials;
public:
	SharedPtr<CMaterial> CreateMaterial(const SMaterialDesc& desc);

	friend class CSingleton<CMaterialManager>;
};

#endif //MATERIAL_H