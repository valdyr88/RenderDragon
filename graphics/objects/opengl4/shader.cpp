#include "shader.h"

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
	GLint length = descriptor.source.length();

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
	gl.LinkProgram(id);

	return CheckLinkStatus();
}

#endif //RD_API_OPENGL4