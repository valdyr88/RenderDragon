#ifndef MATERIAL_H
#define MATERIAL_H

/*
ToDo:
 - sustav koji iz definicije materijala u fileu odlu�i koje uniform buffer strukture treba korisit (zna�i UniformBuffer<OdlucenaStruktura>).
		to mo�e posti� preko templatea nekako, svako kreiranje nove strukture za uniform 
 - napravit descriptore za CMaterial parametre, kao u glMaterial.js u WebGL projektu
*/

#include <string>
#include <vector>
#include "../../utils/pointers.h"
#include "../../descriptors/uniform_buffer_desc.h"

struct SMaterialParam : public SUniformMap{
private:
	void* value = nullptr;
	uint valueSizeInBytes = 0;
	bool bIsValueSet = false;
public:

	SMaterialParam(const char* n, EValueType t, EValueSize s, uint32 c = 1) :
		SUniformMap(n, t, s, c)
	{
		valueSizeInBytes = sizeInBytes(t) * componentCount(s) * count;
		value = __new byte[valueSizeInBytes];
	}

	template <typename Type>
	bool isOfType(){ return (valueSizeInBytes == sizeof(Type) && istypeof<Type>(type, size)); }
	template <typename Type>
	bool isOfType(uint c){ return (valueSizeInBytes == c*sizeof(Type) && istypeof<Type>(type, size) && c == count); }

	template <typename Type>
	Type getAs(){ return (this->isOfType<Type>() && bIsValueSet == true)? *(Type*)value : Type(0); }
	template <typename Type>
	const Type* getAs(uint c){ return (this->isOfType<Type>(c) && bIsValueSet == true)? (Type*)value : nullptr; }

	template <typename Type>
	bool setAs(Type v){ if(this->isOfType<Type>()){ (*(Type*)value) = v; bIsValueSet = true; return true; } return false; }
	template <typename Type>
	bool setAs(Type* v, uint c){ if(this->isOfType<Type>(c)){ memcpy(value, v, valueSizeInBytes); bIsValueSet = true; return true; } return false; }
};

struct SMaterialParamsGroup{
	std::string ubname;
	std::vector<SMaterialParam> params;

	SMaterialParamsGroup(std::string n, const std::vector<SMaterialParam>& p) : ubname(n), params(p) { }
	SMaterialParamsGroup(std::string n) : ubname(n) { }

	bool operator == (const SMaterialParamsGroup& other) const{
		if(ubname != other.ubname) return false;
		if(params.size() != other.params.size()) return false;
		for(uint i = 0; i < params.size(); ++i)
			if(params[i] != other.params[i]) return false;
		return true;
	}
	bool operator !=(const SMaterialParamsGroup& other) const{ return !((*this) == other); }

	SMaterialParamsGroup& operator = (const SMaterialParamsGroup& other){
		ubname = other.ubname;
		params.reserve(other.params.size());
		for(uint i = 0; i < other.params.size(); ++i)
			params[i] = other.params[i];
		return *this;
	}
};

struct SMaterialDesc{
	std::string name;
	std::vector<SMaterialParamsGroup> paramGroups;

	bool operator == (const SMaterialDesc& other) const{
		if(name != other.name) return false;
		if(paramGroups.size() != other.paramGroups.size()) return false;
		for(uint i = 0; i < paramGroups.size(); ++i){
			if(paramGroups[i] != other.paramGroups[i]) return false;
		}
		return true;
	}
	bool operator !=(const SMaterialDesc& other) const{ return !((*this) == other); }

	SMaterialDesc& operator = (const SMaterialDesc& other){
		name = other.name;
		paramGroups.reserve(other.paramGroups.size());
		for(uint i = 0; i < other.paramGroups.size(); ++i)
			paramGroups[i] = other.paramGroups[i];
		return *this;
	}
};

class CMaterial{
protected:
	SMaterialDesc descriptor;
public:
	CMaterial(const SMaterialDesc& desc) : descriptor(desc){}

	static SMaterialDesc CreateMaterialDescFromXML(std::string xml);
	static SMaterialDesc CreateMaterialDescFromXML(void* xmlobject);
};

#endif //MATERIAL_H