
#include "AlphaBlendStateManager.h"
#include "Effect.h"
#include "Utils/AssertDbg.h"
#include "Logging/Log.h"

ImplSingleton(AlphaBlendStateManager)

D3D11_BLEND_OP AlphaBlendState::BlendOpToAPIBlendOp(BO_BlendOp peVal)
{
	switch(peVal)
	{
		case BlendOp_Add: return D3D11_BLEND_OP_ADD;
		case BlendOp_Rev_Sub: return D3D11_BLEND_OP_REV_SUBTRACT; 
		default: assert(false); // "This BlendOp is not supported yet"
		return D3D11_BLEND_OP_ADD;
	};
}

D3D11_BLEND AlphaBlendState::BlendFactorToAPIBlendFactor(BF_BlendFactor peVal)
{
	switch (peVal)
	{
		case BlendFactor_0: return D3D11_BLEND_ZERO;
		case BlendFactor_1: return D3D11_BLEND_ONE;
		case BlendFactor_SrcA: return D3D11_BLEND_SRC_ALPHA;
		case BlendFactor_InvSrcA: return D3D11_BLEND_INV_SRC_ALPHA;
		case BlendFactor_DestA: return D3D11_BLEND_DEST_ALPHA;
		case BlendFactor_InvDestA: return D3D11_BLEND_INV_DEST_ALPHA;
	};

	return D3D11_BLEND_ZERO;
}

//AlphaBlendState::AlphaBlendState()
//{
//    memset(this, 0, sizeof(*this));
//}

AlphaBlendState::AlphaBlendState()
{
	memset(this, 0, sizeof(*this));

	mBlendEnabled = false;

	mRGB_BlendOp = BlendOp_Add;
	mSrcRGBBlendFactor = BlendFactor_SrcA;
	mDstRGBBlendFactor = BlendFactor_InvSrcA;

	mUseSeparateSettingForAlphaChannel = false;
	mAlphaBlendOp = BlendOp_Add;
	mSrcAlphaBlendFactor = BlendFactor_0;
	mDstAlphaBlendFactor = BlendFactor_1;

	mBlendStateObject = NULL;

}

void AlphaBlendState::TranslateToAPIValues()
{
	if (!mBlendEnabled)
	{
		return;
	}

	m_apiRGBBlendOp = BlendOpToAPIBlendOp(mRGB_BlendOp);
	m_apiSrcRGBBlendFactor = BlendFactorToAPIBlendFactor(mSrcRGBBlendFactor);
	m_apiDstRGBBlendFactor = BlendFactorToAPIBlendFactor(mDstRGBBlendFactor);

	if (mUseSeparateSettingForAlphaChannel)
	{
		m_apiAlphaBlendOp = BlendOpToAPIBlendOp(mAlphaBlendOp);
		m_apiSrcAlphaBlendFactor = BlendFactorToAPIBlendFactor(mSrcAlphaBlendFactor);
		m_apiDstAlphaBlendFactor = BlendFactorToAPIBlendFactor(mDstAlphaBlendFactor);
	}
	else
	{
		// set to be the same value
		m_apiAlphaBlendOp = m_apiRGBBlendOp;
		m_apiSrcAlphaBlendFactor = m_apiSrcRGBBlendFactor;
		m_apiDstAlphaBlendFactor = m_apiDstRGBBlendFactor;
	}


	D3D11_BLEND_DESC bdsc;

	// NoBlend //////////////////////////////////////////////////////////////////
	memset(&bdsc, 0, sizeof(bdsc));
	bdsc.IndependentBlendEnable = false; // use only [0] render target
	bdsc.RenderTarget[0].BlendEnable = mBlendEnabled;
	bdsc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;

	// RGB
	bdsc.RenderTarget[0].SrcBlend = m_apiSrcRGBBlendFactor;
	bdsc.RenderTarget[0].DestBlend = m_apiDstRGBBlendFactor;
	bdsc.RenderTarget[0].BlendOp = m_apiRGBBlendOp;

	// A
	bdsc.RenderTarget[0].SrcBlendAlpha = m_apiSrcAlphaBlendFactor;
	bdsc.RenderTarget[0].DestBlendAlpha = m_apiDstAlphaBlendFactor;
	bdsc.RenderTarget[0].BlendOpAlpha = m_apiAlphaBlendOp;
	//bdsc.AlphaToCoverageEnable = true;

	HRESULT hr = GraphicsDriver::Instance()->GetDevice()->CreateBlendState(&bdsc, &mBlendStateObject);

	BZ_ASSERT(hr == S_OK, "Error creating blend state");

}

void AlphaBlendState::SetActive()
{	

	//pDeviceContext->OMSetBlendState(m_pD3D11BlendStateObject, NULL, 0xFFFFFFFF);
	//if(mBlendStateObject)
	GraphicsDriver::Instance()->GetDeviceContext()->OMSetBlendState(mBlendStateObject, NULL, 0x0f);

}

    
    
AlphaBlendStateManager::AlphaBlendStateManager()
{
	Initialize();
}
AlphaBlendStateManager::~AlphaBlendStateManager()
{
	for (int i = 1; i < AlphaBlendState_Count; ++i)
	{
		m_alphaBlendStates[i].mBlendStateObject->Release();
	}
}
    
void AlphaBlendStateManager::Initialize()
{
    m_alphaBlendStates = new AlphaBlendState[AlphaBlendState_Count];
        
	// No blend
    {
        // NoBlend
        // RtRGB = TexRGB
        // RtA = TexA
        AlphaBlendState blendState;
        blendState.mBlendEnabled = false;
        blendState.TranslateToAPIValues();
        m_alphaBlendStates[AlphaBlendState_NoBlend] = blendState;
    }

        
	// Somewhat additive
	{
		// DefaultRGBLerp_A_DestUnchanged
		// RtRGB = TexRGB * TexA + RtRGB * (1-TexA)
		// RtA = RtA
		AlphaBlendState blendState;
		blendState.mBlendEnabled = true;
		blendState.mAlphaBlendOp = AlphaBlendState::BlendOp_Add;
		blendState.mSrcRGBBlendFactor = AlphaBlendState::BlendFactor_SrcA;
		blendState.mDstRGBBlendFactor = AlphaBlendState::BlendFactor_InvSrcA;

		blendState.mUseSeparateSettingForAlphaChannel = true;
		blendState.mAlphaBlendOp = AlphaBlendState::BlendOp_Add;
		blendState.mSrcAlphaBlendFactor = AlphaBlendState::BlendFactor_SrcA;
		blendState.mDstAlphaBlendFactor = AlphaBlendState::BlendFactor_DestA;
		blendState.TranslateToAPIValues();

		m_alphaBlendStates[AlphaBlendState_DefaultRGBLerp_A_DestUnchanged] = blendState;
	}

		// Somewhat additive
	{
		AlphaBlendState blendState;
		blendState.mBlendEnabled = true;
		blendState.mRGB_BlendOp = AlphaBlendState::BlendOp_Add;
		blendState.mSrcRGBBlendFactor = AlphaBlendState::BlendFactor_SrcA;// BlendFactor_1;
		blendState.mDstRGBBlendFactor = AlphaBlendState::BlendFactor_DestA;// BlendFactor_1;

		//blendState.mUseSeparateSettingForAlphaChannel = true;// false;
		//blendState.mAlphaBlendOp = AlphaBlendState::BlendOp_Add;
		//blendState.mSrcAlphaBlendFactor = AlphaBlendState::BlendFactor_1;
		//blendState.mDstAlphaBlendFactor = AlphaBlendState::BlendFactor_1;
		blendState.TranslateToAPIValues();

		m_alphaBlendStates[AlphaBlendState_Additive] = blendState;
	}

	// Rev Subtract for smoke
    {

        AlphaBlendState blendState;
        blendState.mBlendEnabled = true;
        blendState.mAlphaBlendOp = AlphaBlendState::BlendOp_Rev_Sub;
        blendState.mSrcRGBBlendFactor = AlphaBlendState::BlendFactor_SrcA;
        blendState.mDstRGBBlendFactor = AlphaBlendState::BlendFactor_InvSrcA;
            
        blendState.mUseSeparateSettingForAlphaChannel = true;
        blendState.mAlphaBlendOp = AlphaBlendState::BlendOp_Rev_Sub;
        blendState.mSrcAlphaBlendFactor = AlphaBlendState::BlendFactor_1;
        blendState.mDstAlphaBlendFactor = AlphaBlendState::BlendFactor_0;
        blendState.TranslateToAPIValues();
            
        m_alphaBlendStates[AlphaBlendState_DefaultRGBSub_A_DestUnchanged] = blendState;
    }
        
}
    
AlphaBlendState *AlphaBlendStateManager::GetAlphaBlendState(ABS_AlphaBlendState state)
{
    return &m_alphaBlendStates[state];
}
    
    