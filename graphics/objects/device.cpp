
#ifdef RD_API_BASE
#include "base/device.cpp"
#endif

#ifdef RD_API_OPENGL4
#include "opengl4/device.cpp"
#endif

#ifdef RD_API_WEBGL
#include "webgl/device.cpp"
#endif

#ifdef RD_API_VULKAN
#endif