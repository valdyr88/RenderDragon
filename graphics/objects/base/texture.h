#ifndef TEXTURE_H
#define TEXTURE_H

#ifdef RD_API_BASE

#include "../../utils/log.h"
#include "../../utils/types/types.h"
#include "../../descriptors/graphics_enums.h"
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
	bool isTextureCubeMap(){ return descriptor.type == ETextureType::TextureCube; }
	bool isTexture3D(){ return descriptor.type == ETextureType::Texture3D; }
	bool isTexture2D(){ return descriptor.type == ETextureType::Texture2D; }
	bool isTexture1D(){ return descriptor.type == ETextureType::Texture1D; }

	virtual ~CTexture() = default;

	friend class CFramebuffer;
};

#endif //RD_API_BASE
#endif //TEXTURE_H