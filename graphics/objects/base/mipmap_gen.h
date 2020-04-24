#ifndef MIPMAP_GEN_H
#define MIPMAP_GEN_H

#ifdef RD_API_BASE

#include "device.h"

//-----------------------------------------------------------------------------------

class CMipMapGen : public CGraphicObject{
protected:
	SharedPtr<CPipelineState> pipeline;
	SharedPtr<CRenderPass> renderPass;

	bool Create(const SPipelineStateDesc& pipdesc, const SRenderPassDesc& rpdesc);
public:

	CMipMapGen(GPUDevice* dev, const SPipelineStateDesc& pipdesc, const SRenderPassDesc& rpdesc)
		: CGraphicObject(dev){
		Create(pipdesc, rpdesc);
	}

	bool Generate(CTexture* in);
	static SPipelineStateDesc CreateDefaultPipelineStateDesc(SharedPtr<CShaderProgram> program, SharedPtr<CRenderPass> renderPass);
};

//-----------------------------------------------------------------------------------
#endif //RD_API_BASE
#endif //MIPMAP_GEN_H