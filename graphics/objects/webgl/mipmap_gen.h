#ifndef MIPMAP_GEN_H
#define MIPMAP_GEN_H

#ifdef RD_API_WEBGL

#include "device.h"

//-----------------------------------------------------------------------------------

class CMipMapGen : public CGraphicObject{
public:	
	struct UBLevelData{
		vec2 resolution;
		vec2 invResolution;
		int level;

		static std::vector<SUniformMap> desc;
	};

protected:
	SharedPtr<CPipelineState> pipeline;
	SharedPtr<CRenderPass> renderPass;
	SharedPtr<CUniformBuffer<UBLevelData>> dataUB;
	static SharedPtr<CVertexBuffer> quadVB;
	static SharedPtr<CIndexBuffer> quadIB;

	std::vector<SharedPtr<CFramebuffer>> framebuffers;

	bool Create(const SPipelineStateDesc& pipdesc, const SRenderPassDesc& rpdesc);
public:

	CMipMapGen(GPUDevice* dev, const SPipelineStateDesc& pipdesc, const SRenderPassDesc& rpdesc)
		: CGraphicObject(dev){
		Create(pipdesc, rpdesc);
	}
	CMipMapGen(GPUDevice* dev, SharedPtr<CShaderProgram> program, STextureFormatDesc format)
		: CGraphicObject(dev){
		Create(CMipMapGen::CreateDefaultPipelineStateDesc(program), CMipMapGen::CreateDefaultRenderPassDesc(format));
	}

	bool Generate(SharedPtr<CTexture> tx, CUniformBuffer<UBLevelData>* ub = nullptr);

	void ClearTextureReference(){ framebuffers.clear(); }

	static SPipelineStateDesc CreateDefaultPipelineStateDesc(SharedPtr<CShaderProgram> program);
	static SRenderPassDesc CreateDefaultRenderPassDesc(STextureFormatDesc format);
	static SVertexFormat getVertexFormat(){ if(quadVB != nullptr) return quadVB->getVertexFormat(); else return SVertexFormat(); }
};

//-----------------------------------------------------------------------------------
#endif //RD_API_OPENGL4
#endif //MIPMAP_GEN_H