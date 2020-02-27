#ifndef StdParticle_PS
#define StdParticle_PS

#include "Constants.hlsl"
#include "lighthelper.fx"
#include "Std_Structs.hlsl"

float4 main(Std_Particle_PS_IN pIn) : SV_TARGET
{
	//*** for additive blending ***
	float4 combinedColor = gDiffuseMap.Sample(gDiffuseMapSampler, pIn.mTexCoord);
	if (combinedColor.x + combinedColor.y + combinedColor.z < 0.1f)
		combinedColor.w = 0;
	//combinedColor.w = combinedColor.w / 1.8f;
	else
		combinedColor.w = pIn.mLifespan / 1.5f;
	return saturate(combinedColor);

}


#endif
