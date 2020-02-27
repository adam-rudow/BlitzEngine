//#include "Mesh.h"
//#include "PreCompiledHeader.h"
//#include "VertexData.h"
//#include <stdint.h>
//
//Mesh::Mesh()
//{
//
//}
//
//Mesh::~Mesh()
//{
//	//if (mVertexArray) delete(mVertexArray);
//}
//
//bool Mesh::Load(const char* fileName, GraphicsDriver& mGraphics, ID3D11InputLayout* inputLayout)
//{
//	//std::vector<float> vertices =
//	//{
//	//	-0.5f, 0.5f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f,  // top left
//	//	0.5f, 0.5f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f,   // top right
//	//	0.5f, -0.5f, 0.0f, 0.0f, 0.0f, 1.0f, 1.0f  // bottom right
//	//	//-0.5f, -0.5f, 0.0f, 0.0f, 0.0f, 1.0f, // bottom left
//	//};
//
//	//std::vector<float> vertices =
//	//{
//	//	-0.5f, 0.0f, 0.5f, 1.0f, 0.0f, 0.0f, 1.0f,  // top left
//	//	0.5f, 0.0f, 0.5f, 0.0f, 1.0f, 0.0f, 1.0f,   // top right
//	//	0.5f, 0.0f, -0.5f, 0.0f, 0.0f, 1.0f, 1.0f  // bottom right
//	//											   //-0.5f, -0.5f, 0.0f, 0.0f, 0.0f, 1.0f, // bottom left
//	//};
//
//	//std::vector<uint16_t> indices =
//	//{
//	//	0, 1, 2
//	//};
//
//	//mVertexArray = new VertexData(mGraphics, vertices, indices, inputLayout);
//
//	return true;
//}
//
//void Mesh::Init(GraphicsDriver& mGraphics, ID3D11InputLayout* inputLayout, std::vector<float>& vertices, std::vector<uint32_t>& indices)
//{
//	mVertexArray = new VertexData(mGraphics, vertices, indices, inputLayout, 8); // temp
//	
//}
//
//void Mesh::Init(GraphicsDriver& mGraphics, Effect* pEffect, std::vector<float>& vertices, std::vector<uint32_t>& indices, UINT16 vertNumFloats)
//{
//	mVertexArray = new VertexData(mGraphics, pEffect, vertices, indices, vertNumFloats);
//}
//
//void Mesh::Init(VertexData* vertexArray)
//{
//	mVertexArray = vertexArray;
//}
