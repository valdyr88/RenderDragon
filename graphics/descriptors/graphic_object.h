#ifndef GRAPHIC_OBJECT_H
#define GRAPHIC_OBJECT_H

#include "../utils/pointers.h"

class GPUDevice;

class CGraphicObject {
protected:
	GPUDevice* device = nullptr;

	CGraphicObject() = delete;
	CGraphicObject(GPUDevice* dev) : device(dev) {}

	virtual void Release(){}
public:
	virtual ~CGraphicObject(){ Release(); }
};

#endif //GRAPHIC_OBJECT_H