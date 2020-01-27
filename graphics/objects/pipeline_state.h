#ifndef PIPELINE_STATE_H
#define PIPELINE_STATE_H

#include "../../utils/pointers.h"
#include "graphic_object.h"
#include "pipeline_state_desc.h"

class Device;

class PipelineState : public GraphicObject {
protected:
	PipelineStateDesc descriptor;

public:

	PipelineState(WeakPtr<Device>& dev, const PipelineStateDesc& desc)
		: GraphicObject(dev), descriptor(desc) {
	}

	const PipelineStateDesc& getDescriptor() { return descriptor; }
	bool operator == (PipelineState& other) { return descriptor == other.descriptor; }
	bool operator == (PipelineStateDesc& desc) { return descriptor == desc; }

	virtual bool bind() = 0;

	virtual ~PipelineState() = default;

	ShaderProgram* getShaderProgram() { return descriptor.shader.get(); }
};


#endif //PIPELINE_STATE_H
