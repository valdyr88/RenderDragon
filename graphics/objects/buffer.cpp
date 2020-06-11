
#ifdef RD_API_BASE
#include "base/buffer.cpp"
#endif

#ifdef RD_API_OPENGL4
#include "opengl4/buffer.cpp"
#endif

#ifdef RD_API_WEBGL
#include "webgl/buffer.cpp"
#endif

#ifdef RD_API_VULKAN
#endif