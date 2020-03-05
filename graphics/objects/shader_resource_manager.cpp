#include "../utils/containers.h"

#ifdef RD_API_BASE
#include "base/device.h"
#include "base/shader_resource_manager.h"
#include "base/shader_resource_manager.cpp"
#endif

#ifdef RD_API_OPENGL4
#include "opengl4/shader_resource_manager.cpp"
#endif

#ifdef RD_API_WEBGL
#endif

#ifdef RD_API_VULKAN
#endif

//---------------------------------------------------------------------------------
//common code
//---------------------------------------------------------------------------------

#include <algorithm>    // std::sort
#include "../utils/log.h"
#include "../utils/containers.h"

//---------------------------------------------------------------------------

void CShaderResourceSet::CreateBindings(const std::vector<CShaderResource*>& binds){
	ASSERT(device != nullptr);
	ASSERT(binds.size() != 0);
	ASSERT(descriptor.bindingDescs.size() == binds.size());

	std::list<uint32> bindPoints;

	for(uint i = 0; i < binds.size(); ++i){
		auto& desc = descriptor.bindingDescs[i];

		ASSERT(std::CheckNotInList<uint32>(desc.bindPoint, bindPoints));
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

bool CShaderResourceSetDesc::CheckValidity(const std::vector<SShaderResourceBindingDesc>& desc, bool bAssert){
	if(bAssert) ASSERT(desc.size() != 0);
	if(desc.size() == 0) return false;

	uint setNumber = desc[0].setNumber;
	std::list<uint32> bindPoints;
	std::list<std::string> bindPointNames;

	for(uint i = 0; i < desc.size(); ++i){
		bool checkBindPoint = std::CheckNotInList<uint32>(desc[i].bindPoint, bindPoints);
		bool checkBindPointName = std::CheckNotInList<std::string>(desc[i].name, bindPointNames);
		bool sameSetNumber = setNumber == desc[i].setNumber;

		if(bAssert) ASSERT(checkBindPoint);
		if(checkBindPoint == false) return false;
		if(bAssert) ASSERT(sameSetNumber);
		if(sameSetNumber == false) return false;

		bindPoints.emplace_back(desc[i].bindPoint);
		bindPointNames.emplace_back(desc[i].name);
	}

	return true;
}

SharedPtr<CShaderResourceSet> CShaderResourceSetDesc::
CreateResourceSet(std::vector<CShaderResource*>& resources)
{
	ASSERT(device != nullptr);
	ASSERT(bindingDescs.size() == resources.size());

	SharedPtr<CShaderResourceSet> reset = device->CreateShaderResourceSet(this, resources);
	cachedResourceSets.emplace_back(reset);

	return reset;
}

SharedPtr<CShaderResourceSet> CShaderResourceSetDesc::
GetResourceSetWith(std::vector<CShaderResource*> resources)
{
	ASSERT(device != nullptr);
	ASSERT(bindingDescs.size() == resources.size());

	for(auto it = cachedResourceSets.begin(); it != cachedResourceSets.end(); ++it){
		auto& reset = *it->get();
		if(reset == resources)
			return *it;
	}

	return CreateResourceSet(resources);
}

std::vector<SShaderResourceBindingDesc> CShaderResourceSetDesc::
merge(const std::vector<SShaderResourceBindingDesc>& a, const std::vector<SShaderResourceBindingDesc>& b)
{
	using SResDesc = SShaderResourceBindingDesc;
	{
		bool aValid = CShaderResourceSetDesc::CheckValidity(a);
		bool bValid = CShaderResourceSetDesc::CheckValidity(b);

		if(aValid == false && bValid == false) return std::vector<SResDesc>();
		if(aValid == false) return b;
		if(bValid == false) return a;

		if(a.size() == 0) return b;
		if(b.size() == 0) return a;
		if(a[0].setNumber != b[0].setNumber){
			LOG_ERR("setNumbers for two SShaderResourceBindingDesc arrays are not the same");
			return a;
		}
	}

	std::vector<SResDesc> merged(a);

	for(auto it = b.begin(); it != b.end(); ++it){
		auto& desc = *it;

		//check if same to skip
		bool add = std::CheckInContainer<SResDesc, std::vector<SResDesc>>(desc, merged,
			[](const SResDesc& a, SResDesc& b){
				if(a == b) return false;
				return true;
			}
		);
		if(add == false) continue;//skip

		add = std::CheckInContainer<SResDesc, std::vector<SResDesc>>(desc, merged,
			[](const SResDesc& a/*desc*/, SResDesc& b/*elem from merged*/)
			{
				//if shaderStages are different, check if everything else is same and then merge shaderStage bits
				if(a.shaderStages != b.shaderStages){
					SResDesc test(a); test.shaderStages = b.shaderStages;
					if(test == b){
						b.shaderStages |= a.shaderStages; //change b directly (b is a reference to elem in merged)
						return false; //no need to add since b is already in merged
					}
				}
				//if bindPoint is the same and other is not then it's error
				if(a.bindPoint == b.bindPoint){
					LOG_ERR("bindPoint is the same but other resource info is different");
					return false;
				}
				//if name is same the and other is not then it's error
				if(a.name == b.name){
					LOG_ERR("name is the same but other resource info is different");
					return false;
				}

				return true;
			}
		);
		if(add == false) continue;//skip

		merged.emplace_back(desc);
	}

	std::sort(merged.begin(), merged.end(), [](SResDesc& a, SResDesc& b){ return a.bindPoint < b.bindPoint; });

	return std::move(merged);
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


//---------------------------------------------------------------------------------