#ifdef RD_API_WEBGL

#include "../../utils/platform_defines.h"
#include "device.h"

#ifdef PLATFORM_EMSCRIPTEN
	#include "emscripten\emscripten.h"
	#include "SDL\SDL.h"
#endif

SGPUDeviceContext rdInitOpenGL(const SWindow& window, const SGPUDeviceDesc& descriptor){
	SGPUDeviceContext GPUDeviceContext;

#ifdef PLATFORM_EMSCRIPTEN

#ifdef SDL_GL_context
	SDL_GL_SetAttribute(SDL_GLattr::SDL_GL_CONTEXT_MAJOR_VERSION, 3);
	SDL_GL_SetAttribute(SDL_GLattr::SDL_GL_CONTEXT_MINOR_VERSION, 2);
	SDL_GL_SetAttribute(SDL_GLattr::SDL_GL_DOUBLEBUFFER, 1);
	SDL_GL_SetAttribute(SDL_GLattr::SDL_GL_DEPTH_SIZE, 24);

	GPUDeviceContext.context = SDL_GL_CreateContext(window.window);
	//SDL_GL_MakeCurrent(window.window, GPUDeviceContext.context);
#endif

#ifdef EGL_GL_context
	EGLint majorv = 0, minorv = 0;
	GPUDeviceContext.display = eglGetDisplay(EGL_DEFAULT_DISPLAY);
	eglInitialize(GPUDeviceContext.display, &majorv, &minorv);
	{
		EGLint gl_attrib_list[] = { EGL_BIND_TO_TEXTURE_RGBA, EGL_TRUE,
									EGL_RED_SIZE, 8,
									EGL_GREEN_SIZE, 8,
									EGL_BLUE_SIZE, 8,
									EGL_NONE };
		EGLint numconf = 0;
		eglChooseConfig(GPUDeviceContext.display, gl_attrib_list, &GPUDeviceContext.config, 1, &numconf);
	}
	EGLint gl_attrib_list[] = { EGL_CONTEXT_CLIENT_VERSION, 3, EGL_NONE };
	
	GPUDeviceContext.surface = eglCreateWindowSurface(GPUDeviceContext.display, GPUDeviceContext.config, 0, nullptr);
	GPUDeviceContext.context = eglCreateContext(GPUDeviceContext.display, GPUDeviceContext.config, nullptr, gl_attrib_list);
	if(eglMakeCurrent(GPUDeviceContext.display, GPUDeviceContext.surface, nullptr, GPUDeviceContext.context) == false){
		LOG_ERR("Can't create GL context through EGL!"); }

#endif

	glClearColor(1.0, 0.0, 0.0, 1.0);
	glClear(GL_COLOR_BUFFER_BIT);
#endif

	return GPUDeviceContext;
}
SGPUDeviceContext GPUDevice::InitOpenGL(SWindow& win){
	window = win;
	context = rdInitOpenGL(window, this->descriptor);
	gl.FetchFunctionPointers();
	initPipelineState();
	return context;
}

void GPUDevice::Release(){

#ifdef PLATFORM_EMSCRIPTEN
#ifdef SDL_GL_context
	SDL_GL_DeleteContext(context.context); context.context = nullptr;
#endif
#ifdef EGL_GL_context
	eglMakeCurrent(context.display, EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT);
	eglDestroyContext(context.display, context.context); context.context = nullptr;
	eglDestroySurface(context.display, context.surface); context.surface = nullptr;
	eglTerminate(context.display); context.display = nullptr;
#endif

#endif
}


bool GPUDevice::InitContextOnWindow(SWindow& win){
	context = this->InitOpenGL(win);

	SRenderPassDesc rpdesc;
	{
		rpdesc.attachments[0].format = descriptor.swapchain.format;
		rpdesc.attachments[0].loadOp = ELoadStoreOp::Clear;
		rpdesc.attachments[0].storeOp = ELoadStoreOp::Store;
		rpdesc.nofAttachments = 1;
		rpdesc.depthStencil.format = descriptor.swapchain.depthFormat;
		rpdesc.depthStencil.loadOp = ELoadStoreOp::Clear;
		rpdesc.depthStencil.storeOp = ELoadStoreOp::DontCare;
		rpdesc.depthStencil.stencilLoadOp = ELoadStoreOp::Clear;
		rpdesc.depthStencil.stencilStoreOp = ELoadStoreOp::DontCare;
	}

	swapchainRenderPass = SharedPtr<CRenderPass>(__rd_new CRenderPass(this, rpdesc));
	swapchainFramebuffer = SharedPtr<CFramebuffer>(__rd_new CGLSwapchainFramebuffer(this, rpdesc));
	
	rdSetupDeviceForGlobalObjects(this);

	return true;
}

UniquePtr<GPUDevice> GPUDevice::CreateGPUDevice(const SGPUDeviceDesc& desc){
	return NewUnique<GPUDevice>(desc);
}


GPUDevice::~GPUDevice(){
	Release();
}

//------------------------------------------------------------------------------------

template <typename type>
void GPUDevice::addTrackedObject(SharedPtr<type>& obj){
	created.objects.add(obj); }
template <>
void GPUDevice::addTrackedObject(SharedPtr<CShader>& obj){
	created.shaders.add(obj); }
template <>
void GPUDevice::addTrackedObject(SharedPtr<CShaderProgram>& obj){
	created.programs.add(obj); }
template <>
void GPUDevice::addTrackedObject(SharedPtr<CTexture>& obj){
	created.textures.add(obj);}

template <typename type>
SharedPtr<type> FindInList(type* ptr, const stdex::container<WeakPtr<type>>& elems){
	if(ptr == nullptr) return nullptr;
	for(uint i = 0; i < elems.size(); ++i){
		if(((WeakPtrEx<type>*)(&elems[i]))->contains(ptr) == true){
			return elems[i].lock();
		}
	}
	return nullptr;
}
template <typename type>
SharedPtr<type> GPUDevice::getTrackedObject(type* ptr){
	return FindInList(ptr, created.objects);
}
template <>
SharedPtr<CShader> GPUDevice::getTrackedObject(CShader* ptr){
	return FindInList<CShader>(ptr, created.shaders);
}
template <>
SharedPtr<CShaderProgram> GPUDevice::getTrackedObject(CShaderProgram* ptr){
	return FindInList<CShaderProgram>(ptr, created.programs);
}
template <>
SharedPtr<CTexture> GPUDevice::getTrackedObject(CTexture* ptr){
	return FindInList<CTexture>(ptr, created.textures);
}


template <typename type>
SharedPtr<type> GPUDevice::FindSharedPtr(CGraphicObject* ptr){
	return  std::dynamic_pointer_cast<type, CGraphicObject>(getTrackedObject(ptr));
}
template <>
SharedPtr<CShader> GPUDevice::FindSharedPtr(CGraphicObject* ptr){
	return getTrackedObject<CShader>((CShader*)ptr);
}
template <>
SharedPtr<CShaderProgram> GPUDevice::FindSharedPtr(CGraphicObject* ptr){
	return getTrackedObject<CShaderProgram>((CShaderProgram*)ptr);
}
template <>
SharedPtr<CTexture> GPUDevice::FindSharedPtr(CGraphicObject* ptr){
	return getTrackedObject<CTexture>((CTexture*)ptr);
}
//------------------------------------------------------------------------------------

SharedPtr<CPipelineState> GPUDevice::CreatePipelineState(const SPipelineStateDesc& desc){
	auto obj = SharedPtr<CPipelineState>(__rd_new CPipelineState(this, desc));
	addTrackedObject(obj);
	return obj;
}
SharedPtr<CRenderPass> GPUDevice::CreateRenderPass(const SRenderPassDesc& desc){
	auto obj = SharedPtr<CRenderPass>(__rd_new CRenderPass(this, desc));
	addTrackedObject(obj);
	return obj;
}
SharedPtr<CBuffer> GPUDevice::CreateBuffer(const SBufferDesc& desc){
	auto obj = SharedPtr<CBuffer>(__rd_new CBuffer(this, desc));
	addTrackedObject(obj);
	return obj;
}
SharedPtr<CBuffer> rdDeviceCreateBuffer(GPUDevice* device, const SBufferDesc& desc){
	return device->CreateBuffer(desc);}

SharedPtr<CFramebuffer> GPUDevice::CreateFramebuffer(const SRenderPassDesc& desc, std::vector<SharedPtr<CTexture>> textures, SharedPtr<CTexture> depthStencilTexture){
	auto obj = SharedPtr<CFramebuffer>(__rd_new CFramebuffer(this, desc, textures, depthStencilTexture));
	addTrackedObject(obj);
	return obj;
}
//SharedPtr<CShaderResource> GPUDevice::CreateShaderResrouce(const SShaderResourceDesc& desc);
SharedPtr<CSampler> GPUDevice::CreateSampler(const SSamplerDesc& desc){
	auto obj = SharedPtr<CSampler>(__rd_new CSampler(this, desc));
	addTrackedObject(obj);
	return obj;
}
SharedPtr<CVertexBuffer> GPUDevice::CreateVertexBuffer(const SVertexFormat& desc, uint32 count, std::vector<SRawData> data){
	auto obj = SharedPtr<CVertexBuffer>(__rd_new CVertexBuffer(this, desc, count, data));
	addTrackedObject(obj);
	return obj;
}
SharedPtr<CIndexBuffer> GPUDevice::CreateIndexBuffer(EValueType type, uint32 count, SRawData data){
	auto obj = SharedPtr<CIndexBuffer>(__rd_new CIndexBuffer(this, type, count, data));
	addTrackedObject(obj);
	return obj;
}
SharedPtr<CTexture> GPUDevice::CreateTexture(const STextureDesc& desc, const STextureRawData& data){
	auto obj = SharedPtr<CTexture>(__rd_new CTexture(this, desc, data));
	addTrackedObject(obj);
	obj->CreateView(obj);
	return obj;
}
SharedPtr<CTexture> GPUDevice::CreateTexture(const STextureDesc& desc, std::string fileName){
	auto obj = SharedPtr<CTexture>(__rd_new CTexture(this, desc, fileName));
	addTrackedObject(obj);
	obj->CreateView(obj);
	return obj;
}
SharedPtr<CTextureView> GPUDevice::CreateTextureView(const STextureViewDesc& desc, SharedPtr<CTexture> tx){
	auto obj = SharedPtr<CTextureView>(__rd_new CTextureView(this, desc, tx));
	addTrackedObject(obj);
	return obj;
}
SharedPtr<CShaderResourceBinding> GPUDevice::CreateShaderResourceBinding(const SShaderResourceBindingDesc& desc, CShaderResource* resource){
	auto obj = SharedPtr<CShaderResourceBinding>(__rd_new CShaderResourceBinding(this, desc, resource));
	addTrackedObject(obj);
	return obj;
}
SharedPtr<CShaderResourceSetDesc> GPUDevice::CreateShaderResourceSetDesc(const std::vector<SShaderResourceBindingDesc>& binds){
	auto obj = SharedPtr<CShaderResourceSetDesc>(__rd_new CShaderResourceSetDesc(this, binds));
	addTrackedObject(obj);
	return obj;
}
SharedPtr<CShaderResourceSet> GPUDevice::CreateShaderResourceSet(const CShaderResourceSetDesc* desc, const std::vector<CShaderResource*>& rers){
	auto obj = SharedPtr<CShaderResourceSet>(__rd_new CShaderResourceSet(this, desc, rers));
	addTrackedObject(obj);
	return obj;
}
SharedPtr<CShaderProgram> GPUDevice::CreateShaderProgram(std::string uniquename, const std::vector<SharedPtr<CShader>> shaders){
	auto obj = SharedPtr<CShaderProgram>(__rd_new CShaderProgram(this, uniquename, shaders));
	addTrackedObject(obj);
	return obj;
}
SharedPtr<CShader> GPUDevice::CreateShaderModule(const SShaderDesc& desc){
	auto obj = SharedPtr<CShader>(__rd_new CShader(this, desc));
	addTrackedObject(obj);
	return obj;
}

//------------------------------------------------------------------------------------
// gl state setting
//------------------------------------------------------------------------------------

void GPUDevice::initPipelineState(){
	//setup different init values from the default ones so they will be set in set***State() functions
	SPipelineStateDesc newpipelineState;

	for(uint i = 0; i < RD_MAX_RENDER_ATTACHMENTS; ++i){
		newpipelineState.blendDesc.attachmentBlends[i].blendOp = EBlendOperation::Subtract;
		newpipelineState.blendDesc.attachmentBlends[i].blendEnable = false;
		newpipelineState.blendDesc.attachmentBlends[i].blendOpAlpha = EBlendOperation::Subtract;
		newpipelineState.blendDesc.attachmentBlends[i].srcBlend = EBlendFactor::Zero;
		newpipelineState.blendDesc.attachmentBlends[i].dstBlend = EBlendFactor::Zero;
		newpipelineState.blendDesc.attachmentBlends[i].srcBlendAlpha = EBlendFactor::Zero;
		newpipelineState.blendDesc.attachmentBlends[i].dstBlendAlpha = EBlendFactor::Zero;
	}
	newpipelineState.depthDesc.depthFunc = EComparisonOp::Never;
	newpipelineState.depthDesc.depthWriteEnable = true;
	newpipelineState.depthDesc.enable = false;
	newpipelineState.primitiveTopology = EPrimitiveTopology::TriangleListAdjacency;
	newpipelineState.rasterizerDesc.cullMode = ECullMode::None;
	newpipelineState.rasterizerDesc.frontFace = (EFrontFace)((int)EFrontFace::CounterClockwise+(int)(EFrontFace::Clockwise));
	newpipelineState.rasterizerDesc.fillMode = EFillMode::Wireframe;
	newpipelineState.rasterizerDesc.depthBias = 1;
	newpipelineState.rasterizerDesc.depthBiasClamp = 0.1f;
	newpipelineState.rasterizerDesc.slopeScaledDepthBias = 0.1f;
	newpipelineState.rasterizerDesc.antialiasedLineEnable = true;
	newpipelineState.rasterizerDesc.depthClampEnable = true;
	newpipelineState.samplingDesc.count = 2;
	newpipelineState.samplingDesc.sampleMask = 0;
	newpipelineState.stencilDesc.backFace.stencilFailOp = EStencilOp::Dec;
	newpipelineState.stencilDesc.backFace.stencilDepthFailOp = EStencilOp::Inc;
	newpipelineState.stencilDesc.backFace.stencilPassOp = EStencilOp::Dec;
	newpipelineState.stencilDesc.backFace.stencilFunc = EComparisonOp::Never;
	newpipelineState.stencilDesc.backFace.referenceValue = 1;
	newpipelineState.stencilDesc.backFace.ANDMask = 0;
	newpipelineState.stencilDesc.backFace.writeMask = 0;
	newpipelineState.stencilDesc.frontFace.stencilFailOp = EStencilOp::Dec;
	newpipelineState.stencilDesc.frontFace.stencilDepthFailOp = EStencilOp::Inc;
	newpipelineState.stencilDesc.frontFace.stencilPassOp = EStencilOp::Dec;
	newpipelineState.stencilDesc.frontFace.stencilFunc = EComparisonOp::Never;
	newpipelineState.stencilDesc.frontFace.referenceValue = 1;
	newpipelineState.stencilDesc.frontFace.ANDMask = 0;
	newpipelineState.stencilDesc.frontFace.writeMask = 0;
	newpipelineState.stencilDesc.enable = true;
	newpipelineState.stencilDesc.stencilReadMask = 0;
	newpipelineState.stencilDesc.stencilWriteMask = 0;
	newpipelineState.viewports.numViewports = 0;
	for(uint i = 0; i < RD_MAX_VIEWPORTS; ++i){
		newpipelineState.viewports.scissorTest[i].enable = true;
	}
	pipelineState = newpipelineState;
	/*
	this->setBlendState(newpipelineState.blendDesc);
	this->setDepthState(newpipelineState.depthDesc);
	this->setStencilState(newpipelineState.stencilDesc);
	this->setPrimitiveTopology(newpipelineState.primitiveTopology);
	this->setRasterizerState(newpipelineState.rasterizerDesc);
	this->setSampleState(newpipelineState.samplingDesc);
	this->setViewports(newpipelineState.viewports);*/
}

bool GPUDevice::setBlendState(const SBlendStateDesc& mode){
	if(mode.independentBlendEnable == false){
		if(mode.alphaToCoverageEnable == this->pipelineState.blendDesc.alphaToCoverageEnable &&
			mode.attachmentBlends[0] == this->pipelineState.blendDesc.attachmentBlends[0]) return true;}
	else{
		if(mode == this->pipelineState.blendDesc) return true; }
	
	if(mode.alphaToCoverageEnable != this->pipelineState.blendDesc.alphaToCoverageEnable){
		if(mode.alphaToCoverageEnable == true)
			gl.EnableSampleAlphaToCoverage();
		else
			gl.DisableSampleAlphaToCoverage();
		this->pipelineState.blendDesc.alphaToCoverageEnable = mode.alphaToCoverageEnable;
	}

	if(mode.independentBlendEnable == false){
		auto& blends = mode.attachmentBlends[0];
		if(blends != this->pipelineState.blendDesc.attachmentBlends[0]){
			gl.BlendFuncSeparate(glenum(blends.srcBlend),glenum(blends.dstBlend),glenum(blends.srcBlendAlpha),glenum(blends.dstBlendAlpha));
			gl.BlendEquationSeparate(glenum(blends.blendOp), glenum(blends.blendOpAlpha));
		}
		this->pipelineState.blendDesc.attachmentBlends[0] = blends;
	}
	else{
		LOG_ERR("independentBlendEnable is not supported in WebGL");
		/*for(uint i = 0; i < RD_MAX_RENDER_ATTACHMENTS; ++i){
			auto& blends = mode.attachmentBlends[i];
			if(blends != this->pipelineState.blendDesc.attachmentBlends[i]){
				gl.BlendFuncSeparatei(i, glenum(blends.srcBlend), glenum(blends.dstBlend), glenum(blends.srcBlendAlpha), glenum(blends.dstBlendAlpha));
				gl.BlendEquationSeparatei(i, glenum(blends.blendOp), glenum(blends.blendOpAlpha));
			}
			this->pipelineState.blendDesc.attachmentBlends[i] = blends;
		}*/
	}

	return true;
}

bool GPUDevice::setDepthState(const SDepthStateDesc& mode){
	if(mode == this->pipelineState.depthDesc) return true;

	if(mode.enable != this->pipelineState.depthDesc.enable){
		if(mode.enable == true)
			gl.EnableDepthTest();
		else
			gl.DisableDepthTest();
		this->pipelineState.depthDesc.enable = mode.enable;
	}

	if(mode.depthFunc != this->pipelineState.depthDesc.depthFunc){
		gl.DepthFunc(glenum(mode.depthFunc));
		this->pipelineState.depthDesc.depthFunc = mode.depthFunc;
	}

	if(mode.depthWriteEnable != this->pipelineState.depthDesc.depthWriteEnable){
		if(mode.depthWriteEnable == true)
			gl.DepthWriteEnable();
		else
			gl.DepthWriteDisable();
		this->pipelineState.depthDesc.depthWriteEnable = mode.depthWriteEnable;
	}

	return true;
}

bool GPUDevice::setStencilState(const SStencilStateDesc& mode){
	if(mode == this->pipelineState.stencilDesc) return true;

	if(mode.enable != this->pipelineState.stencilDesc.enable){
		if(mode.enable == true)
			gl.EnableStencilTest();
		else
			gl.DisableStencilTest();
		this->pipelineState.stencilDesc.enable = mode.enable;
	}

	if(mode.frontFace != this->pipelineState.stencilDesc.frontFace ||
	   mode.backFace != this->pipelineState.stencilDesc.backFace){

		if(mode.frontFace != mode.backFace){
			gl.StencilFuncSeparate(GL_FRONT, glenum(mode.frontFace.stencilFunc), mode.frontFace.referenceValue, mode.frontFace.ANDMask);
			gl.StencilFuncSeparate(GL_BACK, glenum(mode.backFace.stencilFunc), mode.backFace.referenceValue, mode.backFace.ANDMask);
			gl.StencilMaskSeparate(GL_FRONT, mode.frontFace.writeMask);
			gl.StencilMaskSeparate(GL_BACK, mode.backFace.writeMask);
			gl.StencilOpSeparate(GL_FRONT, glenum(mode.frontFace.stencilFailOp), glenum(mode.frontFace.stencilDepthFailOp), glenum(mode.frontFace.stencilPassOp));
			gl.StencilOpSeparate(GL_BACK, glenum(mode.backFace.stencilFailOp), glenum(mode.backFace.stencilDepthFailOp), glenum(mode.backFace.stencilPassOp));
		}
		else{
			gl.StencilFuncSeparate(GL_FRONT_AND_BACK, glenum(mode.frontFace.stencilFunc), mode.frontFace.referenceValue, mode.frontFace.ANDMask);
			gl.StencilMaskSeparate(GL_FRONT_AND_BACK, mode.frontFace.writeMask);
			gl.StencilOpSeparate(GL_FRONT_AND_BACK, glenum(mode.frontFace.stencilFailOp), glenum(mode.frontFace.stencilDepthFailOp), glenum(mode.frontFace.stencilPassOp));
		}
		
		this->pipelineState.stencilDesc = mode;
	}

	return true;
}

bool GPUDevice::setPrimitiveTopology(const EPrimitiveTopology& mode){
	 this->pipelineState.primitiveTopology = mode; return true;	
}

bool GPUDevice::setRasterizerState(const SRasterizerStateDesc& mode){
	if(mode == this->pipelineState.rasterizerDesc) return true;

	if(mode.fillMode != this->pipelineState.rasterizerDesc.fillMode){
		//gl.PolygonMode(GL_FRONT_AND_BACK, glenum(mode.fillMode));
		this->pipelineState.rasterizerDesc.fillMode = mode.fillMode;
	}
	if(mode.cullMode != this->pipelineState.rasterizerDesc.cullMode){
		if(mode.cullMode == ECullMode::None)
			gl.DisableCullFace();
		else{
			gl.EnableCullFace();
			gl.CullFace(glenum(mode.cullMode));
		}
		this->pipelineState.rasterizerDesc.cullMode = mode.cullMode;
	}
	if(mode.frontFace != this->pipelineState.rasterizerDesc.frontFace){
		gl.FrontFace(glenum(mode.frontFace));
		this->pipelineState.rasterizerDesc.frontFace = mode.frontFace;
	}
	if(mode.depthClampEnable != this->pipelineState.rasterizerDesc.depthClampEnable){
		/*if(mode.depthClampEnable)
			gl.EnableDepthClamp();
		else
			gl.DisableDepthClamp();*/
		this->pipelineState.rasterizerDesc.depthClampEnable = mode.depthClampEnable;
	}
	if(mode.antialiasedLineEnable != this->pipelineState.rasterizerDesc.antialiasedLineEnable){
		/*if(mode.antialiasedLineEnable)
			gl.EnableLineSmooth();
		else
			gl.DisableLineSmooth();*/
		this->pipelineState.rasterizerDesc.antialiasedLineEnable = mode.antialiasedLineEnable;
	}

	if(mode.slopeScaledDepthBias != this->pipelineState.rasterizerDesc.slopeScaledDepthBias){
		if(mode.slopeScaledDepthBias == 0.0f)
			gl.DisablePolygonOffsetFill();
		else
			gl.EnablePolygonOffsetFill();
		gl.PolygonOffset(mode.slopeScaledDepthBias, 0.0f);
		this->pipelineState.rasterizerDesc.slopeScaledDepthBias = mode.slopeScaledDepthBias;
	}	

	return true;
}

bool GPUDevice::setSampleState(const SSampleDesc& mode){
	if(mode == this->pipelineState.samplingDesc) return true;

	if(mode.count != this->pipelineState.samplingDesc.count){
		/*if(mode.count <= 1)
			gl.DisableMultisample();
		else
			gl.EnableMultisample();*/
		this->pipelineState.samplingDesc.count = mode.count;
	}

	if(mode.sampleMask != this->pipelineState.samplingDesc.sampleMask){
		/*if(mode.sampleMask == 0xffffffff)
			gl.DisableSampleMask();
		else
			gl.EnableSampleMask();
		gl.SampleMaski(0, mode.sampleMask);*/
		this->pipelineState.samplingDesc.sampleMask = mode.sampleMask;
	}
	//LOG_ERR("not implemented!");
	return true;
}

bool GPUDevice::setViewports(const SViewports& mode){
	if(mode == this->pipelineState.viewports) return true;

	if(mode.numViewports == 1){
		gl.Viewport(mode.viewports[0].x, mode.viewports[0].y, mode.viewports[0].width, mode.viewports[0].height);
		
		if(mode.scissorTest[0] != this->pipelineState.viewports.scissorTest[0]){
			if(mode.scissorTest[0].enable){
				gl.EnableScissorTest();
				gl.Scissor(mode.scissorTest[0].x, mode.scissorTest[0].y, mode.scissorTest[0].width, mode.scissorTest[0].height);
			}
			else{
				gl.DisableScissorTest();
			}
		}
	}
	else{
		LOG_ERR("multiple glViewports are not supported in WebGL");
		/*for(uint i = 0; i < mode.numViewports; ++i){
			if(mode.viewports[i] != this->pipelineState.viewports.viewports[i])
				gl.ViewportIndexedf(i, mode.viewports[i].x, mode.viewports[i].y, mode.viewports[i].width, mode.viewports[i].height);
		}
		for(uint i = 0; i < mode.numViewports; ++i){
			if(mode.scissorTest[i] != this->pipelineState.viewports.scissorTest[i]){
				if(mode.scissorTest[i].enable){
					gl.EnableScissorTest(i);
					gl.ScissorIndexed(i, mode.scissorTest[i].x, mode.scissorTest[i].y, mode.scissorTest[i].width, mode.scissorTest[i].height);
				}
				else
					gl.DisableScissorTest(i);
			}
		}*/
	}

	this->pipelineState.viewports = mode;

	return true;
}

bool GPUDevice::setDynamicViewports(const SViewports& mode){
	if(this->pipelineState.viewports.dynamicViewportEnable == false) return false;
	return this->setViewports(mode);
}
bool GPUDevice::setDynamicViewport(uint i, const SViewport& mode){
	if(this->pipelineState.viewports.dynamicViewportEnable == false) return false;
	if(i >= this->pipelineState.viewports.numViewports) return false;
	this->pipelineState.viewports.viewports[i] = mode;
	if(i != 0){
		LOG_ERR("multiple glViewports are not supported in WebGL");}
	else
		gl.Viewport(mode.x, mode.y, mode.width, mode.height);
	//gl.ViewportIndexedf(i, mode.x, mode.y, mode.width, mode.height);
	return true;
}

bool GPUDevice::bindShaderProgram(SharedPtr<CShaderProgram>& shader){
	if(shader == nullptr) return false;
	gl.UseProgram(shader->getId());
	return true;
}
//------------------------------------------------------------------------------------

// public functions
//------------------------------------------------------------------------------------
void GPUDevice::ClearAttachments(CRenderPass* rp, CFramebuffer* fb, SClearColorValues clear){

	auto& descriptor = rp->getDescriptor();

	bool attachmentClear[RD_MAX_RENDER_ATTACHMENTS]; memset(attachmentClear, 0, sizeof(bool)* RD_MAX_RENDER_ATTACHMENTS);
	bool depthStencilClear = false;
	bool colorClear = false;
	int colorClearCount = 0;

	for(uint i = 0; i < RD_MAX_RENDER_ATTACHMENTS && i < descriptor.nofAttachments; ++i){
		if(descriptor.attachments[i].loadOp == ELoadStoreOp::Clear ||
		   descriptor.attachments[i].loadOp == ELoadStoreOp::DontCare){
			attachmentClear[i] = true; colorClear = true; ++colorClearCount; }
		else
			attachmentClear[i] = false;
	}
	if(descriptor.depthStencil.loadOp == ELoadStoreOp::Clear ||
	   descriptor.depthStencil.loadOp == ELoadStoreOp::DontCare)
		depthStencilClear = true;
	else
		depthStencilClear = false;

	GLbitfield flags = 0;
	
	this->bindFramebuffer(fb);

	if(colorClearCount != descriptor.nofAttachments){
		for(uint i = 0, d = 0; i < RD_MAX_RENDER_ATTACHMENTS && i < descriptor.nofAttachments; ++i){
			if(attachmentClear[i] == true){
				gl.ClearBufferfv(GL_COLOR, GL_COLOR_ATTACHMENT0+i, &clear.color[i].x);
			}
		}
		if(depthStencilClear == true)
			gl.ClearBufferfi(GL_DEPTH_STENCIL, 0, clear.depth, clear.stencil);
	}
	else{ //clear all
		if(colorClear == true){
			gl.ClearColor(clear.color[0].x, clear.color[0].y, clear.color[0].z, clear.color[0].w);
			flags |= GL_COLOR_BUFFER_BIT;
		}
		if(depthStencilClear == true){
			gl.ClearDepthf(clear.depth);
			flags |= GL_DEPTH_BUFFER_BIT;
			gl.ClearStencil(clear.stencil);
			flags |= GL_STENCIL_BUFFER_BIT;
		}
		gl.Clear(flags);
	}
}

void GPUDevice::bindFramebuffer(CFramebuffer* framebuffer){
	if(boundFramebuffer == framebuffer) return;
	gl.BindFramebuffer(GL_FRAMEBUFFER, framebuffer->getId());
	boundFramebuffer = framebuffer;
}

bool GPUDevice::PresentFrame(){
	gl.Flush();
	#ifdef EGL_SDL_context
		window.SwapBackbuffer();
	#endif
	#ifdef EGL_GL_context
		eglSwapBuffers(context.display, context.surface);
	#endif

	return true;
}
//------------------------------------------------------------------------------------

//------------------------------------------------------------------------------------
// Draw functions
//------------------------------------------------------------------------------------
bool GPUDevice::DrawIndexed(uint count){
	if(boundVertexBuffer == nullptr) return false;
	if(boundIndexBuffer == nullptr) return false;

	if(count == 0) count = boundIndexBuffer->getCount();

	gl.DrawElements(glenum(this->pipelineState.primitiveTopology), count, glenum(boundIndexBuffer->getType()), nullptr);
	return true;
}
//------------------------------------------------------------------------------------

bool rdSetupDeviceForGlobalObjects(GPUDevice* device){
	CSingleton<CShaderResourceSetManager>::get()->setDevice(device);
	CSingleton<CShaderModuleManager>::get()->setDevice(device);
	CSingleton<CShaderProgramManager>::get()->setDevice(device);
	CSingleton<CTextureManager>::get()->setDevice(device);
	rdInitUniformBufferStructreRegistry();

	return true;
}

#endif //RD_API_OPENGL4