#include "material.h"
#include "material.h"
#include "material.h"
#include "../../include.h"

#include "../../utils/rapidxml/rapidxml.hpp"
#include "../../utils/strings.h"

SMaterialDesc CMaterial::CreateMaterialDescFromXML(void* xmlobject){
	rapidxml::xml_node<char>* node = (rapidxml::xml_node<char>*)xmlobject;
	SMaterialDesc desc;

	if(strisequal(node->name(), "material") == false)
		LOG_ERR("node->name() != \"material\"");

	if(node->first_attribute("id") != nullptr)
		desc.name = node->first_attribute("id")->value();
	else
		LOG_ERR("node doesn't have attribute: id");

	if(node->first_node("paramgroup") != nullptr){
		auto pgnode = node->first_node("paramgroup");
		
		if(pgnode->first_attribute("ubstruct") == nullptr)
			LOG_ERR("node paramgroup doesn't have attribute: ubstruct");

		SMaterialParamsGroup paramgroup(pgnode->first_attribute("ubstruct")->value());
		
		for(auto paramnode = pgnode->first_node(); paramnode != nullptr; paramnode = paramnode->next_sibling())
		{
			char* name = (paramnode->first_attribute("name") != nullptr)? paramnode->first_attribute("name")->value() : nullptr;
			char* value = (paramnode->first_attribute("value") != nullptr)? paramnode->first_attribute("value")->value() : nullptr;
			char* type = (paramnode->first_attribute("type") != nullptr)? paramnode->first_attribute("type")->value() : nullptr;

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
					}
				}
			}
			
			paramgroup.params.emplace_back(param);
		}

		desc.paramGroups.emplace_back(paramgroup);
	}
	return desc;
}

SMaterialDesc CMaterial::CreateMaterialDescFromXML(std::string xml){
	rapidxml::xml_document<char>* doc = new rapidxml::xml_document<char>();
	char* cxml = __new char[xml.length() + 1];
	//xml.copy(cxml, xml.length());
	strcpy_s(cxml, xml.length()+1, xml.c_str());
	doc->parse<0>(cxml);
	auto m = doc->first_node("material");

	return CreateMaterialDescFromXML(m);
}

bool CMaterial::Create()
{
	return false;
}

SharedPtr<CMaterialInstance> CMaterial::CreateInstance(GPUDevice* dev, std::vector<SharedPtr<IUniformBuffer>> ubs, std::vector<std::string> ubnames)
{
	auto mi = SharedPtr<CMaterialInstance>(__new CMaterialInstance(this));
	this->materialInstances.add(mi);

	uint added_sharedubs = 0, added_names = 0;
	for(auto pg = this->descriptor.paramGroups.begin(); pg != this->descriptor.paramGroups.end(); ++pg)
	{
		if((ubs.size() > added_sharedubs) && (ubs[added_sharedubs] != nullptr) && (pg->ubstruct == ubs[added_sharedubs]->getUBStructTypeName())){
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
			if(IUniformBuffer::CreateUniformBufferType[pg->ubstruct] != nullptr)
				auto ub = IUniformBuffer::CreateUniformBufferType[pg->ubstruct](dev, name.c_str());
			else{
				LOG_ERR("unregistered UB struct type! : <%s>, register by calling rdRegisterUniformBufferStructure(%s)", pg->ubstruct.c_str(), pg->ubstruct.c_str());
			}
			continue;
		}
	}

	return mi;
}