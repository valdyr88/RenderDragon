#include "material.h"
#include "../../include.h"

#include "../../utils/rapidxml/rapidxml.hpp"
#include <string.h>
/*
namespace xml{

class CXMLbase{
protected:
	const char* pName = nullptr;
	const char* pValue = nullptr;
	CXMLbase* pParent = nullptr;

	CXMLbase(const char* n, const char* v, CXMLbase* p) : pName(n), pValue(v), pParent(p){}
public:
	const char* name(){ return pName; }
	const char* value(){ return pValue; }
	CXMLbase* parent(){ return pParent; }
};

class CXMLnode : public CXMLbase{
	CXMLnode* pNextNode = nullptr;
	CXMLnode* pPrevNode = nullptr;
	std::vector<CXMLattribute*> attributes;
public:
	CXMLnode(const char* n, const char* v, CXMLbase* p) : CXMLbase(n, v, p){}
	CXMLnode* nextSibling(){ return pNextNode; }
	CXMLnode* prevSibling(){ return pPrevNode; }
};

class CXMLattribute : public CXMLbase{
	CXMLattribute* pNextAttribute = nullptr;
	CXMLattribute* pPrevAttribute = nullptr;
public:
	CXMLattribute(const char* n, const char* v, CXMLbase* p) : CXMLbase(n, v, p){}
	CXMLattribute* nextSibling(){ return pNextAttribute; }
	CXMLattribute* prevSibling(){ return pPrevAttribute; }
};

class CXMLdoc : public CXMLnode{
	CXMLnode* node = nullptr;
public:
	CXMLdoc(rapidxml::xml_document<char>& doc);
};

void recursiveAdd(CXMLbase* parent, rapidxml::xml_base<char>* node){

}
void recursiveAdd(CXMLbase* parent, rapidxml::xml_node<char>* node){

}
void recursiveAdd(CXMLbase* parent, rapidxml::xml_attribute<char>* att){

}
void recursiveAdd(CXMLbase* parent, rapidxml::xml_document<char>* doc){
	for(auto n = doc->first_node(); n != nullptr; n = n->next_sibling()){
		CXMLnode* node = new CXMLnode(n->name(), n->value(), parent);
		
	}
}

CXMLdoc::CXMLdoc(rapidxml::xml_document<char>& doc) : CXMLbase(doc.name(), doc.value(), nullptr)
{
	recursiveAdd(this, &doc);
}

}
*/
template<typename type> type strtonum(char* str){ return type(0); }
template<> inline float strtonum(char* str){ return std::stof(str); }
template<> inline int strtonum(char* str){ return std::stoi(str); }

template<typename type, const int C> void strtonum(char* str, type* outs){
	for(uint i = 0; i < C && str != nullptr; ++i){
		char* str2 = str;
		
		for(str2; *str2 != 0 && *str2 != ','; ++str2);
		if(*str2 == ','){ *str2 = 0; ++str2; }
		else if(*str2 == 0) str2 = nullptr;

		outs[i] = strtonum<type>(str);
		if(str2 != nullptr){ size_t l = strlen(str); str[l] = ','; }

		str = str2;
	}
}

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
			char* name = paramnode->first_attribute("name")->value();
			char* value = paramnode->first_attribute("value")->value();
			char* type = paramnode->first_attribute("type")->value();

			EValueType vtype = toEValueType(type);
			EValueSize vsize = toEValueSize(type);

			SMaterialParam param(name, vtype, vsize);
			
			if(value != nullptr){
				if(vtype == EValueType::float32){
					switch(vsize)
					{
						case EValueSize::scalar:{
							float fvalue; strtonum<float, 1>(value, &fvalue);
							param.setAs<float>(fvalue); }
							break;
						case EValueSize::vec2:{
							float fvalues[2]; strtonum<float, 2>(value, fvalues);
							param.setAs<vec2>(vec2(fvalues[0], fvalues[1])); }
							break;
						case EValueSize::vec3:{
							float fvalues[3]; strtonum<float, 3>(value, fvalues);
							param.setAs<vec3>(vec3(fvalues[0], fvalues[1], fvalues[2])); }
							break;
						case EValueSize::vec4:{
							float fvalues[4]; strtonum<float, 4>(value, fvalues);
							param.setAs<vec4>(vec4(fvalues[0], fvalues[1], fvalues[2], fvalues[3])); }
							break;
						case EValueSize::mat2x2:{
							float fvalues[4]; strtonum<float, 4>(value, fvalues);
							param.setAs<mat2x2>(mat2x2(fvalues[0], fvalues[1], fvalues[2], fvalues[3])); }
							break;
						case EValueSize::mat3x3:{
							float fvalues[9]; strtonum<float, 9>(value, fvalues);
							param.setAs<mat3x3>(mat3x3(fvalues[0], fvalues[1], fvalues[2],  fvalues[3], fvalues[4], fvalues[5],  fvalues[6], fvalues[7], fvalues[8])); }
							break;
						case EValueSize::mat4x4:{
							float fvalues[16]; strtonum<float, 16>(value, fvalues);
							param.setAs<mat4x4>(mat4x4(fvalues[0], fvalues[1], fvalues[2], fvalues[3], fvalues[4], fvalues[5], fvalues[6], fvalues[7], fvalues[8], fvalues[9], fvalues[10], fvalues[11], fvalues[12], fvalues[13], fvalues[14], fvalues[15])); }
							break;
						case EValueSize::mat3x2:{
							float fvalues[6]; strtonum<float, 6>(value, fvalues);
							param.setAs<mat3x2>(mat3x2(fvalues[0], fvalues[1], fvalues[2],  fvalues[3], fvalues[4], fvalues[5])); }
							break;
						case EValueSize::mat2x3:{
							float fvalues[6]; strtonum<float, 6>(value, fvalues);
							param.setAs<mat2x3>(mat2x3(fvalues[0], fvalues[1], fvalues[2],  fvalues[3], fvalues[4], fvalues[5])); }
							break;
						case EValueSize::mat4x3:{
							float fvalues[12]; strtonum<float, 12>(value, fvalues);
							param.setAs<mat4x3>(mat4x3(fvalues[0], fvalues[1], fvalues[2],  fvalues[3], fvalues[4], fvalues[5],  fvalues[6], fvalues[7], fvalues[8],  fvalues[9], fvalues[10], fvalues[11])); }
							break;
						case EValueSize::mat3x4:{
							float fvalues[12]; strtonum<float, 12>(value, fvalues);
							param.setAs<mat3x4>(mat3x4(fvalues[0], fvalues[1], fvalues[2],  fvalues[3], fvalues[4], fvalues[5],  fvalues[6], fvalues[7], fvalues[8],  fvalues[9], fvalues[10], fvalues[11])); }
							break;
						case EValueSize::mat4x2:{
							float fvalues[8]; strtonum<float, 8>(value, fvalues);
							param.setAs<mat4x2>(mat4x2(fvalues[0], fvalues[1], fvalues[2],  fvalues[3], fvalues[4], fvalues[5],  fvalues[6], fvalues[7])); }
							break;
						case EValueSize::mat2x4:{
							float fvalues[8]; strtonum<float, 8>(value, fvalues);
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
							int ivalue; strtonum<int, 1>(value, &ivalue);
							param.setAs<int>(ivalue); }
							break;
						case EValueSize::vec2:{
							int ivalues[2]; strtonum<int, 2>(value, ivalues);
							param.setAs<ivec2>(ivec2(ivalues[0], ivalues[1])); }
							break;
						case EValueSize::vec3:{
							int ivalues[3]; strtonum<int, 3>(value, ivalues);
							param.setAs<ivec3>(ivec3(ivalues[0], ivalues[1], ivalues[2])); }
							break;
						case EValueSize::vec4:{
							int ivalues[4]; strtonum<int, 4>(value, ivalues);
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

	/*auto p = m->first_node("param");
	auto p2 = p->next_sibling("param");

	auto a = p2->first_attribute();
	auto a2 = a->next_attribute();
	auto a3 = a2->next_attribute();

	auto v = a->value();
	auto v2 = a2->value();
	auto v3 = a3->value();

	if(strisequal(a3->name(), "type")){
		auto type = toEValueType(a3->value());
		auto size = toEValueSize(a3->value());

		if(istypeof<float>(type, size) == true)
			printf("je float");
		if(istypeof<vec2>(type, size) == true)
			printf("je vec2");
		if(istypeof<vec3>(type, size) == true)
			printf("je vec3");
		if(istypeof<vec4>(type, size) == true)
			printf("je vec4");
	}*/
}