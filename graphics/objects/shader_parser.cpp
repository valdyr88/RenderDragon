
//---------------------------------------------------------------------------------
//common code
//---------------------------------------------------------------------------------
#include "../descriptors/shader_parser.h"
#include "../utils/platform.h"

//---------------------------------------------------------------------------------
std::string CShaderDefines::globalMacrosKeyword = "#global_defines";

std::string CShaderDefines::InsertInto(std::string source, std::string keyword){
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

std::string CShaderFileSource::ParseForIncludes(std::string source, std::string srcFilePath, CShaderFileSource* includeFiles){
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
			line = CShaderFileSource::ParseForIncludes(includeFiles->contents(includeName), includeName, includeFiles);

			istart = nullptr;
		}

		output += line + '\n';
	}
	if(output == "")
		output = std::move(source);

	return output;
}

std::string getFileStringContents(const char* fileName){
	CFile file; file.Open(fileName, CFile::EFileMode::ReadBinary);
	uint length = file.getSize();

	char* contents = __new char[(size_t)length+1];
	file.Read(length, contents);
	contents[length] = '\0';

	std::string rtn = "";
	rtn += contents;

	__release_array(contents);

	return rtn;
}

bool printContentsToFile(const char* fileName, const char* contents, uint length){
	CFile file; file.Open(fileName, CFile::EFileMode::WriteBinary);
	if(file.isOpen() == false) return false;
	file.Read(length, contents);
	return true;
}

std::string TestIncludes(const char* fileName){
	std::string source = getFileStringContents(fileName);
	source = CShaderFileSource::ParseForIncludes(source, fileName, CSingleton<CShaderFileSource>::get());

	std::string outPath = fileName;
	outPath += ".processed.glsl";

	printContentsToFile(outPath.c_str(), source.c_str(), (uint)source.length());

	return std::move(source);
}

//---------------------------------------------------------------------------------
