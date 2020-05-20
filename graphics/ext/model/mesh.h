#ifndef MESH_H
#define MESH_H

#include "../../objects/vertex_buffer.h"
#include "../material/material.h"
//-------------------------------------------------------------------------------------------

enum class EMeshSourceFileType{
	None, 
	Formatted_Obj,
	Formatted_FBX,
	Formatted_3ds,
	Binary_FBX
};

struct SMeshDesc
{
	std::string name;
	std::vector<SMaterialTexture> textures;
	struct CRC64
	{
		uint64 vertex = 0;
		uint64 index = 0;
		uint64 normal = 0;
		uint64 texCoord[RD_MAX_UV_CHANNELS] = { 0 };
		uint64 tangent = 0;

		bool operator ==(const CRC64& other) const{
			for(uint c = 0; c < RD_MAX_UV_CHANNELS; ++c)
				if(texCoord[c] != other.texCoord[c]) return false;
			return vertex == other.vertex &&
				index == other.index &&
				normal == other.normal &&
				tangent == other.tangent;
		}
		bool operator != (const CRC64& other) const{ return !(*this == other); }
	} crc64hash;

	bool operator == (const SMeshDesc& other) const{
		if(crc64hash != other.crc64hash) return false;
		if(textures.size() != other.textures.size()) return false;
		for(uint i = 0; i < textures.size(); ++i)
			if(textures[i] != other.textures[i]) return false;
	}
	bool operator != (const SMeshDesc& other) const{ return !(*this == other); }

	SMeshDesc& operator = (const SMeshDesc& other){
		name = other.name;
		textures.resize(other.textures.size());
		for(uint i = 0; i < other.textures.size(); ++i)
			textures[i] = other.textures[i];
		return *this;
	}


	bool compareCRC64(const SMeshDesc& other) const{
		return this->crc64hash == other.crc64hash;
	}
};

class CMesh : public CGraphicObject{
	SMeshDesc descriptor;

	struct VertexData{
		sizetype count = 0;
		uint vertexDim = 3;
		std::vector<float> vertex;
		EPrimitiveType primitiveType = EPrimitiveType::Triangles;
		std::vector<uint> index;
		uint normalDim = 3;
		std::vector<float> normal;
		uint texCoordDim[RD_MAX_UV_CHANNELS];
		std::vector<float> texCoord[RD_MAX_UV_CHANNELS];
		uint tangentDim = 3;
		std::vector<float> tangent;

		VertexData(){
			for(uint c = 0; c < RD_MAX_UV_CHANNELS; ++c) texCoordDim[c] = 2; }

		bool operator ==(const VertexData& other){
			if(count != other.count) return false;

			if(primitiveType != other.primitiveType) return false;
			if(vertexDim != other.vertexDim) return false;
			if(normalDim != other.normalDim) return false;
			if(tangentDim != other.tangentDim) return false;
			for(uint c = 0; c < RD_MAX_UV_CHANNELS; ++c)
				if(texCoordDim[c] != other.texCoordDim[c]) return false;

			if(vertex.size() != other.vertex.size()) return false;
			if(index.size() != other.index.size()) return false;
			if(normal.size() != other.normal.size()) return false;
			for(uint c= 0; c < RD_MAX_UV_CHANNELS; ++c)
				if(texCoord[c].size() != other.texCoord[c].size()) return false;
			if(tangent.size() != other.tangent.size()) return false;

			for(uint i = 0; i < vertex.size(); ++i)
				if(vertex[i] != other.vertex[i]) return false;
			for(uint i = 0; i < index.size(); ++i)
				if(index[i] != other.index[i]) return false;
			for(uint i = 0; i < normal.size(); ++i)
				if(normal[i] != other.normal[i]) return false;
			for(uint c = 0; c < RD_MAX_UV_CHANNELS; ++c)
				for(uint i = 0; i < texCoord[c].size(); ++i)
					if(texCoord[c][i] != other.texCoord[c][i]) return false;
			for(uint i = 0; i < tangent.size(); ++i)
				if(tangent[i] != other.tangent[i]) return false;

			return true;
		}
	} data;

	SharedPtr<CVertexBuffer> vertexBuffer;
	SharedPtr<CIndexBuffer> indexBuffer;

	bool Create();
public:
	CMesh(GPUDevice* dev, const SMeshDesc& desc) : CGraphicObject(dev), descriptor(desc){}

	static SMeshDesc CreateMeshDescFromXML(std::string xml);
	static SMeshDesc CreateMeshDescFromXML(void* xmlobject);

	bool operator == (const SMeshDesc& other) const{
		return this->descriptor == other; }
	bool operator != (const SMeshDesc& other) const{
		return this->descriptor != other; }

	bool operator == (const SMeshDesc::CRC64& other) const{
		return this->descriptor.crc64hash == other; }
	bool operator != (const SMeshDesc::CRC64& other) const{
		return this->descriptor.crc64hash != other; }

	bool compareCRC64(const SMeshDesc& other) const{
		return descriptor.compareCRC64(other); }

	auto getVertexBuffer(){ return vertexBuffer.get(); }
	auto getIndexBuffer(){ return indexBuffer.get(); }

	friend class CModel;
	friend class CModelLoad;
	friend class CMeshManager;
};


class CMeshTransform{
	vec3 position;
	mat3 rotation;
	SharedPtr<CMesh> mesh;
public:
	CMeshTransform(){ rotation = mat3(); }
	CMeshTransform(SharedPtr<CMesh> msh) : mesh(msh){ rotation = mat3(); }

	CMeshTransform& operator =(const CMeshTransform& other){
		position = other.position;
		rotation = other.rotation;
		mesh = other.mesh;
		return *this;
	}

	vec3 Position(){ return position; }
	mat3 Rotation(){ return rotation; }
	CMesh* Mesh(){ return mesh.get(); }

	friend class CModel;
	friend class CModelLoad;
	friend class CMeshManager;
};

//-------------------------------------------------------------------------------------------

enum class EModelParamType{
	None,
	Material,
	Texture
};

struct SModelParam{
	EModelParamType type = EModelParamType::None;
	SModelParam(EModelParamType t) : type(t){}
};
struct SModelParamMaterial : public SModelParam{
	SMaterialDesc desc;
	SModelParamMaterial() : SModelParam(EModelParamType::Material){}
};
struct SModelParamTexture : public SModelParam{
	SMaterialTexture texture;
	SModelParamTexture() : SModelParam(EModelParamType::Texture){}
};

struct SModelDesc{
	std::string name;
	std::string path;
	EMeshSourceFileType fileType = EMeshSourceFileType::None;
	std::vector<SMaterialTexture> textures;

	bool operator == (const SModelDesc& other) const{
		if(textures.size() != other.textures.size()) return false;
		for(uint i = 0; i < textures.size(); ++i)
			if(textures[i] != other.textures[i]) return false;
		return name == other.name &&
			fileType == other.fileType &&
			path == other.path;
	}
	bool operator != (const SModelDesc& other) const{ return !(*this == other); }

	SModelDesc& operator = (const SModelDesc& other){
		fileType = other.fileType;
		name = other.name;
		path = other.path;
		textures.resize(other.textures.size());
		for(uint i = 0; i < other.textures.size(); ++i)
			textures[i] = other.textures[i];
		return *this;
	}
};


class CModel;

class CModel{
	SModelDesc descriptor;
	std::vector<UniquePtr<CMeshTransform>> meshes;

	bool Create(GPUDevice* dev);
public:
	CModel(GPUDevice* dev, const SModelDesc& desc) : descriptor(desc){
		Create(dev);
	}

	auto& AllMeshes(){ return meshes; }

	static SModelDesc CreateModelDescFromXML(std::string xml);
	static SModelDesc CreateModelDescFromXML(void* xmlobject);

	friend class stdex::iterator<CModel, CMeshTransform*>;
	stdex::iterator<CModel, CMeshTransform*> begin() const{ return stdex::iterator<CModel, CMeshTransform*>(this, 0, [](const CModel* ptr, sizetype position){ return ptr->meshes[position].get(); }); }
	stdex::iterator<CModel, CMeshTransform*> end() const{ return stdex::iterator<CModel, CMeshTransform*>(this, meshes.size(), [](const CModel* ptr, sizetype position){ return ptr->meshes[position].get(); }); }
};
/*
template<>
inline CMeshTransform* stdex::iterator<CModel, CMeshTransform*>::operator*()const{ return pcontainer->meshes[position].get(); }*/
//-------------------------------------------------------------------------------------------

struct SModelAssemblyDesc{

};

class CModelAssembly{

};
//-------------------------------------------------------------------------------------------

class CMeshManager{
protected:
	stdex::container<SharedPtr<CMesh>> meshes;

	CMeshManager(){}
	bool add(SharedPtr<CMesh> mesh);
	SharedPtr<CMesh> CreateSharedMesh(CMesh** mesh);
public:

	SharedPtr<CMesh> FindByDescription(const SMeshDesc desc);
	SharedPtr<CMesh> FindByData(const CMesh& mesh);

	friend class CMesh;
	friend class CModel;
	friend class CSingleton<CMeshManager>;
};

//-------------------------------------------------------------------------------------------
#endif //MESH_H