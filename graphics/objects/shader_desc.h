#ifndef SHADER_DESC_H
#define SHADER_DESC_H

#include "../../utils/log.h"
#include "../../utils/pointers.h"
#include "../../utils/types/types.h"
#include "../../utils/types/vectypes.h"
#include "../enum/graphics_enums.h"
#include "graphic_object.h"

class Device;

struct ShaderResourceDesc{
	ShaderResourceType type;
	ShaderResourceUpdateType updateType = ShaderResourceUpdateType::Static;
	uint stages = 0x00000000; //bitflag of ShaderStage

	uint operator |= (ShaderStage& stage){ stages |= stage; return stages; }
	uint operator | (ShaderStage& stage){ return stages | stage; }
	uint operator |= (uint stage){ stages |= stage; return stages; }
	uint operator | (uint stage){ return stages | stage; }

	ShaderResourceDesc(const ShaderResourceDesc& other) :
		type(other.type), updateType(other.updateType), stages(other.stages){}
	ShaderResourceDesc(ShaderResourceType t, ShaderResourceUpdateType u, uint s) : 
		type(t), updateType(u), stages(s){}
};

class ShaderResource : public GraphicObject{
protected:
	ShaderResourceDesc resourceDescriptor;
public:
	ShaderResource(WeakPtr<Device>& dev, const ShaderResourceDesc& desc) : 
		GraphicObject(dev), resourceDescriptor(desc){}
};


struct SamplerDesc{
	TextureWrapping uWrapping = TextureWrapping::Wrap;
	TextureWrapping vWrapping = TextureWrapping::Wrap;
	TextureWrapping wWrapping = TextureWrapping::Wrap;
	TextureFiltering minFilter = TextureFiltering::Linear;
	TextureFiltering magFilter = TextureFiltering::Linear;
	TextureFiltering mipFilter = TextureFiltering::Linear;
};

class Sampler : public ShaderResource{
protected:
	SamplerDesc descriptor;
public:
	Sampler(WeakPtr<Device>& dev, const ShaderResourceDesc& sr, const SamplerDesc& desc) :
		ShaderResource(dev, sr), descriptor(desc){}
};


class Texture;
class TextureView : public ShaderResource{
protected:
	SharedPtr<Texture> texture;
	SharedPtr<Sampler> sampler;
public:
	TextureView(WeakPtr<Device>& dev, const ShaderResourceDesc& sr, SharedPtr<Texture>& tx) :
		ShaderResource(dev, sr), texture(tx), sampler(nullptr){}

	TextureView(WeakPtr<Device>& dev, const ShaderResourceDesc& sr, SharedPtr<Texture>& tx, SharedPtr<Sampler>& s) :
		ShaderResource(dev, sr), texture(tx), sampler(s){}
};

class IUniformBuffer : public ShaderResource{
protected:
public:
	IUniformBuffer(WeakPtr<Device>& dev, const ShaderResourceDesc& sr) : ShaderResource(dev, sr){}

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
	
	virtual void upload() = 0;
	virtual bool isShared() = 0;
};

#endif //SHADER_DESC_H