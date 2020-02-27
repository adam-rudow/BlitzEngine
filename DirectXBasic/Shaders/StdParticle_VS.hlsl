#ifndef StdParticle_VS
#define StdParticle_VS

#include "Constants.hlsl"
#include "Std_Structs.hlsl"

StructuredBuffer<Particle> updatedParticles : register(t70);

Std_Particle_VS_IN main(uint id : SV_VertexID)
{
	Std_Particle_VS_IN vOut;
	Particle p = updatedParticles[id];

	vOut.mPos = p.mPos;
	vOut.mSize = p.mSize;

	vOut.mLifespan = p.mLifespan;

	return vOut;
}


#endif