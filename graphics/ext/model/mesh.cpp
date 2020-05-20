//#define ASSIMP_DLL
#include <assimp/types.h>
#include <assimp/config.h>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
//
#include "../../objects/device.h"
#include "../../utils/hash.h"
#include "mesh.h"

#include "../../utils/rapidxml/rapidxml.hpp"
#include "../../utils/strings.h"
uint getMaterialTextureTypeFromString(const char* cstr);

SModelDesc CModel::CreateModelDescFromXML(void* xmlobject){
	rapidxml::xml_node<char>* node = (rapidxml::xml_node<char>*)xmlobject;
	SModelDesc desc;
	
	if(node == nullptr){
		LOG_ERR("root node is nullptr!"); return desc; }

	if(strisequal(node->name(), "model") == false)
		LOG_ERR("root node is not model!");

	if(node->first_attribute("id") != nullptr)
		desc.name = node->first_attribute("id")->value();
	else
		LOG_ERR("model doesn't have attribute: id");

	LOG("Parsing model: <%s>", desc.name.c_str());

	if(node->first_attribute("src") != nullptr)
		desc.path = node->first_attribute("src")->value();
	else
		LOG_ERR("model doesn't have attribute: src");
	
	if(node->first_node("texture") != nullptr){
		for(auto txnode = node->first_node("texture"); txnode != nullptr; txnode = txnode->next_sibling("texture"))
		{
			SMaterialTexture texture;

			const char* name = (txnode->first_attribute("name") != nullptr) ? txnode->first_attribute("name")->value() : nullptr;
			const char* path = (txnode->first_attribute("path") != nullptr) ? txnode->first_attribute("path")->value() : nullptr;
			const char* type = (txnode->first_attribute("type") != nullptr) ? txnode->first_attribute("type")->value() : "D";

			if(name == nullptr && path == nullptr){
				if(name == nullptr)
					LOG_ERR("texture doesn't have attribute: name");
				if(path == nullptr)
					LOG_ERR("texture doesn't have attribute: path");
			}
			if(name == nullptr)	name = "";
			if(path == nullptr)	path = "";

			texture.name = name;
			texture.path = path;
			texture.type |= getMaterialTextureTypeFromString(type);

			desc.textures.emplace_back(texture);
		}
	}

	return desc;
}
SModelDesc CModel::CreateModelDescFromXML(std::string xml){
	rapidxml::xml_document<char>* doc = __new rapidxml::xml_document<char>();
	char* cxml = __new char[xml.length() + 1];
	//xml.copy(cxml, xml.length());
	strcpy_s(cxml, xml.length() + 1, xml.c_str());
	doc->parse<0>(cxml);
	auto m = doc->first_node("model");

	if(m == nullptr)
		LOG_ERR("root node is not model!");

	return CModel::CreateModelDescFromXML(m);
}

//-------------------------------------------------------------------------------------------

class CModelLoad{
public:
	static CMesh* LoadFromAssimp(GPUDevice* dev, aiMesh* aimesh);
	static UniquePtr<CMeshTransform> GetTransformFromAssimp(aiMesh* aimesh, SharedPtr<CMesh>& mesh);

	template<typename type>
	static uint64 crc64t(std::vector<type>& v){
		return hash::crc64((uint8*)v.data(), sizeof(type)*v.size()); }

	static void fillCRC32desc(CMesh* mesh){
		mesh->descriptor.crc64hash.vertex = CModelLoad::crc64t<float>(mesh->data.vertex);
		mesh->descriptor.crc64hash.index = CModelLoad::crc64t<uint>(mesh->data.index);
		mesh->descriptor.crc64hash.normal = CModelLoad::crc64t<float>(mesh->data.normal);
		mesh->descriptor.crc64hash.tangent = CModelLoad::crc64t<float>(mesh->data.tangent);
		for(uint c = 0; c < RD_MAX_UV_CHANNELS; ++c)
			mesh->descriptor.crc64hash.texCoord[c] = CModelLoad::crc64t<float>(mesh->data.texCoord[c]);
	}
};

CMesh* CModelLoad::LoadFromAssimp(GPUDevice* dev, aiMesh* aimesh){
	SMeshDesc desc;
	desc.name = aimesh->mName.C_Str();
		
	CMesh* mesh = new CMesh(dev, desc);

	if(aimesh->mNumVertices > 0){
		mesh->data.vertex.reserve(3*aimesh->mNumVertices);
		for(uint i = 0; i < aimesh->mNumVertices; ++i){
			auto& aiv = aimesh->mVertices[i];
			vec3 v = vec3(aiv.x, aiv.y, aiv.z);
			mesh->data.vertex.emplace_back(v.x);
			mesh->data.vertex.emplace_back(v.y);
			mesh->data.vertex.emplace_back(v.z);
		}
	}
	if(aimesh->GetNumUVChannels() > 0){
		for(uint c = 0; c < aimesh->GetNumUVChannels(); ++c){
			auto& texCoord = mesh->data.texCoord[c];

			texCoord.reserve(aimesh->mNumUVComponents[c] * aimesh->mNumVertices);
			for(uint i = 0; i < aimesh->mNumVertices; ++i){
				auto& uv = aimesh->mTextureCoords[c][i];
				texCoord.emplace_back(uv.x);
				if(aimesh->mNumUVComponents[c] > 1)
					texCoord.emplace_back(uv.y);
				if(aimesh->mNumUVComponents[c] > 2)
					texCoord.emplace_back(uv.z);
			}

			mesh->data.texCoordDim[c] = aimesh->mNumUVComponents[c];
		}
	}
	if(aimesh->HasNormals() == true){
		mesh->data.normal.reserve(3 * aimesh->mNumVertices);
		for(uint i = 0; i < aimesh->mNumVertices; ++i){
			auto& v = aimesh->mNormals[i];
			mesh->data.normal.emplace_back(v.x);
			mesh->data.normal.emplace_back(v.y);
			mesh->data.normal.emplace_back(v.z);
		}
	}
	if(aimesh->HasFaces() == true){
		uint pointCount = 1;
		switch(aimesh->mPrimitiveTypes)
		{
			case aiPrimitiveType_POINT:
				mesh->data.primitiveType = EPrimitiveType::Points;
				pointCount = 1;
				break;
			case aiPrimitiveType_LINE:
				mesh->data.primitiveType = EPrimitiveType::Lines;
				pointCount = 2;
				break;
			case aiPrimitiveType_TRIANGLE:
				mesh->data.primitiveType = EPrimitiveType::Triangles;
				pointCount = 3;
				break;
			case aiPrimitiveType_POLYGON:
			default:
				LOG_ERR("PrimitiveType not supported, use aiProcess_Triangulate and aiProcess_SortByPType");
				break;
		}

		mesh->data.index.reserve(pointCount * aimesh->mNumFaces);
		for(uint i = 0; i < aimesh->mNumFaces; ++i){
			auto& face = aimesh->mFaces[i];
			ASSERT(face.mNumIndices == pointCount);
			for(uint f = 0; f < face.mNumIndices; ++f)
				mesh->data.index.emplace_back(face.mIndices[f]);
		}
	}
	if(aimesh->HasTangentsAndBitangents() == true){
		mesh->data.tangent.reserve(3 * aimesh->mNumVertices);
		for(uint i = 0; i < aimesh->mNumVertices; ++i){
			auto& v = aimesh->mTangents[i];
			mesh->data.tangent.emplace_back(v.x);
			mesh->data.tangent.emplace_back(v.y);
			mesh->data.tangent.emplace_back(v.z);
		}
	}

	mesh->data.count = aimesh->mNumVertices;

	CModelLoad::fillCRC32desc(mesh);
		
	return mesh;
}

UniquePtr<CMeshTransform> CModelLoad::GetTransformFromAssimp(aiMesh* aimesh, SharedPtr<CMesh>& mesh){
	auto meshT = UniquePtr<CMeshTransform>(new CMeshTransform(mesh));

	meshT->position = vec3(0.0f, 0.0f, 0.0f);
	meshT->rotation = mat3();

	return meshT;
}

//-------------------------------------------------------------------------------------------

bool CMesh::Create(){
	if(device == nullptr) return false;

	uint txCoordCount = 0;
	
	SVertexFormat fmt;
	if(this->data.vertex.size() > 0){
		SVertexAttributeDesc vdesc("vertex", 0, EValueType::float32, toEValueSizeVec(this->data.vertexDim));
		fmt.attributes.emplace_back(vdesc);
	}
	if(this->data.normal.size() > 0){
		SVertexAttributeDesc vdesc("normal", 1, EValueType::float32, toEValueSizeVec(this->data.normalDim));
		fmt.attributes.emplace_back(vdesc);
	}
	if(this->data.tangent.size() > 0){
		SVertexAttributeDesc vdesc("tangent", 2, EValueType::float32, toEValueSizeVec(this->data.tangentDim));
		fmt.attributes.emplace_back(vdesc);
	}
	{
		for(uint c = 0; c < RD_MAX_UV_CHANNELS; ++c) if(this->data.texCoord[c].size() > 0) ++txCoordCount; else break;

		if(txCoordCount == 1){
			SVertexAttributeDesc vdesc("texCoord", 3, EValueType::float32, toEValueSizeVec(this->data.texCoordDim[0]));
			fmt.attributes.emplace_back(vdesc);
		}
		else{
			for(uint c = 0; c < txCoordCount; ++c){
				std::string name = "texCoord[" + std::to_string(c) + "]";
				SVertexAttributeDesc vdesc(name, 3+c, EValueType::float32, toEValueSizeVec(this->data.texCoordDim[c]));
				fmt.attributes.emplace_back(vdesc);
			}
		}
	}
	fmt.layout = EAttributeLayout::Interleaved;

	std::vector<byte*> srcbytedata = { (byte*)this->data.vertex.data(), (byte*)this->data.normal.data(), (byte*)this->data.tangent.data() };
	for(uint c = 0; c < txCoordCount; ++c){ srcbytedata.emplace_back((byte*)this->data.texCoord[c].data()); }
	
	auto databytes = CVertexBuffer::InterleaveVertexData(fmt, srcbytedata, this->data.count);

	this->vertexBuffer = device->CreateVertexBuffer(fmt, (uint32)this->data.vertex.size(), {SRawData(databytes)});
	this->indexBuffer = device->CreateIndexBuffer(EValueType::uint32, (uint32)this->data.index.size(), this->data.index);

	return true;
}

//-------------------------------------------------------------------------------------------

bool CModel::Create(GPUDevice* dev){
	Assimp::Importer importer;

	const aiScene* scene = importer.ReadFile(descriptor.path, aiProcess_CalcTangentSpace | aiProcess_Triangulate | aiProcess_SortByPType);
	
	for(uint i = 0; i < scene->mNumMeshes; ++i)
	{
		auto pmesh = scene->mMeshes[i];
		if(pmesh != nullptr){
			CMesh* mesh = CModelLoad::LoadFromAssimp(dev, pmesh);
			if(mesh != nullptr)
			{
				auto meshManager = CSingleton<CMeshManager>::get();
				auto refMesh = meshManager->CreateSharedMesh(&mesh);
				if(mesh != nullptr){ 
					delete mesh; mesh = nullptr; }

				auto upMeshTransf = CModelLoad::GetTransformFromAssimp(pmesh, refMesh);
				this->meshes.emplace_back(std::move(upMeshTransf));

				refMesh->Create();
			}
		}
	}

	return true;
}


//-------------------------------------------------------------------------------------------
SharedPtr<CMesh> CMeshManager::CreateSharedMesh(CMesh** mesh){
	if(mesh == nullptr) return nullptr;
	auto found = FindByData(**mesh);
	if(found != nullptr) return found;

	SharedPtr<CMesh> shmesh = SharedPtr<CMesh>(*mesh); *mesh = nullptr;
	this->meshes.add(shmesh);
	return shmesh;
}

SharedPtr<CMesh> CMeshManager::FindByDescription(const SMeshDesc desc){
	for(uint i = 0; i < meshes.size(); ++i){
		auto mesh = meshes[i];
		if(mesh == nullptr) continue;
		if(mesh->compareCRC64(desc) == true){
			return mesh; }
	}
	return nullptr;
}
SharedPtr<CMesh> CMeshManager::FindByData(const CMesh& mesh){
	if(&mesh == nullptr) return nullptr;
	auto found = FindByDescription(mesh.descriptor);
	if(found == nullptr) return nullptr;
	if(found->data == mesh.data)
		return found;
	return nullptr;
}

bool CMeshManager::add(SharedPtr<CMesh> mesh){
	auto found = FindByData(*mesh.get());
	if(found != nullptr)
		return false;

	this->meshes.add(mesh);
	return true;
}