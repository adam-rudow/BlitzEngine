#pragma once
#include <vector>
//#include <d3d11_1.h>
//#include "Utils/RenderUtility.h"
//#include "GraphicsDriver.h"
#include "RenderUtility.h"

class ParticleBuffer;
class ResourceBufferGPU;
struct Effect;

class VertexData
{
public:
	VertexData(/*class GraphicsDriver& gDriver, */std::vector<float>& vertices, std::vector<uint32_t>& indices, ID3D11InputLayout* inputLayout, UINT16 vertSize);
	VertexData(/*class GraphicsDriver& gDriver, */Effect* pEffect, std::vector<float>& vertices, std::vector<uint32_t>& indices, UINT16 vertSize);
	
	// For buffers from src code -- no normals
	VertexData(/*class GraphicsDriver& gDriver, */std::vector<float>& vertices, std::vector<uint32_t>& indices, std::vector<float>& texCoords, ID3D11InputLayout* inputLayout, UINT16 vertSize);
	
	// Create vertex array from particle buffer data
	VertexData(/*class GraphicsDriver& gDriver, */ParticleBuffer* particles);

	~VertexData();

	inline void SetEffect(Effect* pEffect) { mEffect = pEffect; }

	void SetActive();

	inline size_t GetVertexCount() const { return mVertexCount; }
	inline size_t GetVertexSize() const { return mVertexSize; }
	inline size_t GetIndexCount() const { return mIndexCount; }
	inline BufferGPU* GetVertexBuffer() const { return mVertexBuffer; }
	inline BufferGPU* GetIndexBuffer() const { return mIndexBuffer; }
	inline Effect* GetMainEffect() const { return mEffect; }
	inline DrawCallType GetDrawCallType() const { return mDrawCallType; }

	inline UINT32 GetNumTriangles() const { return mIndexCount / 3; }

private:
	//class GraphicsDriver& mGraphics;
	BufferGPU* mVertexBuffer;
	BufferGPU* mIndexBuffer;
	VertexInputLayout* mInputLayout;
	size_t mVertexCount;
	size_t mVertexSize;
	size_t mIndexCount;
	DrawCallType mDrawCallType;
	//ID3D11Buffer** mVertexBuffers;

	D3D11_PRIMITIVE_TOPOLOGY mPrimitiveTopology;

	Effect* mEffect;
};