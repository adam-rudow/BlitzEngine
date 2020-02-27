#include "Utils/AssertDbg.h"
#include "SamplerState.h"

ImplSingleton(SamplerStateManager)

	SamplerStateManager::SamplerStateManager()
	{
		Initialize();
	}

SamplerStateManager::~SamplerStateManager()
{
	for (int i = 1; i < SamplerStateCustom0; ++i)
	{
		mSamplerStates[i].mSamplerStateObject->Release();
	}
}

	SamplerState& SamplerStateManager::GetSamplerState(ESamplerState state)
	{
		DbgAssert(state < SamplerState_Count && state >= 0, "");

		return mSamplerStates[state];
	}

	void SamplerStateManager::Initialize()
	{
		SamplerState ss;
		memset(&ss, 0, sizeof(ss));

		D3D11_SAMPLER_DESC sdesc;
		memset(&sdesc, 0, sizeof(sdesc));

		sdesc.Filter = D3D11_FILTER_ANISOTROPIC;//D3D11_FILTER_MIN_MAG_MIP_LINEAR;
		sdesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
		sdesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
		sdesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;

		sdesc.MinLOD = 0;
		sdesc.MaxLOD = D3D11_FLOAT32_MAX;
		sdesc.MipLODBias = 0;
		sdesc.MaxAnisotropy = 16;
		//sdesc.ComparisonFunc = NULL;
		memset(sdesc.BorderColor,0, sizeof(sdesc.BorderColor));
		HRESULT hr = GraphicsDriver::Instance()->GetDevice()->CreateSamplerState( &sdesc, &ss.mSamplerStateObject);
		DbgAssert(SUCCEEDED(hr), "");
				
		mSamplerStates[SamplerState_MipLerp_MinTexelLerp_MagTexelLerp_Wrap] = ss;

		sdesc.AddressU = D3D11_TEXTURE_ADDRESS_CLAMP;
		sdesc.AddressV = D3D11_TEXTURE_ADDRESS_CLAMP;
		sdesc.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;
		hr = GraphicsDriver::Instance()->GetDevice()->CreateSamplerState( &sdesc, &ss.mSamplerStateObject);
		DbgAssert(SUCCEEDED(hr), "");

		mSamplerStates[SamplerState_MipLerp_MinTexelLerp_MagTexelLerp_Clamp] = ss;

		sdesc.Filter = D3D11_FILTER_MIN_LINEAR_MAG_MIP_POINT;
		sdesc.MaxLOD = 0;
		hr = GraphicsDriver::Instance()->GetDevice()->CreateSamplerState( &sdesc, &ss.mSamplerStateObject);
		DbgAssert(SUCCEEDED(hr), "");

		mSamplerStates[SamplerState_NoMips_MinTexelLerp_NoMagTexelLerp_Clamp] = ss;

		sdesc.Filter = D3D11_FILTER_MIN_MAG_MIP_POINT;
		hr = GraphicsDriver::Instance()->GetDevice()->CreateSamplerState( &sdesc, &ss.mSamplerStateObject);
		DbgAssert(SUCCEEDED(hr), "");

		mSamplerStates[SamplerState_NoMips_NoMinTexelLerp_NoMagTexelLerp_Clamp] = ss;

		sdesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
		sdesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
		sdesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
		DbgAssert(SUCCEEDED(hr), "");

		mSamplerStates[SamplerState_NoMips_NoMinTexelLerp_NoMagTexelLerp_Wrap] = ss;

		sdesc.AddressU = D3D11_TEXTURE_ADDRESS_CLAMP;
		sdesc.AddressV = D3D11_TEXTURE_ADDRESS_CLAMP;
		sdesc.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;
		sdesc.Filter = D3D11_FILTER_MIN_POINT_MAG_LINEAR_MIP_POINT;
		hr = GraphicsDriver::Instance()->GetDevice()->CreateSamplerState( &sdesc, &ss.mSamplerStateObject);
		DbgAssert(SUCCEEDED(hr), "");

		mSamplerStates[SamplerState_NoMips_NoMinTexelLerp_MagTexelLerp_Clamp] = ss;

		sdesc.AddressU = D3D11_TEXTURE_ADDRESS_CLAMP;
		sdesc.AddressV = D3D11_TEXTURE_ADDRESS_CLAMP;
		sdesc.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;
		sdesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
		hr = GraphicsDriver::Instance()->GetDevice()->CreateSamplerState(&sdesc, &ss.mSamplerStateObject);
		DbgAssert(SUCCEEDED(hr), "");

		mSamplerStates[SamplerState_NoMips_MinLerp_MaxLerp_Clamp] = ss;
	}

bool SamplerState::NeedsMipMaps()
{
	D3D11_SAMPLER_DESC sdescs;
	mSamplerStateObject->GetDesc(&sdescs);

	return (sdescs.Filter % 2) > 0;
}

