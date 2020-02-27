
#include "RasterizerStateManager.h"
#include "Effect.h"

ImplSingleton(RasterizerStateManager)

RasterizerState::RasterizerState()
{
	mCullModeType = RasterizerCullMode_None;
	mRasterizerStateObject = NULL;
}

void RasterizerState::SetActive()
{
	GraphicsDriver::Instance()->GetDeviceContext()->RSSetState(mRasterizerStateObject);
}


RasterizerStateManager::RasterizerStateManager()
{	
	Initialize();
}

RasterizerStateManager::~RasterizerStateManager()
{
	for (int i = 0; i < RasterizerState_Count; ++i)
	{
		mRasterizerStates[i].mRasterizerStateObject->Release();
	}
	/*mRasterizerStates[RasterizerState_SolidTriBackCull].mRasterizerStateObject->Release();
	mRasterizerStates[RasterizerState_SolidTriFrontCull].mRasterizerStateObject->Release();
	mRasterizerStates[RasterizerState_SolidTriNoCull].mRasterizerStateObject->Release();
	mRasterizerStates[RasterizerState_Line].mRasterizerStateObject->Release();*/
}

void RasterizerStateManager::Initialize()
{
	mRasterizerStates = new RasterizerState[RasterizerState_Count];

	RasterizerState rs;
	rs.mCullModeType = RasterizerState::RasterizerCullMode_Back;
	rs.mFillModeType = RasterizerState::RasterizerFillMode_Solid;
	rs.mCullMode = D3D11_CULL_BACK;
	rs.mFillMode = D3D11_FILL_SOLID;
	rs.mMSAAEnabled = false;
	GraphicsDriver::Instance()->CreateRasterizerState(&rs);
	mRasterizerStates[RasterizerState_SolidTriBackCull] = rs;
	
	rs.mCullModeType = RasterizerState::RasterizerCullMode_Front;
	rs.mFillModeType = RasterizerState::RasterizerFillMode_Solid;
	rs.mCullMode = D3D11_CULL_FRONT;
	rs.mFillMode = D3D11_FILL_SOLID;
	rs.mMSAAEnabled = false;
	GraphicsDriver::Instance()->CreateRasterizerState(&rs);
	mRasterizerStates[RasterizerState_SolidTriFrontCull] = rs;

	rs.mCullModeType = RasterizerState::RasterizerCullMode_None;
	rs.mFillModeType = RasterizerState::RasterizerFillMode_Solid;
	rs.mCullMode = D3D11_CULL_NONE;
	rs.mFillMode = D3D11_FILL_SOLID;
	rs.mMSAAEnabled = false;
	GraphicsDriver::Instance()->CreateRasterizerState(&rs);
	mRasterizerStates[RasterizerState_SolidTriNoCull] = rs;

	rs.mCullModeType = RasterizerState::RasterizerCullMode_None;
	rs.mFillModeType = RasterizerState::RasterizerFillMode_Solid;
	rs.mCullMode = D3D11_CULL_NONE;
	rs.mFillMode = D3D11_FILL_SOLID;
	rs.mMSAAEnabled = true;
	GraphicsDriver::Instance()->CreateRasterizerState(&rs);
	mRasterizerStates[RasterizerState_SolidTriNoCull_4xMSAA] = rs;

	rs.mCullModeType = RasterizerState::RasterizerCullMode_None;
	rs.mFillModeType = RasterizerState::RasterizerFillMode_SolidLine;
	rs.mCullMode = D3D11_CULL_NONE;
	rs.mFillMode = D3D11_FILL_WIREFRAME;
	rs.mMSAAEnabled = false;
	GraphicsDriver::Instance()->CreateRasterizerState(&rs);
	mRasterizerStates[RasterizerState_Line] = rs;
}

RasterizerState* RasterizerStateManager::GetRasterizerState(RS_RasterizerState state)
{
	return &mRasterizerStates[state];
}

