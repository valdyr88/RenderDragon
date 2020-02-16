#ifndef PIPELINE_STATE_H
#define PIPELINE_STATE_H

#ifdef RD_API_BASE

#include "../../utils/pointers.h"
#include "../../descriptors/graphic_object.h"
#include "../../descriptors/pipeline_state_desc.h"

class GPUDevice;

class CPipelineState : public CGraphicObject {
protected:
	SPipelineStateDesc descriptor;

public:

	CPipelineState(GPUDevice* dev, const SPipelineStateDesc& desc)
		: CGraphicObject(dev), descriptor(desc) {
	}

	const auto& getDescriptor(){ return descriptor; }

	bool operator == (CPipelineState& other) { return descriptor == other.descriptor; }
	bool operator == (SPipelineStateDesc& desc) { return descriptor == desc; }

	bool bind(){ return true; }

	CShaderProgram* getShaderProgram() { return descriptor.shader.get(); }

	virtual ~CPipelineState() = default;
};

#endif //RD_API_BASE
#endif //PIPELINE_STATE_H