#include "VertexData.h"
//[S1]
//#include "AssetLoader.h"
#include "Effect.h"
#include "EffectManager.h"
#include "ParticleCPU.h"

VertexData::VertexData(/*class GraphicsDriver& gDriver, */std::vector<float>& vertices, std::vector<uint32_t>& indices, ID3D11InputLayout* inputLayout, UINT16 vertSize)
	: /*mGraphics(gDriver), */mInputLayout(inputLayout), mEffect(NULL)
{
	mVertexBuffer = GraphicsDriver::Instance()->CreateVertexBuffer(vertices);
	mVertexCount = vertices.size() / static_cast<size_t>(vertSize);
	mVertexSize = vertSize;
	mIndexBuffer = GraphicsDriver::Instance()->CreateIndexBuffer(indices);
	mIndexCount = indices.size();
	mPrimitiveTopology = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;

	//[S1] Fix Dependecy
	//AssetLoader::Instance()->StoreVertexArray("", this);
}

VertexData::VertexData(/*class GraphicsDriver& gDriver, */Effect* pEffect, std::vector<float>& vertices, std::vector<uint32_t>& indices, UINT16 vertSize)
//	: mGraphics(gDriver)
{
	mEffect = pEffect;
	if(pEffect) mInputLayout = pEffect->mVSInputLayout;
	mVertexBuffer = GraphicsDriver::Instance()->CreateVertexBuffer(vertices);
	mVertexCount = vertices.size() / static_cast<size_t>(vertSize);
	mVertexSize = vertSize;
	mIndexBuffer = GraphicsDriver::Instance()->CreateIndexBuffer(indices);
	mIndexCount = indices.size();
	mDrawCallType = VBUF_INDBUF;
	mPrimitiveTopology = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;

	//[S1] Fix Dependecy
	//AssetLoader::Instance()->StoreVertexArray("", this);
}

// For buffers from src code
VertexData::VertexData(/*class GraphicsDriver& gDriver, */std::vector<float>& vertices, std::vector<uint32_t>& indices, std::vector<float>& texCoords, ID3D11InputLayout* inputLayout, UINT16 vertSize)
	: /*mGraphics(gDriver), */mInputLayout(inputLayout), mEffect(NULL)
{
	mVertexBuffer = GraphicsDriver::Instance()->CreateVertexBufferFromSrc_Minimal(vertices, texCoords);
	mVertexCount = vertices.size() / static_cast<size_t>(vertSize);
	mVertexSize = vertSize;
	mIndexBuffer = GraphicsDriver::Instance()->CreateIndexBuffer(indices);
	mIndexCount = indices.size();
	mDrawCallType = VBUF_INDBUF;
	mPrimitiveTopology = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;

	//[S1] Fix Dependecy
	//AssetLoader::Instance()->StoreVertexArray("", this);
}

VertexData::VertexData(/*class GraphicsDriver& gDriver, */ParticleBuffer* particles) //: mGraphics(gDriver)
{
	UINT32 ivbval = 0;
	UINT32 numFloatsPerParticle = sizeof(Particle) / sizeof(float);
	unsigned long numParticles = particles->mParticles.size();

	std::vector<float> rawData(numParticles * numFloatsPerParticle);
	//memset(&rawData[0], 0, rawData.size());

	for (unsigned long ip = 0; ip < numParticles; ip++)
	{
		memcpy(&rawData[ivbval], &particles->mParticles[ip], sizeof(Particle));

		ivbval += numFloatsPerParticle;
	}

	mEffect = particles->GetRenderEffect();
	mInputLayout = EffectManager::Instance()->GetInputLayout(VertexFormat_StdParticle);
	mVertexBuffer = NULL;// gDriver.CreateVertexBuffer(rawData, true);
	mVertexCount = numParticles;
	mVertexSize = sizeof(Particle);
	mIndexBuffer = NULL;
	mIndexCount = 0;
	mDrawCallType = VBUF_INDIRECT;
	mPrimitiveTopology = D3D11_PRIMITIVE_TOPOLOGY_POINTLIST;

	//[S1] Fix Dependecy
	//AssetLoader::Instance()->StoreVertexArray("", this);
}

VertexData::~VertexData()
{
	if (mVertexBuffer)	mVertexBuffer->Release();
	if (mIndexBuffer)	mIndexBuffer->Release();
}

void VertexData::SetActive()
{
	GraphicsDriver* gDriver = GraphicsDriver::Instance();
	gDriver->SetInputTopology(mPrimitiveTopology);

	if (mDrawCallType != VBUF_INDIRECT)
	{
		gDriver->BindInputLayout(mInputLayout);
		gDriver->BindVertexBuffer(mVertexBuffer, mVertexSize);
		gDriver->BindIndexBuffer(mIndexBuffer, mIndexCount);
	}
	else
	{
		if (mEffect) mEffect->SetActive();
		//mGraphics.BindInputLayout(mInputLayout);
		//mGraphics.BindVertexBuffer(mVertexBuffer, mVertexSize);
	}

	// Only bind effect if we set one initially
	//if (mEffect)
	//	mEffect->SetActive();
		//mGraphics.BindEffect(mEffect);
}
