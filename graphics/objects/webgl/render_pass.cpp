#ifdef RD_API_WEBGL

#include "device.h"
#include "render_pass.h"

bool CRenderPass::DoAttachmentsNeedClearing(){
	for(uint i = 0; i < descriptor.nofAttachments; ++i)
		if(descriptor.attachments[i].loadOp == ELoadStoreOp::Clear || descriptor.attachments[i].loadOp == ELoadStoreOp::DontCare)
			return true;
	if(descriptor.depthStencil.loadOp == ELoadStoreOp::Clear || descriptor.depthStencil.loadOp == ELoadStoreOp::DontCare)
		return true;
	return false;
}

bool CRenderPass::Begin(CFramebuffer* fb, SClearColorValues clear){
	if(device == nullptr) return false;
	if(fb == nullptr) return false;

	boundFramebuffer = fb;
	clearColor = clear;

	if(this->DoAttachmentsNeedClearing())
		device->ClearAttachments(this, boundFramebuffer, clearColor);
	else
		device->bindFramebuffer(boundFramebuffer);

	return true;
}


#endif
