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
	STextureDesc description;
	UniquePtr<CTextureView> view;
public:
	CTexture(WeakPtr<GPUDevice>& dev, const STextureDesc& desc, const SRawData& data) :
		CGraphicObject(dev), description(desc){
	}

	virtual ~CTexture() = default;
};

#endif //RD_API_OPENGL4
#endif //TEXTURE_H