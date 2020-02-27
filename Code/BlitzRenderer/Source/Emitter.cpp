
#include "Utils/AssertDbg.h"
#include "Utils/Utility.h"
#include "Utils/RandomGen.h"

#include "Effect.h"
#include "Emitter.h"
#include "GraphicsDriver.h"
#include "Texture.h"
#include "SamplerState.h"



//template <typename Particle_t>
Emitter::Emitter(/*class Game& game, */ParticleSystemTypes type) : DrawComponent(/*game*/)
	//, mParticleBuffer(context, arena) // TODO: pass in number or particles (size of buffer) via lua script or based on particle system/emitter type
{
	mParticleType = type;
	mForceMin = 1.0f;// 0.025f;
	mDrawOrderMask = DrawOrder_Last;

	DrawComponent::Register();
}

void Emitter::Initialize(float maxSpawnRate, float maxLifeTime)
{
	unsigned long arraySize = S_CAST(unsigned long, ceil(maxSpawnRate)) * S_CAST(unsigned long, ceil(maxLifeTime));
	int bufferPadding = 32 - arraySize % 32;
	InitializeInternal(arraySize + bufferPadding, maxSpawnRate, maxLifeTime);
}

#if 1 //_DEBUG_
// We will most likely need to expand this with some sort of create emitter
//template <typename Particle_t>
void Emitter::InitializeInternal(unsigned long numParticles, float maxSpawnRate, float maxLifeTime)
{
	//PE::Handle hBuffer("ParticleBuffer", sizeof(ParticleBuffer));
	mParticleBuffer = new ParticleBuffer(numParticles);

	// temp!
	SetRotation(Quaternion(Vector3(1,0,0), -90.0f));

	Particle p;
	p.mPosition		= mBase.GetTranslation();


	mParticleEmitFloor		= 500;

	mEmissionFloor	= 0.001f;
	mEmissionCeiling = 1.0f / maxSpawnRate;
	mEmissionTime	= mEmissionCeiling;

	mLambdaFloor	= 1.0f;//max(maxLifeTime - 1.5f, 0.2f);
	mLambdaCeiling	= maxLifeTime;
			
	mParticleSizeFloor		= 10;
	mParticleSizeCeiling	= 100; 
			
	mForceMin	= 100.0f;
	mForceMax	= 30.0f;
	mGravity	= -0.6f;

	mThetaClamp			= PI / 3;
	mParticlesToSpawn	= 0;

	mParticlesSpanwedThisFrame = 0;
	
	p.mLifeSpan = -5.0f;// mLambdaFloor;

	// Generate Random sine waves
	mRandomSineTriples.mAngleWave		= RandomGen::CreateSineTriple();
	mRandomSineTriples.mLifespanWave	= RandomGen::CreateSineTriple();
	mRandomSineTriples.mSizeWave		= RandomGen::CreateSineTriple();
	mRandomSineTriples.mZDistWave		= RandomGen::CreateSineTriple();

	mParticleBuffer->mParticles.resize(numParticles);
	for (unsigned long i = 0; i < numParticles; ++i)
	{			
		//float size = mParticleSizeFloor;// randInRange<float>(mParticleSizeFloor, mParticleSizeCeiling);
		//p.mSize = Vector2(size, size);
		p.mVelocity = GetRandomizeDirection();// *mForceMin;// randInRange<float>(mForceMin, mForceMax);
		mParticleBuffer->mParticles[i] = p;
	}


	mParticleBuffer->InitMaterials("flames_particle2.dds", "Default", "StdParticle_Tech", "Particle_Update_Tech", "Particle_Spawn_Tech");
	/*if(mParticleType == FLAMES_YELLOW)
		mParticleBuffer->InitMaterials("flames_particle2.dds", "Default", "StdParticle_Tech", "Particle_Compute_Update_Tech", "Particle_Compute_Spawn_Tech");
	else if (mParticleType == FLAMES_RED)
		mParticleBuffer->InitMaterials("Imrod_Glow.dds", "Default", "StdParticle_Tech", "Particle_Compute_Update_Tech", "Particle_Compute_Spawn_Tech");
	else if (mParticleType == SMOKE)
		mParticleBuffer->InitMaterials("smokeTexture.dds", "Default", "StdParticle_Grayscale_A_Tech", "Particle_Compute_Update_Tech", "Particle_Compute_Spawn_Tech");
	*/
	mParticleBuffer->CreateBuffer(); // TODO: allow different amounts of particles
}

//void Emitter::Initialize(Events::Event *pEvt)
//{
//	Events::Event_CREATE_EMITTER *pRealEvent = (Events::Event_CREATE_EMITTER *)pEvt;
//
//	mEmissionCeiling = pRealEvent->mEmissionCeiling;
//	mEmissionFloor = pRealEvent->mEmissionFloor;
//	mLambdaCeiling = pRealEvent->mLambdaCeiling;
//	mLambdaFloor = pRealEvent->mLambdaFloor;
//	mThetaClamp = pRealEvent->mThetaClamp * PI / 180.f;
//	m_base = pRealEvent->m_base;
//	mForce = pRealEvent->mForce;
//	mDecayTime = pRealEvent->mDecayTime;
//	mGravity = pRealEvent->mGravity;
//	mParticleEmitFloor = pRealEvent->mParticleEmitFloor;
//	mParticleEmitCeiling = pRealEvent->mParticleEmitCeiling;
//	mParticleSizeFloor = pRealEvent->mParticleSizeFloor;
//	mParticleSizeCeiling = pRealEvent->mParticleSizeCeiling;
//	mParticlesToSpawn = 0;
//	mParticlesSpanwedThisFrame = 0;
//
//	PE::Handle hBuffer("ParticleBuffer", sizeof(ParticleBuffer));
//	mParticleBuffer = new ParticleBuffer(*mPContext, m_arena, hBuffer, 
//		mLambdaCeiling, mEmissionFloor, mParticleEmitCeiling);
//			
//	//addComponent(hBuffer);
//	//mPContext->getGPUScreen()->AcquireRenderContextOwnership(mPContext->mGameThreadThreadOwnershipMask);
//			
//	Particle p;
//	p.mPosition = m_base.getPos();
//			
//	for (int i = 0; i < mParticleBuffer->mParticles.m_capacity; i++) {
//		p.m_velocity = randomizeDirection();
//		float size = randInRange<float>(mParticleSizeFloor, mParticleSizeCeiling);
//		p.m_size = Vector2(size, size);
//			
//		p.mLifeSpan = 0.f;
//		mParticleBuffer->mParticles.add(p);
//		mDecayTimers.push_back(mDecayTime);
//	}
//
//	mParticleBuffer->CreateBuffer(); // TODO: allow different amounts of particles
//	mParticleType = (ParticleSystemTypes)pRealEvent->mPSystemType;
//	if(mParticleType == FLAMES_YELLOW)
//		mParticleBuffer->InitMaterials("flames_particle2.dds", "Default", "StdParticle_Tech", "Particle_Compute_Update_Tech", "Particle_Compute_Spawn_Tech");
//	else if (mParticleType == FLAMES_RED)
//		mParticleBuffer->InitMaterials("Imrod_Glow.dds", "Default", "StdParticle_Tech", "Particle_Compute_Update_Tech", "Particle_Compute_Spawn_Tech");
//	else if (mParticleType == SMOKE)
//		mParticleBuffer->InitMaterials("smokeTexture.dds", "Default", "StdParticle_Grayscale_A_Tech", "Particle_Compute_Update_Tech", "Particle_Compute_Spawn_Tech");
//}
#endif

void Emitter::Draw(Renderer& render)
{
	// Bind texture and sampler state to use
	ID3D11SamplerState* ss = SamplerStateManager::Instance()->GetSamplerState(SamplerState_NoMips_MinTexelLerp_NoMagTexelLerp_Clamp).mSamplerStateObject;
	render.GetGraphicsDriver().BindPSTexture(mParticleBuffer->mMaterial->mShaderResourceView, 2);
	render.GetGraphicsDriver().BindPSSamplerState(ss, 2);

	// Bind buffer that contains the updated particles
	//render.GetGraphicsDriver().BindResourceBuffer(mParticleBuffer->mVertexUAVBufferGPU, GpuResourceSlot_ParticleDataCS_SRV_ConstResource);
	if (mConsumeAppendIter == 0)
	{
		render.GetGraphicsDriver().BindResourceBuffer(mParticleBuffer->mParticlesToSpawnBufferGPU, GpuResourceSlot_ParticleDataCS_SRV_ConstResource);
		mConsumeAppendIter = 1;
	}
	else
	{
		render.GetGraphicsDriver().BindResourceBuffer(mParticleBuffer->mVertexUAVBufferGPU, GpuResourceSlot_ParticleDataCS_SRV_ConstResource);
		mConsumeAppendIter = 0;
	}

	// Make sure no vertex buffer is bound and activate the rendering effect
	render.GetGraphicsDriver().BindVertexBuffer(NULL, mParticleBuffer->mVertexBuffer->GetVertexCount());
	mParticleBuffer->mVertexBuffer->SetActive();
	render.GetGraphicsDriver().GetDeviceContext()->DrawInstancedIndirect(mParticleBuffer->mIndirectArgsBuffer, 0);
	
	// Unbind resource
	render.GetGraphicsDriver().UnbindResourceFromSlot(GpuResourceSlot_ParticleDataCS_SRV_ConstResource);
}

// Update the particles in the buffer
#if 0 //_DEBUG_
void Emitter::Update(PrimitiveTypes::Float32 mFrameTime)
{
#if 1
	Particle *p;
	PrimitiveTypes::Int16 particlesLeft = 0;

	// DEBUGGING
	PrimitiveTypes::Int16 emissionsRate = 2;
	// END DEBUGGING CODE

	mEmissionTime -= mFrameTime;

	if (mEmissionTime < 0.f) {
		// Emitting particles
		particlesLeft = emissionsRate;
		mEmissionTime = mEmissionFloor; // TODO: Variable Time-based emission
	}

	//return;

	// Update Particles
	for (int i = 0; i < mParticleBuffer->mParticles.m_size; i++) {
		p = &mParticleBuffer->mParticles[i];

		switch (p->mLifeSpan > 0) {
		case true:
		{ // particle is alive, Update or kill if needed
			p->mLifeSpan -= mFrameTime;

			// TODO: Add decay to the forces
			p->mPosition += (p->m_velocity * mForce); // Move along direction vector at force

			// Add variability
			// TODO
		}
		break;

		case false:
		{ // Particle is dead either skip or reinit
			if (particlesLeft > 0) {
				// TODO: Time-based emission variability
				// Randomize the direction of this particle within the clamp
				//if (mThetaClamp != 0.0f) 
				{
					p->m_velocity = randomizeDirection();
				}

				p->mLifeSpan = 3.0f;
				p->mPosition = m_base.getPos();
				particlesLeft--;
			}
		}
		break;
		}

		// Copy Updated particle data to the GPU vertex buffer
		mParticleBuffer->MapParticlesToGPUBuffer();
	}
#endif
}
#else
void Emitter::Update(float frameTime)
{
	GraphicsDriver* gDriver = GraphicsDriver::Instance();
	Renderer* gRenderer = reinterpret_cast<Renderer*>(IRenderer::Instance());

	PerParticleEmitterConstants consts = CreateEmitterConstants();
	gDriver->UploadBuffer(gRenderer->GetPerEmitterBuffer(), &consts, sizeof(PerParticleEmitterConstants));
	gDriver->SetConstantBuffer(gRenderer->GetPerEmitterBuffer(), PerParticleEmitterSlot);

	//*****************************************
	// Init for spawning particles
	//*****************************************

	UINT32 numToSpawn = 1;

	mEmissionTime -= frameTime;
	if (frameTime > mEmissionCeiling)
		numToSpawn = S_CAST(UINT32, floor(frameTime / mEmissionCeiling));

	if (false && mEmissionTime <= 0.0f) 
	{
		//BEPrint("Spawn Particle\n");

		mParticleBuffer->GetSpawnEffect()->SetActive();


		if (mConsumeAppendIter == 0)
		{
			gDriver->BindResourceBuffer(mParticleBuffer->mVertexUAVBufferGPU, GpuResourceSlot_ParticleDataCSUAV, false, firstFrame);
		}
		else
		{
			gDriver->BindResourceBuffer(mParticleBuffer->mParticlesToSpawnBufferGPU, GpuResourceSlot_ParticleDataCSUAV, false, firstFrame);
		}

		gDriver->GetDeviceContext()->Dispatch(numToSpawn, 1, 1);

		mEmissionTime = mEmissionCeiling + mEmissionTime;// randInRange<float>(mEmissionFloor, mEmissionCeiling);
		
		firstFrame = false;
	}


	//*****************************************
	// Trigger update compute shaders
	//*****************************************
	
	mParticleBuffer->GetUpdateEffect()->SetActive();

	//gDriver->BindResourceBuffer(mParticleBuffer->mVertexUAVBufferGPU, GpuResourceSlot_ParticleDataCSUAV);
	if (mConsumeAppendIter == 0)
	{
		gDriver->BindResourceBuffer(mParticleBuffer->mVertexUAVBufferGPU, GpuResourceSlot_ParticleDataCSUAV, false, false);
		gDriver->BindResourceBuffer(mParticleBuffer->mParticlesToSpawnBufferGPU, GpuResourceSlot_ParticleSpawns_ConsumeBuffer, false, true);
	}
	else
	{
		gDriver->BindResourceBuffer(mParticleBuffer->mParticlesToSpawnBufferGPU, GpuResourceSlot_ParticleDataCSUAV, false, false);
		gDriver->BindResourceBuffer(mParticleBuffer->mVertexUAVBufferGPU, GpuResourceSlot_ParticleSpawns_ConsumeBuffer, false, true);
	}

	DbgAssert(mParticleBuffer->mParticles.size() % 32 == 0, "");

	gDriver->GetDeviceContext()->Dispatch(static_cast<int>(mParticleBuffer->mParticles.size() / 32), 1, 1);


	// Unbind resource
	gDriver->UnbindResourceFromSlot(GpuResourceSlot_ParticleDataCSUAV);
	gDriver->UnbindResourceFromSlot(GpuResourceSlot_ParticleSpawns_ConsumeBuffer);

	return;

	// Update Particles
	//for (int i = 0; i < mParticleBuffer->mParticles.size(); i++) {
	//	p = &mParticleBuffer->mParticles[i];
	//
	//	switch (p->mLifeSpan > 0) {
	//	case true:
	//	{ // particle is alive, Update or kill if needed
	//		p->mLifeSpan -= frameTime;
	//		float force = mForce;
	//		float percentDecay = 0.0f;
	//		Vector3 gravity = Vector3(0.f, 0.f, 0.f);
	//		//float allowedDisp;
	//				
	//		if (mDecayTime != 0.f) {
	//			// Subtract from the decay timer
	//			mDecayTimers[i] -= frameTime;
	//			// Apply a decay as a percentage
	//			percentDecay = mDecayTimers[i] / mDecayTime;
	//			if (percentDecay >= 0.f) // Still have force
	//				force = mForce * percentDecay;
	//			else// Zero out the force completely
	//				force = 0.f;
	//		}
	//		//allowedDisp = force * mFrameTime;
	//		gravity.m_y = mGravity * frameTime;
	//		p->mPosition += (p->m_velocity * (force * frameTime) - gravity);
	//	}
	//	break;
	//
	//	case false:
	//	{ // Particle is dead either turn it off or reinit
	//		if (mParticlesToSpawn > 0) {
	//			// Randomize the direction of this particle within the clamp
	//			if (mThetaClamp != 0.0f) {
	//				p->m_velocity = randomizeDirection();
	//			}
	//			if (mDecayTime != 0.0f) {
	//				mDecayTimers[i] = mDecayTime;
	//			}
	//
	//			p->mLifeSpan = randInRange<float>(mLambdaFloor, mLambdaCeiling);
	//			p->mPosition = m_base.getPos();
	//			mParticlesToSpawn--;
	//		}
	//	}
	//	break;
	//	}
	//
	//	// Copy Updated particle data to the GPU vertex buffer
	//	mParticleBuffer->MapParticlesToGPUBuffer();
	//}
}
#endif

PerParticleEmitterConstants Emitter::CreateEmitterConstants()
{
	PerParticleEmitterConstants consts;
	consts.mWorldTransform = mWorldTransform;
	consts.mLifeSpans = Vector2(mLambdaFloor, mLambdaCeiling);
	consts.mSizes = Vector2(mParticleSizeFloor, mParticleSizeCeiling);
	consts.mForces = Vector2(mForceMin, mForceMax);
	consts.mGravitMagnitude = mGravity;
	consts.mThetaClamp = mThetaClamp;
	consts.mRandomSeed = mRandomSeed;
	consts.mRandomWaves = mRandomSineTriples;

	return consts;
}

/***************************************************************
* Vector3 randomizeDirection()
*
* Description:
*	This calculates the randomized direction
*
* Returns: Unit vector indicating the randomized direction
***************************************************************/
Vector3 Emitter::GetRandomizeDirection()
{
	// Requires the use of cosine and sine
	Vector3 randomizedVec, other;
	float phi, minTheta, zDistribution, theta;

	// Generate random value between -PI and PI
	phi = RandomGen::randInRange(-PI, PI);
	minTheta = cosf(mThetaClamp);
	zDistribution = RandomGen::randInRange(minTheta, 1.f);
	theta = acosf(zDistribution);
	//randomizedVec = sinf(theta) * (cosf(phi) * mBase.getU() + sinf(phi) * m_base.getV()) + cosf(theta) * m_base.getN();
	randomizedVec = sinf(theta) * (cosf(phi) * mWorldTransform.GetRight() + sinf(phi) * mWorldTransform.GetUp()) + cosf(theta) * mWorldTransform.GetForward();
	randomizedVec.Normalize();

	return randomizedVec;
}


