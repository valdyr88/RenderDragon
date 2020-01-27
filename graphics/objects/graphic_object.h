#ifndef GRAPHIC_OBJECT_H
#define GRAPHIC_OBJECT_H

#include "../../utils/pointers.h"

class Device;

class GraphicObject {
protected:
	WeakPtr<Device> device;

	GraphicObject() = delete;
	GraphicObject(WeakPtr<Device>& dev) : device(dev) {}

public:
	virtual ~GraphicObject() = default;
};

#endif //GRAPHIC_OBJECT_H