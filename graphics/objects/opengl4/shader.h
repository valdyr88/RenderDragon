#ifndef SHADER_H
#define SHADER_H

#ifdef RD_API_OPENGL4

#include <vector>
#include <list>
#include <map>
#include "../../utils/pointers.h"
#include "../../utils/singleton.h"
#include "../../utils/strings.h"
#include "../../utils/types/types.h"
#include "../../descriptors/graphics_enums.h"
#include "../../descriptors/shader_desc.h"
#include "../../descriptors/graphic_object.h"
#include "shader_resource_manager.h"

class IUniformBuffer;
class CTextureView;
class CShaderFileSource;
class CShaderDefines;

//-----------------------------------------------------------------------------------

class CShader : public CGraphicObject{
protected:
	SShaderDesc descriptor;

	GLuint id = 0;
	GLuint getId(){ return id; }

	IUniformBuffer* getUniformBuffer(uint bindPoint);

	std::string info_string;
	bool CheckCompileStatus();
	bool CompileShader();

	virtual void Release() override;
public:

	CShader(GPUDevice* dev, const SShaderDesc& desc) :
		CGraphicObject(dev), descriptor(desc){
		CompileShader();
	}

	bool RecompileFromSrcFile(std::string path = "");

	const auto& getDescriptor(){ return descriptor; }

	virtual ~CShader() override{ Release(); }

	static std::string ReadSouceFromFile(std::string path, CShaderFileSource* includeFiles = CSingleton<CShaderFileSource>::get(), CShaderDefines* shaderDefines = CSingleton<CShaderDefines>::get());
	static std::string ReadSouceFromFile(std::string path, CShaderDefines* shaderDefines, CShaderFileSource* includeFiles = CSingleton<CShaderFileSource>::get());
	
	friend class CShaderProgram;
	friend class CPipelineState;
	friend class CShaderModuleManager;
};

//-----------------------------------------------------------------------------------

class CShaderProgram : public CGraphicObject{
protected:
	uint numStages;
	SharedPtr<CShader> shader[(uint)EShaderStage::NumShaderStages];
	std::vector<SharedPtr<CShaderResourceSetDesc>> resourceSetDescs;
	std::map<std::string, std::pair<uint, uint>> resourceBindingPoints;

	std::string name = "";
	static std::map<std::string, CShaderProgram*> programs;

	struct SShaderProgramIntrospection{
		struct SResourceBinding{
			EShaderResourceType type = EShaderResourceType::UniformBuffer;
			uint set = 0;
			uint binding = 0;
			std::string name;
			std::string ubstructname;
			sizetype size = 0;
		};
		std::vector<SResourceBinding> resourceBindings;
	} introspection;

	GLuint id = 0;

	bool MergeShaderResourceSetDescs();

	std::string info_string;
	bool CheckLinkStatus();
	bool LinkProgram();
	bool CheckResourceBindings();

	void CheckUniformBuffer(uint set, uint binding, IUniformBuffer* ub);
public:

	CShaderProgram(GPUDevice* dev, std::string uniquename, std::vector<SharedPtr<CShader>> shaders);

	bool Contains(const std::vector<SharedPtr<CShader>>& shaders);

	bool setUniformBuffer(uint set, uint binding, IUniformBuffer* ub);
	bool setTexture(uint set, uint binding, CTextureView* tx);
	bool setUniformBuffer(std::string name, IUniformBuffer* ub);
	bool setTexture(std::string name, CTextureView* tx);

	GLuint getId(){ return id; }

	uint getNofStages(){ return numStages; }
	IUniformBuffer* getUniformBuffer(EShaderStage stage, uint binding);
	const char* getName(){ return name.c_str(); }

	bool Recompile();

	virtual void Release() override;
	virtual ~CShaderProgram() override{
		CShaderProgram::programs[name] = nullptr;
		Release();
	}

	friend class CPipelineState;
	friend class CShaderProgramManager;
};

//-----------------------------------------------------------------------------------
class GPUDevice;

class CShaderModuleManager{
protected:
	GPUDevice* device = nullptr;

	CShaderModuleManager(){}
	CShaderModuleManager(GPUDevice* dev) : device(dev){}
	void setDevice(GPUDevice* dev){ device = dev; }
	bool add(SharedPtr<CShader> shader);
public:

	SharedPtr<CShader> FindByName(std::string name);
	SharedPtr<CShader> FindBySource(std::string src);
	SharedPtr<CShader> CreateShaderModule(const SShaderDesc& desc);

	friend class GPUDevice;
	friend class CSingleton<CShaderModuleManager>;
	friend bool rdSetupDeviceForGlobalObjects(GPUDevice* device);
};

//-----------------------------------------------------------------------------------

class CShaderProgramManager{
protected:
	GPUDevice* device = nullptr;

	CShaderProgramManager(){}
	CShaderProgramManager(GPUDevice* dev) : device(dev){}
	void setDevice(GPUDevice* dev){ device = dev; }
	bool add(SharedPtr<CShaderProgram> shader);
public:

	SharedPtr<CShaderProgram> FindByName(std::string name);
	SharedPtr<CShaderProgram> FindByStages(const std::vector<SharedPtr<CShader>> shaders);
	SharedPtr<CShaderProgram> CreateShaderProgram(std::string name, const std::vector<SharedPtr<CShader>> shaders);

	friend class GPUDevice;
	friend class CSingleton<CShaderProgramManager>;
	friend bool rdSetupDeviceForGlobalObjects(GPUDevice* device);
};

#endif //RD_API_OPENGL4
#endif //SHADER_H