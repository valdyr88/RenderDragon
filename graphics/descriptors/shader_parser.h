#ifndef SHADER_PARSER_H
#define SHADER_PARSER_H

#include <sstream>
#include "../utils/pointers.h"
#include "../utils/singleton.h"
#include "../utils/strings.h"
#include "../utils/containers.h"
#include "../utils/types/types.h"
#include "../descriptors/graphics_enums.h"
#include "../descriptors/shader_desc.h"
#include "../descriptors/graphic_object.h"

class CShaderFileSource{
	std::map<std::string, std::string> sources;

public:
	bool add(const std::string& name, const std::string& contents){
		if(sources.find(name) != sources.end()) return false;
		sources.insert({ name, contents });
		return true;
	}
	std::string contents(const std::string& name){
		if(sources.find(name) == sources.end()) return "";
		return sources[name];
	}
	std::string operator[](const std::string& name){
		return contents(name);
	}

public:

	static std::string parseForIncludes(std::string source, std::string srcFilePath, CShaderFileSource* includeFiles);
};
typedef CSingleton<CShaderFileSource> ShaderSourceList;

struct SMacroDefine{
	std::string macro;
	std::string value;

	void set(const char* Macro, const char* Value){ macro = Macro; value = Value; }
	void set(std::string Macro, std::string Value){ macro = Macro; value = Value; }

	SMacroDefine& operator =(const SMacroDefine& other){
		macro = other.macro;
		value = other.value;
		return *this;
	}

	std::string construct(){ return "#define " + macro + " " + value; }
};

class CShaderDefines{
	stdex::container<SMacroDefine> macros;
	static std::string globalMacrosKeyword;
public:
	uint add(std::string Macro, std::string Value){
		SMacroDefine& m = macros.add();
		m.set(Macro, Value);
		return macros.size() - 1;
	}
	uint add(const char* Macro, const char* Value){
		std::string M(Macro), V(Value);
		return add(M, V);
	}

	std::string construct(){
		std::string out = "";
		for(uint i = 0; i < macros.size(); ++i){
			out += macros[i].construct() + '\n';
		}
		return out;
	}

	const SMacroDefine& get(uint i){
		return macros[i];
	}

	~CShaderDefines(){
		macros.clear();
	}

	CShaderDefines& operator =(const CShaderDefines& other){
		macros.clear();
		macros = other.macros;
		return *this;
	}
	CShaderDefines& operator +=(const CShaderDefines& other){
		macros += other.macros;
		return *this;
	}

	std::string insertInto(std::string source, std::string keyword = CShaderDefines::globalMacrosKeyword);
};
typedef CSingleton<CShaderDefines> ShaderGlobalDefines;

bool printContentsToFile(const char* fileName, const char* contents, size_t length);
std::string getFileStringContents(const char* fileName);
std::string TestIncludes(const char* fileName);

#endif //SHADER_PARSER_H