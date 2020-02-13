#ifndef TEXTURE_H
#define TEXTURE_H

#ifdef RD_API_OPENGL4

#include "../../utils/log.h"
#include "../../utils/pointers.h"
#include "../../descriptors/texture_desc.h"
#include "../../descriptors/graphic_object.h"
#include "../../descriptors/shader_desc.h"

class CTexture : public CGraphicObject{
protected:
	STextureDesc descriptor;
	UniquePtr<CTextureView> view;
public:
	CTexture(GPUDevice* dev, const STextureDesc& desc, const SRawData& data) :
		CGraphicObject(dev), descriptor(desc){
	}

	const auto& getDescriptor(){ return descriptor; }

	virtual ~CTexture() = default;
};

#endif //RD_API_OPENGL4
#endif //TEXTURE_H