#ifdef RD_API_BASE

#include "device.h"

UniquePtr<GPUDevice> GPUDevice::CreateGPUDevice(const SGPUDeviceDesc& desc){
	return NewUnique<GPUDevice>(desc);
}

#endif //RD_API_OPENGL4