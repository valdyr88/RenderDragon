#ifndef SHADER_H
#define SHADER_H

#ifdef RD_API_OPENGL4

#include <vector>
#include <list>
#include "../../utils/pointers.h"
#include "../../utils/strings.h"
#include "../../utils/types/types.h"
#include "../../descriptors/graphics_enums.h"
#include "../../descriptors/shader_desc.h"
#include "../../descriptors/graphic_object.h"

class CShader : public CGraphicObject{
protected:
	SShaderDesc descriptor;

	IUniformBuffer* getUniformBuffer(uint bindPoint);
public:

	CShader(GPUDevice* dev, const SShaderDesc& desc) :
		CGraphicObject(dev), descriptor(desc){}

	virtual ~CShader() = default;

	friend class CShaderProgram;
	friend class CPipelineState;
};

class CShaderProgram : public CGraphicObject{
protected:
	SharedPtr<CShader> shader[EShaderStage::NumStages];
	uint numStages;
public:

	CShaderProgram(GPUDevice* dev, std::vector<SharedPtr<CShader>> shaders) :
		CGraphicObject(dev){
		numStages = 0;

		for(auto it = shaders.begin(); it != shaders.end(); ++it){
			auto& sh = *it;
			if(sh->descriptor.stage < EShaderStage::NumStages){
				shader[sh->descriptor.stage] = sh; ++numStages;
			}
		}
	}

	uint getNofStages(){ return numStages; }
	IUniformBuffer* getUniformBuffer(EShaderStage stage, uint bindPoint);

	virtual ~CShaderProgram() = default;

	friend class CPipelineState;
};

#endif //RD_API_OPENGL4
#endif //SHADER_H