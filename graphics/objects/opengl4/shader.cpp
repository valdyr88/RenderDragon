#include "shader.h"
#include "uniform_buffer.h"
#include "texture.h"

#ifdef RD_API_OPENGL4

// CShader
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
		char* info_chars = __new char[infoLogLen + 1];
		gl.GetShaderInfoLog(id, infoLogLen+1, nullptr, info_chars);

		info_string.reserve(infoLogLen + 1);
		info_string.assign(info_chars);

		LOG("shader compile failed: <%s> :\n%s", descriptor.name.c_str(), info_chars);

		__release_array(info_chars);
		return false;
	}
	return true;
}

void CShader::Release(){
	if(this->device == nullptr) return;
	if(this->id == 0) return;

	auto& gl = device->gl;
	gl.DeleteShader(id); id = 0;
}

// CShaderProgram
//---------------------------------------------------------------------------------

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
			char* cname = __new char[namelen+1];
			gl.GetActiveUniformBlockName(this->id, i, namelen, nullptr, cname);
			cname[namelen] = 0;
			gl.GetActiveUniformBlockiv(this->id, i, GL_UNIFORM_BLOCK_BINDING, &binding);

			SShaderProgramIntrospection::SResourceBinding resbind;
			resbind.name = cname;
			resbind.type = EShaderResourceType::UniformBuffer;
			resbind.set = 0;
			resbind.binding = binding;//gl.GetUniformLocation(this->id, cname);//(GLint)gl.GetUniformBlockIndex(this->id, cname);//

			introspection.resourceBindings.emplace_back(resbind);
			__release_array(cname);
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

#endif //RD_API_OPENGL4