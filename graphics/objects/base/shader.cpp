#include "shader.h"

#ifdef RD_API_BASE

IUniformBuffer* CShader::getUniformBuffer(uint bindPoint)
{
	/*auto& resources = descriptor.resources;

	for(auto it = resources.begin(); it != resources.end(); ++it){
		auto& resource = *it;
		if(resource.first == bindPoint){
			if(resource.second->getResourceType() == EShaderResourceType::UniformBuffer)
				return (IUniformBuffer*) resource.second.get();
			else
				return nullptr;
		}
	}*/
	//ToDo: implement this
	return nullptr;
}

IUniformBuffer* CShaderProgram::getUniformBuffer(EShaderStage stage, uint bindPoint)
{
	if(stage >= EShaderStage::NumStages) return nullptr;
	if(shader[stage] == nullptr) return nullptr;

	return shader[stage]->getUniformBuffer(bindPoint);
}

#endif RD_API_BASE