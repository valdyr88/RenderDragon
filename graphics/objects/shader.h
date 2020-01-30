#ifndef SHADER_H
#define SHADER_H

#include <string>
#include <list>
#include "../../utils/pointers.h"
#include "../../utils/types/types.h"
#include "../enum/graphics_enums.h"
#include "shader_desc.h"
#include "graphic_object.h"

class CShader : CGraphicObject{
protected:
	EShaderStage stage;
	std::string source;

	std::list<std::pair<uint, SharedPtr<CShaderResource>>> resources;

	IUniformBuffer* getUniformBuffer(uint bindPoint);
public:

	friend class CShaderProgram;
	friend class CPipelineState;
};



class CShaderProgram : CGraphicObject{
protected:
	SharedPtr<CShader> shader[EShaderStage::NumStages];
	uint numStages;
public:
	uint getNofStages(){ return numStages; }

	IUniformBuffer* getUniformBuffer(EShaderStage stage, uint bindPoint);

	friend class CPipelineState;
};

#endif //SHADER_H