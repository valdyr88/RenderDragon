#ifndef SHADER_H
#define SHADER_H

#include <vector>
#include <string>
#include <list>
#include "../../utils/pointers.h"
#include "../../utils/types/types.h"
#include "../../descriptors/graphics_enums.h"
#include "../../descriptors/shader_desc.h"
#include "../../descriptors/graphic_object.h"

class CShader : CGraphicObject{
protected:
	SShaderDesc descriptor;

	IUniformBuffer* getUniformBuffer(uint bindPoint);
public:

	CShader(WeakPtr<IDevice>& dev, const SShaderDesc& desc) :
		CGraphicObject(dev), descriptor(desc){}

	friend class CShaderProgram;
	friend class CPipelineState;
};


class CShaderProgram : CGraphicObject{
protected:
	SharedPtr<CShader> shader[EShaderStage::NumStages];
	uint numStages;
public:

	CShaderProgram(WeakPtr<IDevice>& dev, std::vector<SharedPtr<CShader>> shaders) :
		CGraphicObject(dev){
		for(auto it = shaders.begin(); it != shaders.end(); ++it){
			auto& sh = *it;
			if(sh->descriptor.stage < EShaderStage::NumStages){
				shader[sh->descriptor.stage] = sh;
			}
		}
	}

	uint getNofStages(){ return numStages; }

	IUniformBuffer* getUniformBuffer(EShaderStage stage, uint bindPoint);

	friend class CPipelineState;
};

#endif //SHADER_H