
//#include "Utils/RenderUtility.h"
//#include "Utils/Utility.h"
#include "Utils/AssertDbg.h"
#include "Effect.h"
#include "EffectManager.h"
#include "DepthStencilStateManager.h"
#include "RasterizerStateManager.h"
#include "AlphaBlendStateManager.h"

Effect::Effect(class GraphicsDriver& gDriver)
	: mGraphics(gDriver)
	, mBlendState(NULL)
	, mRasterizerState(NULL)
    , mDepthStencilState(NULL)
	, mVSInputLayout(NULL)
{
	mVSVertexFormat = VertexFormat_Count;
	mVS = NULL; 
	mPS = NULL;
	mGS = NULL;
	mCS = NULL;
}

Effect::~Effect()
{
	if (mVS)				mVS->Release();
	if (mGS)				mGS->Release();
	if (mPS)				mPS->Release();
	if (mCS)				mCS->Release();
	//if (mVSInputLayout)		mVSInputLayout->Release();
	//if (mBlendState->mBlendStateObject)		
	//	mBlendState->mBlendStateObject->Release();
	//
	//if (mRasterizerState->mRasterizerStateObject)	
	//	mRasterizerState->mRasterizerStateObject->Release();
	//
	//if (mDepthStencilState->mDepthStencilStateObject)	
	//	mDepthStencilState->mDepthStencilStateObject->Release();
	
}

void Effect::loadTechnique(
	const char *vsFilenameNoExt, const char *vsName, 
	const char *gsFilenameNoExt, const char *gsName,
	const char *psFilenameNoExt, const char *psName,
	const char *csFilenameNoExt, const char *csName,
	RS_RasterizerState rasterizerStateId,
	DSS_DepthStencilState depthStencilStateId,
	ABS_AlphaBlendState blendStateId,
    const char *techniqueName,
	D3D_SHADER_MACRO* defines)
{
	const char *vsFilename = NULL;
	const char *gsFilename = NULL;
	const char *psFilename = NULL;
	const char *csFilename = NULL;

	if (vsFilenameNoExt){ /*sprintf(vsFilenameBuf, "%s.%s", vsFilenameNoExt, "cgvs");*/ vsFilename = vsFilenameNoExt;}
	if (gsFilenameNoExt){ /*sprintf(gsFilenameBuf, "%s.%s", gsFilenameNoExt, "cggs");*/ gsFilename = gsFilenameNoExt;}
	if (psFilenameNoExt){ /*sprintf(psFilenameBuf, "%s.%s", psFilenameNoExt, "cgps");*/ psFilename = psFilenameNoExt;}
	if (csFilenameNoExt){ /*sprintf(csFilenameBuf, "%s.%s", csFilenameNoExt, "cgcs");*/ csFilename = csFilenameNoExt;}

	ID3DBlob* vsBlob = loadTechnique(vsFilename, vsName, gsFilename, gsName, psFilename, psName, csFilename, csName, techniqueName, defines);

	//validate vertex shader name:
	if (vsName != 0)
	{
		if(mVSVertexFormat == VertexFormat_Count)
			mVSVertexFormat = GetFormatFromVSName(vsFilename);
		//D3D11_INPUT_ELEMENT_DESC* ied;

		// Check if input layout has already been created
		mVSInputLayout = EffectManager::Instance()->GetInputLayout(mVSVertexFormat);
		if (mVSInputLayout == NULL)
		{
			if (mVSVertexFormat == VertexFormat_Count)
			{
				int i = 0;
				DbgAssert(false, "No vertex format set");
			}
			else if (mVSVertexFormat == VertexFormat_MinimalMesh && vsBlob != NULL)
			{
				D3D11_INPUT_ELEMENT_DESC ied[] =
				{
					{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT,	0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
					{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT,		0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 }
					//{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 }
				};

				mVSInputLayout = mGraphics.CreateInputLayout(ied, 2, &vsBlob);
				EffectManager::Instance()->RegisterInputLayout(VertexFormat_MinimalMesh, mVSInputLayout);
			}
			else if (mVSVertexFormat == VertexFormat_StdMesh && vsBlob != NULL)
			{
				D3D11_INPUT_ELEMENT_DESC ied[] =
				{
					{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT,	0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
					{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT,		0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
					{ "NORMAL", 0,	 DXGI_FORMAT_R32G32B32_FLOAT,	0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 }
				};

				mVSInputLayout = mGraphics.CreateInputLayout(ied, 3, &vsBlob);
				EffectManager::Instance()->RegisterInputLayout(VertexFormat_StdMesh, mVSInputLayout);
			}
			else if (mVSVertexFormat == VertexFormat_DetailedMesh && vsBlob != NULL)
			{
				D3D11_INPUT_ELEMENT_DESC ied[] =
				{
					{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT,	0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
					{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT,		0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
					{ "NORMAL", 0,	 DXGI_FORMAT_R32G32B32_FLOAT,	0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
					{ "TANGENT", 0,	 DXGI_FORMAT_R32G32B32_FLOAT,	0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 }
				};

				mVSInputLayout = mGraphics.CreateInputLayout(ied, 4, &vsBlob);
				EffectManager::Instance()->RegisterInputLayout(VertexFormat_DetailedMesh, mVSInputLayout);
			}
			else if (mVSVertexFormat == VertexFormat_StdParticle && vsBlob != NULL)
			{
				D3D11_INPUT_ELEMENT_DESC ied[] =
				{
					{ "POSITION",	0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
					{ "VELOCITY",	0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
					{ "SIZE",		0, DXGI_FORMAT_R32G32_FLOAT,	0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
					{ "LIFESPAN",	0, DXGI_FORMAT_R32_FLOAT,		0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 }
				};

				mVSInputLayout = mGraphics.CreateInputLayout(ied, 4, &vsBlob);
				EffectManager::Instance()->RegisterInputLayout(VertexFormat_StdParticle, mVSInputLayout);
			}
		}
	}

	if(vsBlob)
		vsBlob->Release();

	if (csFilename)
	{
		int i = 0;
		B_ASSERT(vsFilename == psFilename && vsFilename == 0, "Compute shaders have to be the only shader in technique");
	}
	else
	{
		mBlendState			= AlphaBlendStateManager::Instance()->GetAlphaBlendState(blendStateId);
		mRasterizerState	= RasterizerStateManager::Instance()->GetRasterizerState(rasterizerStateId);
		mDepthStencilState	= DepthStencilStateManager::Instance()->GetDepthStencilState(depthStencilStateId);
	}

}

ID3DBlob* Effect::loadTechnique(const char *vsFilename, const char *vsName,
	const char *gsFilename, const char *gsName,
	const char *psFilename, const char *psName,
	const char *csFilename, const char *csName,
	const char *techniqueName, D3D_SHADER_MACRO* defines)
{
	mTechniqueName = techniqueName;

	if (gsFilename != NULL)
	{
		ID3D10Blob* blob = NULL;
		mGS = mGraphics.CreateGeometryShader(&blob, gsFilename, gsName, defines);
		blob->Release();
	}
	if (psFilename != NULL)
	{
		ID3D10Blob* blob = NULL;
		mPS = mGraphics.CreatePixelShader(&blob, psFilename, psName, defines);
		blob->Release();
	}
	if (csFilename != NULL)
	{
		ID3D10Blob* blob = NULL;
		mCS = mGraphics.CreateComputeShader(&blob, csFilename, csName, defines);
		blob->Release();
	}
	if (vsFilename != NULL)
	{
		ID3D10Blob* blob = NULL;
		mVS = mGraphics.CreateVertexShader(&blob, vsFilename, vsName, defines);
		return blob;
	}

	return NULL;
}

void Effect::SetActive()
{
	mGraphics.BindEffect(this);

	B_ASSERT( mCS || mBlendState != NULL, "Every Non-CS Technique has to have a blend state");
	if (mBlendState)
		mBlendState->SetActive();
	
	B_ASSERT(mCS || mRasterizerState != NULL, "Every Non-CS Technique has to have a rasterizer state");
	if (mRasterizerState)
		mRasterizerState->SetActive();
	
	B_ASSERT(mCS || mDepthStencilState != NULL, "Every Non-CS Technique has to have a depth stencil state");
	if (mDepthStencilState)
		mDepthStencilState->SetActive();
}

void Effect::setConstantBuffer(ID3D11Device *pDevice, ID3D11DeviceContext *pDeviceContex, ID3D11Buffer * pBuf, UINT32 slotId, void *pVal, UINT32 size)
{			
	/*
			typedef enum D3D11_MAP {
				D3D11_MAP_READ                 = 1,
				D3D11_MAP_WRITE                = 2,
				D3D11_MAP_READ_WRITE           = 3,
				D3D11_MAP_WRITE_DISCARD        = 4,
				D3D11_MAP_WRITE_NO_OVERWRITE   = 5 
			} D3D11_MAP; */

			
	D3D11_MAPPED_SUBRESOURCE mappedResource;
	HRESULT hr = pDeviceContex->Map(
		pBuf, // resource
		0, // subresource
		D3D11_MAP_WRITE_DISCARD, 
		0, 
		&mappedResource);
	DbgAssert(SUCCEEDED(hr), "");
	// set value
	memcpy(mappedResource.pData, pVal, size);

	pDeviceContex->Unmap(pBuf, 0);

	pDeviceContex->CSSetConstantBuffers(slotId, 1, &pBuf);

	pDeviceContex->VSSetConstantBuffers(slotId, 1, &pBuf);
	pDeviceContex->GSSetConstantBuffers(slotId, 1, &pBuf);
	pDeviceContex->PSSetConstantBuffers(slotId, 1, &pBuf);
}

