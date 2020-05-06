#ifndef MESH_H
#define MESH_H

#include "../../objects/vertex_buffer.h"
#include "../material/material.h"
//-------------------------------------------------------------------------------------------

struct SMeshDesc{

};

class CMesh : public CGraphicObject{

	SharedPtr<CVertexBuffer> vertices;
	SharedPtr<CIndexBuffer> indices;
public:

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
	
};

struct SModelDesc{

};

class CModel{

};
//-------------------------------------------------------------------------------------------

struct SModelAssemblyDesc{

};

class CModelAssembly{

};
//-------------------------------------------------------------------------------------------

#endif //MESH_H