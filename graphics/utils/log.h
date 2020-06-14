#ifndef LOG_H
#define LOG_H

#ifdef _DEBUG

#include <stdio.h>

#ifdef _MSC_VER
	#define debugbreak __debugbreak()
#else
	#define debugbreak
#endif

#ifdef _MSC_VER
	#define LOG_ERR(format, ...)	{ printf_s("Error: " __FUNCTION__ "() " format "\n", __VA_ARGS__); debugbreak; }
	#define LOG_WARN(format, ...)	{ printf_s("Warning: " __FUNCTION__ "() " format "\n", __VA_ARGS__); }
	#define LOG(format, ...)		{ printf_s(format "\n", __VA_ARGS__); }

	#define ASSERT(x) {if(!(x)) LOG_ERR(#x" is false");} 
#endif //_MSC_VER

#ifdef __EMSCRIPTEN__
	#define LOG_ERR(format, args...)	{ printf("Error: %s() " format "\n", __func__, ##args); debugbreak; }
	#define LOG_WARN(format, args...)	{ printf("Warning: %s() " format "\n", __func__, ##args); }
	#define LOG(format, args...)		{ printf(format "\n", ##args); }

	#define ASSERT(x) {if(!(x)) LOG_ERR(#x" is false");} 
#endif //__EMSCRIPTEN__

#else

#define debugbreak
#define LOG_ERR(format, ...)	{}
#define LOG_WARN(format, ...)	{}
#define LOG(format, ...)		{}
#define ASSERT(x)				{}

#endif

#endif //LOG_H