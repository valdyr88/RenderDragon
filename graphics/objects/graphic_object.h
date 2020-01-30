#ifndef GRAPHIC_OBJECT_H
#define GRAPHIC_OBJECT_H

#include "../../utils/pointers.h"

class IDevice;

class CGraphicObject {
protected:
	WeakPtr<IDevice> device;

	CGraphicObject() = delete;
	CGraphicObject(WeakPtr<IDevice>& dev) : device(dev) {}

public:
	virtual ~CGraphicObject() = default;
};

#endif //GRAPHIC_OBJECT_H