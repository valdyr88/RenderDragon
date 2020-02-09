#ifndef SHADER_DESC_H
#define SHADER_DESC_H

#include <list>
#include "../utils/log.h"
#include "../utils/pointers.h"
#include "../utils/types/types.h"
#include "../utils/types/vectypes.h"
#include "graphics_enums.h"
#include "graphic_object.h"

class GPUDevice;

struct SShaderResourceDesc{
	EShaderResourceType type;
	EShaderResourceUpdateType updateType = EShaderResourceUpdateType::Static;
	uint stages = 0x00000000; //bitflag of EShaderStage

	uint operator |= (EShaderStage& stage){ stages |= stage; return stages; }
	uint operator | (EShaderStage& stage){ return stages | stage; }
	uint operator |= (uint stage){ stages |= stage; return stages; }
	uint operator | (uint stage){ return stages | stage; }

	SShaderResourceDesc(EShaderResourceType t, EShaderResourceUpdateType ut = EShaderResourceUpdateType::Static, uint s = 0x00000000) :
		type(t), updateType(ut), stages(s) {}
	SShaderResourceDesc(const SShaderResourceDesc& other) :
		type(other.type), updateType(other.updateType), stages(other.stages){}

	bool operator == (const SShaderResourceDesc& other){
		return type == other.type &&
			updateType == other.updateType &&
			stages == other.stages;
	}
	bool operator != (const SShaderResourceDesc& other){ return !(*this == other); }

	SShaderResourceDesc& operator = (const SShaderResourceDesc& other){
		type = other.type;
		updateType = other.updateType;
		stages = other.stages;
		return *this;
	}
};

struct SSamplerDesc{
	ETextureWrapping uWrapping = ETextureWrapping::Wrap;
	ETextureWrapping vWrapping = ETextureWrapping::Wrap;
	ETextureWrapping wWrapping = ETextureWrapping::Wrap;
	ETextureFiltering minFilter = ETextureFiltering::Linear;
	ETextureFiltering magFilter = ETextureFiltering::Linear;
	ETextureFiltering mipFilter = ETextureFiltering::Linear;

	bool operator == (const SSamplerDesc& other){
		return uWrapping == other.uWrapping &&
			vWrapping == other.wWrapping &&
			wWrapping == other.wWrapping &&
			minFilter == other.minFilter &&
			magFilter == other.magFilter &&
			mipFilter == other.mipFilter;
	}
	bool operator != (const SSamplerDesc& other){ return !(*this == other); }

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

class CShaderResource : public CGraphicObject{
protected:
	SShaderResourceDesc resourceDescriptor;
public:
	CShaderResource(GPUDevice* dev, const SShaderResourceDesc& desc) : 
		CGraphicObject(dev), resourceDescriptor(desc){}
	const SShaderResourceDesc getDescriptor(){ return resourceDescriptor; }
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
public:
	CSampler(GPUDevice* dev, const SShaderResourceDesc& sr, const SSamplerDesc& desc) :
		CShaderResource(dev, sr), descriptor(desc){}
};


class CTexture;
class CTextureView : public CShaderResource{
protected:
	SharedPtr<CTexture> texture;
	SharedPtr<CSampler> sampler;
public:
	CTextureView(GPUDevice* dev, const SShaderResourceDesc& sr, SharedPtr<CTexture>& tx) :
		CShaderResource(dev, sr), texture(tx), sampler(nullptr){}

	CTextureView(GPUDevice* dev, const SShaderResourceDesc& sr, SharedPtr<CTexture>& tx, SharedPtr<CSampler>& s) :
		CShaderResource(dev, sr), texture(tx), sampler(s){}
};

class IUniformBuffer : public CShaderResource{
protected:
public:
	IUniformBuffer(GPUDevice* dev, const SShaderResourceDesc& sr) : CShaderResource(dev, sr){}

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