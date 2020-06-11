#include "mipmap_gen.h"

#ifdef RD_API_WEBGL

bool CMipMapGen::Generate(SharedPtr<CTexture> intx, CUniformBuffer<CMipMapGen::UBLevelData>* pub){
	if(intx == nullptr) return false;
	if(device == nullptr) return false;
	
	//use default ub if none is provided
	if(pub == nullptr)
		pub = this->dataUB.get();
	auto& ub = *pub;

	auto rpdesc = renderPass->getDescriptor();
	if(rpdesc.nofAttachments != 1) return false;

	//delete framebuffers if a new texture is different from the one in the framebuffers
	if(framebuffers.size() > 0)
		if(framebuffers[0]->hasAttachment(intx.get()) == false)
			ClearTextureReference();

	auto& txdesc = intx->getDescriptor();
	uint numMips = rdCalcNumberOfMips(txdesc.width, txdesc.height, txdesc.depth);
	
	if(txdesc.bGenMipmaps == false)
		intx->AllocateMipmaps();
	
	//create framebuffers if not created
	if(framebuffers.size() == 0){
		for(uint i = 1; i < numMips; ++i){
			rpdesc.attachments[0].level = i;
			rpdesc.attachments[0].levelCount = 1;
			auto framebuffer = device->CreateFramebuffer(rpdesc, { intx });
			framebuffers.emplace_back(framebuffer);
		}
	}

	device->BindVertexBuffer(CMipMapGen::quadVB.get());
	device->BindIndexBuffer(CMipMapGen::quadIB.get());
	
	auto shader = pipeline->getShaderProgram();

	for(uint i = 1; i < numMips; ++i){
		auto size = rdCalcMipDimension(i, txdesc.width, txdesc.height, txdesc.depth);
			ub->resolution = vec2(size.x, size.y);
			ub->invResolution = 1.0f / ub->resolution;
			ub->level = i;
		ub.Upload();

		renderPass->Begin(framebuffers[i-1].get());
			pipeline->Bind();
				shader->setUniformBuffer(0, 1, &ub);
				shader->setTexture(0, 0, intx->getView());
				device->setDynamicViewport(0, SViewport((float)size.x, (float)size.y));
				device->DrawIndexed();
		renderPass->End();
	}

	return true;
}

#endif //RD_API_OPENGL4