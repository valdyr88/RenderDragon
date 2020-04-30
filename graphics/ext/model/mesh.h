#ifndef MESH_H
#define MESH_H

#include "../../objects/vertex_buffer.h"

class CMesh : public CGraphicObject{

	SharedPtr<CVertexBuffer> vertices;
	SharedPtr<CIndexBuffer> indices;
public:

};

#endif //MESH_H