#ifndef LOG_H
#define LOG_H

#ifdef _DEBUG

#include <stdio.h>

#ifdef _MSC_VER
	#define debugbreak __debugbreak()
#else
	#define debugbreak raise(SIGTRAP)
#endif

#define LOG_ERR(format, ...)	{ printf_s("Error: " __FUNCTION__ "() " format "\n", __VA_ARGS__); debugbreak; }
#define LOG_WARN(format, ...)	printf_s("%s() Warning: " __FUNCTION__ "() " format "\n", __FUNCTION__, __VA_ARGS__)
#define LOG(format, ...)		printf_s(format "\n", __VA_ARGS__)

#else

#define LOG_ERR(format, ...)
#define LOG_WARN(format, ...)
#define LOG(format, ...)

#endif

#endif //LOG_H