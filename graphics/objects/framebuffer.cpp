
#ifdef RD_API_BASE
#include "base/framebuffer.cpp"
#endif

#ifdef RD_API_OPENGL4
#include "opengl4/framebuffer.cpp"
#endif

#ifdef RD_API_WEBGL
#include "webgl/framebuffer.cpp"
#endif

#ifdef RD_API_VULKAN
#endif