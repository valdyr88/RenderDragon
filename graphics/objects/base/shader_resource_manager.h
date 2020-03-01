#ifndef SHADER_RESOURCE_MANAGER_H
#define SHADER_RESOURCE_MANAGER_H

#ifdef RD_API_BASE

#include <string>
#include "../../utils/types/types.h"
#include "../../utils/pointers.h"
#include "../../descriptors/shader_desc.h"
#include "../../descriptors/graphics_enums.h"
#include "../../descriptors/graphic_object.h"
#include "shader.h"

//---------------------------------------------------------------------------

struct SShaderResourceBindingDesc{
	uint32 setNumber = 0;
	uint32 bindPoint = 0;
	std::string name = "";

	EShaderResourceType type;
	EShaderResourceUsageType usage = EShaderResourceUsageType::Static;
	uint32 shaderStages = 0x00000000; //bitflag of EShaderStage
	uint32 count = 0;
	
	uint operator |= (EShaderStage& stage){ shaderStages |= stage; return shaderStages; }
	uint operator | (EShaderStage& stage){ return shaderStages | stage; }
	uint operator |= (uint stage){ shaderStages |= stage; return shaderStages; }
	uint operator | (uint stage){ return shaderStages | stage; }

	SShaderResourceBindingDesc(uint32 setNum, uint32 bindPt, std::string nam,
		EShaderResourceType t, uint s, EShaderResourceUsageType ut = EShaderResourceUsageType::Static, uint32 cnt = 1) :
		setNumber(setNum), bindPoint(bindPt), name(nam),
		type(t), shaderStages(s), usage(ut), count(cnt) {}
	SShaderResourceBindingDesc(const SShaderResourceBindingDesc& other) :
		type(other.type), usage(other.usage), shaderStages(other.shaderStages),
		count(other.count), name(other.name), bindPoint(other.bindPoint), setNumber(other.setNumber){}

	//Note: this operator doesn't check setNumber.
	bool operator == (const SShaderResourceBindingDesc& other) const{
		return type == other.type &&
			usage == other.usage &&
			shaderStages == other.shaderStages &&
			count == other.count &&
			//setNumber == other.setNumber && //this is a check without set number.
			bindPoint == other.bindPoint &&
			name == other.name;
	}
	//Note: this operator doesn't check setNumber.
	bool operator != (const SShaderResourceBindingDesc& other) const{ return !(*this == other); }

	SShaderResourceBindingDesc& operator = (const SShaderResourceBindingDesc& other){
		type = other.type;
		usage = other.usage;
		shaderStages = other.shaderStages;
		count = other.count;
		setNumber = other.setNumber;
		bindPoint = other.bindPoint;
		name = other.name;
		return *this;
	}
};

class CShaderResourceBinding : public CGraphicObject{
protected:
	SShaderResourceBindingDesc descriptor;
	CShaderResource* resource;

	CShaderResourceBinding() = default;

public:
	CShaderResourceBinding(GPUDevice* dev, const SShaderResourceBindingDesc& desc, CShaderResource* res)
		: CGraphicObject(dev), descriptor(desc), resource(res){}

	bool operator == (CShaderResourceBinding& other) const{ return descriptor == other.descriptor && resource == other.resource; }
	bool operator == (SShaderResourceBindingDesc& other) const{ return descriptor == other; }
	bool operator == (CShaderResource* res) const{ return resource == res; }

	bool operator != (CShaderResourceBinding& other) const{ return !(*this == other); }
	bool operator != (SShaderResourceBindingDesc& other) const{ return !(*this == other); }
	bool operator != (CShaderResource* res) const{ return !(*this == res); }

	const auto& getDescriptor(){ return descriptor; }

	friend class CShaderResourceSet;
};


//---------------------------------------------------------------------------
class CShaderResourceSetDesc;

class CShaderResourceSet : public CGraphicObject{
protected:
	const CShaderResourceSetDesc& descriptor;
	std::vector<SharedPtr<CShaderResourceBinding>> bindings;

	void CreateBindings(const std::vector<CShaderResource*>& binds);
public:
	CShaderResourceSet(GPUDevice* dev, const CShaderResourceSetDesc* desc, const std::vector<CShaderResource*>& binds)
		: CGraphicObject(dev), descriptor(*desc)
	{
		CreateBindings(binds);
	}

	bool operator ==(const std::vector<SharedPtr<CShaderResourceBinding>>& binds) const{
		if(bindings.size() != binds.size()) return false;
		for(uint i = 0; i < bindings.size(); ++i){
			if(*bindings[i].get() != *binds[i].get()) return false;
		}
		return true;
	}
	bool operator ==(const std::vector<CShaderResource*>& res) const{
		if(bindings.size() != res.size()) return false;
		for(uint i = 0; i < bindings.size(); ++i){
			if(bindings[i]->resource != res[i]) return false;
		}
		return true;
	}
	bool operator ==(const CShaderResourceSet& other) const;
	
	bool operator !=(const std::vector<SharedPtr<CShaderResourceBinding>>& binds) const{ return !(*this == binds); }
	bool operator !=(const std::vector<CShaderResource*>& res) const{ return !(*this == res); }
	bool operator !=(const CShaderResourceSet& other) const{ return !(*this == other); }

	const auto& getDescriptor(){ return descriptor; }
};

class CShaderResourceSetDesc : public CGraphicObject{
protected:
	std::list<std::pair<uint32, CShaderProgram*>> usedInPipelines;
	std::vector<SShaderResourceBindingDesc> bindingDescs;
	std::list<SharedPtr<CShaderResourceSet>> cachedResourceSets;

	SharedPtr<CShaderResourceSet>
		CreateResourceSet(std::vector<CShaderResource*>&);

	bool CheckValidity();

public:
	SharedPtr<CShaderResourceSet>
		GetResourceSetWith(std::vector<CShaderResource*>);

	CShaderResourceSetDesc(GPUDevice* dev, const std::vector<SShaderResourceBindingDesc>& binds)
		: CGraphicObject(dev), bindingDescs(binds)
	{
		CheckValidity();
	}

	bool operator == (const CShaderResourceSetDesc& other) const{
		if(bindingDescs.size() != other.bindingDescs.size()) return false;
		for(uint i = 0; i < bindingDescs.size(); ++i){
			if(bindingDescs[i] != other.bindingDescs[i]) return false;
		}
		return true;
	}
	bool operator == (const std::vector<SShaderResourceBindingDesc>& binds) const{
		if(bindingDescs.size() != binds.size()) return false;
		for(uint i = 0; i < bindingDescs.size(); ++i){
			if(bindingDescs[i] != binds[i]) return false;
		}
		return true;
	}
	bool operator != (const CShaderResourceSetDesc& other) const{ return !(*this == other); }
	bool operator != (const std::vector<SShaderResourceBindingDesc>& other) const{ return !(*this == other); }

	friend class CShaderResourceSet;
};

inline bool CShaderResourceSet::operator ==(const CShaderResourceSet& other) const{
	if(descriptor != other.descriptor) return false;
	if(*this != other.bindings) return false;
	return true;
}

//---------------------------------------------------------------------------

class CShaderResourceSetManager : public CGraphicObject{
protected:
	std::list<SharedPtr<CShaderResourceSetDesc>> resourceSetDesc;

	SharedPtr<CShaderResourceSetDesc> CreateResourceSetDesc(std::vector<SShaderResourceBindingDesc>&);
public:

	CShaderResourceSetManager(GPUDevice* dev)
		: CGraphicObject(dev){}

	SharedPtr<CShaderResourceSetDesc> GetResourceSetDesc(std::vector<SShaderResourceBindingDesc>);
};

//---------------------------------------------------------------------------
#endif //RD_API_BASE
#endif //SHADER_RESOURCE_MANAGER_H