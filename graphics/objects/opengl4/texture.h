#ifndef TEXTURE_H
#define TEXTURE_H

#ifdef RD_API_OPENGL4

#include "glinclude.h"
#include "../../utils/log.h"
#include "../../utils/types/types.h"
#include "../../utils/containers.h"
#include "../../utils/singleton.h"
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
	bool ApplySampler(GLenum target);
	static bool ApplySampler(GPUDevice* device, GLenum target, const SSamplerDesc& desc);
public:
	CSampler(GPUDevice* dev, const SSamplerDesc& desc) :
		CShaderResource(dev, EShaderResourceType::Sampler), descriptor(desc){}

	const auto& getDescriptor() const{ return descriptor; }

	friend class CTextureView;
	friend class CTexture;
};

class CTexture;
class CTextureView : public CShaderResource{
protected:
	STextureViewDesc descriptor;
	SharedPtr<CTexture> texture;
	SharedPtr<CSampler> sampler;

	GLuint id = 0;
	GLuint getId(){ return id; }

	uint set = 0;
	uint binding = 0;

	bool Create();
	CTextureView() = delete;

	virtual void Release() override;
public:
	CTextureView(GPUDevice* dev, const STextureViewDesc& desc, SharedPtr<CTexture>& tx) :
		CShaderResource(dev, EShaderResourceType::Texture), descriptor(desc), texture(tx), sampler(nullptr){
		Create();
	}

	CTextureView(GPUDevice* dev, const STextureViewDesc& desc, SharedPtr<CTexture>& tx, SharedPtr<CSampler>& s) :
		CShaderResource(dev, EShaderResourceType::CombinedTexSampler), descriptor(desc), texture(tx), sampler(s){
		Create();
	}

	const auto& getDescriptor(){ return descriptor; }
	CTexture* getActualTexture(){ return texture.get(); }

	bool Bind(uint set, uint binding);

	virtual ~CTextureView() override{ Release(); }

	friend class CShaderProgram;
	friend class CSampler;
	friend class CTexture;
};

//-----------------------------------------------------------------------------------

class CTexture : public CGraphicObject{
protected:
	STextureDesc descriptor;
	SharedPtr<CTextureView> view;
	
	GLuint id = 0;
	GLuint getId(){ return id; }

	bool Create(const STextureRawData& data);
	bool Create(std::string& fileName);
	bool CreateView(SharedPtr<CTexture> tx);
	bool CreateView(CTexture* tx);

	SSamplerDesc sampler;
	bool ApplySampler(const SSamplerDesc& desc);
	bool ApplySampler(const CSampler* s);

	virtual void Release() override;
public:
	CTexture(GPUDevice* dev, const STextureDesc& desc, const STextureRawData& data) :
		CGraphicObject(dev), descriptor(desc){
		this->Create(data);
	}
	CTexture(GPUDevice* dev, const STextureDesc& desc, std::string fileName) :
		CGraphicObject(dev), descriptor(desc){
		this->Create(fileName);
	}

	bool UpdateLevelData(const STextureSliceRawData& data);
	bool AllocateMipmaps();

	CTextureView* getView(){ return view.get(); }
	SharedPtr<CTextureView> getViewSharedPtr(){ return view; }

	const auto& getDescriptor(){ return descriptor; }
	bool isTextureCubeMap(){ return descriptor.type == ETextureType::TextureCube; }
	bool isTexture3D(){ return descriptor.type == ETextureType::Texture3D; }
	bool isTexture2D(){ return descriptor.type == ETextureType::Texture2D; }
	bool isTexture1D(){ return descriptor.type == ETextureType::Texture1D; }

	virtual ~CTexture() override{ Release(); }

	friend class GPUDevice;
	friend class CFramebuffer;
	friend class CTextureView;
	friend class CSampler;
	friend class CTextureManager;
};

//-----------------------------------------------------------------------------------
class GPUDevice;

class CTextureManager{
protected:
	GPUDevice* device = nullptr;

	CTextureManager(){}
	CTextureManager(GPUDevice* dev) : device(dev){}
	void setDevice(GPUDevice* dev){ device = dev; }
public:

	SharedPtr<CTexture> FindByName(std::string name);
	SharedPtr<CTexture> CreateTexture(std::string name, const STextureDesc* desc = nullptr);

	friend class GPUDevice;
	friend class CSingleton<CTextureManager>;
	friend bool rdSetupDeviceForGlobalObjects(GPUDevice* device);
};

#endif //RD_API_OPENGL4
#endif //TEXTURE_H