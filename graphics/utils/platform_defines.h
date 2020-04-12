#ifndef PLATFORM_DEFINES_H
#define PLATFORM_DEFINES_H

#if defined(__WIN32__) || defined(_WIN32) || defined(_WIN64)
	#define PLATFORM_WINDOWS

	#ifdef _WIN64
		#define PLATFOM_64BIT
	#elif _WIN32
		#define PLATFORM_32BIT
	#endif

	#define NOMINMAX

#endif //PLATFORM_WINDOWS

#ifdef __LINUX__
	#define PLATFORM_LINUX
#endif //PLATFORM_LINUX

#ifdef __APPLE__
	#define PLATFORM_MAC
#endif //PLATFORM_MAC

#endif