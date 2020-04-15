#ifdef RD_API_OPENGL4

#include "device.h"
#include "texture.h"

#define STB_IMAGE_IMPLEMENTATION
#include "..\..\utils\stb\stb_image.h"

//ToDo: loading of different images with stb image library

template <typename type> type* stbi_load_from_memory_t(byte* in_image_data, uint in_size, int* out_width, int* out_height, int* out_components){
	return nullptr;}
template <> uint8* stbi_load_from_memory_t(byte* in_image_data, uint in_size, int* out_width, int* out_height, int* out_components){
	return stbi_load_from_memory(in_image_data, in_size, out_width, out_height, out_components, *out_components); }
template <> uint16* stbi_load_from_memory_t(byte* in_image_data, uint in_size, int* out_width, int* out_height, int* out_components){
	return stbi_load_16_from_memory(in_image_data, in_size, out_width, out_height, out_components, *out_components); }
template <> float* stbi_load_from_memory_t(byte* in_image_data, uint in_size, int* out_width, int* out_height, int* out_components){
	return stbi_loadf_from_memory(in_image_data, in_size, out_width, out_height, out_components, *out_components);
}

template <typename type>
bool rdLoadImageData(byte* in_image_data, uint in_size, type** out_image_data, uint* out_size, uint* out_width, uint* out_height, uint* out_components){
	if(in_image_data == nullptr || in_size == 0) return false;
	if(out_image_data == nullptr || out_size == nullptr || out_width == nullptr || out_height == nullptr || out_components == nullptr) return false;

	stbi_set_flip_vertically_on_load(true);
	
	int width = 0, height = 0, components = *out_components;
	*out_image_data = stbi_load_from_memory_t<type>(in_image_data, in_size, &width, &height, &components);
	*out_width = (uint)width; *out_height = (uint)height; *out_components = (uint)components;
	*out_size = (uint)(width*height*components*sizeof(type));
	return true;
}

bool CTexture::ApplySampler(const SSamplerDesc& s){
	if(device == nullptr) return false;
	if(sampler == s) return true;

	sampler = s;
	GLenum target = glenum(descriptor.type);

	device->gl.BindTexture(target, id);
	device->gl.TexParameteri(target, GL_TEXTURE_WRAP_S, glenum(sampler.uWrapping));
	device->gl.TexParameteri(target, GL_TEXTURE_WRAP_T, glenum(sampler.vWrapping));
	device->gl.TexParameteri(target, GL_TEXTURE_WRAP_R, glenum(sampler.wWrapping));
	device->gl.TexParameteri(target, GL_TEXTURE_MIN_FILTER, glenum(sampler.minFilter, sampler.mipFilter));
	device->gl.TexParameteri(target, GL_TEXTURE_MAG_FILTER, glenum(sampler.magFilter, ETextureFiltering::None));

	return true;
}

bool CTexture::Create(std::string& fileName){
	CFile file(fileName, CFile::EFileMode::ReadBinary);

	uint size = 0;
	byte* data = __new byte[file.getSize()];
	file.Read(file.getSize(), &data, &size);

	byte* image_data = nullptr;
	uint image_size = 0, width = 0, height = 0, components = 0;

	switch(descriptor.valueType)
	{
		default:
		case EValueType::int8:
		case EValueType::uint8:
			rdLoadImageData<uint8>(data, size, &image_data, &image_size, &width, &height, &components);
			break;
		case EValueType::int16:
		case EValueType::uint16:
		case EValueType::uint24:
		case EValueType::int32:
		case EValueType::uint32:
			rdLoadImageData<uint16>(data, size, (uint16**)&image_data, &image_size, &width, &height, &components);
			break;
		case EValueType::float16:
		case EValueType::float24:
		case EValueType::float32:
		case EValueType::float64:
			rdLoadImageData<float>(data, size, (float**)&image_data, &image_size, &width, &height, &components);
			break;
	}
	
	STextureRawData txData(width, height, 1, 1, false);
	txData.slices[0].data = image_data;
	txData.slices[0].size = image_size;
	
	descriptor.width = width;
	descriptor.height = height;
	descriptor.type = ETextureType::Texture2D;

	switch(components){
		case 1: descriptor.format = ETextureFormat::R; break;
		case 2: descriptor.format = ETextureFormat::RG; break;
		case 3: descriptor.format = ETextureFormat::RGB; break;
		case 4: descriptor.format = ETextureFormat::RGBA; break;
	}

	bool rtn = Create(txData);
	
	__release_array(data); size = 0;
	__release_array(image_data);
	return rtn;
}

bool CTexture::Create(const STextureRawData& ptr){
	if(device == nullptr) return false;
	this->Release();

	auto& gl = device->gl;

	GLenum target = glenum(descriptor.type);

	gl.GenTextures(1, &this->id);
		gl.BindTexture(target, id);
		gl.TexParameteri(target, GL_TEXTURE_WRAP_S, glenum(sampler.uWrapping));
		gl.TexParameteri(target, GL_TEXTURE_WRAP_T, glenum(sampler.vWrapping));
		gl.TexParameteri(target, GL_TEXTURE_WRAP_R, glenum(sampler.wWrapping));
		gl.TexParameteri(target, GL_TEXTURE_MIN_FILTER, glenum(sampler.minFilter, sampler.mipFilter));
		gl.TexParameteri(target, GL_TEXTURE_MAG_FILTER, glenum(sampler.magFilter, ETextureFiltering::None));
		
		if(ptr.slices != nullptr){
			switch(descriptor.type)
			{
				case ETextureType::Texture1D:
					for(uint i = 0; i < ptr.numMips; ++i){
						ivec3 dim = CalcMipDimension(i, descriptor.width, descriptor.height, descriptor.depth);
						gl.TexImage1D(target, i, glenum(descriptor.format), dim.x, 0, glenum(descriptor.format), glenum(descriptor.valueType), ptr.slices[i].data);
					}
					break;
				case ETextureType::Texture2D:
					for(uint i = 0; i < ptr.numMips; ++i){
						ivec3 dim = CalcMipDimension(i, descriptor.width, descriptor.height, descriptor.depth);
						gl.TexImage2D(target, i, glenum(descriptor.format), dim.x, dim.y, 0, glenum(descriptor.format), glenum(descriptor.valueType), ptr.slices[i].data);
					}
					break;
				case ETextureType::Texture3D:
					for(uint i = 0; i < ptr.numMips; ++i){
						ivec3 dim = CalcMipDimension(i, descriptor.width, descriptor.height, descriptor.depth);
						gl.TexImage3D(target, i, glenum(descriptor.format), dim.x, dim.y, dim.z, 0, glenum(descriptor.format), glenum(descriptor.valueType), ptr.slices[i].data);
					}
					break;
				case ETextureType::TextureCube:
					if(ptr.numSlices / ptr.numMips != 6){
						LOG_ERR("wrong number of sides for cubemap (%d)!", ptr.numSlices / ptr.numMips); }
					else{
						uint i = 0;
						for(uint side = 0; side < 6; ++side){
							for(uint mip = 0; mip < ptr.numMips; ++mip){
								ivec3 dim = CalcMipDimension(i, descriptor.width, descriptor.height, descriptor.depth);
								gl.TexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + side, mip, glenum(descriptor.format), dim.x, dim.y, 0, glenum(descriptor.format), glenum(descriptor.valueType), ptr.slices[i].data);
								++i;
							}
						}
					}
					break;
				case ETextureType::Texture1DArray:
					LOG_ERR("not implemented!");
					break;
				case ETextureType::Texture2DArray:
					LOG_ERR("not implemented!");
					break;
				case ETextureType::TextureCubeArray:
					LOG_ERR("not implemented!");
					break;
				default:
					LOG_ERR("texture of wrong type!");
					break;
			}
		}

		if(descriptor.bGenMipmaps)
			gl.GenerateMipmap(target);

	return true;
}

bool CTexture::Bind(uint s, uint b){
	if(this->device == nullptr) return false;
	auto& gl = this->device->gl;
	set = s; binding = b;

	gl.ActiveTexture(GL_TEXTURE0 + b);
	gl.BindTexture(glenum(descriptor.type), this->id);
	return true;
}

void CTexture::Release(){
	if(this->id != 0)
		device->gl.DeleteTextures(1, &this->id);
	this->id = 0;
}
#endif //RD_API_OPENGL4