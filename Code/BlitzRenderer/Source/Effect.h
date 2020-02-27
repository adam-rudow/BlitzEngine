#pragma once

// Outer-Engine includes

// Inter-Engine includes

#include "Renderer.h"
#include "Math/math.h"
//#include "Utils/RenderUtility.h"

struct DepthStencilState;
struct RasterizerState;
struct AlphaBlendState;
enum DSS_DepthStencilState;
enum RS_RasterizerState;
enum ABS_AlphaBlendState;

struct RENDERER_EXPORT Effect
{
	Effect(class GraphicsDriver& gDriver);
	~Effect();

	void loadTechnique(const char *vsFilename, const char *vsName, 
		const char *gsFilename, const char *gsName,
		const char *psFilename, const char *psName,
		const char *csFilename, const char *csName,
		RS_RasterizerState rasterizerStateId,
		DSS_DepthStencilState depthStencilStateId,
		ABS_AlphaBlendState blendStateId,
		const char *techniqueName,
		D3D_SHADER_MACRO* defines = NULL);
	


	ID3DBlob* loadTechnique(const char *vsFilename, const char *vsName, 
		const char *gsFilename, const char *gsName,
		const char *psFilename, const char *psName,
		const char *csFilename, const char *csName,
		const char *techniqueName,
		D3D_SHADER_MACRO* defines = NULL);

	void loadShader_D3D11(const char *filename, const char *name,
		D3D_SHADER_MACRO *pMacros,
		const char *version,
		ID3DBlob* &pShaderBuffer);

	void SetActive();

	static void setConstantBuffer(ID3D11Device *pDevice, ID3D11DeviceContext *pDeviceContex, ID3D11Buffer * pBuf, UINT32 slotId, void *pVal, UINT32 size);

	//******************************************
	class GraphicsDriver& mGraphics;

	const char* mTechniqueName;

	VertexShader *mVS;
	GeometryShader *mGS;
	PixelShader *mPS;
	ComputeShader *mCS;

	VertexInputLayout*	mVSInputLayout;
	VertexFormatGPU		mVSVertexFormat;
	AlphaBlendState *	mBlendState;
    DepthStencilState*	mDepthStencilState;
	RasterizerState*	mRasterizerState;
	
};

