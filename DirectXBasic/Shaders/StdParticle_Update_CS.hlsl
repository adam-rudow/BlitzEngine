#ifndef PARTICLEUPDATE_CS
#define PARTICLEUPDATE_CS

#include "Constants.hlsl"
#include "Random.hlsl"

#define PARTICLE_DEATH_TOLERANCE -0.06f

//RWStructuredBuffer<Particle> srcParticleBuffer : register(u0);

ConsumeStructuredBuffer<Particle> currentParticles : register(u0);
AppendStructuredBuffer<Particle> outParticles : register(u1);

//*************************************************************
//
// TODO: Find a better way to do respawning particles without the thread contention of "RWByteAddressBuffer particlesToSpawn"
//
//*************************************************************

float generateRandomNumberInRange(float min, float max, float frameTime, int id)
{
	float2 seed = float2(frameTime * 1234, id);
	float2 r = float2(23.1406926327792690,  // e^pi (Gelfond's constant)
		2.6651441426902251); // 2^sqrt(2) (Gelfond–Schneider constant)
	float dif = max - min;

	return min + dif * abs(frac(cos(123456789. % (1e-7 + 256. * dot(seed, r)))));
}



//each thread processes particle from an Emitter

[numthreads(32, 1, 1)]
void main(uint3 DTid : SV_DispatchThreadID)
{
	int idx = DTid.x;// +DTid.y + DTid.z;
	if (idx < 0)
		return;

	//Particle p = srcParticleBuffer[idx];
	Particle p = currentParticles.Consume();

	float prevLifeSpan = p.mLifespan;
	p.mLifespan = p.mLifespan - gFrameTime;

	if (prevLifeSpan > PARTICLE_DEATH_TOLERANCE)
	{
		p.mPos = p.mPos + (p.mVelocity * gParticleForces.x) * gFrameTime;
		//p.mPos = p.mPos + float3(0, 10, 0) * gFrameTime;

		//float3 gravity = float3(0.0f, gGravitMagnitude, 0.0f);
		//p.mVelocity = p.mVelocity - gravity * gFrameTime;

		//srcParticleBuffer[idx] = p;
		outParticles.Append(p);
	}
	else
	{
		p.mLifespan = randomInRange(gParticleLifeSpans.x, gParticleLifeSpans.y, gParticlesRandomWaves.mLifespanWave, idx);
		p.mSize		= randomInRange(gParticleSizes.x, gParticleSizes.y, gParticlesRandomWaves.mSizeWave, idx);
		p.mPos		= float3(gEmitterWorldTransform[3][0], gEmitterWorldTransform[3][1], gEmitterWorldTransform[3][2]);
		//p.mVelocity = GetRandomDirection(idx, gParticlesRandomWaves.mAngleWave, gParticlesRandomWaves.mZDistWave);
		//srcParticleBuffer[idx] = p;
		outParticles.Append(p);
	}

	//else
	//{
	//	int numToSpawn;
	//	//particlesToSpawn.InterlockedAdd(0, -1, numToSpawn);
	//	//if(numToSpawn > 0)
	//	{
	//		p.m_lifespan = 4.0f;//generateRandomNumberInRange(gLambdaMinMax.x, gLambdaMinMax.y, frameTime, idx);
	//
	//		//p.m_pos = float3(gWt[3][0], gWt[3][1], gWt[3][2]);
	//		//p.m_velocity = getRandomDirection(frameTime, idx);
	//
	//		//float size = generateRandomNumberInRange(gParticleSizeMinMax.x, gParticleSizeMinMax.y, frameTime, idx);
	//		//p.m_size = float2(size, size);
	//
	//		//srcParticleBuffer[idx] = p;
	//		outParticles.Append(p);
	//	}
	//}
}

#endif
