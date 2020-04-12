#ifndef SHADER_DESC_H
#define SHADER_DESC_H

#include <list>
#include <vector>
#include "../utils/log.h"
#include "../utils/pointers.h"
#include "../utils/types/types.h"
#include "../utils/types/vectypes.h"
#include "../utils/strings.h"
#include "graphics_enums.h"
#include "graphic_object.h"
#include "vertex_buffer_desc.h"

class GPUDevice;
//-----------------------------------------------------------------------------------

struct SShaderResourceBindingDesc{
	uint32 setNumber = 0;
	uint32 bindPoint = 0;
	std::string name = "";

	EShaderResourceType type;
	EShaderResourceUsageType usage = EShaderResourceUsageType::Static;
	uint32 shaderStages = 0x00000000; //bitflag of EShaderStage
	uint32 count = 0;

	uint operator |= (EShaderStage& stage){ shaderStages |= stage; return shaderStages; }
	uint operator | (EShaderStage& stage){ return shaderStages | stage; }
	uint operator |= (uint stage){ shaderStages |= stage; return shaderStages; }
	uint operator | (uint stage){ return shaderStages | stage; }

	SShaderResourceBindingDesc(uint32 setNum, uint32 bindPt, std::string nam,
							   EShaderResourceType t, uint s, EShaderResourceUsageType ut = EShaderResourceUsageType::Static, uint32 cnt = 1) :
		setNumber(setNum), bindPoint(bindPt), name(nam),
		type(t), shaderStages(s), usage(ut), count(cnt) {}
	SShaderResourceBindingDesc(const SShaderResourceBindingDesc& other) :
		type(other.type), usage(other.usage), shaderStages(other.shaderStages),
		count(other.count), name(other.name), bindPoint(other.bindPoint), setNumber(other.setNumber){}

	//Note: this operator doesn't check setNumber.
	bool operator == (const SShaderResourceBindingDesc& other) const{
		return type == other.type &&
			usage == other.usage &&
			shaderStages == other.shaderStages &&
			count == other.count &&
			//setNumber == other.setNumber && //this is a check without set number.
			bindPoint == other.bindPoint &&
			name == other.name;
	}
	//Note: this operator doesn't check setNumber.
	bool operator != (const SShaderResourceBindingDesc& other) const{ return !(*this == other); }

	SShaderResourceBindingDesc& operator = (const SShaderResourceBindingDesc& other){
		type = other.type;
		usage = other.usage;
		shaderStages = other.shaderStages;
		count = other.count;
		setNumber = other.setNumber;
		bindPoint = other.bindPoint;
		name = other.name;
		return *this;
	}
};

//-----------------------------------------------------------------------------------

struct SShaderDesc{
	EShaderStage stage = EShaderStage::FragmentShader;
	std::string name = "";
	std::string source = "";
	std::list<std::vector<SShaderResourceBindingDesc>> bindingSetsDesc;

	SVertexFormat vertexFormat;

	SShaderDesc() = default;

	SShaderDesc(EShaderStage s, std::string n, std::string src, std::list<std::vector<SShaderResourceBindingDesc>> binds)
		: stage(s), name(n), source(src), bindingSetsDesc(binds){
		VerifyBindingDesc();
	}

protected:
	bool VerifyBindingDesc();
};

//-----------------------------------------------------------------------------------

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
// shader resources
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

//-----------------------------------------------------------------------------------

#endif //SHADER_DESC_H