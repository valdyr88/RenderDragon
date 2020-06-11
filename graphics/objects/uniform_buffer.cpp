
#ifdef RD_API_BASE
#include "base/uniform_buffer.cpp"
#endif

#ifdef RD_API_OPENGL4
#include "opengl4/uniform_buffer.cpp"
#endif

#ifdef RD_API_WEBGL
#include "webgl/uniform_buffer.cpp"
#endif

#ifdef RD_API_VULKAN
#endif

//---------------------------------------------------------------------------------
//common code
//---------------------------------------------------------------------------------
const std::vector<SUniformMap> IUniformBuffer::nodesc = {};

void IUniformBuffer::CreateMapping(const std::vector<SUniformMap> maps){

	uint offset = 0;
	for(auto it = maps.begin(); it != maps.end(); ++it){
		auto map = *it;
		map.offset = offset;

		mapping.emplace(it->name, map);

		int bytesize = sizeInBytes(it->type);
		bytesize = (bytesize < 4) ? 4 : bytesize;

		offset += it->count * count(it->size) * bytesize;
	}
}

void IUniformBuffer::Release(){
	if(this->buffer != nullptr)
		this->buffer->Release();
	this->buffer = nullptr;
}

//std::map<std::string, SharedPtr<IUniformBuffer>(*)(GPUDevice*, const char*)> IUniformBuffer::CreateUniformBufferType;
std::map<std::string, std::function< SharedPtr<IUniformBuffer>(GPUDevice*, const char*) >> IUniformBuffer::CreateUniformBufferType;

std::vector<void(*)()>* internal_rdGetRegisterUniformBufferStructureFuncitonCalls(){
	static UniquePtr<std::vector<void(*)()>> rdRegisterUniformBufferStructureFuncitonCalls = nullptr;
	if(rdRegisterUniformBufferStructureFuncitonCalls == nullptr)
		rdRegisterUniformBufferStructureFuncitonCalls = NewUnique<std::vector<void(*)()>>();
	return rdRegisterUniformBufferStructureFuncitonCalls.get();
}
size_t internal_rdAppendRegisterUniformBufferStructureFunctionCall(void (*func)()){ //std::string name, SharedPtr<IUniformBuffer>(*func)(GPUDevice*, const char*)
	if(internal_rdGetRegisterUniformBufferStructureFuncitonCalls() == nullptr)
		LOG_ERR("rdRegisterUniformBufferStructureFuncitonCalls == nullptr");

	internal_rdGetRegisterUniformBufferStructureFuncitonCalls()->push_back(func);
	return internal_rdGetRegisterUniformBufferStructureFuncitonCalls()->size();
}

void internal_rdCallRegisterUniformBufferStructureFunctionCall(){
	auto functions = *internal_rdGetRegisterUniformBufferStructureFuncitonCalls();
	for(uint i = 0; i < functions.size(); ++i){
		functions[i]();
	}
	functions.clear();;
}
//---------------------------------------------------------------------------------