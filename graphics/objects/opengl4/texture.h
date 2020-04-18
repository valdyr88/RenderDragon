#ifndef TEXTURE_H
#define TEXTURE_H

#ifdef RD_API_OPENGL4

#include "glinclude.h"
#include "../../utils/log.h"
#include "../../utils/types/types.h"
#include "../../descriptors/graphics_enums.h"
#include "../../descriptors/texture_desc.h"
#include "../../descriptors/graphic_object.h"
#include "../../descriptors/shader_desc.h"

class GPUDevice;

//-----------------------------------------------------------------------------------

class CSampler : public CShaderResource{
protected:
	SSamplerDesc descriptor;

	CSampler() = delete;
public:
	CSampler(GPUDevice* dev, const SSamplerDesc& desc) :
		CShaderResource(dev, EShaderResourceType::Sampler), descriptor(desc){}

	const auto& getDescriptor() const{ return descriptor; }
};

class CTexture;
class CTextureView : public CShaderResource{
protected:
	SharedPtr<CTexture> texture;
	SharedPtr<CSampler> sampler;

	uint set = 0;
	uint binding = 0;

	CTextureView() = delete;
public:
	CTextureView(GPUDevice* dev, SharedPtr<CTexture>& tx) :
		CShaderResource(dev, EShaderResourceType::Texture), texture(tx), sampler(nullptr){}

	CTextureView(GPUDevice* dev, SharedPtr<CTexture>& tx, SharedPtr<CSampler>& s) :
		CShaderResource(dev, EShaderResourceType::CombinedTexSampler), texture(tx), sampler(s){}

	bool Bind(uint set, uint binding);

	friend class CShaderProgram;
};

//-----------------------------------------------------------------------------------

class CTexture : public CGraphicObject{
protected:
	STextureDesc descriptor;
	UniquePtr<CTextureView> view;
	
	GLuint id = 0;
	GLuint getId(){ return id; }

	bool Create(const STextureRawData& data);
	bool Create(std::string& fileName);
	virtual void Release() override;

	SSamplerDesc sampler;
	bool ApplySampler(const SSamplerDesc& desc);
	bool ApplySampler(const CSampler* s);
public:
	CTexture(GPUDevice* dev, const STextureDesc& desc, const STextureRawData& data) :
		CGraphicObject(dev), descriptor(desc){
		this->Create(data);
	}
	CTexture(GPUDevice* dev, const STextureDesc& desc, std::string fileName) :
		CGraphicObject(dev), descriptor(desc){
		Create(fileName);
	}

	const auto& getDescriptor(){ return descriptor; }
	bool isTextureCubeMap(){ return descriptor.type == ETextureType::TextureCube; }
	bool isTexture3D(){ return descriptor.type == ETextureType::Texture3D; }
	bool isTexture2D(){ return descriptor.type == ETextureType::Texture2D; }
	bool isTexture1D(){ return descriptor.type == ETextureType::Texture1D; }

	virtual ~CTexture() = default;

	friend class CFramebuffer;
	friend class CTextureView;
	friend class CSampler;
};

#endif //RD_API_OPENGL4
#endif //TEXTURE_H