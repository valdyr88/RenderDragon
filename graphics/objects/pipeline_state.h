#ifndef PIPELINE_STATE_H
#define PIPELINE_STATE_H

#include "../../utils/pointers.h"
#include "graphic_object.h"
#include "pipeline_state_desc.h"

class IDevice;

class CPipelineState : public CGraphicObject {
protected:
	SPipelineStateDesc descriptor;

public:

	CPipelineState(WeakPtr<IDevice>& dev, const SPipelineStateDesc& desc)
		: CGraphicObject(dev), descriptor(desc) {
	}

	const SPipelineStateDesc& getDescriptor() { return descriptor; }
	bool operator == (CPipelineState& other) { return descriptor == other.descriptor; }
	bool operator == (SPipelineStateDesc& desc) { return descriptor == desc; }

	virtual bool bind() = 0;

	virtual ~CPipelineState() = default;

	CShaderProgram* getShaderProgram() { return descriptor.shader.get(); }
};


#endif //PIPELINE_STATE_H
