
#ifdef RD_API_BASE
#include "base/pipeline_state.cpp"
#endif

#ifdef RD_API_OPENGL4
#include "opengl4/pipeline_state.cpp"
#endif

#ifdef RD_API_WEBGL
#include "webgl/pipeline_state.cpp"
#endif

#ifdef RD_API_VULKAN
#endif