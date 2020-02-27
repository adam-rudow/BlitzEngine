#pragma once

// Outer-Engine includes

// Inter-Engine includes

#include "Renderer.h"

// Sibling/Children includes

struct Effect;
    
enum DSS_DepthStencilState
{
    DepthStencilState_ZBuffer,
    DepthStencilState_NoZBuffer,
        
    DepthStencilState_Count
};
    
struct DepthStencilState
{        
	DepthStencilState();
                
    void SetActive();
                
    bool mDepthTestEnabled;
        
	D3D11_CULL_MODE mCullMode;
    ID3D11DepthStencilState * mDepthStencilStateObject;
};
    
struct DepthStencilState;
class DepthStencilStateManager
{
	DeclSingleton(DepthStencilStateManager)

public:
	DepthStencilStateManager();
	~DepthStencilStateManager();

    void Initialize();
        
    DepthStencilState* GetDepthStencilState(DSS_DepthStencilState state);
        
private:
    DepthStencilState* mDepthStencilStates;
};
   
