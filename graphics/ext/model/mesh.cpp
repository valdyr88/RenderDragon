//#define ASSIMP_DLL
#include <assimp/types.h>
#include <assimp/config.h>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
//
#include "../../objects/device.h"
#include "../../utils/hash.h"
#include "../../descriptors/shader_parser.h"
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
	static SMaterialDesc CreateMaterialDescFromAssimp(uint id, std::string name, std::string path, aiMaterial* aimat);

	static void CorrectTangentsAssimp(aiMesh* aimesh, uint face, bool dirCW = true);

	template<typename type>
	static uint64 crc64t(std::vector<type>& v){
		return hash::crc64((uint8*)v.data(), sizeof(type)*v.size()); }

	static void fillCRC32desc(CMesh* mesh){
		mesh->descriptor.crc64hash.vertex = CModelLoad::crc64t<float32>(mesh->data.vertex);
		mesh->descriptor.crc64hash.index = CModelLoad::crc64t<uint32>(mesh->data.index);
		mesh->descriptor.crc64hash.normal = CModelLoad::crc64t<float32>(mesh->data.normal);
		mesh->descriptor.crc64hash.tangent = CModelLoad::crc64t<float32>(mesh->data.tangent);
		mesh->descriptor.crc64hash.flags = CModelLoad::crc64t<uint32>(mesh->data.flags);
		for(uint c = 0; c < RD_MAX_UV_CHANNELS; ++c)
			mesh->descriptor.crc64hash.texCoord[c] = CModelLoad::crc64t<float32>(mesh->data.texCoord[c]);
	}
};

bool isClockwise(aiVector3t<ai_real> a, aiVector3t<ai_real> b, aiVector3t<ai_real> c, aiVector3t<ai_real> N){
	return isClockwise(vec3(a.x,a.y,a.z), vec3(b.x,b.y,b.z), vec3(c.x,c.y,c.z), vec3(N.x,N.y,N.z)); }
bool isClockwise(aiVector2t<ai_real> a, aiVector2t<ai_real> b, aiVector2t<ai_real> c){
	return isClockwise(vec2(a.x,a.y), vec2(b.x,b.y), vec2(c.x,c.y)); }
bool isClockwise(aiVector3t<ai_real> a, aiVector3t<ai_real> b, aiVector3t<ai_real> c){
	return isClockwise(vec2(a.x,a.y), vec2(b.x,b.y), vec2(c.x,c.y)); }

void CModelLoad::CorrectTangentsAssimp(aiMesh* aimesh, uint fId, bool dirCW){
	if(fId >= aimesh->mNumFaces) return;
	auto& face = aimesh->mFaces[fId];

	std::vector<vec2> UVs;
	std::vector<vec3> Ns;
	std::vector<vec3> Ts;
	std::vector<vec3> Bs;
	std::vector<aiVector3t<ai_real>*> aiTs;

	const vec2 u = vec2(0.0, 1.0);
	const vec2 v = vec2(1.0, 0.0);

	for(uint f = 0; f < face.mNumIndices; ++f){
		uint id = face.mIndices[f];
		auto& uv = aimesh->mTextureCoords[0][id];
		auto& n = aimesh->mNormals[id];
		auto& t = aimesh->mTangents[id];
		auto& b = aimesh->mBitangents[id];

		UVs.emplace_back(vec2(uv.x, uv.y));
		Ns.emplace_back(vec3(n.x, n.y, n.z));
		Ts.emplace_back(vec3(t.x, t.y, t.z));
		Bs.emplace_back(vec3(b.x, b.y, b.z));
		aiTs.emplace_back(&t);
	}

	bool isCW = isClockwise(UVs[0], UVs[1], UVs[2]);

	if(isCW != dirCW)
	{
		for(uint f = 0; f < face.mNumIndices; ++f){
			auto& t = *aiTs[f];
			t = -t;
		}
	}
	
}

CMesh* CModelLoad::LoadFromAssimp(GPUDevice* dev, aiMesh* aimesh){
	SMeshDesc desc;
	desc.name = aimesh->mName.C_Str();
		
	CMesh* mesh = new CMesh(dev, desc);

	mesh->data.flags.resize(aimesh->mNumVertices);
	for(auto& f : mesh->data.flags) f = 0x00000000;

	if(aimesh->HasPositions() == true){
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

			for(uint f = 0; f < face.mNumIndices; ++f){
				mesh->data.index.emplace_back(face.mIndices[f]);
			}
		}
	}
	if(false && aimesh->HasTangentsAndBitangents() == true){
		mesh->data.tangent.reserve(3 * aimesh->mNumVertices);
		for(uint i = 0; i < aimesh->mNumVertices; ++i){
			auto& ait = aimesh->mTangents[i];
			auto& aib = aimesh->mBitangents[i];
			auto& ain = aimesh->mNormals[i];

			vec3 t = vec3(ait.x, ait.y, ait.z);

			mesh->data.tangent.emplace_back(t.x);
			mesh->data.tangent.emplace_back(t.y);
			mesh->data.tangent.emplace_back(t.z);
		}
	}
	if(mesh->data.primitiveType == EPrimitiveType::Triangles && (aimesh->HasPositions() && aimesh->HasNormals() && aimesh->HasFaces() && aimesh->HasTextureCoords(0))){
		mesh->data.tangent.reserve(3*aimesh->mNumVertices);
		{
			auto asvec3 = [](const aiVector3D& v){ return vec3(v.x, v.y, v.z); };
			auto asvec2 = [](const aiVector3D& v){ return vec2(v.x, v.y); };

			const uint vertexCount = aimesh->mNumVertices;
			const uint triangleCount = aimesh->mNumFaces;

			uint isCCWcount = 0;

			vec3* tan1 = new vec3[vertexCount * 2];
			vec3* tan2 = tan1 + vertexCount;
			zero_memory(tan1, vertexCount * sizeof(vec3) * 2);

			bool* isUVCCW = new bool[vertexCount];
			bool* isUVWindingDifferent = new bool[vertexCount];
			zero_memory(isUVCCW, vertexCount*sizeof(bool));
			zero_memory(isUVWindingDifferent, vertexCount*sizeof(bool));

			for(uint a = 0; a < triangleCount; ++a)
			{
				auto& face = aimesh->mFaces[a];

				long i1 = face.mIndices[0];
				long i2 = face.mIndices[1];
				long i3 = face.mIndices[2];

				const auto& v1 = aimesh->mVertices[i1];
				const auto& v2 = aimesh->mVertices[i2];
				const auto& v3 = aimesh->mVertices[i3];

				const auto& w1 = aimesh->mTextureCoords[0][i1];
				const auto& w2 = aimesh->mTextureCoords[0][i2];
				const auto& w3 = aimesh->mTextureCoords[0][i3];

				const auto& n = aimesh->mNormals[i1];
				
				float x1 = v2.x - v1.x;
				float x2 = v3.x - v1.x;
				float y1 = v2.y - v1.y;
				float y2 = v3.y - v1.y;
				float z1 = v2.z - v1.z;
				float z2 = v3.z - v1.z;

				float s1 = w2.x - w1.x;
				float s2 = w3.x - w1.x;
				float t1 = w2.y - w1.y;
				float t2 = w3.y - w1.y;

				bool bIsCCW = isClockwise(w1, w2, w3);
				bool bIsUVWindingDifferent = isClockwise(w1, w2, w3) != isClockwise(v1, v2, v3, n);
				
				if(bIsCCW == true){ isUVCCW[i1] = true; }
				if(bIsUVWindingDifferent == true){ isUVWindingDifferent[i1] = true; }

				float r = 1.0f / (s1 * t2 - s2 * t1);
				vec3 sdir((t2 * x1 - t1 * x2) * r, (t2 * y1 - t1 * y2) * r,
					(t2 * z1 - t1 * z2) * r);
				vec3 tdir((s1 * x2 - s2 * x1) * r, (s1 * y2 - s2 * y1) * r,
					(s1 * z2 - s2 * z1) * r);

				tan1[i1] += sdir;
				tan1[i2] += sdir;
				tan1[i3] += sdir;

				tan2[i1] += tdir;
				tan2[i2] += tdir;
				tan2[i3] += tdir;
			}

			for(uint a = 0; a < vertexCount; ++a)
			{
				const vec3 n = asvec3(aimesh->mNormals[a]);
				const vec3& t = tan1[a];

				// Gram-Schmidt orthogonalize
				vec3 tangent = glm::normalize(t - n * glm::dot(n, t));

				mesh->data.tangent.emplace_back(tangent.x);
				mesh->data.tangent.emplace_back(tangent.y);
				mesh->data.tangent.emplace_back(tangent.z);/**/

				// Calculate handedness
				bool bHandedness = glm::dot(glm::cross(n, t), tan2[a]) < 0.0f;

				uint32& flag = mesh->data.flags[a];
				flag = (bHandedness)? flag | CMesh::VertexFlags::TangentHandedness : flag;
				flag = (isUVCCW[a]) ? flag | CMesh::VertexFlags::UVisCCW : flag;
				flag = (isUVWindingDifferent[a]) ? flag | CMesh::VertexFlags::UVWindIsDifferent : flag;
			}
			
			delete[] tan1; tan1 = tan2 = nullptr;
			delete[] isUVCCW; isUVCCW = nullptr;
			delete[] isUVWindingDifferent; isUVWindingDifferent = nullptr;
		}

		/*
		for(uint i = 0; i < aimesh->mNumFaces; ++i){
			auto& face = aimesh->mFaces[i];
			ASSERT(face.mNumIndices == 3);

			auto asvec3 = [](aiVector3D& v){ return vec3(v.x, v.y, v.z); };
			auto asvec2 = [](aiVector3D& v){ return vec2(v.x, v.y); };

			vec3 V[3], N[3]; uint vId[3]; vec2 UV[3];
			for(uint f = 0; f < face.mNumIndices; ++f){
				vId[f] = face.mIndices[f];

				V[f] = asvec3(aimesh->mVertices[vId[f]]);
				N[f] = glm::normalize(asvec3(aimesh->mNormals[vId[f]]));
				UV[f] = asvec2(aimesh->mTextureCoords[0][f]);
			}

			vec3 n = calcNormal(V[0], V[1], V[2]);

			//izracunat tangente iz V[0]-V[2] ili V[1]-V[2] cross sa N, a u slucaju da dot(n,N) < 0.0f onda je obrnuta tangenta
			vec3 T[3];
			vec3 dv0 = glm::normalize(V[0] - V[2]), dv1 = glm::normalize(V[1] - V[2]);
			for(uint f = 0; f < face.mNumIndices; ++f){
				T[f] = glm::normalize(glm::cross(dv0, N[f])); }

			if(isClockwise(V[0],V[1],V[2],n) != isClockwise(UV[0],UV[1],UV[2])){
				for(uint f = 0; f < face.mNumIndices; ++f)
					T[f] = -T[f];
			}
			for(uint f = 0; f < face.mNumIndices; ++f){
				if(glm::dot(N[f], n) < 0.0f)
					T[f] = -T[f];
			}
			for(uint f = 0; f < face.mNumIndices; ++f){
				if(isClockwise(V[0], V[1], V[2], N[f]) != isClockwise(UV[0], UV[1], UV[2]))
					T[f] = -T[f];
			}
			
			for(uint v = 0; v < 3; ++v){
				mesh->data.tangent[vId[v]*3+0] = T[v].x;
				mesh->data.tangent[vId[v]*3+1] = T[v].y;
				mesh->data.tangent[vId[v]*3+2] = T[v].z;
			}
		}*/
	}

	//for(auto& f : mesh->data.flags) f = rand() % 2;

	mesh->data.count = aimesh->mNumVertices;

	CModelLoad::fillCRC32desc(mesh);
	
	LOG("loaded mesh: %s", mesh->descriptor.name.c_str());
	return mesh;
}

UniquePtr<CMeshTransform> CModelLoad::GetTransformFromAssimp(aiMesh* aimesh, SharedPtr<CMesh>& mesh){
	auto meshT = UniquePtr<CMeshTransform>(new CMeshTransform(mesh));

	meshT->position = vec3(0.0f, 0.0f, 0.0f);
	meshT->rotation = mat3();

	return meshT;
}

SMaterialDesc CModelLoad::CreateMaterialDescFromAssimp(uint id, std::string modelName, std::string modelPath, aiMaterial* aimat){
	SMaterialDesc desc;
	if(aimat == nullptr) return desc;

	if(aimat->GetName().length > 0)
		desc.name = aimat->GetName().C_Str();
	else
		desc.name = "material_" + modelName + "_" + std::to_string(id);

	//Textures
	{
		aiString path;
		uint uvindex = 0;
		aiTextureMapping mapping;
		aiTextureMapMode mapmode[4] = { aiTextureMapMode_Wrap, aiTextureMapMode_Wrap, aiTextureMapMode_Wrap, aiTextureMapMode_Wrap };

		auto toMaterialTexture = [&](uint ty, uint t, EMaterialTextureType txType){
			aiReturn rtn = aimat->GetTexture((aiTextureType)ty, t, &path, &mapping, &uvindex, nullptr, nullptr, mapmode);
			if(rtn != aiReturn::aiReturn_SUCCESS) return;

			SMaterialTexture txDesc;
			{
				txDesc.path = modelPath + "/" + path.C_Str();
				txDesc.name = path.C_Str();
				txDesc.type = (uint)txType;
			}
			desc.textures.emplace_back(txDesc);
		};

		for(uint ty = 0; ty < aiTextureType_UNKNOWN; ++ty){
			switch((aiTextureType)ty)
			{
				case aiTextureType_NONE:
					for(uint t = 0; t < aimat->GetTextureCount(aiTextureType_NONE); ++t){
						toMaterialTexture(aiTextureType_NONE, t, EMaterialTextureType::None);}
					break;
				case aiTextureType_DIFFUSE:
					for(uint t = 0; t < aimat->GetTextureCount(aiTextureType_DIFFUSE); ++t){
						toMaterialTexture(aiTextureType_DIFFUSE, t, EMaterialTextureType::Albedo);}
					break;
				case aiTextureType_SPECULAR:
					for(uint t = 0; t < aimat->GetTextureCount(aiTextureType_SPECULAR); ++t){
						toMaterialTexture(aiTextureType_SPECULAR, t, EMaterialTextureType::Specular);}
					break;
				case aiTextureType_AMBIENT:
					for(uint t = 0; t < aimat->GetTextureCount(aiTextureType_AMBIENT); ++t){
						toMaterialTexture(aiTextureType_AMBIENT, t, EMaterialTextureType::AmbientMap);}
					break;
				case aiTextureType_EMISSIVE:
					for(uint t = 0; t < aimat->GetTextureCount(aiTextureType_EMISSIVE); ++t){
						toMaterialTexture(aiTextureType_EMISSIVE, t, EMaterialTextureType::Emissive);}
					break;
				case aiTextureType_HEIGHT:
					for(uint t = 0; t < aimat->GetTextureCount(aiTextureType_HEIGHT); ++t){
						toMaterialTexture(aiTextureType_HEIGHT, t, EMaterialTextureType::Height);}
					break;
				case aiTextureType_NORMALS:
					for(uint t = 0; t < aimat->GetTextureCount(aiTextureType_NORMALS); ++t){
						toMaterialTexture(aiTextureType_NORMALS, t, EMaterialTextureType::Normal);}
					break;
				case aiTextureType_SHININESS:
					for(uint t = 0; t < aimat->GetTextureCount(aiTextureType_SHININESS); ++t){
						toMaterialTexture(aiTextureType_SHININESS, t, EMaterialTextureType::Roughness);}
					break;
				case aiTextureType_OPACITY:
					for(uint t = 0; t < aimat->GetTextureCount(aiTextureType_OPACITY); ++t){
						toMaterialTexture(aiTextureType_OPACITY, t, EMaterialTextureType::Opacity); }
					break;
				case aiTextureType_DISPLACEMENT:
					for(uint t = 0; t < aimat->GetTextureCount(aiTextureType_DISPLACEMENT); ++t){
						toMaterialTexture(aiTextureType_DISPLACEMENT, t, EMaterialTextureType::Height); }
					break;
				case aiTextureType_LIGHTMAP:
					for(uint t = 0; t < aimat->GetTextureCount(aiTextureType_LIGHTMAP); ++t){
						toMaterialTexture(aiTextureType_LIGHTMAP, t, EMaterialTextureType::Emissive); }
					break;
				case aiTextureType_REFLECTION:
					for(uint t = 0; t < aimat->GetTextureCount(aiTextureType_REFLECTION); ++t){
						toMaterialTexture(aiTextureType_REFLECTION, t, EMaterialTextureType::AmbientMap); }
					break;
				case aiTextureType_BASE_COLOR:
					for(uint t = 0; t < aimat->GetTextureCount(aiTextureType_BASE_COLOR); ++t){
						toMaterialTexture(aiTextureType_BASE_COLOR, t, EMaterialTextureType::Albedo); }
					break;
				case aiTextureType_NORMAL_CAMERA:
					for(uint t = 0; t < aimat->GetTextureCount(aiTextureType_NORMAL_CAMERA); ++t){
						toMaterialTexture(aiTextureType_NORMAL_CAMERA, t, EMaterialTextureType::Normal);}
					break;
				case aiTextureType_EMISSION_COLOR:
					for(uint t = 0; t < aimat->GetTextureCount(aiTextureType_EMISSION_COLOR); ++t){
						toMaterialTexture(aiTextureType_EMISSION_COLOR, t, EMaterialTextureType::Emissive);}
					break;
				case aiTextureType_METALNESS:
					for(uint t = 0; t < aimat->GetTextureCount(aiTextureType_METALNESS); ++t){
						toMaterialTexture(aiTextureType_METALNESS, t, EMaterialTextureType::Metalness);}
					break;
				case aiTextureType_DIFFUSE_ROUGHNESS:
					for(uint t = 0; t < aimat->GetTextureCount(aiTextureType_DIFFUSE_ROUGHNESS); ++t){
						toMaterialTexture(aiTextureType_DIFFUSE_ROUGHNESS, t, EMaterialTextureType::Roughness);}
					break;
				case aiTextureType_AMBIENT_OCCLUSION:
					for(uint t = 0; t < aimat->GetTextureCount(aiTextureType_AMBIENT_OCCLUSION); ++t){
						toMaterialTexture(aiTextureType_AMBIENT_OCCLUSION, t, EMaterialTextureType::AmbientOcclusion);}
					break;
				case aiTextureType_UNKNOWN:
					for(uint t = 0; t < aimat->GetTextureCount(aiTextureType_UNKNOWN); ++t){
						toMaterialTexture(aiTextureType_UNKNOWN, t, EMaterialTextureType::None);}
					break;
				default:
					break;
			}
		}
	}

	//Params
	if(aimat->mNumProperties > 0)
	{
		desc.paramGroups.resize(1);
		desc.paramGroups[0].ubstruct = "-"; //no name yet

		for(uint i = 0; i < aimat->mNumProperties; ++i){
			auto mproperty = aimat->mProperties[i];			
			SMaterialParam param;

			param.name = &(mproperty->mKey.C_Str()[1]);
			param.size = EValueSize::scalar;
			
			switch(mproperty->mType)
			{
				case aiPTI_Float:
					param.count = 1;
					param.type = EValueType::float32;
					param.Reserve();
					param.setAs<float32>((float*)mproperty->mData, 1);
					break;
				case aiPTI_Double:
					param.count = 1;
					param.type = EValueType::float64;
					param.Reserve();
					param.setAs<float64>((float64*)mproperty->mData, 1);
					break;
				case aiPTI_String:
					{
						aiString str;
						aimat->Get<aiString>(mproperty->mKey.C_Str(), 0, 0, str);

						param.type = EValueType::char8;
						param.count = str.length;
						param.Reserve();
						//i m the boss. and princess. please go pajki. and kup kup
						param.setAs<char8>((const char8*)str.C_Str(), param.count);
					}
					break;
				case aiPTI_Integer:
					param.count = 1;
					param.type = EValueType::uint32;
					param.Reserve();
					param.setAs<uint32>((uint32*)mproperty->mData, 1);
					break;
				case aiPTI_Buffer:
					break;
				default:
					break;
			}

			desc.paramGroups[0].params.emplace_back(param);
		}
	}

	return desc;
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
	if(this->data.flags.size() > 0){
		SVertexAttributeDesc vdesc("flags", 3, EValueType::uint32, toEValueSizeVec(1));
		fmt.attributes.emplace_back(vdesc);
	}
	{
		for(uint c = 0; c < RD_MAX_UV_CHANNELS; ++c) if(this->data.texCoord[c].size() > 0) ++txCoordCount; else break;

		if(txCoordCount == 1){
			SVertexAttributeDesc vdesc("texCoord", 4, EValueType::float32, toEValueSizeVec(this->data.texCoordDim[0]));
			fmt.attributes.emplace_back(vdesc);
		}
		else{
			for(uint c = 0; c < txCoordCount; ++c){
				std::string name = "texCoord[" + std::to_string(c) + "]";
				SVertexAttributeDesc vdesc(name, 4+c, EValueType::float32, toEValueSizeVec(this->data.texCoordDim[c]));
				fmt.attributes.emplace_back(vdesc);
			}
		}
	}
	fmt.layout = EAttributeLayout::Interleaved;

	std::vector<byte*> srcbytedata = { (byte*)this->data.vertex.data(), (byte*)this->data.normal.data(), (byte*)this->data.tangent.data(), (byte*)this->data.flags.data() };
	for(uint c = 0; c < txCoordCount; ++c){ srcbytedata.emplace_back((byte*)this->data.texCoord[c].data()); }
	
	auto databytes = CVertexBuffer::InterleaveVertexData(fmt, srcbytedata, this->data.count);

	this->vertexBuffer = device->CreateVertexBuffer(fmt, (uint32)this->data.vertex.size(), {SRawData(databytes)});
	this->indexBuffer = device->CreateIndexBuffer(EValueType::uint32, (uint32)this->data.index.size(), this->data.index);

	return true;
}

CShaderDefines CMesh::ConstructVertexFlagsShaderMacros(){
	CShaderDefines defines;
#define CMesh_MacroStringFromEnum(c,e) ""#c"_"#e, std::to_string((uint)e)

	defines.add(CMesh_MacroStringFromEnum(VertexFlags,UVisCCW));
	defines.add(CMesh_MacroStringFromEnum(VertexFlags,TangentHandedness));
	defines.add(CMesh_MacroStringFromEnum(VertexFlags,UVWindIsDifferent));

	return defines;
}

//-------------------------------------------------------------------------------------------

//one-time init calls
bool CModel::StaticInit(){
	static bool isInit = false; if(isInit == true) return false;
	{
		auto globalShaderDefines = CSingleton<CShaderDefines>::get()->add(CMesh::ConstructVertexFlagsShaderMacros());
	}
	isInit = true; return true;
}

bool CModel::Create(GPUDevice* dev){
	static bool bStaticInit = CModel::StaticInit();

	Assimp::Importer importer;

	std::string dirpath = descriptor.path;
	{
		sizetype pos = dirpath.find_last_of('/');
		dirpath.erase(pos, dirpath.size() - pos);
	}
	//aiProcess_CalcTangentSpace | 
	const aiScene* scene = importer.ReadFile(descriptor.path, aiProcess_Triangulate | aiProcess_SortByPType | aiProcess_OptimizeMeshes | aiProcess_RemoveRedundantMaterials);
	
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
	
	for(uint i = 0; i < scene->mNumMaterials; ++i)
	{
		auto pmat = scene->mMaterials[i];
		if(pmat != nullptr)
		{
			SMaterialDesc desc = CModelLoad::CreateMaterialDescFromAssimp(i, descriptor.name, dirpath, pmat);
			auto matManager = CSingleton<CMaterialManager>::get();
			auto refMaterial = matManager->CreateMaterial(desc);
			auto matInstance = refMaterial->CreateInstance(dev, {});
			this->materials.emplace_back(matInstance);

			if(scene->mNumMeshes == scene->mNumMaterials){
				SMeshMaterialPair pair(this->meshes[i].get(), this->materials[i].get());
				this->meshMaterialPairs.emplace_back(pair);
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