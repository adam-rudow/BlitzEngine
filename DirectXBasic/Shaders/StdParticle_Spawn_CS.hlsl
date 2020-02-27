#ifndef PARTICLESPAWN_CS
#define PARTICLESPAWN_CS

//#include "Constants.hlsl"
#include "Random.hlsl"


AppendStructuredBuffer<Particle> srcParticleBuffer : register(u0);


//float generateRandomNumberInRange(float min, float max, float frameTime, int id)
//{
//	float2 seed = float2(frameTime * 1234, id);
//	float2 r = float2(23.1406926327792690,  // e^pi (Gelfond's constant)
//		2.6651441426902251); // 2^sqrt(2) (Gelfond–Schneider constant)
//	float dif = max - min;
//
//	return min + dif * abs(frac(cos(123456789. % (1e-7 + 256. * dot(seed, r)))));
//}
//
//float3 getRandomDirection2(float frameTime, int id)
//{
//	NumberGenerator ng;
//	ng.SetSeed(gRandomSeed);
//	ng.Cycle(id);
//	float phi = ng.GetRandomFloat(0, 2 * 3.141592);
//	float minTheta = cos(gThetaClamp);
//	float zDistribution = generateRandomNumberInRange(minTheta, 1.0f, frameTime, id);
//	float theta = acos(zDistribution);
//
//	float3 randomizedVec = sin(theta)
//		* (cos(phi) * float3(gEmitterWorldTransform[0][0], gEmitterWorldTransform[0][1], gEmitterWorldTransform[0][2]) 
//		+ sin(phi) * float3(gEmitterWorldTransform[1][0], gEmitterWorldTransform[1][1], gEmitterWorldTransform[1][2]))
//		+ cos(theta) * float3(gEmitterWorldTransform[2][0], gEmitterWorldTransform[2][1], gEmitterWorldTransform[2][2]);
//
//	return normalize(randomizedVec);
//
//}


[numthreads(1, 1, 1)]
void main(uint3 DTid : SV_DispatchThreadID)
{
	int idx = DTid.x;

	//if(idx < gToSpawnCount)
	{

		/*NumberGenerator ng;
		ng.SetSeed(gRandomSeed);

		Particle p;

		ng.Cycle(idx);
		p.mLifespan = ng.GetRandomFloat(gParticleLifeSpans.x, gParticleLifeSpans.y);
		p.mPos = float3(gEmitterWorldTransform[3][0], gEmitterWorldTransform[3][1], gEmitterWorldTransform[3][2]);

		ng.Cycle(idx);
		float size = ng.GetRandomFloat(gParticleSizes.x, gParticleSizes.y);
		p.mSize = float2(size, size);

		p.mVelocity = getRandomDirection2(gFrameTime, idx * 2);
*/
		Particle p;

		PerlinSineTriple wave = gParticlesRandomWaves.mAngleWave;

		p.mPos = float3(gEmitterWorldTransform[3][0], gEmitterWorldTransform[3][1], gEmitterWorldTransform[3][2]);
		p.mVelocity = GetRandomDirection(idx, wave, gParticlesRandomWaves.mZDistWave);
		p.mLifespan = randomInRange(gParticleLifeSpans.x, gParticleLifeSpans.y, gParticlesRandomWaves.mLifespanWave, idx);
		float size = randomInRange(gParticleSizes.x, gParticleSizes.y, gParticlesRandomWaves.mSizeWave, idx);
		p.mSize = float2(size, size);
		
		srcParticleBuffer.Append(p);
	}
}

#endif
