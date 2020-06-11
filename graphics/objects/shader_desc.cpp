#include "../descriptors/shader_desc.h"
#include "../utils/containers.h"

#ifdef RD_API_BASE
#include "base/shader_resource_manager.h"
#endif

#ifdef RD_API_OPENGL4
#include "opengl4/shader_resource_manager.h"
#endif

#ifdef RD_API_WEBGL
#include "webgl/shader_resource_manager.h"
#endif

#ifdef RD_API_VULKAN
#endif

//---------------------------------------------------------------------------------
//common code
//---------------------------------------------------------------------------------

bool SShaderDesc::VerifyBindingDesc(){
	std::list<uint32> setNumbers;

	for(auto it = bindingSetsDesc.begin(); it != bindingSetsDesc.end(); ++it){
		auto& setDesc = *it;

		if(setDesc.size() == 0) return false;
		uint32 setNumber = setDesc[0].setNumber;

		bool bCheck = CShaderResourceSetDesc::CheckValidity(setDesc, true);
		if(bCheck == false)
			return false;

		bCheck = stdex::CheckInContainer<uint32, std::list<uint32>>(setNumber, setNumbers,
				[](const uint32& a, uint32& b){
					if(a == b) return false;
					return true;
				});
		if(bCheck == false)
			return false;

		setNumbers.emplace_back(setNumber);
	}
	return true;
}

//---------------------------------------------------------------------------------