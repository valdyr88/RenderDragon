#include "shader.h"

IUniformBuffer* CShader::getUniformBuffer(uint bindPoint)
{
	for(auto it = resources.begin(); it != resources.end(); ++it){
		auto& resource = *it;
		if(resource.first == bindPoint){
			if(resource.second->getDescriptor().type == EShaderResourceType::UniformBuffer)
				return (IUniformBuffer*) resource.second.get();
			else
				return nullptr;
		}
	}
	return nullptr;
}

IUniformBuffer* CShaderProgram::getUniformBuffer(EShaderStage stage, uint bindPoint)
{
	if(stage >= EShaderStage::NumStages) return nullptr;
	if(shader[stage] == nullptr) return nullptr;

	return shader[stage]->getUniformBuffer(bindPoint);
}
