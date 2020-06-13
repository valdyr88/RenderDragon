
//---------------------------------------------------------------------------------
//common code
//---------------------------------------------------------------------------------
#include "../descriptors/shader_parser.h"

//---------------------------------------------------------------------------------
std::string CShaderDefines::globalMacrosKeyword = "#global_defines";

std::string CShaderDefines::insertInto(std::string source, std::string keyword){
	std::istringstream srcStream(source);
	std::string output = "";

	for(std::string line = ""; std::getline(srcStream, line); ){
		auto cline = line.c_str();

		size_t istartNo = 0;
		auto istart = str::find_first_occurence(keyword.c_str(), cline, 0, &istartNo);

		if(istart != nullptr){
			line = this->construct() + "\n //" + line;
			istart = nullptr;
		}

		output += line + '\n';
	}
	if(output == "")
		output = std::move(source);

	return output;
}

//---------------------------------------------------------------------------------

std::string CShaderFileSource::parseForIncludes(std::string source, std::string srcFilePath, CShaderFileSource* includeFiles){
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

			istart = nullptr;
		}

		output += line + '\n';
	}
	if(output == "")
		output = std::move(source);

	return output;
}

std::string getFileStringContents(const char* fileName){
	FILE* file = nullptr; fopen_s(&file, fileName, "rb");
	if(file == nullptr) return "";
	fseek(file, 0, SEEK_END);
	size_t length = ftell(file);
	fseek(file, 0, SEEK_SET);

	char* contents = __rd_new char[length + 1];
	fread(contents, 1, length, file);
	contents[length] = '\0';
	fclose(file); file = nullptr;

	std::string rtn = "";
	rtn += contents;

	__rd_release_array(contents);

	return rtn;
}

bool printContentsToFile(const char* fileName, const char* contents, size_t length){
	FILE* file = nullptr; fopen_s(&file, fileName, "wb");
	if(file == nullptr) return false;
	fwrite(contents, 1, length, file);
	fclose(file); file = nullptr;
	return true;
}

std::string TestIncludes(const char* fileName){

	std::string source = getFileStringContents(fileName);
	source = CShaderFileSource::parseForIncludes(source, fileName, CSingleton<CShaderFileSource>::get());

	std::string outPath = fileName;
	outPath += ".processed.glsl";

	printContentsToFile(outPath.c_str(), source.c_str(), source.length());

	return std::move(source);
}

//---------------------------------------------------------------------------------
