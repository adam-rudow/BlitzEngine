#pragma once
#include "Math/Matrix4.h"

#include "DrawComponent.h"

#include "ParticleCPU.h"
#include "ParticleSystemTypesEnum.h"
#include "ParticleSystem.h"
#include "Renderer.h"

struct EmitterInitData
{

};

//class ParticleBuffer;

//template <typename Particle_t>
class RENDERER_EXPORT Emitter : public DrawComponent
{
private:
	void InitializeInternal(unsigned long numParticles, float maxSpawnRate = 5.0f, float maxLifeTime = 2.5f);

public:

	/*************************************************
	* Constructors
	**************************************************/
	//Emitter(PE::GameContext &context, PE::MemoryArena arena, PE::Handle hMyself, const Events::Event_CREATE_EMITTER *pEvt);
	Emitter(/*class Game& game, */ParticleSystemTypes type);

	void Initialize(float maxSpawnRate, float maxLifeTime);
	//void Initialize(Events::Event *pEvt);

	/*************************************************
	* Member Methods
	**************************************************/
	void Update(float frameTime);
	virtual void Draw(class Renderer& render) override;
	
	Vector3 GetRandomizeDirection();

	inline ParticleBuffer* GetParticleBuffer() const { return mParticleBuffer; }

	virtual VertexData* GetVertexData() override { return mParticleBuffer->mVertexBuffer; }

	PerParticleEmitterConstants CreateEmitterConstants();

	/*************************************************
	* Templated Member Methods
	**************************************************/
	//template<typename type>
	//type randInRange(type min, type max);
	
	/*************************************************
	* Variables
	**************************************************/
	UINT16 mParticleIndices[2] = {0, 0};

	Matrix4 mBase;
	char mName[32];

	// Lifespan min/max
	float mLambdaFloor;
	float mLambdaCeiling;

	// Initial Force
	float mForceMin;
	float mForceMax;

	// Max initial angle deviation
	float mThetaClamp;

	// Emission min/max time range and current emission time
	float mEmissionFloor;
	float mEmissionCeiling;
	float mEmissionTime;

	// Number of Particles Emitted
	UINT16 mParticleEmitFloor;
	UINT16 mParticleEmitCeiling;

	// Force Decay
	float mDecayTime; // We could have this vary too...
	float mGravity;

	// ParticleSize Floor and Ceiling
	float mParticleSizeFloor, mParticleSizeCeiling;
	float mParticlesToSpawn;
	float mParticlesSpanwedThisFrame;

	int mRandomSeed;

	// Container object that holds the material/vertex/effect info to be passed to the GPU
	ParticleBuffer* mParticleBuffer;
	std::vector<float> mDecayTimers;

	BufferGPU* mEmitterConstBuffer;

	ParticleSystemTypes mParticleType;
	
	EmitterPerlinData mRandomSineTriples;


	UINT32 mConsumeAppendIter = 0;

	bool firstFrame = true;

	//PEAlpheBlendState ??? TODO: look more into this as it isn't used in the code base yet


};
