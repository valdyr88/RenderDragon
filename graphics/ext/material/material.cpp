#include <algorithm>
#include "material.h"
#include "../../include.h"

#include "../../utils/rapidxml/rapidxml.hpp"
#include "../../utils/strings.h"

uint getMaterialTextureTypeFromString(const char* cstr){
	if(cstr == nullptr) return (uint)EMaterialTextureType::Albedo;

	std::string str = cstr;
	std::transform(str.begin(), str.end(), str.begin(),
				   [](unsigned char c){ return (c >= 'A' && c <= 'Z')? c+'a'-'A' : c; });

	if(str == "d") return (uint)EMaterialTextureType::Albedo;
	if(str == "bc") return (uint)EMaterialTextureType::Albedo;
	if(str == "albedo") return (uint)EMaterialTextureType::Albedo;
	if(str == "diffuse") return (uint)EMaterialTextureType::Albedo;
	if(str == "basecolor") return (uint)EMaterialTextureType::Albedo;
	if(str == "normal") return (uint)EMaterialTextureType::Normal;
	if(str == "n") return (uint)EMaterialTextureType::Normal;
	if(str == "roughness") return (uint)EMaterialTextureType::Roughness;
	if(str == "r") return (uint)EMaterialTextureType::Roughness;
	if(str == "specular") return (uint)EMaterialTextureType::Specular;
	if(str == "s") return (uint)EMaterialTextureType::Specular;
	if(str == "ambientocclusion") return (uint)EMaterialTextureType::AmbientOcclusion;
	if(str == "ao") return (uint)EMaterialTextureType::AmbientOcclusion;
	if(str == "metalness") return (uint)EMaterialTextureType::Metalness;
	if(str == "mt") return (uint)EMaterialTextureType::Metalness;
	if(str == "emissive") return (uint)EMaterialTextureType::Emissive;
	if(str == "em") return (uint)EMaterialTextureType::Emissive;
	if(str == "aors" || str == "srao") return EMaterialTextureType::AmbientOcclusion | EMaterialTextureType::Roughness | EMaterialTextureType::Specular;
	if(str == "aorsem" || str == "sremao" || str == "aorems" || str == "emraos" || str == "emrsao") return EMaterialTextureType::AmbientOcclusion | EMaterialTextureType::Roughness | EMaterialTextureType::Specular | EMaterialTextureType::Emissive;
	if(str == "aorsmt" || str == "srmtao" || str == "aormts" || str == "mtraos" || str == "mtrsao") return EMaterialTextureType::AmbientOcclusion | EMaterialTextureType::Roughness | EMaterialTextureType::Specular | EMaterialTextureType::Metalness;
	if(str == "srmt" || str == "mtrs") return EMaterialTextureType::Roughness | EMaterialTextureType::Specular | EMaterialTextureType::Metalness;
	if(str == "srem" || str == "emrs") return EMaterialTextureType::Roughness | EMaterialTextureType::Specular | EMaterialTextureType::Emissive;
	if(str == "ambient" || str == "amb" || str == "ambientmap") return (uint)EMaterialTextureType::AmbientMap;
	if(str == "height" || str == "h" || str == "depth") return (uint)EMaterialTextureType::Height;
	if(str == "opacity" || str == "a" || str == "alpha") return (uint)EMaterialTextureType::Opacity;

	return (uint)EMaterialTextureType::None;
}

//----------------------------------------------------------------------------------------------
//	CMaterial
//----------------------------------------------------------------------------------------------

SMaterialDesc CMaterial::CreateMaterialDescFromXML(void* xmlobject){
	rapidxml::xml_node<char>* node = (rapidxml::xml_node<char>*)xmlobject;
	SMaterialDesc desc;

	if(node == nullptr){
		LOG_ERR("root node is nullptr!"); return desc; }

	if(strisequal(node->name(), "material") == false)
		LOG_ERR("root node is not material!");

	if(node->first_attribute("id") != nullptr)
		desc.name = node->first_attribute("id")->value();
	else
		LOG_ERR("material doesn't have attribute: id");

	LOG("Parsing material: <%s>", desc.name.c_str());

	if(node->first_attribute("shader") != nullptr)
		desc.shader = node->first_attribute("shader")->value();
	else
		LOG_ERR("material doesn't have attribute: shader");

	if(node->first_node("paramgroup") != nullptr){
		for(auto pgnode = node->first_node("paramgroup"); pgnode != nullptr; pgnode = pgnode->next_sibling("paramgroup"))
		{
			if(pgnode->first_attribute("ubstruct") == nullptr)
				LOG_ERR("paramgroup doesn't have attribute: ubstruct");

			SMaterialParamsGroup paramgroup(pgnode->first_attribute("ubstruct")->value());
		
			for(auto paramnode = pgnode->first_node(); paramnode != nullptr; paramnode = paramnode->next_sibling())
			{
				const char* name = (paramnode->first_attribute("name") != nullptr)? paramnode->first_attribute("name")->value() : nullptr;
				const char* value = (paramnode->first_attribute("value") != nullptr)? paramnode->first_attribute("value")->value() : nullptr;
				const char* type = (paramnode->first_attribute("type") != nullptr)? paramnode->first_attribute("type")->value() : nullptr;

				if(name == nullptr)
					LOG_ERR("param doesn't have attribute: name");
				if(type == nullptr)
					LOG_ERR("param doesn't have attribute: type");

				EValueType vtype = toEValueType(type);
				EValueSize vsize = toEValueSize(type);

				SMaterialParam param(name, vtype, vsize);
			
				if(value != nullptr){
					if(vtype == EValueType::float32){
						switch(vsize)
						{
							case EValueSize::scalar:{
								float fvalue; str::strtonum<float, 1>(value, ',', &fvalue);
								param.setAs<float>(fvalue); }
								break;
							case EValueSize::vec2:{
								float fvalues[2]; str::strtonum<float, 2>(value, ',', fvalues);
								param.setAs<vec2>(vec2(fvalues[0], fvalues[1])); }
								break;
							case EValueSize::vec3:{
								float fvalues[3]; str::strtonum<float, 3>(value, ',', fvalues);
								param.setAs<vec3>(vec3(fvalues[0], fvalues[1], fvalues[2])); }
								break;
							case EValueSize::vec4:{
								float fvalues[4]; str::strtonum<float, 4>(value, ',', fvalues);
								param.setAs<vec4>(vec4(fvalues[0], fvalues[1], fvalues[2], fvalues[3])); }
								break;
							case EValueSize::mat2x2:{
								float fvalues[4]; str::strtonum<float, 4>(value, ',', fvalues);
								param.setAs<mat2x2>(mat2x2(fvalues[0], fvalues[1], fvalues[2], fvalues[3])); }
								break;
							case EValueSize::mat3x3:{
								float fvalues[9]; str::strtonum<float, 9>(value, ',', fvalues);
								param.setAs<mat3x3>(mat3x3(fvalues[0], fvalues[1], fvalues[2],  fvalues[3], fvalues[4], fvalues[5],  fvalues[6], fvalues[7], fvalues[8])); }
								break;
							case EValueSize::mat4x4:{
								float fvalues[16]; str::strtonum<float, 16>(value, ',', fvalues);
								param.setAs<mat4x4>(mat4x4(fvalues[0], fvalues[1], fvalues[2], fvalues[3], fvalues[4], fvalues[5], fvalues[6], fvalues[7], fvalues[8], fvalues[9], fvalues[10], fvalues[11], fvalues[12], fvalues[13], fvalues[14], fvalues[15])); }
								break;
							case EValueSize::mat3x2:{
								float fvalues[6]; str::strtonum<float, 6>(value, ',', fvalues);
								param.setAs<mat3x2>(mat3x2(fvalues[0], fvalues[1], fvalues[2],  fvalues[3], fvalues[4], fvalues[5])); }
								break;
							case EValueSize::mat2x3:{
								float fvalues[6]; str::strtonum<float, 6>(value, ',', fvalues);
								param.setAs<mat2x3>(mat2x3(fvalues[0], fvalues[1], fvalues[2],  fvalues[3], fvalues[4], fvalues[5])); }
								break;
							case EValueSize::mat4x3:{
								float fvalues[12]; str::strtonum<float, 12>(value, ',', fvalues);
								param.setAs<mat4x3>(mat4x3(fvalues[0], fvalues[1], fvalues[2],  fvalues[3], fvalues[4], fvalues[5],  fvalues[6], fvalues[7], fvalues[8],  fvalues[9], fvalues[10], fvalues[11])); }
								break;
							case EValueSize::mat3x4:{
								float fvalues[12]; str::strtonum<float, 12>(value, ',', fvalues);
								param.setAs<mat3x4>(mat3x4(fvalues[0], fvalues[1], fvalues[2],  fvalues[3], fvalues[4], fvalues[5],  fvalues[6], fvalues[7], fvalues[8],  fvalues[9], fvalues[10], fvalues[11])); }
								break;
							case EValueSize::mat4x2:{
								float fvalues[8]; str::strtonum<float, 8>(value, ',', fvalues);
								param.setAs<mat4x2>(mat4x2(fvalues[0], fvalues[1], fvalues[2],  fvalues[3], fvalues[4], fvalues[5],  fvalues[6], fvalues[7])); }
								break;
							case EValueSize::mat2x4:{
								float fvalues[8]; str::strtonum<float, 8>(value, ',', fvalues);
								param.setAs<mat2x4>(mat2x4(fvalues[0], fvalues[1], fvalues[2],  fvalues[3], fvalues[4], fvalues[5],  fvalues[6], fvalues[7])); }
								break;
							default:
								break;
						}
					}
					else if(vtype == EValueType::int32){
						switch(vsize)
						{
							case EValueSize::scalar:{
								int ivalue; str::strtonum<int, 1>(value, ',', &ivalue);
								param.setAs<int>(ivalue); }
								break;
							case EValueSize::vec2:{
								int ivalues[2]; str::strtonum<int, 2>(value, ',', ivalues);
								param.setAs<ivec2>(ivec2(ivalues[0], ivalues[1])); }
								break;
							case EValueSize::vec3:{
								int ivalues[3]; str::strtonum<int, 3>(value, ',', ivalues);
								param.setAs<ivec3>(ivec3(ivalues[0], ivalues[1], ivalues[2])); }
								break;
							case EValueSize::vec4:{
								int ivalues[4]; str::strtonum<int, 4>(value, ',', ivalues);
								param.setAs<ivec4>(ivec4(ivalues[0], ivalues[1], ivalues[2], ivalues[3])); }
								break;
							default:
								break;
						}
					}
				}
			
				paramgroup.params.emplace_back(param);
			}

			desc.paramGroups.emplace_back(paramgroup);
		}
	}

	if(node->first_node("texture") != nullptr){
		for(auto txnode = node->first_node("texture"); txnode != nullptr; txnode = txnode->next_sibling("texture"))
		{
			SMaterialTexture texture;

			const char* name = (txnode->first_attribute("name") != nullptr)? txnode->first_attribute("name")->value() : nullptr;
			const char* path = (txnode->first_attribute("path") != nullptr)? txnode->first_attribute("path")->value() : nullptr;
			const char* type = (txnode->first_attribute("type") != nullptr)? txnode->first_attribute("type")->value() : "D";
			
			if(name == nullptr)
				LOG_ERR("texture doesn't have attribute: name");
			if(path == nullptr)
				LOG_ERR("texture doesn't have attribute: path");

			texture.name = name;
			texture.path = path;
			texture.type |= getMaterialTextureTypeFromString(type);

			desc.textures.emplace_back(texture);
		}
	}
	
	if(desc.paramGroups.size() == 0 && desc.textures.size() == 0)
		LOG_WARN("material <%s> doesn't have any params", desc.name.c_str());

	LOG("Parsing material end");

	return desc;
}

SMaterialDesc CMaterial::CreateMaterialDescFromXML(std::string xml){
	rapidxml::xml_document<char>* doc = __rd_new rapidxml::xml_document<char>();
	char* cxml = __rd_new char[xml.length() + 1];
	//xml.copy(cxml, xml.length());
	#ifdef PLATFORM_WINDOWS
		strcpy_s(cxml, xml.length()+1, xml.c_str());
	#else
		strcpy(cxml, xml.c_str());
	#endif
	doc->parse<0>(cxml);
	auto m = doc->first_node("material");

	if(m == nullptr)
		LOG_ERR("root node is not material!");

	return CMaterial::CreateMaterialDescFromXML(m);
}

bool CMaterial::Create()
{
	return false;
}

bool CMaterial::CheckIfParamsGroupAndUBAreEqual(const std::vector<SMaterialParam>& mpg, const std::vector<SUniformMap>& ub){
	if(mpg.size() != ub.size()) return false;
	for(uint i = 0; i < ub.size(); ++i){
		if(mpg[i] != ub[i]) return false;
	}
	return true;
}
bool CMaterial::CheckIfParamsGroupAndUBAreEqual(const SMaterialParamsGroup& mpg, const IUniformBuffer& ub){
	return CMaterial::CheckIfParamsGroupAndUBAreEqual(mpg.params, ub.getUBStructDesc());
}

SharedPtr<CMaterialInstance> CMaterial::CreateInstance(GPUDevice* dev, std::vector<SharedPtr<IUniformBuffer>> ubs, std::vector<std::string> ubnames)
{
	auto mi = SharedPtr<CMaterialInstance>(__rd_new CMaterialInstance(this));
	this->materialInstances.add(mi);

	uint added_sharedubs = 0, added_names = 0;
	for(auto pg = this->descriptor.paramGroups.begin(); pg != this->descriptor.paramGroups.end(); ++pg)
	{
		if(pg->ubstruct == "-")//skipping ubstruct
			continue;

		if((ubs.size() > added_sharedubs) && (ubs[added_sharedubs] != nullptr) && (pg->ubstruct == ubs[added_sharedubs]->getUBStructTypeName())){
			if(CMaterial::CheckIfParamsGroupAndUBAreEqual(*pg, *ubs[added_sharedubs]) == false)
				LOG_ERR("ParamGroup and UniformBuffer structure mismatch for struct <%s>!, members of the ParamGroup and UniformBuffer are not equal!", pg->ubstruct.c_str());
			mi->uniformBuffers.add(ubs[added_sharedubs]);
			++added_sharedubs;
			continue;
		}
		else
		{
			std::string name = "";
			if((ubnames.size() > added_names)){
				name = ubnames[added_names];
				++added_names;
			}
			else
				LOG_WARN("Creating UniformBuffer type <%s> without name!", pg->ubstruct.c_str());

			if(IUniformBuffer::CreateUniformBufferType[pg->ubstruct] != nullptr){
				auto ub = IUniformBuffer::CreateUniformBufferType[pg->ubstruct](dev, name.c_str());
				if(CMaterial::CheckIfParamsGroupAndUBAreEqual(*pg, *ub) == false)
					LOG_ERR("ParamGroup and UniformBuffer structure mismatch for struct <%s>!, members of the ParamGroup and UniformBuffer are not equal!", pg->ubstruct.c_str());
				mi->uniformBuffers.add(ub);
			}
			else{
				LOG_ERR("unregistered UniformBuffer struct type <%s>!, register by calling rdRegisterUniformBufferStructure(%s)", pg->ubstruct.c_str(), pg->ubstruct.c_str());
			}
			continue;
		}
	}
	
	auto txManager = CSingleton<CTextureManager>::get();
	for(auto pt = this->descriptor.textures.begin(); pt != this->descriptor.textures.end(); ++pt)
	{
		auto tx = txManager->CreateTexture(pt->path);
		if(tx == nullptr){
			LOG_ERR("Can't find or create texture <%s>!", pt->path.c_str()); }

		mi->textures.add(tx->getViewSharedPtr());
	}

	return mi;
}

//----------------------------------------------------------------------------------------------
//	CMaterialInstance()
//----------------------------------------------------------------------------------------------

CMaterialInstance::~CMaterialInstance(){
	
}

//----------------------------------------------------------------------------------------------
//	CMaterialManager
//----------------------------------------------------------------------------------------------

SharedPtr<CMaterial> CMaterialManager::CreateMaterial(const SMaterialDesc& desc){
	for(uint i = 0; i < materials.size(); ++i){
		auto& material = *materials[i];
		if(material.descriptor == desc){
			return materials[i];
		}
	}
	SharedPtr<CMaterial> material = SharedPtr<CMaterial>(__rd_new CMaterial(desc));
	materials.add(material);
	return material;
}
//----------------------------------------------------------------------------------------------