#ifndef LOG_H
#define LOG_H

#ifdef _DEBUG

#define LOG_ERR(format, ...)	printf_s("Error: "format"\n", __VA_ARGS__)
#define LOG_WARN(format, ...)	printf_s("Warning: "format"\n", __VA_ARGS__)
#define LOG(format, ...)		printf_s(format"\n", __VA_ARGS__)

#else

#define LOG_ERR(format, ...)
#define LOG_WARN(format, ...)
#define LOG(format, ...)

#endif

#endif //LOG_H