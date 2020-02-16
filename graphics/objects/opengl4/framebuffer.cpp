#ifdef RD_API_OPENGL4

#include "device.h"
#include "opengl4/framebuffer.h"
#include "texture.h"

void CFramebuffer::Release(){
	if(this->id != 0)
		device->gl.DeleteFramebuffers(1, &this->id);
	this->id = 0;
}

bool CFramebuffer::CreateAndSetupAttachments(){
	if(this->device == nullptr) return false;
	this->Release();

	device->gl.GenFramebuffers(1, &this->id);
	device->gl.BindFramebuffer(GL_FRAMEBUFFER, this->id);

	for(uint i = 0; i < RD_MAX_RENDER_ATTACHMENTS; ++i){
		if(this->Attachments[i] != nullptr){
			GLenum textarget = (this->Attachments[i]->isTextureCubeMap() && i < 6)? GL_TEXTURE_CUBE_MAP_POSITIVE_X+i : GL_TEXTURE_2D;
			device->gl.FramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0+i, textarget, this->Attachments[i]->getId(), 0);
		}
	}
	if(this->DepthStencil != nullptr){
		device->gl.FramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_STENCIL, GL_TEXTURE_2D, this->DepthStencil->getId(), 0);
	}

	return true;
}

#endif //RD_API_OPENGL4