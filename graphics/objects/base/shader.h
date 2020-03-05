#ifndef SHADER_H
#define SHADER_H

#ifdef RD_API_BASE

#include <vector>
#include <list>
#include "../../utils/pointers.h"
#include "../../utils/strings.h"
#include "../../utils/types/types.h"
#include "../../descriptors/graphics_enums.h"
#include "../../descriptors/shader_desc.h"
#include "../../descriptors/graphic_object.h"
#include "shader_resource_manager.h"

class CShader : public CGraphicObject{
protected:
	SShaderDesc descriptor;

	IUniformBuffer* getUniformBuffer(uint bindPoint);
public:

	CShader(GPUDevice* dev, const SShaderDesc& desc) :
		CGraphicObject(dev), descriptor(desc){}

	const auto& getDescriptor(){ return descriptor; }

	virtual ~CShader() = default;

	friend class CShaderProgram;
	friend class CPipelineState;
};

class CShaderProgram : public CGraphicObject{
protected:
	uint numStages;
	SharedPtr<CShader> shader[EShaderStage::NumStages];
	std::vector<SharedPtr<CShaderResourceSetDesc>> resourceSetDescs;

	bool MergeShaderResourceSetDescs();
public:

	CShaderProgram(GPUDevice* dev, std::vector<SharedPtr<CShader>> shaders) :
		CGraphicObject(dev)
	{
		numStages = 0;
		
		for(auto it = shaders.begin(); it != shaders.end(); ++it){
			auto& sh = *it;
			if(sh->descriptor.stage < EShaderStage::NumStages){
				shader[sh->descriptor.stage] = sh; ++numStages;
			}
		}

		MergeShaderResourceSetDescs();
	}

	uint getNofStages(){ return numStages; }
	IUniformBuffer* getUniformBuffer(EShaderStage stage, uint bindPoint);

	virtual ~CShaderProgram() = default;

	friend class CPipelineState;
};

#endif //RD_API_BASE
#endif //SHADER_H