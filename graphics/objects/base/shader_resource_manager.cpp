#ifdef RD_API_BASE

#include "../../utils/log.h"
#include "../../utils/containers.h"
#include "device.h"
#include "shader_resource_manager.h"

//---------------------------------------------------------------------------

bool CheckNotInList(uint pt, std::list<uint>& points){
	for(auto it = points.begin(); it != points.end(); ++it)
		if(pt == *it) return false;
	return true;
}

void CShaderResourceSet::CreateBindings(const std::vector<CShaderResource*>& binds){
	ASSERT(device != nullptr);
	ASSERT(binds.size() != 0);
	ASSERT(descriptor.bindingDescs.size() == binds.size());

	std::list<uint> bindPoints;

	for(uint i = 0; i < binds.size(); ++i){
		auto& desc = descriptor.bindingDescs[i];

		ASSERT(CheckNotInList(desc.bindPoint, bindPoints));
		ASSERT(desc.type == binds[i]->getResourceType());
		
		bindPoints.emplace_back(desc.bindPoint);
	}
	
	for(uint i = 0; i < binds.size(); ++i){
		SharedPtr<CShaderResourceBinding> bind = 
			device->CreateShaderResourceBinding(descriptor.bindingDescs[i], binds[i]);
		bindings.emplace_back(bind);
	}
}

//---------------------------------------------------------------------------

bool CShaderResourceSetDesc::CheckValidity(){
	std::list<uint> bindPoints;
	ASSERT(bindingDescs.size() != 0);
	if(bindingDescs.size() == 0) return false;

	uint setNumber = bindingDescs[0].setNumber;

	for(uint i = 0; i < bindingDescs.size(); ++i){
		bool checkBindPoint = CheckNotInList(bindingDescs[i].bindPoint, bindPoints);
		bool sameSetNumber = setNumber == bindingDescs[i].setNumber;

		ASSERT(checkBindPoint);
		if(checkBindPoint == false) return false;
		ASSERT(sameSetNumber);
		if(sameSetNumber == false) return false;

		bindPoints.emplace_back(bindingDescs[i].bindPoint);
	}

	return true;
}

SharedPtr<CShaderResourceSet> CShaderResourceSetDesc::
	CreateResourceSet(std::vector<CShaderResource*>& resrs)
{
	ASSERT(device != nullptr);

	SharedPtr<CShaderResourceSet> reset = device->CreateShaderResourceSet(this, resrs);
	cachedResourceSets.emplace_back(reset);

	return reset;
}

SharedPtr<CShaderResourceSet> CShaderResourceSetDesc::
	GetResourceSetWith(std::vector<CShaderResource*> resrs)
{
	ASSERT(device != nullptr);

	for(auto it = cachedResourceSets.begin(); it != cachedResourceSets.end(); ++it){
		auto& reset = *it->get();
		if(reset == resrs)
			return *it;
	}

	return CreateResourceSet(resrs);
}

//---------------------------------------------------------------------------

SharedPtr<CShaderResourceSetDesc> CShaderResourceSetManager::
	CreateResourceSetDesc(std::vector<SShaderResourceBindingDesc>& binds)
{
	ASSERT(device != nullptr);

	SharedPtr<CShaderResourceSetDesc> rest = device->CreateShaderResourceSetDesc(binds);
	resourceSetDesc.push_back(rest);
	
	return rest;
}

SharedPtr<CShaderResourceSetDesc> CShaderResourceSetManager::
	GetResourceSetDesc(std::vector<SShaderResourceBindingDesc> binds)
{
	ASSERT(device != nullptr);

	for(auto it = resourceSetDesc.begin(); it != resourceSetDesc.end(); ++it){
		auto& resetdes = *it->get();
		if(resetdes == binds)
			return *it;
	}

	return CreateResourceSetDesc(binds);
}

//---------------------------------------------------------------------------


#endif
