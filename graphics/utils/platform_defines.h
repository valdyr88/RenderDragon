#ifndef PLATFORM_DEFINES_H
#define PLATFORM_DEFINES_H

#if defined(__EMSCRIPTEN__)
	#define PLATFORM_EMSCRIPTEN
	#define EGL_GL_context
	//#define SDL_GL_context

//PLATFORM_EMSCRIPTEN
#elif defined(__WIN32__) || defined(_WIN32) || defined(_WIN64)
	#define PLATFORM_WINDOWS

	#ifdef _WIN64
		#define PLATFOM_64BIT
	#elif _WIN32
		#define PLATFORM_32BIT
	#endif

	#define NOMINMAX

//PLATFORM_WINDOWS
#elif defined(__LINUX__)
	#define PLATFORM_LINUX
//PLATFORM_LINUX
#elif defined(__APPLE__)
	#define PLATFORM_MAC
//PLATFORM_MAC
#endif 

#endif //PLATFORM_DEFINES_H