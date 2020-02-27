
#include "DepthStencilStateManager.h"
#include "Effect.h"

ImplSingleton(DepthStencilStateManager)
    
DepthStencilState::DepthStencilState()
{
	memset(this, 0, sizeof(*this));
    mDepthTestEnabled = false;
	mDepthStencilStateObject = NULL;
} 

DepthStencilStateManager::~DepthStencilStateManager()
{
	for (int i = 0; i < DepthStencilState_Count; ++i)
	{
		mDepthStencilStates[i].mDepthStencilStateObject->Release();
	}
}
    
//void DepthStencilState::setAPIValues()
//{        
//    D3D11Renderer *pD3D11Renderer = static_cast<D3D11Renderer *>(m_pContext->getGPUScreen());
//    ID3D11Device *pDevice = pD3D11Renderer->m_pD3DDevice;
//    ID3D11DeviceContext *pDeviceContext = pD3D11Renderer->m_pD3DContext;
//        
//    D3D11_DEPTH_STENCIL_DESC dsDesc;
//        
//    // Depth test parameters
//    dsDesc.DepthEnable = m_depthTestEnabled;
//    dsDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
//    dsDesc.DepthFunc = D3D11_COMPARISON_LESS_EQUAL;
//        
//    // Stencil test parameters
//    dsDesc.StencilEnable = false;
//    //dsDesc.StencilReadMask = 0xFF;
//    //dsDesc.StencilWriteMask = 0xFF;
//        
//    // Stencil operations if pixel is front-facing
//    //dsDesc.FrontFace.StencilFailOp = D3D10_STENCIL_OP_KEEP;
//    //dsDesc.FrontFace.StencilDepthFailOp = D3D10_STENCIL_OP_INCR;
//    //dsDesc.FrontFace.StencilPassOp = D3D10_STENCIL_OP_KEEP;
//    //dsDesc.FrontFace.StencilFunc = D3D10_COMPARISON_ALWAYS;
//        
//    // Stencil operations if pixel is back-facing
//    //dsDesc.BackFace.StencilFailOp = D3D10_STENCIL_OP_KEEP;
//    //dsDesc.BackFace.StencilDepthFailOp = D3D10_STENCIL_OP_DECR;
//    //dsDesc.BackFace.StencilPassOp = D3D10_STENCIL_OP_KEEP;
//    //dsDesc.BackFace.StencilFunc = D3D10_COMPARISON_ALWAYS;
//        
//    // Create depth stencil state
//    pDevice->CreateDepthStencilState(&dsDesc, &m_pD3D11DepthStencilStateObject);
//
//}
    
void DepthStencilState::SetActive()
{
	GraphicsDriver::Instance()->SetDepthStencilState(mDepthStencilStateObject);// ->GetDeviceContext()->OMSetDepthStencilState(mDepthStencilStateObject, 0);
}
    
DepthStencilStateManager::DepthStencilStateManager()
{
	Initialize();
}
    
void DepthStencilStateManager::Initialize()
{
        
    mDepthStencilStates = new DepthStencilState[DepthStencilState_Count];
        
    {
        DepthStencilState dss;
        dss.mDepthTestEnabled = true;
		dss.mDepthStencilStateObject = GraphicsDriver::Instance()->CreateDepthStencilState(dss.mDepthTestEnabled);
		mDepthStencilStates[DepthStencilState_ZBuffer] = dss;
    }
        
    {
		DepthStencilState dss;
        dss.mDepthTestEnabled = false;
		dss.mDepthStencilStateObject = GraphicsDriver::Instance()->CreateDepthStencilState(dss.mDepthTestEnabled);
		mDepthStencilStates[DepthStencilState_NoZBuffer] = dss;
    }
        
        
}
    
DepthStencilState *DepthStencilStateManager::GetDepthStencilState(DSS_DepthStencilState state)
{
    return &mDepthStencilStates[state];
}
    
    
    
