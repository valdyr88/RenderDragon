#ifndef DEVICE_DESC_H
#define DEVICE_DESC_H

#include "../utils/pointers.h"
#include "../utils/types/types.h"
#include "../utils/types/vectypes.h"
#include "graphics_enums.h"
#include "graphic_object.h"

struct SSwapchainDesc{
	ETextureFormat format = ETextureFormat::RGBA;
	EValueType type = EValueType::uint8;
	ETextureFormat depthFormat = ETextureFormat::None;
	EValueType depthType = EValueType::float24;
	EValueType stencilType = EValueType::uint8;
	ESwapchainBuffer buffering = ESwapchainBuffer::DoubleBuffer;
};

struct SDeviceDesc{
	SSwapchainDesc swapchain;
};

#endif //_DESC