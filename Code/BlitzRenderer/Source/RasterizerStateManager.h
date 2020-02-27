#pragma once

// Outer-Engine includes

// Inter-Engine includes

#include "Renderer.h"

struct Effect;

enum RS_RasterizerState
{
	RasterizerState_SolidTriBackCull,
	RasterizerState_SolidTriNoCull,
	RasterizerState_SolidTriNoCull_4xMSAA,
	RasterizerState_Line,
	RasterizerState_SolidTriFrontCull,

	RasterizerState_Count
};

struct RasterizerState
{
	RasterizerState();

	void SetActive();

	enum RCM_RasterizerCullMode
	{
		RasterizerCullMode_None   = 0,
		RasterizerCullMode_Front  = 1,
		RasterizerCullMode_Back   = 2
	};

	enum RFM_RasterizerFillMode
	{
		RasterizerFillMode_Solid   = 0,
		RasterizerFillMode_SolidLine
	};

	RCM_RasterizerCullMode mCullModeType;
	RFM_RasterizerFillMode mFillModeType;

	D3D11_CULL_MODE mCullMode;
	D3D11_FILL_MODE mFillMode;

	BOOL mMSAAEnabled;

	ID3D11RasterizerState * mRasterizerStateObject;
};

struct RasterizerState;
class RasterizerStateManager
{
	DeclSingleton(RasterizerStateManager)

public:

	RasterizerStateManager();
	~RasterizerStateManager();

	void Initialize();

	RasterizerState *GetRasterizerState(RS_RasterizerState state);

private:
	RasterizerState *mRasterizerStates;
};


