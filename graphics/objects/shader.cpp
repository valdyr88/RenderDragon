#include "../utils/containers.h"

#ifdef RD_API_BASE
#include "base/device.h"
#include "base/shader.cpp"
#endif

#ifdef RD_API_OPENGL4
#include "opengl4/device.h"
#include "opengl4/shader.cpp"
#endif

#ifdef RD_API_WEBGL
#endif

#ifdef RD_API_VULKAN
#endif

//---------------------------------------------------------------------------------
//common code
//---------------------------------------------------------------------------------

bool CShaderProgram::MergeShaderResourceSetDescs(){
	typedef std::vector<SShaderResourceBindingDesc> SRDescVector;
	std::list<SRDescVector> mergedDescs;

	for(uint s = 0; s < EShaderStage::NumStages; ++s){
		if(shader[s].get() == nullptr) continue;
		auto& sh = shader[s];

		//go through all SRDesc sets in all shaders, and merge the ones that have same setNumber
		auto& srcDescList = sh->descriptor.bindingSetsDesc;
		if(srcDescList.size() > 0){

			for(auto sr = srcDescList.begin(); sr != srcDescList.end(); ++sr){
				SRDescVector& srcResDesc = *sr;
				if(srcResDesc.size() == 0)
					continue;

				//check if there is already a setNum in mergedDescs
				bool isAdded = !stdex::CheckInContainer<SRDescVector, std::list<SRDescVector>>(srcResDesc, mergedDescs,
					[](const SRDescVector& a, SRDescVector& b)
					{
						if(a[0].setNumber == b[0].setNumber){ //if already in list, merge
							b = CShaderResourceSetDesc::merge(b, a);
							return false;
						}
						return true;
					});

				if(isAdded)
					continue;

				mergedDescs.emplace_back(srcResDesc);
			}
		}
	}

	//check all resource names if there aren't any duplicates.
	{
		std::list<std::string> names;
		for(auto it = mergedDescs.begin(); it != mergedDescs.end(); ++it){
			
			for(auto n = it->begin(); n != it->end(); ++n){
				if(names.size() == 0) break;

				bool hasName = !stdex::CheckInContainer<std::string, std::list<std::string>>(n->name, names,
					[](const std::string& a, std::string& b){
						if(a == b){
							LOG_ERR("shader resource name <%s> is already used in another resource set", a.c_str());
							return false;
						}
						return true;
					});
			}

			for(auto n = it->begin(); n != it->end(); ++n){
				names.emplace_back(n->name); }
		}
	}
	
	auto SRManager = device->GetShaderResourceManager();

	for(auto it = mergedDescs.begin(); it != mergedDescs.end(); ++it){
		auto setDesc = SRManager.GetResourceSetDesc(*it);
		resourceSetDescs.emplace_back(setDesc);
	}

	return true;
}

//---------------------------------------------------------------------------------