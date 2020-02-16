#ifdef PLATFORM_WINDOWS

#define GLEW_STATIC
#include "gl\glew.h"
#include "gl\gl.h"
#include "gl\glext.h"
#include "gl\wglext.h"

#endif
#ifdef PLATFORM_LINUX

#include <gl/glx.h>
#include <X11/X.h>
#include <X11/Xlib.h>

#endif
#ifdef PLATFORM_MAC
#endif

#include "gl/GLState.h"