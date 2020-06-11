#ifndef SHADER_RESOURCE_MANAGER_H
#define SHADER_RESOURCE_MANAGER_H

#ifdef RD_API_WEBGL

#include <string>
#include "../../utils/types/types.h"
#include "../../utils/pointers.h"
#include "../../utils/singleton.h"
#include "../../descriptors/shader_desc.h"
#include "../../descriptors/graphics_enums.h"
#include "../../descriptors/graphic_object.h"

//---------------------------------------------------------------------------

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

class CShaderProgram;

class CShaderResourceSetDesc : public CGraphicObject{
protected:
	std::list<std::pair<uint32, CShaderProgram*>> usedInPipelines;
	std::vector<SShaderResourceBindingDesc> bindingDescs;
	std::list<SharedPtr<CShaderResourceSet>> cachedResourceSets;

	SharedPtr<CShaderResourceSet>
		CreateResourceSet(std::vector<CShaderResource*>&);

public:
	SharedPtr<CShaderResourceSet>
		GetResourceSetWith(std::vector<CShaderResource*>);

	CShaderResourceSetDesc(GPUDevice* dev, const std::vector<SShaderResourceBindingDesc>& binds)
		: CGraphicObject(dev), bindingDescs(binds)
	{
		CShaderResourceSetDesc::CheckValidity(bindingDescs, true);
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
	
	static bool CheckValidity(const std::vector<SShaderResourceBindingDesc>&, bool bAssert = false);
	static std::vector<SShaderResourceBindingDesc> merge(const std::vector<SShaderResourceBindingDesc>& a, const std::vector<SShaderResourceBindingDesc>& b);

	friend class CShaderResourceSet;
	friend class CShaderProgram;
	friend class CShader;
};

inline bool CShaderResourceSet::operator ==(const CShaderResourceSet& other) const{
	if(descriptor != other.descriptor) return false;
	if(*this != other.bindings) return false;
	return true;
}

//---------------------------------------------------------------------------

class CShaderResourceSetManager{
protected:
	GPUDevice* device = nullptr;

	std::list<SharedPtr<CShaderResourceSetDesc>> resourceSetDesc;
	SharedPtr<CShaderResourceSetDesc> CreateResourceSetDesc(std::vector<SShaderResourceBindingDesc>&);
	
	CShaderResourceSetManager(){}
	CShaderResourceSetManager(GPUDevice* dev) : device(dev){}
	void setDevice(GPUDevice* dev){ device = dev; }
public:
	SharedPtr<CShaderResourceSetDesc> GetResourceSetDesc(std::vector<SShaderResourceBindingDesc>);

	friend class CSingleton<CShaderResourceSetManager>;
	friend bool rdSetupDeviceForGlobalObjects(GPUDevice* device);
};

//---------------------------------------------------------------------------
#endif //RD_API_OPENGL4
#endif //SHADER_RESOURCE_MANAGER_H