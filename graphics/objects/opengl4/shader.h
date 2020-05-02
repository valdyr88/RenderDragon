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
#include "shader_resource_manager.h"

class IUniformBuffer;
class CTextureView;

class CShader : public CGraphicObject{
protected:
	SShaderDesc descriptor;

	GLuint id = 0;
	GLuint getId(){ return id; }

	IUniformBuffer* getUniformBuffer(uint bindPoint);

	std::string info_string;
	bool CheckCompileStatus();
	bool CompileShader();
public:

	CShader(GPUDevice* dev, const SShaderDesc& desc) :
		CGraphicObject(dev), descriptor(desc){
		CompileShader();
	}

	const auto& getDescriptor(){ return descriptor; }

	virtual void Release() override;
	virtual ~CShader() override{ Release(); }

	friend class CShaderProgram;
	friend class CPipelineState;
};

class CShaderProgram : public CGraphicObject{
protected:
	uint numStages;
	SharedPtr<CShader> shader[(uint)EShaderStage::NumShaderStages];
	std::vector<SharedPtr<CShaderResourceSetDesc>> resourceSetDescs;

	GLuint id = 0;

	bool MergeShaderResourceSetDescs();
	
	bool CheckLinkStatus();
	bool LinkProgram();
public:

	CShaderProgram(GPUDevice* dev, std::vector<SharedPtr<CShader>> shaders) :
		CGraphicObject(dev)
	{
		numStages = 0;

		for(auto it = shaders.begin(); it != shaders.end(); ++it){
			auto& sh = *it;
			if(sh->descriptor.stage < EShaderStage::NumShaderStages){
				shader[getStageNumber(sh->descriptor.stage)] = sh; ++numStages;
			}
		}

		MergeShaderResourceSetDescs();
		if(LinkProgram() == false){
			LOG_ERR("linking failed");
		}
	}

	bool setUniformBuffer(uint set, uint binding, IUniformBuffer* ub);
	bool setTexture(uint set, uint binding, CTextureView* tx);

	GLuint getId(){ return id; }

	uint getNofStages(){ return numStages; }
	IUniformBuffer* getUniformBuffer(EShaderStage stage, uint binding);

	virtual void Release() override;
	virtual ~CShaderProgram() override{ Release(); }

	friend class CPipelineState;
};

#endif //RD_API_OPENGL4
#endif //SHADER_H