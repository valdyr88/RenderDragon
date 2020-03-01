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

	std::string construct(){ return "#define " + macro + " " + value; }
};

class CShaderDefines{
	std::container<SMacroDefine> macros;

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

	const SMacroDefine& get(uint i){
		return macros[i];
	}

	~CShaderDefines(){
		macros.clear();
	}
};

inline std::string CShaderFileSource::parseForIncludes(std::string source, std::string srcFilePath, CShaderFileSource* includeFiles){
	std::istringstream srcStream(source);
	std::string output = "";

	for(std::string line = ""; std::getline(srcStream, line); ){
		auto cline = line.c_str();

		size_t istartNo = 0;
		auto istart = str::find_first_occurence("#include", cline, 0, &istartNo);

		if(istart != nullptr)
		{
			size_t iNameStartNo = 0, iNameEndNo = 0;
			str::find_first_occurence("\"", cline + istartNo, 0, &iNameStartNo); iNameStartNo += istartNo + 1;
			str::find_first_occurence("\"", cline + iNameStartNo, 0, &iNameEndNo); iNameEndNo += iNameStartNo;

			auto includeName = line.substr(iNameStartNo, iNameEndNo - iNameStartNo);
			line = parseForIncludes(includeFiles->contents(includeName), includeName, includeFiles);
		}

		output += line + "\n";
	}
	if(output == "")
		output = source;

	return output;
}

inline std::string getFileStringContents(const char* fileName){
	FILE* file = nullptr; fopen_s(&file, fileName, "rb");
	if(file == nullptr) return "";
	fseek(file, 0, SEEK_END);
	size_t length = ftell(file);
	fseek(file, 0, SEEK_SET);

	char* contents = __new char[length + 1];
	fread(contents, 1, length, file);
	contents[length] = '\0';
	fclose(file); file = nullptr;

	std::string rtn = "";
	rtn += contents;

	__release_array(contents);

	return rtn;
}

inline bool printContentsToFile(const char* fileName, const char* contents, size_t length){
	FILE* file = nullptr; fopen_s(&file, fileName, "wb");
	if(file == nullptr) return false;
	fwrite(contents, 1, length, file);
	fclose(file); file = nullptr;
	return true;
}

inline void TestIncludes(const char* fileName){

	std::string source = getFileStringContents(fileName);
	source = CShaderFileSource::parseForIncludes(source, fileName, CSingleton<CShaderFileSource>::get());

	std::string outPath = fileName;
	outPath += ".processed.glsl";

	printContentsToFile(outPath.c_str(), source.c_str(), source.length());
}

#endif //SHADER_PARSER_H