#ifdef RD_API_OPENGL4

#include "pipeline_state.h"
#include "device.h"

bool CPipelineState::Bind(){
	if(device == nullptr) return false;
	auto& gl = device->gl;

	if(this->descriptor == device->pipelineState)
		return true;
	
	device->setBlendState(this->descriptor.blendDesc);
	device->setDepthState(this->descriptor.depthDesc);
	device->setStencilState(this->descriptor.stencilDesc);
	device->setPrimitiveTopology(this->descriptor.primitiveTopology);
	device->setRasterizerState(this->descriptor.rasterizerDesc);
	device->setSampleState(this->descriptor.samplingDesc);
	device->setViewports(this->descriptor.viewports);

	device->bindShaderProgram(this->descriptor.shader);

	return true;
}

#endif //RD_API_OPENGL4