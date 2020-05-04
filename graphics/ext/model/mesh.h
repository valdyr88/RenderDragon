#ifndef MESH_H
#define MESH_H

#include "../../objects/vertex_buffer.h"
//-------------------------------------------------------------------------------------------

struct SMeshDesc{

};

class CMesh : public CGraphicObject{

	SharedPtr<CVertexBuffer> vertices;
	SharedPtr<CIndexBuffer> indices;
public:

};
//-------------------------------------------------------------------------------------------

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