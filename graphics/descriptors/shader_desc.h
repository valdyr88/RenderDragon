#ifndef SHADER_DESC_H
#define SHADER_DESC_H

#include <list>
#include "../utils/log.h"
#include "../utils/pointers.h"
#include "../utils/types/types.h"
#include "../utils/types/vectypes.h"
#include "../utils/strings.h"
#include "graphics_enums.h"
#include "graphic_object.h"

class GPUDevice;

struct SSamplerDesc{
	ETextureWrapping uWrapping = ETextureWrapping::Wrap;
	ETextureWrapping vWrapping = ETextureWrapping::Wrap;
	ETextureWrapping wWrapping = ETextureWrapping::Wrap;
	ETextureFiltering minFilter = ETextureFiltering::Linear;
	ETextureFiltering magFilter = ETextureFiltering::Linear;
	ETextureFiltering mipFilter = ETextureFiltering::Linear;

	bool operator == (const SSamplerDesc& other) const{
		return uWrapping == other.uWrapping &&
			vWrapping == other.wWrapping &&
			wWrapping == other.wWrapping &&
			minFilter == other.minFilter &&
			magFilter == other.magFilter &&
			mipFilter == other.mipFilter;
	}
	bool operator != (const SSamplerDesc& other) const{ return !(*this == other); }

	SSamplerDesc& operator = (const SSamplerDesc& other){
		uWrapping = other.uWrapping;
		vWrapping = other.vWrapping;
		wWrapping = other.wWrapping;
		minFilter = other.minFilter;
		magFilter = other.magFilter;
		mipFilter = other.mipFilter;
		return *this;
	}
};

//-----------------------------------------------------------------------------------

class CShaderResource : public CGraphicObject{
protected:
	EShaderResourceType type;

	CShaderResource() = delete;
public:
	CShaderResource(GPUDevice* dev, const EShaderResourceType& t) :
		CGraphicObject(dev), type(t){}
	const EShaderResourceType getResourceType(){ return type; }
};


struct SShaderDesc{
	EShaderStage stage = EShaderStage::FragmentShader;
	std::string source = "";
	std::list<std::pair<uint, SharedPtr<CShaderResource>>> resources;
};

//-----------------------------------------------------------------------------------
// actual resources (api abstracted)
//-----------------------------------------------------------------------------------

class CSampler : public CShaderResource{
protected:
	SSamplerDesc descriptor;

	CSampler() = delete;
public:
	CSampler(GPUDevice* dev, const SSamplerDesc& desc) :
		CShaderResource(dev, EShaderResourceType::Sampler), descriptor(desc){}

	const auto& getDescriptor(){ return descriptor; }
};


class CTexture;
class CTextureView : public CShaderResource{
protected:
	SharedPtr<CTexture> texture;
	SharedPtr<CSampler> sampler;

	CTextureView() = delete;
public:
	CTextureView(GPUDevice* dev, SharedPtr<CTexture>& tx) :
		CShaderResource(dev, EShaderResourceType::Texture), texture(tx), sampler(nullptr){}

	CTextureView(GPUDevice* dev, SharedPtr<CTexture>& tx, SharedPtr<CSampler>& s) :
		CShaderResource(dev, EShaderResourceType::CombinedTexSampler), texture(tx), sampler(s){}
};

class IUniformBuffer : public CShaderResource{
protected:

	IUniformBuffer() = delete;
public:
	IUniformBuffer(GPUDevice* dev) : CShaderResource(dev, EShaderResourceType::UniformBuffer){}

	virtual bool setUniform(const char* name, float value){ LOG_ERR("not implemented!"); return false; }
	virtual bool setUniform(const char* name, vec2 value){ LOG_ERR("not implemented!"); return false; }
	virtual bool setUniform(const char* name, vec3 value){ LOG_ERR("not implemented!"); return false; }
	virtual bool setUniform(const char* name, vec4 value){ LOG_ERR("not implemented!"); return false; }

	virtual bool setUniform(const char* name, mat2 value){ LOG_ERR("not implemented!"); return false; }
	virtual bool setUniform(const char* name, mat3 value){ LOG_ERR("not implemented!"); return false; }
	virtual bool setUniform(const char* name, mat4 value){ LOG_ERR("not implemented!"); return false; }

	virtual bool setUniform(const char* name, mat2x3 value){ LOG_ERR("not implemented!"); return false; }
	virtual bool setUniform(const char* name, mat3x2 value){ LOG_ERR("not implemented!"); return false; }
	virtual bool setUniform(const char* name, mat4x3 value){ LOG_ERR("not implemented!"); return false; }
	virtual bool setUniform(const char* name, mat3x4 value){ LOG_ERR("not implemented!"); return false; }
	virtual bool setUniform(const char* name, mat4x2 value){ LOG_ERR("not implemented!"); return false; }
	virtual bool setUniform(const char* name, mat2x4 value){ LOG_ERR("not implemented!"); return false; }

	virtual bool setUniform(const char* name, int value){ LOG_ERR("not implemented!"); return false; }
	virtual bool setUniform(const char* name, ivec2 value){ LOG_ERR("not implemented!"); return false; }
	virtual bool setUniform(const char* name, ivec3 value){ LOG_ERR("not implemented!"); return false; }
	virtual bool setUniform(const char* name, ivec4 value){ LOG_ERR("not implemented!"); return false; }
	
	virtual bool setUniform(const char* name, uint count, float* value){ LOG_ERR("not implemented!"); return false; }
	virtual bool setUniform(const char* name, uint count, vec2* value){ LOG_ERR("not implemented!"); return false; }
	virtual bool setUniform(const char* name, uint count, vec3* value){ LOG_ERR("not implemented!"); return false; }
	virtual bool setUniform(const char* name, uint count, vec4* value){ LOG_ERR("not implemented!"); return false; }

	virtual bool setUniform(const char* name, uint count, mat2* value){ LOG_ERR("not implemented!"); return false; }
	virtual bool setUniform(const char* name, uint count, mat3* value){ LOG_ERR("not implemented!"); return false; }
	virtual bool setUniform(const char* name, uint count, mat4* value){ LOG_ERR("not implemented!"); return false; }

	virtual bool setUniform(const char* name, uint count, mat2x3* value){ LOG_ERR("not implemented!"); return false; }
	virtual bool setUniform(const char* name, uint count, mat3x2* value){ LOG_ERR("not implemented!"); return false; }
	virtual bool setUniform(const char* name, uint count, mat4x3* value){ LOG_ERR("not implemented!"); return false; }
	virtual bool setUniform(const char* name, uint count, mat3x4* value){ LOG_ERR("not implemented!"); return false; }
	virtual bool setUniform(const char* name, uint count, mat4x2* value){ LOG_ERR("not implemented!"); return false; }
	virtual bool setUniform(const char* name, uint count, mat2x4* value){ LOG_ERR("not implemented!"); return false; }

	virtual bool setUniform(const char* name, uint count, int* value){ LOG_ERR("not implemented!"); return false; }
	virtual bool setUniform(const char* name, uint count, ivec2* value){ LOG_ERR("not implemented!"); return false; }
	virtual bool setUniform(const char* name, uint count, ivec3* value){ LOG_ERR("not implemented!"); return false; }
	virtual bool setUniform(const char* name, uint count, ivec4* value){ LOG_ERR("not implemented!"); return false; }
	
	virtual void Upload() = 0;
	virtual bool isShared() = 0;
};

#endif //SHADER_DESC_H