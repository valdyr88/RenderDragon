#ifdef RD_API_OPENGL4

#include "device.h"
#include "render_pass.h"

bool CRenderPass::begin(CFramebuffer* fb, SClearColorValues clear){
	if(device == nullptr) return false;
	if(fb == nullptr) return false;

	boundFramebuffer = fb;
	clearColor = clear;

	device->ClearAttachments(this, boundFramebuffer, clearColor);
	return true;
}


#endif
