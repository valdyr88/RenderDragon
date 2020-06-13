#include "shader.h"
#include "..\..\descriptors\shader_parser.h"
#include "uniform_buffer.h"
#include "texture.h"

#ifdef RD_API_WEBGL

//---------------------------------------------------------------------------------
//	CShader
//---------------------------------------------------------------------------------

IUniformBuffer* CShader::getUniformBuffer(uint bindPoint)
{
	/*auto& resources = descriptor.resources;

	for(auto it = resources.begin(); it != resources.end(); ++it){
		auto& resource = *it;
		if(resource.first == bindPoint){
			if(resource.second->getDescriptor().type == EShaderResourceType::UniformBuffer)
				return (IUniformBuffer*) resource.second.get();
			else
				return nullptr;
		}
	}*/
	//ToDo: implement this, but in CShaderProgram instead of here.
	return nullptr;
}

bool CShader::CheckCompileStatus(){
	if(this->device == nullptr) return false;
	auto& gl = device->gl;
	GLint isCompiled = GL_FALSE;
	gl.GetShaderiv(id, GL_COMPILE_STATUS, &isCompiled);
	return isCompiled == GL_TRUE;
}
bool CShader::CompileShader(){
	if(this->device == nullptr) return false;
	this->Release();

	auto& gl = device->gl;

	const GLchar* src = descriptor.source.c_str();
	GLint length = (GLint)descriptor.source.length();

	id = gl.CreateShader(glenum(descriptor.stage));
	gl.ShaderSource(id, 1, &src, &length);
	gl.CompileShader(id);

	if(CheckCompileStatus() == false)
	{
		GLint infoLogLen = 0;
		gl.GetShaderiv(id, GL_INFO_LOG_LENGTH, &infoLogLen);
		char* info_chars = __rd_new char[(size_t)infoLogLen + 1];
		gl.GetShaderInfoLog(id, infoLogLen+1, nullptr, info_chars);

		info_string.reserve((size_t)infoLogLen + 1);
		info_string.assign(info_chars);

		LOG("shader compile failed: <%s> :\n%s", descriptor.name.c_str(), info_chars);

		__rd_release_array(info_chars);
		return false;
	}
	return true;
}


bool CShader::RecompileFromSrcFile(std::string path){
	if(path.size() == 0) path = descriptor.path;
	if(path.size() == 0) return false;

	descriptor.source = CShader::ReadSouceFromFile(path);
	return this->CompileShader();
}

void CShader::Release(){
	if(this->device == nullptr) return;
	if(this->id == 0) return;

	auto& gl = device->gl;
	gl.DeleteShader(id); id = 0;
}

std::string CShader::ReadSouceFromFile(std::string path, CShaderFileSource* includeFiles, CShaderDefines* shaderDefines){

	std::string source = "";
	{
		CFile file; file.Open(path, CFile::EFileMode::ReadBinary);
		uint length = file.getSize();

		char* contents = __rd_new char[(size_t)length + 1];
		file.Read(length, contents);
		contents[length] = '\0';
		file.Close();

		source += contents;

		__rd_release_array(contents);
	}

	source = CShaderFileSource::ParseForIncludes(source, path, includeFiles);

	if(shaderDefines != nullptr)
		source = shaderDefines->InsertInto(source);

	return source;
}
std::string CShader::ReadSouceFromFile(std::string path, CShaderDefines* shaderDefines, CShaderFileSource* includeFiles){
	return CShader::ReadSouceFromFile(path, includeFiles, shaderDefines); }

//---------------------------------------------------------------------------------
//	CShaderProgram
//---------------------------------------------------------------------------------

std::map<std::string, CShaderProgram*> CShaderProgram::programs;

CShaderProgram::CShaderProgram(GPUDevice* dev, std::string uniquename, std::vector<SharedPtr<CShader>> shaders) :
	CGraphicObject(dev), name(uniquename)
{
	{
		auto program = CShaderProgram::programs.find(uniquename);
		if(program != CShaderProgram::programs.end() && program->second != nullptr){
			LOG_ERR("shader program with name <%s> already exists!", uniquename.c_str());
		}
		CShaderProgram::programs[uniquename] = this;
	}

	numStages = 0;

	for(auto it = shaders.begin(); it != shaders.end(); ++it){
		auto& sh = *it;
		if(sh->descriptor.stage < EShaderStage::NumShaderStages){
			shader[getStageNumber(sh->descriptor.stage)] = sh; ++numStages;
		}
	}

	MergeShaderResourceSetDescs();
	if(LinkProgram() == false && device != nullptr)
	{
		auto& gl = device->gl;

		GLint infoLogLen = 0;
		gl.GetProgramiv(id, GL_INFO_LOG_LENGTH, &infoLogLen);
		char* info_chars = __rd_new char[(size_t)infoLogLen + 1];
		gl.GetProgramInfoLog(id, infoLogLen + 1, nullptr, info_chars);

		info_string.reserve((size_t)infoLogLen + 1);
		info_string.assign(info_chars);

		LOG("shader linking failed: <%s> :\n%s", name.c_str(), info_string.c_str());
		__rd_release_array(info_chars);
	}
	CheckResourceBindings();
}

IUniformBuffer* CShaderProgram::getUniformBuffer(EShaderStage stage, uint bindPoint)
{
	uint stageNum = getStageNumber(stage);
	if(stageNum >= EShaderStage::NumShaderStages) return nullptr;
	if(shader[stageNum] == nullptr) return nullptr;

	return shader[stageNum]->getUniformBuffer(bindPoint);
}

bool CShaderProgram::CheckLinkStatus(){
	if(this->device == nullptr) return false;
	auto& gl = device->gl;
	GLint param = GL_FALSE;
	gl.GetProgramiv(id, GL_LINK_STATUS, &param);
	return param == GL_TRUE;
}


bool CShaderProgram::LinkProgram(){
	if(this->device == nullptr) return false;
	this->Release();

	auto& gl = device->gl;
	id = gl.CreateProgram();

	for(uint i = 0; i < EShaderStage::NumShaderStages; ++i){
		if(shader[i] != nullptr)
			gl.AttachShader(id, shader[i]->getId());
	}
	auto& vs = shader[getStageNumber(EShaderStage::VertexShader)];
	if(vs != nullptr){
		auto& format = vs->getDescriptor().vertexFormat;
		for(uint i = 0; i < format.attributes.size(); ++i){
			auto& att = format.attributes[i];
			gl.BindAttribLocation(id, att.binding, att.name.c_str());
		}
	}
	gl.LinkProgram(id);

	return CheckLinkStatus();
}

bool CShaderProgram::Recompile(){

	for(auto sh : shader){
		if(sh != nullptr) sh->RecompileFromSrcFile();
	}
	return this->LinkProgram();
}

bool CShaderProgram::CheckResourceBindings(){
	if(this->device == nullptr) return false;
	auto& gl = device->gl;
	bool hasError = false;

	std::string name = "";
	if(this->shader[getStageNumber(EShaderStage::FragmentShader)] != nullptr) name = this->shader[getStageNumber(EShaderStage::FragmentShader)]->descriptor.name;
	else if(this->shader[getStageNumber(EShaderStage::GeometryShader)] != nullptr) name = this->shader[getStageNumber(EShaderStage::GeometryShader)]->descriptor.name;
	else if(this->shader[getStageNumber(EShaderStage::TessControlShader)] != nullptr) name = this->shader[getStageNumber(EShaderStage::TessControlShader)]->descriptor.name;
	else if(this->shader[getStageNumber(EShaderStage::ComputeShader)] != nullptr) name = this->shader[getStageNumber(EShaderStage::ComputeShader)]->descriptor.name;
	else if(this->shader[getStageNumber(EShaderStage::VertexShader)] != nullptr) name = this->shader[getStageNumber(EShaderStage::VertexShader)]->descriptor.name;

	{
		GLint numberOfBlocks = 0;
		gl.GetProgramiv(this->id, GL_ACTIVE_UNIFORM_BLOCKS, &numberOfBlocks);

		for(int i = 0; i < numberOfBlocks; ++i){
			GLint namelen = 0; GLint binding = 0;
			gl.GetActiveUniformBlockiv(this->id, i, GL_UNIFORM_BLOCK_NAME_LENGTH, &namelen);
			char* cname = __rd_new char[(size_t)namelen+1];
			gl.GetActiveUniformBlockName(this->id, i, namelen, nullptr, cname);
			cname[namelen] = 0;
			gl.GetActiveUniformBlockiv(this->id, i, GL_UNIFORM_BLOCK_BINDING, &binding);

			SShaderProgramIntrospection::SResourceBinding resbind;
			resbind.name = cname;
			resbind.type = EShaderResourceType::UniformBuffer;
			resbind.set = 0;
			resbind.binding = binding;//gl.GetUniformLocation(this->id, cname);//(GLint)gl.GetUniformBlockIndex(this->id, cname);//

			introspection.resourceBindings.emplace_back(resbind);
			__rd_release_array(cname);
		}
	}

	for(auto rsdit = resourceSetDescs.begin(); rsdit != resourceSetDescs.end(); ++rsdit){
		for(auto it = (*rsdit)->bindingDescs.begin(); it != (*rsdit)->bindingDescs.end(); ++it){

			GLint binding = -1;
			switch(it->type)
			{
				case EShaderResourceType::Texture:
					binding = gl.GetUniformLocation(this->id, it->name.c_str());
					break;
				case EShaderResourceType::Sampler:
					binding = gl.GetUniformLocation(this->id, it->name.c_str());
					break;
				case EShaderResourceType::CombinedTexSampler:
					binding = gl.GetUniformLocation(this->id, it->name.c_str());
					break;
				case EShaderResourceType::UniformBuffer:
					binding = (GLint)gl.GetUniformBlockIndex(this->id, it->name.c_str());
					//binding = gl.GetUniformLocation(this->id, it->name.c_str());
					if(binding == -1){
						for(auto in = introspection.resourceBindings.begin(); in != introspection.resourceBindings.end(); ++in){
							if(in->type != EShaderResourceType::UniformBuffer) continue;

							if(in->binding == it->bindPoint){
								//LOG("bindPoint <%d> in shader <%s> corresponding to descriptor <%s> is named <%s> in shader", it->bindPoint, name.c_str(), it->name.c_str(), in->name.c_str());
								LOG("Resource descriptor <%s, bindPoint=%d> in shader <%s> has name <%s>", it->name.c_str(), it->bindPoint, name.c_str(), in->name.c_str());
								binding = it->bindPoint;
								break;
							}
						}
					}
					break;
				default:
					break;
			}

			//add to introspection if it's not UniformBuffer (we have that already)
			if(binding != -1)
			{
				SShaderProgramIntrospection::SResourceBinding resbind;
				resbind.name = it->name;
				resbind.type = it->type;
				resbind.set = 0;
				resbind.binding = binding;
				introspection.resourceBindings.emplace_back(resbind);
			}

			if(binding != it->bindPoint){
				LOG_WARN("bindPoint mismatch for shader: <%s> uniform name: <%s>. shader's binding is %d, descriptor's binding is %d", name.c_str(), it->name.c_str(), binding, it->bindPoint);
				hasError = true;
			}
		}
	}
	return !hasError;
}

bool CShaderProgram::setUniformBuffer(uint set, uint binding, IUniformBuffer* ub){
	if(ub == nullptr) return false;
	return ub->Bind(this, set, binding);
}
bool CShaderProgram::setTexture(uint set, uint binding, CTextureView* tx){
	if(tx == nullptr) return false;
	return tx->Bind(set, binding);
}

bool CShaderProgram::setUniformBuffer(std::string name, IUniformBuffer* ub){
	uint set = 0, binding = 0;
	auto& setbindpair = this->resourceBindingPoints[name];
	if(&setbindpair != nullptr)
		return setUniformBuffer(setbindpair.first, setbindpair.second, ub);
	return false;
}
bool CShaderProgram::setTexture(std::string name, CTextureView* tx){
	uint set = 0, binding = 0;
	auto& setbindpair = this->resourceBindingPoints[name];
	if(&setbindpair != nullptr)
		return setTexture(setbindpair.first, setbindpair.second, tx);
	return false;
}


void CShaderProgram::Release(){
	if(this->device == nullptr) return;
	if(this->id == 0) return;

	auto& gl = device->gl;
	gl.DeleteProgram(id); id = 0;
}

bool CShaderProgram::Contains(const std::vector<SharedPtr<CShader>>& shlist){
	for(auto it = shlist.begin(); it != shlist.end(); ++it){
		auto& sh = *it;
		auto stage = sh->getDescriptor().stage;
		uint stageNum = getStageNumber(stage);

		if(shader[stageNum] != nullptr){
			if(shader[stageNum].get() != sh.get())
				return false;
		}
		else return false;
	}
	return true;
}

//-----------------------------------------------------------------------------------
//	CShaderModuleManager
//-----------------------------------------------------------------------------------

bool CShaderModuleManager::add(SharedPtr<CShader> shader){
	if(device == nullptr) return false;
	auto found = FindByName(shader->getDescriptor().name);
	if(found != nullptr && found.get() != shader.get()){
		LOG_WARN("different shader with same name <%s> is already in CShaderModuleManager", shader->getDescriptor().name.c_str()); return false; }
	found = FindBySource(shader->getDescriptor().source);
	if(found != nullptr && found.get() != shader.get()){
		LOG_WARN("different shaders <new: %s, found: %s> with same source is already in CShaderModuleManager", shader->getDescriptor().name.c_str(), found->getDescriptor().name.c_str()); return false; }

	device->created.shaders.add(shader);
	return true;
}

SharedPtr<CShader> CShaderModuleManager::FindByName(std::string name){
	if(device == nullptr) return nullptr;
	for(uint i = 0; i < device->created.shaders.size(); ++i){
		auto shader = device->created.shaders[i].lock();
		if(shader != nullptr && shader->getDescriptor().name == name)
			return shader;
	}
	return nullptr;
}
SharedPtr<CShader> CShaderModuleManager::FindBySource(std::string src){
	if(device == nullptr) return nullptr;
	for(uint i = 0; i < device->created.shaders.size(); ++i){
		auto shader = device->created.shaders[i].lock();
		if(shader != nullptr && shader->getDescriptor().source == src)
			return shader;
	}
	return nullptr;
}
SharedPtr<CShader> CShaderModuleManager::CreateShaderModule(const SShaderDesc& desc){
	if(device == nullptr) return nullptr;

	auto shader = FindByName(desc.name);
	if(shader != nullptr) return shader;
	shader = FindBySource(desc.source);
	if(shader != nullptr) return shader;

	return device->CreateShaderModule(desc);
}

//-----------------------------------------------------------------------------------
//	CShaderProgramManager
//-----------------------------------------------------------------------------------

bool CShaderProgramManager::add(SharedPtr<CShaderProgram> shader){
	if(FindByName(shader->getName()) != nullptr){
		LOG_WARN("different shader with same name <%s> is already in CShaderProgramManager", shader->getName()); return false;}
	std::vector<SharedPtr<CShader>> shaders; shaders.reserve(shader->getNofStages());
	for(uint i = 0; i < EShaderStage::NumShaderStages; ++i)
		if(shader->shader[i] != nullptr) shaders.emplace_back(shader->shader[i]);
	if(FindByStages(shaders) != nullptr){
		LOG_WARN("different shader <%s> with same stages is already in CShaderProgramManager", shader->getName()); return false;}

	device->created.programs.add(shader);
	return true;
}

SharedPtr<CShaderProgram> CShaderProgramManager::FindByName(std::string name){
	if(device == nullptr) return nullptr;
	auto program = CShaderProgram::programs[name];
	return device->FindSharedPtr<CShaderProgram>(program);

	/*for(uint i = 0; i < device->created.programs.size(); ++i){
		auto shader = device->created.programs[i].lock();
		if(shader != nullptr && shader->getName() == name)
			return shader;
	}
	return nullptr;*/
}
SharedPtr<CShaderProgram> CShaderProgramManager::FindByStages(const std::vector<SharedPtr<CShader>> shaderlist){
	if(device == nullptr) return nullptr;
	for(uint i = 0; i < device->created.programs.size(); ++i){
		auto shader = device->created.programs[i].lock();
		if(shader != nullptr && shader->Contains(shaderlist) == true)
			return shader;
	}
	return nullptr;
}
SharedPtr<CShaderProgram> CShaderProgramManager::CreateShaderProgram(std::string name, const std::vector<SharedPtr<CShader>> shaders){
	if(device == nullptr) return nullptr;

	auto program = FindByName(name);
	if(program != nullptr) return program;
	program = FindByStages(shaders);
	if(program != nullptr) return program;

	if(shaders.size() == 0) return nullptr;

	return device->CreateShaderProgram(name, shaders);
}

#endif //RD_API_OPENGL4