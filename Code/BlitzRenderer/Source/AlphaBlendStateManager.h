#pragma once

// Outer-Engine includes

// Inter-Engine includes

#include "Renderer.h"


struct Effect;

struct AlphaBlendState
{

	AlphaBlendState();

	void SetActive();

	void TranslateToAPIValues();
	
	enum BO_BlendOp
	{
		BlendOp_Add, // default, example: source * source_alpha + dest * dest_alpha
		BlendOp_Rev_Sub,
	};

	enum BF_BlendFactor
	{
		BlendFactor_0,
		BlendFactor_1,
		BlendFactor_SrcA,
		BlendFactor_InvSrcA,
		BlendFactor_DestA,
		BlendFactor_InvDestA,
	};

	BO_BlendOp mAlphaBlendOp;
	BF_BlendFactor mSrcAlphaBlendFactor;
	BF_BlendFactor mDstAlphaBlendFactor;

	BO_BlendOp mRGB_BlendOp;
	BF_BlendFactor mSrcRGBBlendFactor;
	BF_BlendFactor mDstRGBBlendFactor;

	bool mBlendEnabled : 1;
	bool mUseSeparateSettingForAlphaChannel : 1;

	static D3D11_BLEND_OP BlendOpToAPIBlendOp(BO_BlendOp peVal);
	static D3D11_BLEND BlendFactorToAPIBlendFactor(BF_BlendFactor peVal);

	D3D11_BLEND_OP m_apiAlphaBlendOp;
	D3D11_BLEND m_apiSrcAlphaBlendFactor;
	D3D11_BLEND m_apiDstAlphaBlendFactor;

	D3D11_BLEND_OP m_apiRGBBlendOp;
	D3D11_BLEND m_apiSrcRGBBlendFactor;
	D3D11_BLEND m_apiDstRGBBlendFactor;

	ID3D11BlendState * mBlendStateObject;

};

enum ABS_AlphaBlendState
{
	AlphaBlendState_NoBlend,
	AlphaBlendState_DefaultRGBLerp_A_DestUnchanged,
	AlphaBlendState_DefaultRGBSub_A_DestUnchanged,
	AlphaBlendState_Additive,
	AlphaBlendState_Count
};
    
class AlphaBlendStateManager
{
	DeclSingleton(AlphaBlendStateManager)

public:
	AlphaBlendStateManager();
	~AlphaBlendStateManager();

	void Initialize();
    
	AlphaBlendState *GetAlphaBlendState(ABS_AlphaBlendState state);
    
	AlphaBlendState *m_alphaBlendStates;
        
};
    