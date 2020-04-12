
#ifdef RD_API_BASE
#include "base/uniform_buffer.cpp"
#endif

#ifdef RD_API_OPENGL4
#include "opengl4/uniform_buffer.cpp"
#endif

#ifdef RD_API_WEBGL
#endif

#ifdef RD_API_VULKAN
#endif

//---------------------------------------------------------------------------------
//common code
//---------------------------------------------------------------------------------

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



//---------------------------------------------------------------------------------