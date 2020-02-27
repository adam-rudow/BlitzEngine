#pragma once

// Outer-Engine includes

// Inter-Engine includes
#include "Renderer.h"


struct SamplerState
{
	ID3D11SamplerState* mSamplerStateObject;
	bool NeedsMipMaps();
};

enum ESamplerState
{
	SamplerState_INVALID = -1,
	SamplerState_NotNeeded, // used for resources that dont use sampler
		
	SamplerState_NoMips_MinTexelLerp_NoMagTexelLerp_Clamp,
	SamplerState_NoMips_NoMinTexelLerp_NoMagTexelLerp_Clamp,
	SamplerState_NoMips_NoMinTexelLerp_NoMagTexelLerp_Wrap,
	SamplerState_NoMips_NoMinTexelLerp_MagTexelLerp_Clamp,

	SamplerState_MipLerp_MinTexelLerp_MagTexelLerp_Wrap,
	SamplerState_MipLerp_MinTexelLerp_MagTexelLerp_Clamp,

	SamplerState_NoMips_MinLerp_MaxLerp_Clamp,

	SamplerStateCustom0,
	// space to add more that are nto potentially static or known at the beginning
	SamplerStateCustom100 = SamplerStateCustom0 + 100,
	SamplerState_Count
};



class SamplerStateManager
{
	DeclSingleton(SamplerStateManager)

public:
	SamplerStateManager();
	~SamplerStateManager();

	void Initialize();

	SamplerState& GetSamplerState(ESamplerState state);

private:
	SamplerState mSamplerStates[SamplerState_Count];
};

