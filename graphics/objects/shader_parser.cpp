
//---------------------------------------------------------------------------------
//common code
//---------------------------------------------------------------------------------
#include "../descriptors/shader_parser.h"
#include "../utils/platform.h"

#include "../utils/spirv-cross/spirv_glsl.hpp"
#include <regex>

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

	std::string currentDir = srcFilePath.substr(0, srcFilePath.find_last_of("/"));

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
			includeName = currentDir + "/" + includeName;
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
	if(file.isOpen() == false){
		LOG_ERR("Can't open file <%s>", fileName); return ""; }

	uint length = file.getSize();

	char* contents = __rd_new char[(size_t)length+1];
	file.Read(length, contents);
	contents[length] = '\0';

	std::string rtn = "";
	rtn += contents;

	__rd_release_array(contents);

	return rtn;
}

bool printContentsToFile(const char* fileName, const char* contents, uint length){
	CFile file; file.Open(fileName, CFile::EFileMode::WriteBinary);
	if(file.isOpen() == false) return false;
	file.Write(contents,length);
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

std::string transfSPIRVtoGLSL(byte* spirvData, sizetype length, uint version, bool es){
	if(length % sizeof(uint32) != 0){//size needs to be multiple of sizeof(uint32)
		LOG_ERR("spirv data size not multiple of 4"); return ""; }

	spirv_cross::CompilerGLSL glsl((uint32*)spirvData, length/sizeof(uint32));
	spirv_cross::CompilerGLSL::Options compile_options;
	compile_options.version = version;
	compile_options.es = es;
	glsl.set_common_options(compile_options);

	std::string source = glsl.compile();

	std::regex match("(#version[ ]+[0-9es]+)");
	std::smatch match_res;
	if(std::regex_search(source, match_res, match) == true){
		if(match_res.size() > 1){
			auto len = match_res[1].length();
			source.insert(len, "\n"+CShaderDefines::globalMacrosKeyword+"\n");
		}
	}
	return source;
}
std::string transfSPIRVtoGLSL(std::string fileName, uint version, bool es){
	CFile file; file.Open(fileName, CFile::EFileMode::ReadBinary);
	if(file.isOpen() == false){
		LOG_ERR("Can't open file <%s>", fileName.c_str()); return ""; }
	uint size = file.getSize();
	
	std::vector<byte> data; data.resize(size);
	file.Read<byte>(size, data.data());
	file.Close();

	return transfSPIRVtoGLSL(data.data(), size, version, es);
}

std::string transfGLSLtoGLSL(std::string source, uint version, bool es){
	std::istringstream srcStream(source);
	std::string output = "";
	uint lineNo = 0;

	for(std::string line = ""; std::getline(srcStream, line); ){
		auto cline = line.c_str();

		//#version
		if(lineNo == 0)
		{
			size_t istartNo = 0;
			auto istart = str::find_first_occurence("#version", cline, 0, &istartNo);

			std::string version_precision = 
				std::string("#version ") + std::to_string(version) + std::string((es) ? " es" : "") + "\n"
				+ "precision highp float;\nprecision highp int;\n";

			if(istart != nullptr)
				line = version_precision;
			else
				line = version_precision + line;
		}

		//layout
		{
			size_t istartNo = 0;
			auto istart = str::find_first_occurence("layout", cline, 0, &istartNo);

			if(istart != nullptr){
				{
					std::string replace = "";
					std::regex match("layout[ \t]*\\((.*)\\)");
					std::smatch match_res;

					if(std::regex_search(line, match_res, match) == true){
						if(match_res.size() > 1){
							std::list<std::string> v = str::split(match_res[1], ",");
							for(auto& l : v){
								if(l.find("binding") != std::string::npos) continue;
								if(l.find("location") != std::string::npos) continue;
								//if(l.find("") != std::string::npos) continue;

								if(replace.size() == 0)
									replace = l;
								else
									replace = replace + "," + l;
							}
						}
					}
					if(replace.size() != 0)
						replace = "layout(" + replace + ") ";

					line = std::regex_replace(line, match, replace);
				}
			}
		}

		output += line + '\n';
		++lineNo;
	}
	if(output == "")
		output = std::move(source);

	return output;
}