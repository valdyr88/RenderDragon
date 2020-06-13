#ifdef RD_API_BASE

#include "device.h"

UniquePtr<GPUDevice> GPUDevice::CreateGPUDevice(const SGPUDeviceDesc& desc){
	return NewUnique<GPUDevice>(desc);
}

UniquePtr<CShaderResourceBinding> GPUDevice::CreateShaderResourceBinding(const SShaderResourceBindingDesc& desc, CShaderResource* resource){
	return UniquePtr<CShaderResourceBinding>(__rd_new CShaderResourceBinding(this, desc, resource));
}

UniquePtr<CShaderResourceSetDesc> GPUDevice::CreateShaderResourceSetDesc(const std::vector<SShaderResourceBindingDesc>& binds){
	return UniquePtr<CShaderResourceSetDesc>(__rd_new CShaderResourceSetDesc(this, binds));
}
UniquePtr<CShaderResourceSet> GPUDevice::CreateShaderResourceSet(const CShaderResourceSetDesc* desc, const std::vector<CShaderResource*>& rers){
	return UniquePtr<CShaderResourceSet>(__rd_new CShaderResourceSet(this, desc, rers));
}

#endif //RD_API_OPENGL4