#include "ParticleCPU.h"

#include "Effect.h"
#include "VertexData.h"
#include "ResourceBufferGPU.h"
#include "Texture.h"
#include "EffectManager.h"

//[S1] Fix Dependecy
//#include "AssetLoader.h"

#include "GraphicsDriver.h"



ParticleBuffer::ParticleBuffer(unsigned long numParticles)
{
	mParticles.reserve(numParticles);
}

/// MOHRHARD CODE APPEND
ParticleBuffer::ParticleBuffer(float maxLifetime, float fastestEmissionRate, unsigned maxParticleEmission)
{
	int arraySize = /*2*/100 + maxParticleEmission * S_CAST(int, ceil(maxLifetime / fastestEmissionRate));
	int bufferPadding = 32 - arraySize % 32;
	mParticles.reserve(arraySize + bufferPadding);
}
/// MOHRHARD CODE APPEND END

//ParticleBuffer() : Components::Component(NULL, NULL) {}

void ParticleBuffer::InitMaterials(const char* textureFilename, const char* package, const char* renderEffectName, const char* computeUpdateEffectName, const char* computeSpawnEffectName)
{
	// Load texture to be used
	//[S1] Fix Dependecy
	mMaterial = nullptr;// AssetLoader::Instance()->LoadTexture(textureFilename);

	// Set the effects of these particles/buffer
	Effect* hComputeSpawnEffect = EffectManager::Instance()->GetEffect(computeSpawnEffectName);
	mEffects.push_back(hComputeSpawnEffect);

	// Set the effects of these particles/buffer
	Effect* hComputeUpdateEffect = EffectManager::Instance()->GetEffect(computeUpdateEffectName);
	mEffects.push_back(hComputeUpdateEffect);

	// Set effect for how these particles should be drawn
	Effect* hRenderEffect = EffectManager::Instance()->GetEffect(renderEffectName);
	mEffects.push_back(hRenderEffect);

}

void ParticleBuffer::CreateBuffer()
{
	// Create dummy vertex structure
	mVertexBuffer = new VertexData(this);

	// Buffer to hold arguments for indirect draw calls
	UINT32 indirectArgs[4] = { mParticles.size(), 1, 0, 0 };
	mIndirectArgsBuffer = GraphicsDriver::Instance()->CreateIndirectArgumentsBuffer(&indirectArgs[0]);

	mVertexUAVBufferGPU = new ResourceBufferGPU(*GraphicsDriver::Instance(), sizeof(Particle), mParticles.size(), true, true);
	mVertexUAVBufferGPU->CreateBuffer(&mParticles[0], true);

	mParticlesToSpawnBufferGPU = new ResourceBufferGPU(*GraphicsDriver::Instance(), sizeof(Particle), mParticles.size(), true, true);
	mParticlesToSpawnBufferGPU->CreateBuffer(&mParticles[0], true);
}

//void ParticleBuffer::CreateBufferForParticlesToSpawn(unsigned numParticles)
//{
//	return;
//	if (numParticles < 1)
//		numParticles = 1;
//
//	if (m_hParticlesToSpawnBufferGPU.isValid())
//	{
//		ResourceBufferGPU *prbgpu = m_hParticlesToSpawnBufferGPU.getObject<ResourceBufferGPU>();
//		int* newParticles = new int[1];
//		(*newParticles) = numParticles;
//
//		void* mappedData = prbgpu->mapToPtr();
//		memcpy(mappedData, newParticles, sizeof(int));
//
//		prbgpu->releasePtr();
//		free(newParticles);
//	}
//
//
//
//}

//void ParticleBuffer::MapParticlesToGPUBuffer()
//{
//	VertexBufferGPU* vBuf = mVertexBufferGPUH.getObject<VertexBufferGPU>();
//	vBuf->MapDataToBuffer(&mParticles[0], getByteSize());
//}