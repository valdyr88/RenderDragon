
#ifdef RD_API_BASE
#include "base/vertex_buffer.cpp"
#endif

#ifdef RD_API_OPENGL4
#include "opengl4/vertex_buffer.cpp"
#endif

#ifdef RD_API_WEBGL
#include "webgl/vertex_buffer.cpp"
#endif

#ifdef RD_API_VULKAN
#endif

//---------------------------------------------------------------------------------
//common code
//---------------------------------------------------------------------------------

std::vector<byte> CVertexBuffer::InterleaveVertexData(const SVertexFormat& fmt, const std::vector<byte*>& srcbytedata, sizetype vertexCount){
	if(srcbytedata.size() != fmt.attributes.size()) return {};

	uint vtxbytesize = 0; std::vector<uint> elemsize;
	for(auto att : fmt.attributes){
		vtxbytesize += componentCount(att.count) * sizeInBytes(att.type); 
		elemsize.emplace_back(componentCount(att.count) * sizeInBytes(att.type));
	}
	
	std::vector<byte> databytes; databytes.resize(vertexCount*vtxbytesize);
	for(auto& b : databytes) b = (byte)0x00;

	{
		byte* dataptr = databytes.data();

		for(sizetype vtxId = 0; vtxId < vertexCount; ++vtxId)
		{
			for(uint a = 0; a < fmt.attributes.size(); ++a){
				auto& att = fmt.attributes[a];
				auto elembytesize = elemsize[a];//componentCount(att.count) * sizeInBytes(att.type);
				auto srcdata = srcbytedata[a] + vtxId * elembytesize;

				for(uint b = 0; b < elembytesize; ++b)
					dataptr[b] = srcdata[b];

				dataptr += elembytesize;
			}
		}
	}

	return databytes;
}

//---------------------------------------------------------------------------------