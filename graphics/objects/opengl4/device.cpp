#ifdef RD_API_OPENGL4

#include "../../utils/platform_defines.h"
#include "device.h"

UniquePtr<GPUDevice> CreateGPUDevice(const SGPUDeviceDesc& desc){
	return NewUnique<GPUDevice>(desc);
}

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
			LOG("wrong swapchain format!")
				break;
	}

	byte depth_buffer_bits = 0;
	byte stencil_buffer_bits = 0;
	switch(descriptor.swapchain.format)
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
		LOG("wglCreateContext() error = 0x%08x\n", error); fflush(stdout);
	}

	if(wglMakeCurrent(window.windowDeviceContext, GPUDeviceContext.context)){
		LOG("init OpenGL success!");
	}
	else{
		LOG("wglMakeCurrent() error = 0x%08x\n", error); fflush(stdout);
	}
#endif //PLATFORM_WINDOWS

	return GPUDeviceContext;
}

SGPUDeviceContext rdInitOpenGL_ARB(const SWindow& window, const SGPUDeviceDesc& descriptor, int GLVersion_Major, int GLVersion_Minor){
	SGPUDeviceContext context;
	
	if(glewInit() == GLEW_OK){ //to fetch OpenGL functions
		LOG("glewInit() failed!"); fflush(stdout); }

	auto wglChoosePixelFormatARB = ( PFNWGLCHOOSEPIXELFORMATARBPROC) wglGetProcAddress("wglChoosePixelFormatARB");
	auto wglCreateContextAttribsARB = ( PFNWGLCREATECONTEXTATTRIBSARBPROC) wglGetProcAddress("wglCreateContextAttribsARB");

	if(wglChoosePixelFormatARB == NULL || wglCreateContextAttribsARB == NULL){
		LOG("wglChoosePixelFormatARB == NULL || wglCreateContextAttribsARB == NULL!\n"); fflush(stdout); }


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
			LOG("wrong swapchain format!")
				break;
	}

	byte color_bits = 8*sizeInBytes(descriptor.swapchain.type);
	byte depth_buffer_bits = 8*sizeInBytes(descriptor.swapchain.depthType);
	byte stencil_buffer_bits = 8*sizeInBytes(descriptor.swapchain.stencilType);

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
	
	//TODO: destroy current window and create new one to use new pixel format

	return context;
}

SGPUDeviceContext GPUDevice::InitOpenGL(const SWindow& window){
	auto context = rdInitOpenGL(window, this->descriptor);
	context = rdInitOpenGL_ARB(window, this->descriptor, 4, 0);
	return context;
}

#endif //RD_API_OPENGL4