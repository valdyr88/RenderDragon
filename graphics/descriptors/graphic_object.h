#ifndef GRAPHIC_OBJECT_H
#define GRAPHIC_OBJECT_H

#include "../utils/pointers.h"

class GPUDevice;

class CGraphicObject {
protected:
	GPUDevice* device = nullptr;

	CGraphicObject() = delete;
	CGraphicObject(GPUDevice* dev) : device(dev) {}

public:
	virtual void Release(){}
	virtual ~CGraphicObject(){ Release(); }
};

#endif //GRAPHIC_OBJECT_H