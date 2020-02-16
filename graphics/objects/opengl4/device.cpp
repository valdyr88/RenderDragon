#ifdef RD_API_OPENGL4

#include "../../utils/platform_defines.h"
#include "device.h"

SGPUDeviceContext rdInitOpenGL(const SWindow& window, const SGPUDeviceDesc& descriptor){
	SGPUDeviceContext GPUDeviceContext;

#ifdef PLATFORM_WINDOWS

	uint PFD_MULTIBUFFER_TYPE = 0;
	switch(descriptor.swapchain.buffering)
	{
		case ESwapchainBuffer::SingleBuffer:
			PFD_MULTIBUFFER_TYPE = PFD_DOUBLEBUFFER_DONTCARE;
			break;
		case ESwapchainBuffer::DoubleBuffer:
			PFD_MULTIBUFFER_TYPE = PFD_DOUBLEBUFFER;
			break;
		case ESwapchainBuffer::TripleBuffer:
			PFD_MULTIBUFFER_TYPE = PFD_DOUBLEBUFFER;
			break;
	}

	byte PFD_TYPE = 0;
	switch(descriptor.swapchain.format)
	{
		case ETextureFormat::RGB:
			PFD_TYPE = PFD_TYPE_RGBA;
			break;
		case ETextureFormat::RGBA:
			PFD_TYPE = PFD_TYPE_RGBA;
			break;
		default:
			LOG("wrong swapchain format!");
			break;
	}

	byte depth_buffer_bits = 0;
	byte stencil_buffer_bits = 0;
	switch(descriptor.swapchain.depthFormat)
	{
		case ETextureFormat::Depth:
			stencil_buffer_bits = 0;
			depth_buffer_bits = 8*sizeInBytes(descriptor.swapchain.depthType);
			break;
		case ETextureFormat::DepthStencil:
			depth_buffer_bits = 8*sizeInBytes(descriptor.swapchain.depthType);
			stencil_buffer_bits = 8*sizeInBytes(descriptor.swapchain.stencilType);
			break;
		default:
			break;
	}

	PIXELFORMATDESCRIPTOR pfd =
	{
		sizeof(PIXELFORMATDESCRIPTOR),
		1,
		PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_MULTIBUFFER_TYPE,    //Flags
		PFD_TYPE,            //The kind of framebuffer. RGBA or palette.
		(byte)(depth_buffer_bits + stencil_buffer_bits),//Colordepth of the framebuffer.
		0, 0, 0, 0, 0, 0,
		0,
		0,
		0,
		0, 0, 0, 0,
		depth_buffer_bits,       //Number of bits for the depthbuffer
		stencil_buffer_bits,      //Number of bits for the stencilbuffer
		0,                        //Number of Aux buffers in the framebuffer.
		PFD_MAIN_PLANE,
		0,
		0, 0, 0
	};

	int indexOfPixelFormat = ChoosePixelFormat(window.windowDeviceContext, &pfd);
	SetPixelFormat(window.windowDeviceContext, indexOfPixelFormat, &pfd);

	GPUDeviceContext.context = wglCreateContext(window.windowDeviceContext);

	if(GPUDeviceContext.context == nullptr){
		DWORD error = GetLastError();
		LOG("wglCreateContext() error = 0x%08x\n", error);
	}

	if(wglMakeCurrent(window.windowDeviceContext, GPUDeviceContext.context)){
		LOG("init OpenGL success!");
		LOG("GL version: %s", glGetString(GL_VERSION));
	}
	else{
		DWORD error = GetLastError();
		LOG("wglMakeCurrent() error = 0x%08x\n", error);
	}
#endif //PLATFORM_WINDOWS

	return GPUDeviceContext;
}

SGPUDeviceContext rdInitOpenGL_ARB(SWindow& window, const SGPUDeviceDesc& descriptor, SGPUDeviceContext& context, int GLVersion_Major, int GLVersion_Minor){
	
	if(glewInit() != GLEW_OK){ //to fetch OpenGL functions
		LOG("glewInit() failed!"); }

	auto wglChoosePixelFormatARB = ( PFNWGLCHOOSEPIXELFORMATARBPROC) wglGetProcAddress("wglChoosePixelFormatARB");
	auto wglCreateContextAttribsARB = ( PFNWGLCREATECONTEXTATTRIBSARBPROC) wglGetProcAddress("wglCreateContextAttribsARB");

	if(wglChoosePixelFormatARB == NULL || wglCreateContextAttribsARB == NULL){
		LOG("wglChoosePixelFormatARB == NULL || wglCreateContextAttribsARB == NULL!\n"); }


	int WGL_BUFFER_TYPE = 0; int WGL_BUFFER_TYPE_BOOL = 0;
	switch(descriptor.swapchain.buffering)
	{
		case ESwapchainBuffer::SingleBuffer:
			WGL_BUFFER_TYPE = WGL_DOUBLE_BUFFER_ARB;
			WGL_BUFFER_TYPE_BOOL = GL_FALSE;
			break;
		case ESwapchainBuffer::DoubleBuffer:
			WGL_BUFFER_TYPE = WGL_DOUBLE_BUFFER_ARB;
			WGL_BUFFER_TYPE_BOOL = GL_TRUE;
			break;
		case ESwapchainBuffer::TripleBuffer:
			WGL_BUFFER_TYPE = WGL_DOUBLE_BUFFER_ARB;
			WGL_BUFFER_TYPE_BOOL = GL_TRUE;
			break;
	}

	int WGL_TYPE = 0;
	switch(descriptor.swapchain.format)
	{
		case ETextureFormat::RGB:
			WGL_TYPE = WGL_TYPE_RGBA_ARB;
			break;
		case ETextureFormat::RGBA:
			WGL_TYPE = WGL_TYPE_RGBA_ARB;
			break;
		default:
			LOG("wrong swapchain format!");
			break;
	}

	byte color_bits = 8*sizeInBytes(descriptor.swapchain.type)*count(descriptor.swapchain.format);
	byte depth_buffer_bits = isDepthFormat(descriptor.swapchain.depthFormat)? 8*sizeInBytes(descriptor.swapchain.depthType) : 0;
	byte stencil_buffer_bits = isStencilFormat(descriptor.swapchain.depthFormat)? 8*sizeInBytes(descriptor.swapchain.stencilType) : 0;

	const int pixelFormatAttribList[] =
	{
		WGL_DRAW_TO_WINDOW_ARB, GL_TRUE,
		WGL_SUPPORT_OPENGL_ARB, GL_TRUE,
		WGL_BUFFER_TYPE, WGL_BUFFER_TYPE_BOOL,
		WGL_PIXEL_TYPE_ARB, WGL_TYPE,
		WGL_COLOR_BITS_ARB, color_bits,
		WGL_DEPTH_BITS_ARB, depth_buffer_bits,
		WGL_STENCIL_BITS_ARB, stencil_buffer_bits,
		0, 0      //End
	};
	int pixelFormat;
	UINT numFormats;

	if(wglChoosePixelFormatARB(window.windowDeviceContext, pixelFormatAttribList, NULL, 1, &pixelFormat, &numFormats) == false){
		LOG("wglChoosePixelFormatARB() == false!\n"); fflush(stdout);
	}
	LOG("wglChoosePixelFormatARB() PixelFormat chosen == %d\n", pixelFormat);
	
	//ToDo: destroy current window and create new one to use new pixel format
	DestroyWindow(window.window); window.window = nullptr;
	window.CreateProgramWindow(window.name, window.width, window.height, window.posX, window.posY, window.flags, true);

	PIXELFORMATDESCRIPTOR pfd; memset(&pfd, 0, sizeof(PIXELFORMATDESCRIPTOR)); pfd.nSize = sizeof(PIXELFORMATDESCRIPTOR);
	SetPixelFormat(window.windowDeviceContext, pixelFormat, &pfd);
	
	const int contextAttribList[] =
	{
		WGL_CONTEXT_MAJOR_VERSION_ARB, GLVersion_Major,
		WGL_CONTEXT_MINOR_VERSION_ARB, GLVersion_Minor,
		0,0
	};

	auto temp_hglrc = context.context;
	context.context = nullptr;
	
	context.context = wglCreateContextAttribsARB(window.windowDeviceContext, NULL, contextAttribList);

	if(context.context == nullptr){
		LOG("wglCreateContextAttribsARB() == false!\n"); }
	else
		LOG("GL version: %s", glGetString(GL_VERSION));
	
	wglDeleteContext(temp_hglrc); temp_hglrc = nullptr;

	if(wglMakeCurrent(window.windowDeviceContext, NULL) == false){
		LOG("wglMakeCurrent(hdc, NULL) == false!\n"); }

	if(wglMakeCurrent(window.windowDeviceContext, context.context) == false){
		DWORD error = GetLastError();
		LOG("wglMakeCurrent(hdc, hglrc) == false!, error == 0x%08x\n", error); }

	return context;
}

SGPUDeviceContext GPUDevice::InitOpenGL(SWindow& window){
	context = rdInitOpenGL(window, this->descriptor);
	context = rdInitOpenGL_ARB(window, this->descriptor, context, 4, 0);
	return context;
}

bool GPUDevice::InitContextOnWindow(SWindow& window){
	context = this->InitOpenGL(window);
	return true;
}

UniquePtr<GPUDevice> GPUDevice::CreateGPUDevice(const SGPUDeviceDesc& desc){
	return NewUnique<GPUDevice>(desc);
}

SharedPtr<CPipelineState> GPUDevice::CreatePipelineState(const SPipelineStateDesc& desc){
	auto obj = SharedPtr<CPipelineState>(new CPipelineState(this, desc));
	objects.emplace_back(obj);
	return obj;
}
SharedPtr<CRenderPass> GPUDevice::CreateRenderPass(const SRenderPassDesc& desc){
	auto obj = SharedPtr<CRenderPass>(new CRenderPass(this, desc));
	objects.emplace_back(obj);
	return obj;
}
SharedPtr<CBuffer> GPUDevice::CreateBuffer(const SBufferDesc& desc){
	auto obj = SharedPtr<CBuffer>(new CBuffer(this, desc));
	objects.emplace_back(obj);
	return obj;
}
SharedPtr<CBuffer> rdDeviceCreateBuffer(GPUDevice* device, const SBufferDesc& desc){
	return device->CreateBuffer(desc);}

SharedPtr<CFramebuffer> GPUDevice::CreateFramebuffer(const SRenderPassDesc& desc, std::vector<SharedPtr<CTexture>> textures, SharedPtr<CTexture> depthStencilTexture){
	auto obj = SharedPtr<CFramebuffer>(new CFramebuffer(this, desc, textures, depthStencilTexture));
	objects.emplace_back(obj);
	return obj;
}
//SharedPtr<CShader> GPUDevice::CreateShaderModule(const SShaderDesc& desc);
//SharedPtr<CShaderResource> GPUDevice::CreateShaderResrouce(const SShaderResourceDesc& desc);
SharedPtr<CSampler> GPUDevice::CreateSampler(const SShaderResourceDesc& sr, const SSamplerDesc& desc){
	auto obj = SharedPtr<CSampler>(new CSampler(this, sr, desc));
	objects.emplace_back(obj);
	return obj;
}
SharedPtr<CVertexBuffer> GPUDevice::CreateVertexBuffer(const SVertexFormat& desc, uint32 count){
	auto obj = SharedPtr<CVertexBuffer>(new CVertexBuffer(this, desc, count));
	objects.emplace_back(obj);
	return obj;
}
SharedPtr<CIndexBuffer> GPUDevice::CreateIndexBuffer(EValueType type, uint32 count){
	auto obj = SharedPtr<CIndexBuffer>(new CIndexBuffer(this, type, count));
	objects.emplace_back(obj);
	return obj;
}
SharedPtr<CTexture> GPUDevice::CreateTexture(const STextureDesc& desc, const STextureRawData& data){
	auto obj = SharedPtr<CTexture>(new CTexture(this, desc, data));
	objects.emplace_back(obj);
	return obj;
}

//------------------------------------------------------------------------------------
// gl state setting
//------------------------------------------------------------------------------------

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
		for(uint i = 0; i < RD_MAX_RENDER_ATTACHMENTS; ++i){
			auto& blends = mode.attachmentBlends[i];
			if(blends != this->pipelineState.blendDesc.attachmentBlends[i]){
				gl.BlendFuncSeparatei(i, glenum(blends.srcBlend), glenum(blends.dstBlend), glenum(blends.srcBlendAlpha), glenum(blends.dstBlendAlpha));
				gl.BlendEquationSeparatei(i, glenum(blends.blendOp), glenum(blends.blendOpAlpha));
			}
			this->pipelineState.blendDesc.attachmentBlends[i] = blends;
		}
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
		gl.DepthMask(mode.depthWriteEnable);
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
			gl.EnableStencilTest();
		this->pipelineState.stencilDesc.enable = mode.enable;
	}

	if(mode.frontFace != this->pipelineState.stencilDesc.frontFace ||
	   mode.backFace != this->pipelineState.stencilDesc.backFace){

		if(mode.frontFace != mode.backFace){
			gl.StencilFuncSeparate(glenum(mode.frontFace.stencilFunc), glenum(mode.backFace.stencilFunc), mode.frontFace.referenceValue, mode.frontFace.ANDMask);
			gl.StencilMaskSeparate(GL_FRONT, mode.frontFace.writeMask);
			gl.StencilMaskSeparate(GL_BACK, mode.backFace.writeMask);
			gl.StencilOpSeparate(GL_FRONT, glenum(mode.frontFace.stencilFailOp), glenum(mode.frontFace.stencilDepthFailOp), glenum(mode.frontFace.stencilPassOp));
			gl.StencilOpSeparate(GL_BACK, glenum(mode.backFace.stencilFailOp), glenum(mode.backFace.stencilDepthFailOp), glenum(mode.backFace.stencilPassOp));
		}
		else{
			gl.StencilFuncSeparate(glenum(mode.frontFace.stencilFunc), glenum(mode.backFace.stencilFunc), mode.frontFace.referenceValue, mode.frontFace.ANDMask);
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
		gl.PolygonMode(GL_FRONT_AND_BACK, glenum(mode.fillMode));
		this->pipelineState.rasterizerDesc.fillMode = mode.fillMode;
	}
	if(mode.cullMode != this->pipelineState.rasterizerDesc.cullMode){
		gl.CullFace(glenum(mode.cullMode));
		this->pipelineState.rasterizerDesc.cullMode = mode.cullMode;
	}
	if(mode.frontFace != this->pipelineState.rasterizerDesc.frontFace){
		gl.FrontFace(glenum(mode.frontFace));
		this->pipelineState.rasterizerDesc.frontFace = mode.frontFace;
	}
	if(mode.depthClampEnable != this->pipelineState.rasterizerDesc.depthClampEnable){
		if(mode.depthClampEnable)
			gl.EnableDepthClamp();
		else
			gl.DisableDepthClamp();
		this->pipelineState.rasterizerDesc.depthClampEnable = mode.depthClampEnable;
	}
	if(mode.antialiasedLineEnable != this->pipelineState.rasterizerDesc.antialiasedLineEnable){
		if(mode.antialiasedLineEnable)
			gl.EnableLineSmooth();
		else
			gl.DisableLineSmooth();
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
	//ToDo: implement this

	LOG_ERR("not implemented!");
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
		for(uint i = 0; i < mode.numViewports; ++i){
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
		}
	}

	this->pipelineState.viewports = mode;

	return true;
}


bool GPUDevice::bindShaderProgram(SharedPtr<CShaderProgram>& shader){
	//ToDo: implement this

	LOG_ERR("not implemented!");
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

	for(uint i = 0; i < RD_MAX_RENDER_ATTACHMENTS; ++i){
		if(descriptor.Attachments[i].loadOp == ELoadStoreOp::Clear ||
		   descriptor.Attachments[i].loadOp == ELoadStoreOp::DontCare){
			attachmentClear[i] = true; colorClear = true; }
		else
			attachmentClear[i] = false;
	}
	if(descriptor.DepthStencil.loadOp == ELoadStoreOp::Clear ||
	   descriptor.DepthStencil.loadOp == ELoadStoreOp::DontCare)
		depthStencilClear = true;
	else
		depthStencilClear = false;

	//ToDo: implement this, activate coresponding attachment with glDrawBuffers() and glDepthWrite(true/false) & glStencilWrite(true/false) and clear those attachments
	LOG_WARN("not implemented completely!");
	GLbitfield flags = 0;
	
	this->bindFramebuffer(fb);

	if(colorClear == true){
		gl.ClearColor(clear.color[0].r, clear.color[0].g, clear.color[0].b, clear.color[0].a);
		flags |= GL_COLOR_BUFFER_BIT;
	}
	if(depthStencilClear == true){
		gl.ClearDepth(clear.depth);
		flags |= GL_DEPTH_BUFFER_BIT;
		gl.ClearStencil(clear.stencil);
		flags |= GL_STENCIL_BUFFER_BIT;
	}

	gl.Clear(flags);
}

void GPUDevice::bindFramebuffer(CFramebuffer* framebuffer){
	if(boundFramebuffer == framebuffer) return;
	gl.BindFramebuffer(GL_FRAMEBUFFER, framebuffer->getId());
	boundFramebuffer = framebuffer;
}
//------------------------------------------------------------------------------------

#endif //RD_API_OPENGL4