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

//-----------------------------------------------------------------------------------
// CSampler
//-----------------------------------------------------------------------------------
bool CSampler::ApplySampler(GPUDevice* device, GLenum target, const SSamplerDesc& descriptor){
	if(device == nullptr) return false;
	auto& gl = device->gl;
	gl.TexParameteri(target, GL_TEXTURE_WRAP_S, glenum(descriptor.uWrapping));
	gl.TexParameteri(target, GL_TEXTURE_WRAP_T, glenum(descriptor.vWrapping));
	gl.TexParameteri(target, GL_TEXTURE_WRAP_R, glenum(descriptor.wWrapping));
	gl.TexParameteri(target, GL_TEXTURE_MIN_FILTER, glenum(descriptor.minFilter, descriptor.mipFilter));
	gl.TexParameteri(target, GL_TEXTURE_MAG_FILTER, glenum(descriptor.magFilter, ETextureFiltering::None));
	return true;
}

bool CSampler::ApplySampler(GLenum target){
	return CSampler::ApplySampler(this->device, target, this->descriptor);
}
//-----------------------------------------------------------------------------------

//-----------------------------------------------------------------------------------
// CTexture
//-----------------------------------------------------------------------------------
bool CTexture::ApplySampler(const SSamplerDesc& s){
	if(device == nullptr) return false;
	if(sampler == s) return true;
	auto& gl = device->gl;

	sampler = s;
	GLenum target = glenum(descriptor.type);

	gl.BindTexture(target, id);
	CSampler::ApplySampler(device, target, sampler);
	gl.BindTexture(target, 0);

	return true;
}
bool CTexture::ApplySampler(const CSampler* sampler){
	if(sampler == nullptr) return false;
	return this->ApplySampler(sampler->getDescriptor());
}

//ToDo: za Web/wasm ovo treba imat delayed loading, dok se file ne ucita. Pozvat CFile::DelayedOpen()
bool CTexture::Create(std::string& fileName){
	CFile file(fileName, CFile::EFileMode::ReadBinary);

	uint size = 0;
	byte* data = __new byte[file.getSize()];
	file.Read(file.getSize(), data, &size);

	byte* image_data = nullptr;
	uint image_size = 0, width = 0, height = 0, components = 0;

	switch(descriptor.valueType)
	{
		default:
		case EValueType::int8:
		case EValueType::uint8:
			rdLoadImageData<uint8>(data, size, &image_data, &image_size, &width, &height, &components);
			descriptor.valueType = EValueType::uint8;
			break;
		case EValueType::int16:
		case EValueType::uint16:
		case EValueType::uint24:
		case EValueType::int32:
		case EValueType::uint32:
			rdLoadImageData<uint16>(data, size, (uint16**)&image_data, &image_size, &width, &height, &components);
			descriptor.valueType = EValueType::uint16;
			break;
		case EValueType::float16:
		case EValueType::float24:
		case EValueType::float32:
		case EValueType::float64:
			rdLoadImageData<float>(data, size, (float**)&image_data, &image_size, &width, &height, &components);
			descriptor.valueType = EValueType::float32;
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

	descriptor.name = fileName;

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
		CSampler::ApplySampler(device, target, sampler);

		uint numMips = ptr.numMips;
		if(descriptor.bGenMipmaps == true)
			numMips = rdCalcNumberOfMips(descriptor.width, descriptor.height, descriptor.depth);
		
		if(ptr.slices != nullptr){
			switch(descriptor.type)
			{
				case ETextureType::Texture1D:
					gl.TexStorage1D(target, numMips, glenumTypeless(descriptor.format, descriptor.valueType), descriptor.width);
					for(uint i = 0; i < ptr.numMips; ++i){
						ivec3 dim = rdCalcMipDimension(i, descriptor.width, descriptor.height, descriptor.depth);
						//gl.TexImage1D(target, i, glenum(descriptor.format, descriptor.valueType), dim.x, 0, glenum(descriptor.format), glenum(descriptor.valueType), ptr.slices[i].data);
						gl.TexSubImage1D(target, i, 0, dim.x, glenum(descriptor.format), glenum(descriptor.valueType), ptr.slices[i].data);
					}
					break;
				case ETextureType::Texture2D:
					gl.TexStorage2D(target, numMips, glenumTypeless(descriptor.format, descriptor.valueType), descriptor.width, descriptor.height);
					for(uint i = 0; i < ptr.numMips; ++i){
						ivec3 dim = rdCalcMipDimension(i, descriptor.width, descriptor.height, descriptor.depth);
						//gl.TexImage2D(target, i, glenum(descriptor.format, descriptor.valueType), dim.x, dim.y, 0, glenum(descriptor.format), glenum(descriptor.valueType), ptr.slices[i].data);
						gl.TexSubImage2D(target, i, 0, 0, dim.x, dim.y, glenum(descriptor.format), glenum(descriptor.valueType), ptr.slices[i].data);
					}
					break;
				case ETextureType::Texture3D:
					gl.TexStorage3D(target, numMips, glenumTypeless(descriptor.format, descriptor.valueType), descriptor.width, descriptor.height, descriptor.depth);
					for(uint i = 0; i < ptr.numMips; ++i){
						ivec3 dim = rdCalcMipDimension(i, descriptor.width, descriptor.height, descriptor.depth);
						//gl.TexImage3D(target, i, glenum(descriptor.format, descriptor.valueType), dim.x, dim.y, dim.z, 0, glenum(descriptor.format), glenum(descriptor.valueType), ptr.slices[i].data);
						gl.TexSubImage3D(target, i, 0, 0, 0, dim.x, dim.y, dim.z, glenum(descriptor.format), glenum(descriptor.valueType), ptr.slices[i].data);
					}
					break;
				case ETextureType::TextureCube:
					gl.TexStorage2D(target, numMips, glenumTypeless(descriptor.format, descriptor.valueType), descriptor.width, descriptor.height);
					if(ptr.numSlices / ptr.numMips != 6){
						LOG_ERR("wrong number of sides for cubemap (%d)!", ptr.numSlices / ptr.numMips); }
					else{
						uint i = 0;
						for(uint side = 0; side < 6; ++side){
							for(uint mip = 0; mip < ptr.numMips; ++mip){
								ivec3 dim = rdCalcMipDimension(mip, descriptor.width, descriptor.height, descriptor.depth);
								//gl.TexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + side, mip, glenum(descriptor.format, descriptor.valueType), dim.x, dim.y, 0, glenum(descriptor.format), glenum(descriptor.valueType), ptr.slices[i].data);
								gl.TexSubImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + side, mip, 0, 0, dim.x, dim.y, glenum(descriptor.format), glenum(descriptor.valueType), ptr.slices[i].data);
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

		int immutable = false; gl.GetTextureParameteriv(this->id, GL_TEXTURE_IMMUTABLE_FORMAT, &immutable);
		ASSERT(immutable);

	return true;
}

bool CTexture::AllocateMipmaps(){
	if(device == nullptr) return false;
	if(id == 0) return false;
	if(descriptor.bGenMipmaps == true) return true;
	auto& gl = device->gl;

	GLenum target = glenum(descriptor.type);

	gl.BindTexture(target, id);
	gl.GenerateMipmap(target);
	descriptor.bGenMipmaps = true;
	return true;
}

bool CTexture::CreateView(SharedPtr<CTexture> tx){
	if(device == nullptr) return false;
	if(this->id == 0) return false;
	if(tx == nullptr || tx.get() != this) return false;

	STextureViewDesc vdesc; { vdesc = this->descriptor; }
	view = device->CreateTextureView(vdesc, tx);

	return true;
}
bool CTexture::CreateView(CTexture* ptx){
	if(device == nullptr) return false;
	if(this->id == 0) return false;

	auto tx = device->FindSharedPtr<CTexture>(ptx);
	return this->CreateView(tx);
}

bool CTexture::UpdateLevelData(const STextureSliceRawData& data){
	if(device == nullptr) return false;
	if(this->id == 0) return false;
	auto& gl = device->gl;
	
	GLenum target = glenum(descriptor.type);
	gl.BindTexture(target, id);

	ivec3 dim = rdCalcMipDimension(data.mipLevel, descriptor.width, descriptor.height, descriptor.depth);
	{
		switch(descriptor.type)
		{
			case ETextureType::Texture1D:{
					//gl.TexImage1D(target, data.mipLevel, glenum(descriptor.format), dim.x, 0, glenum(descriptor.format), glenum(descriptor.valueType), data.data);
					gl.TexSubImage1D(target, data.mipLevel, 0, dim.x, glenum(descriptor.format), glenum(descriptor.valueType), data.data);
				}
				break;
			case ETextureType::Texture2D:{
					//gl.TexImage2D(target, data.mipLevel, glenum(descriptor.format), dim.x, dim.y, 0, glenum(descriptor.format), glenum(descriptor.valueType), data.data);
					gl.TexSubImage2D(target, data.mipLevel, 0, 0, dim.x, dim.y, glenum(descriptor.format), glenum(descriptor.valueType), data.data);
				}
				break;
			case ETextureType::Texture3D:{
					//gl.TexImage3D(target, data.mipLevel, glenum(descriptor.format), dim.x, dim.y, dim.z, 0, glenum(descriptor.format), glenum(descriptor.valueType), data.data);
					gl.TexSubImage3D(target, data.mipLevel, 0, 0, 0, dim.x, dim.y, dim.z, glenum(descriptor.format), glenum(descriptor.valueType), data.data);
				}
				break;
			case ETextureType::TextureCube:{
					//gl.TexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + data.side, data.mipLevel, glenum(descriptor.format), dim.x, dim.y, 0, glenum(descriptor.format), glenum(descriptor.valueType), data.data);
					gl.TexSubImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + data.side, data.mipLevel, 0, 0, dim.x, dim.y, glenum(descriptor.format), glenum(descriptor.valueType), data.data);
				}
				break;
			case ETextureType::Texture1DArray:
				LOG_ERR("not implemented!"); return false;
				break;
			case ETextureType::Texture2DArray:
				LOG_ERR("not implemented!"); return false;
				break;
			case ETextureType::TextureCubeArray:
				LOG_ERR("not implemented!"); return false;
				break;
			default:
				LOG_ERR("texture of wrong type!"); return false;
				break;
		}
	}
	return true;
}

void CTexture::Release(){
	if(device == nullptr) return;
	if(this->id != 0)
		device->gl.DeleteTextures(1, &this->id);
	this->id = 0;
}

//-----------------------------------------------------------------------------------
// CTextureView
//-----------------------------------------------------------------------------------
bool CTextureView::Create(){
	if(this->device == nullptr) return false;
	if(this->texture == nullptr) return false;
	auto& gl = this->device->gl;

	uint32 numMipLevels = (this->texture->descriptor.bGenMipmaps == false)? 1 
						: glm::min(this->descriptor.numMipLevels, rdCalcNumberOfMips(this->descriptor.width, this->descriptor.height, this->descriptor.depth));
	uint32 numLayers = (this->texture->descriptor.type != ETextureType::TextureCube)? 1 : 6; //glm::min(this->descriptor.numLayers, 1U);

	gl.GenTextures(1, &this->id);
	gl.TextureView(this->id,
				   glenum(this->descriptor.type),
				   this->texture->id,
				   glenumTypeless(this->descriptor.format, this->descriptor.valueType),
				   this->descriptor.mipLevelBase,
				   numMipLevels,
				   this->descriptor.layerBase,
				   numLayers);
	if((CGLState::EGLError)gl.GetError() != CGLState::EGLError::EGL_NO_ERROR)
		return false;

	auto& sampler = (this->sampler != nullptr)? this->sampler->descriptor : this->texture->sampler;
	GLenum target = glenum(this->descriptor.type);

	gl.BindTexture(target, id);
	CSampler::ApplySampler(device, target, sampler);

	return true;
}

bool CTextureView::Bind(uint s, uint b){
	if(this->device == nullptr) return false;
	if(this->id == 0) return false;
	auto& gl = this->device->gl;
	set = s; binding = b;

	gl.ActiveTexture(GL_TEXTURE0 + b);
	gl.BindTexture(glenum(this->descriptor.type), this->id);

	return true;
}

void CTextureView::Release(){
	if(device == nullptr) return;
	if(this->id != 0)
		device->gl.DeleteTextures(1, &this->id);
	this->id = 0;
}

//-----------------------------------------------------------------------------------
// CTextureManager
//-----------------------------------------------------------------------------------
SharedPtr<CTexture> CTextureManager::FindByName(std::string name){
	for(uint i = 0; i < device->created.textures.size(); ++i){
		auto texture = device->created.textures[i].lock();
		if(texture != nullptr && texture->descriptor.name == name)
			return texture;
	}
	return nullptr;
}
SharedPtr<CTexture> CTextureManager::CreateTexture(std::string name, const STextureDesc* pdesc){
	auto texture = this->FindByName(name);
	if(texture != nullptr) return texture;

	if(device == nullptr) return nullptr;

	STextureDesc desc;
	if(pdesc != nullptr){ desc = *pdesc; }
	return device->CreateTexture(desc, name);
}
//-----------------------------------------------------------------------------------

#endif //RD_API_OPENGL4