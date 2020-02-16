#ifdef RD_API_OPENGL4

#include "device.h"
#include "render_pass.h"

bool CRenderPass::doAttachmentsNeedClearing(){
	for(uint i = 0; i < descriptor.NofAttachments; ++i)
		if(descriptor.Attachments[i].loadOp == ELoadStoreOp::Clear || descriptor.Attachments[i].loadOp == ELoadStoreOp::DontCare)
			return true;
	if(descriptor.DepthStencil.loadOp == ELoadStoreOp::Clear || descriptor.DepthStencil.loadOp == ELoadStoreOp::DontCare)
		return true;
	return false;
}

bool CRenderPass::begin(CFramebuffer* fb, SClearColorValues clear){
	if(device == nullptr) return false;
	if(fb == nullptr) return false;

	boundFramebuffer = fb;
	clearColor = clear;

	if(this->doAttachmentsNeedClearing())
		device->ClearAttachments(this, boundFramebuffer, clearColor);
	else
		device->bindFramebuffer(boundFramebuffer);

	return true;
}


#endif
