#pragma once

// Outer-Engine includes
#include <assert.h>
#include <string>
#include <vector>

// Inter-Engine includes
#include "Math/Vector3.h"
#include "Math/Vector2.h"
//#include "Effect.h"
//#include "VertexData.h"
//#include "ResourceBufferGPU.h"
////#include "PrimeEngine/Geometry/MaterialCPU/MaterialSetCPU.h"
//#include "EffectManager.h"

// Sibling/Children includes
//#include "Mesh.h"

struct ID3D11Buffer;
struct Effect;
class ResourceBufferGPU;
struct TextureGPU;
class VertexData;

struct Particle
{
	// Constructor -------------------------------------------------------------
	Particle()
	{
				
	}

	~Particle() {}

	Vector3 mPosition;
	Vector3 mVelocity;
	Vector2 mSize;
	float mLifeSpan;
};

//template <typename Particle_t>
//struct ParticleBuffer : PE::PEAllocatableAndDefragmentable
class ParticleBuffer// : Components::Component
{

public:

	ParticleBuffer(unsigned long numParticles);

	ParticleBuffer(float maxLifetime, float fastestEmissionRate, unsigned maxParticleEmission);

	//ParticleBuffer() : Components::Component(NULL, NULL) {}

	void InitMaterials(const char* textureFilename, const char* package, const char* renderEffectName, const char* computeUpdateEffectName, const char* computeSpawnEffectName);

	void CreateBuffer();

	void CreateBufferForParticlesToSpawn(unsigned numParticles);

	void MapParticlesToGPUBuffer();

	// Start of the vertex data in memory
	inline void* getStartAddress() { return &mParticles[0]; }

	inline unsigned getByteSize() { return mParticles.size() * sizeof(Particle); }

	// Get Effects
	inline Effect* GetSpawnEffect() const { return mEffects[0]; }
	inline Effect* GetUpdateEffect() const { return mEffects[1]; }
	inline Effect* GetRenderEffect() const { return mEffects[2]; }

	// List of the actual Particle objects
	std::vector<Particle> mParticles;

	// Handle to the vertex buffer the holds the particle data to be bound to the GPU
	VertexData* mVertexBuffer;

	// Buffer that holds the arguments for the indirect draw call
	ID3D11Buffer* mIndirectArgsBuffer;

	// Handle to the list of materials that will be textured onto these particles
	TextureGPU* mMaterial;

	// Handle to the Unordered Access Buffer that will be read/written by Compute Shader
	ResourceBufferGPU* mVertexUAVBufferGPU;

	// Handle to the consume buffer used to "spawn" particles in the Compute Shader
	ResourceBufferGPU* mParticlesToSpawnBufferGPU;

	// List of effects (set of shaders for the GPU pipeline), could be used for multiple passes for extra effects
	std::vector<Effect*> mEffects;

};
