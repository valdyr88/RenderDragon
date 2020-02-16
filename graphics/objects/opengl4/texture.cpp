#ifdef RD_API_OPENGL4

#include "device.h"
#include "texture.h"

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

bool CTexture::Create(const STextureRawData& ptr){
	if(device == nullptr) return false;
	this->Release();

	GLenum target = glenum(descriptor.type);

	device->gl.GenTextures(1, &this->id);
		device->gl.BindTexture(target, id);
		device->gl.TexParameteri(target, GL_TEXTURE_WRAP_S, glenum(sampler.uWrapping));
		device->gl.TexParameteri(target, GL_TEXTURE_WRAP_T, glenum(sampler.vWrapping));
		device->gl.TexParameteri(target, GL_TEXTURE_WRAP_R, glenum(sampler.wWrapping));
		device->gl.TexParameteri(target, GL_TEXTURE_MIN_FILTER, glenum(sampler.minFilter, sampler.mipFilter));
		device->gl.TexParameteri(target, GL_TEXTURE_MAG_FILTER, glenum(sampler.magFilter, ETextureFiltering::None));
		
		//ToDo: bug, width/height/depth need to lower depending on mip level (i)!
		if(ptr.slices != nullptr){
			switch(descriptor.type)
			{
				case ETextureType::Texture1D:
					for(uint i = 0; i < ptr.numMips; ++i){
						ivec3 dim = CalcMipDimension(i, descriptor.width, descriptor.height, descriptor.depth);
						device->gl.TexImage1D(target, i, glenum(descriptor.format), dim.x, 0, glenum(descriptor.format), glenum(descriptor.valueType), ptr.slices[i].data);
					}
					break;
				case ETextureType::Texture2D:
					for(uint i = 0; i < ptr.numMips; ++i){
						ivec3 dim = CalcMipDimension(i, descriptor.width, descriptor.height, descriptor.depth);
						device->gl.TexImage2D(target, i, glenum(descriptor.format), dim.x, dim.y, 0, glenum(descriptor.format), glenum(descriptor.valueType), ptr.slices[i].data);
					}
					break;
				case ETextureType::Texture3D:
					for(uint i = 0; i < ptr.numMips; ++i){
						ivec3 dim = CalcMipDimension(i, descriptor.width, descriptor.height, descriptor.depth);
						device->gl.TexImage3D(target, i, glenum(descriptor.format), dim.x, dim.y, dim.z, 0, glenum(descriptor.format), glenum(descriptor.valueType), ptr.slices[i].data);
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
								device->gl.TexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + side, mip, glenum(descriptor.format), dim.x, dim.y, 0, glenum(descriptor.format), glenum(descriptor.valueType), ptr.slices[i].data);
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
			device->gl.GenerateMipmap(target);

	return true;
}

void CTexture::Release(){
	if(this->id != 0)
		device->gl.DeleteTextures(1, &this->id);
	this->id = 0;
}
#endif //RD_API_OPENGL4