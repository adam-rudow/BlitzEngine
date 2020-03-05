#define NOMINMAX

// Sibling/Children includes
//[S1]
//#include "AssetLoader.h"
#include "Utils/AssertDbg.h"
#include "Effect.h"
#include "EffectManager.h"
//#include "Game.h"
#include "Mesh.h"
#include "MeshComponent.h"
#include "ResourceBufferGPU.h"
#include "SamplerState.h"
#include "Texture.h"
#include "VoxelGrid.h"

ImplSingleton(EffectManager)

EffectManager::EffectManager()
	: mCurRenderTarget(NULL)
{
	mNumVPLPropogations = 2;
	gDriver = GraphicsDriver::Instance();
}

EffectManager::~EffectManager()
{
	// Free effects
	for (auto e : mNameToEffectMap)
	{
		delete(e.second);
	}

	// Free InputLayouts
	for (auto i : mInputLayoutMap)
	{
		if(i.second) i.second->Release();
	}

	// Free Render target textures
	delete(mGBufferPositionTexture);
	delete(mGBufferNormalsTexture);
	delete(mGBufferDiffuseTexture);
	delete(mDeferredLightingResult);
	delete(mLightVolumeVertexBuffer);
	//delete(mQuadVertexBuffer);
	delete(mVoxelGridStructuredBuffer);
	//delete(mVoxelGridEffect);
	delete(mVoxelizationRenderTarget);
	//delete(mVPLCreateEffect);
	//delete(mVPLPropogationNoOccEffect);
	//delete(mVPLPropogationWithOccEffect);
	delete(mVPL_RedSHCoefficients0);
	delete(mVPL_GreenSHCoefficients0);
	delete(mVPL_BlueSHCoefficients0);
	delete(mVPL_RedSHCoefficients1);
	delete(mVPL_GreenSHCoefficients1);
	delete(mVPL_BlueSHCoefficients1);
	//delete(mVoxelGridClearComputeEffect);
	//delete(mVoxelGIIndirectIlluminationApply);
	delete(mVoxelGIFinalTexture);
	delete(mShadowMapDepthTexture);
}

void EffectManager::RegisterEffect(Effect* pEffect)
{ 
	mNameToEffectMap.emplace(std::string(pEffect->mTechniqueName), pEffect); 
}

void EffectManager::RegisterEffect(ShaderType pType, Effect* pEffect) 
{
	std::string s(pEffect->mTechniqueName);
	mNameToEffectMap[s] = pEffect;
	mTypeToEffectMap[pType] = pEffect;
}

void EffectManager::RegisterEffect(VertexFormatGPU vertFormat, Effect* pEffect)
{
	std::string s(pEffect->mTechniqueName);
	mNameToEffectMap[s] = pEffect;
	mFormatToEffectMap[vertFormat] = pEffect;
}

void EffectManager::RegisterZOnlyEffect(ShaderType pType, Effect* pEffect)
{
	std::string s(pEffect->mTechniqueName);
	mNameToEffectMap[s] = pEffect;
	mTypeToZOnlyEffectMap[pType] = pEffect;
}

void EffectManager::RegisterZOnlyEffect(VertexFormatGPU vertFormat, Effect* pEffect)
{
	std::string s(pEffect->mTechniqueName);
	mNameToEffectMap[s] = pEffect;
	mFormatToZOnlyEffectMap[vertFormat] = pEffect;
}

void EffectManager::RegisterVoxelEffect(Effect* pEffect)
{
	std::string s(pEffect->mTechniqueName);
	mNameToEffectMap[s] = pEffect;
	mVoxelEffectMap[s] = pEffect;
}

void EffectManager::RegisterVoxelEffect(VertexFormatGPU vertFormat, Effect* pEffect)
{
	std::string s(pEffect->mTechniqueName);
	mNameToEffectMap[s] = pEffect;
	mFormatToVoxelEffectMap[vertFormat] = pEffect;
}

void EffectManager::RegisterInputLayout(VertexFormatGPU eFormat, ID3D11InputLayout* layout)
{
	if (mInputLayoutMap.find(eFormat) == mInputLayoutMap.end())
		mInputLayoutMap[eFormat] = layout;
}

Effect* EffectManager::GetEffect(const char *pEffectFilename)
{
	std::map<std::string, Effect*>::iterator it = mNameToEffectMap.find(pEffectFilename);
	if (it == mNameToEffectMap.end())
		return NULL;

	return it->second;
}

ID3D11InputLayout* EffectManager::GetInputLayout(VertexFormatGPU eFormat)
{
	std::map<VertexFormatGPU, ID3D11InputLayout*>::iterator it = mInputLayoutMap.find(eFormat);
	if (it == mInputLayoutMap.end())
		return NULL;

	return it->second;
}

Effect* EffectManager::GetEffect(ShaderType pType)
{
	std::map<ShaderType, Effect*>::iterator it = mTypeToEffectMap.find(pType);
	if (it == mTypeToEffectMap.end())
		return NULL;

	return it->second;
}

Effect* EffectManager::GetEffect(VertexFormatGPU vertFormat)
{
	std::map<VertexFormatGPU, Effect*>::iterator it = mFormatToEffectMap.find(vertFormat);
	if (it != mFormatToEffectMap.end())
	{
		return it->second;
	}

	return NULL;
}

Effect* EffectManager::GetZOnlyEffect(ShaderType pType)
{
	std::map<ShaderType, Effect*>::iterator it = mTypeToZOnlyEffectMap.find(pType);
	if (it == mTypeToZOnlyEffectMap.end())
		return NULL;

	return it->second;
}

Effect* EffectManager::GetZOnlyEffect(VertexFormatGPU vertFormat)
{
	std::map<VertexFormatGPU, Effect*>::iterator it = mFormatToZOnlyEffectMap.find(vertFormat);
	if (it != mFormatToZOnlyEffectMap.end())
	{
		return it->second;
	}

	return NULL;
}

Effect* EffectManager::GetVoxelizationEffect(Effect* normalEffect)
{
	if (normalEffect == NULL)
		return NULL;

	std::string effectName(normalEffect->mTechniqueName);
	if (/*effectName.length() >= 12 && effectName.substr(0, 12) == "DetailedMesh"*/ effectName.find("Detail") != std::string::npos)
	{
		return mVoxelEffectMap.find("DetailedMesh_Voxelization_Tech")->second;
	}
	else if (effectName.length() >= 12 && effectName.substr(0, 12) == "DetailedSkin")
	{
		return mVoxelEffectMap.find("DetailedSkin")->second;
	}
	else if (/*effectName.length() >= 7 &&*/ effectName.substr(0, 9) == "BasicMesh")
	{
		return mVoxelEffectMap.find("StdMesh_Voxelization_Tech")->second;
	}

	return NULL;
}

Effect* EffectManager::GetVoxelizationEffect(VertexFormatGPU vertFormat)
{
	std::map<VertexFormatGPU, Effect*>::iterator it = mFormatToVoxelEffectMap.find(vertFormat);
	if (it != mFormatToVoxelEffectMap.end())
	{
		return it->second;
	}

	return NULL;
}

void EffectManager::LoadGPUResources()
{
	gRenderer = reinterpret_cast<Renderer*>(IRenderer::Instance());

	// Set up Render Target textures and Shader Resources
	GraphicsDriver* gDriver = GraphicsDriver::Instance();

	//**************************************
	// Textures and Render Targets

	// Shadow Map - need to lerp minimization and at this point we dont lerp magnification
	mShadowMapDepthTexture = new TextureGPU(*gDriver);
	mShadowMapDepthTexture->createDrawableIntoDepthTexture(2048, 2048, SamplerState_NoMips_MinTexelLerp_NoMagTexelLerp_Clamp);

	// Deferred Rendering Targets/Resources
	mGBufferPositionTexture	= new TextureGPU(*gDriver);
	mGBufferPositionTexture->createDeferredRenderTexture(Renderer::Window_Width, Renderer::Window_Height);
	mGBufferNormalsTexture	= new TextureGPU(*gDriver);
	mGBufferNormalsTexture->createDeferredRenderTexture(Renderer::Window_Width, Renderer::Window_Height);
	mGBufferDiffuseTexture	= new TextureGPU(*gDriver);
	mGBufferDiffuseTexture->createDeferredRenderTexture(Renderer::Window_Width, Renderer::Window_Height);

	mDeferredLightingResult = new TextureGPU(*gDriver);
	mDeferredLightingResult->createDrawableIntoColorTextureWithDepth(Renderer::Window_Width, Renderer::Window_Height, SamplerState_NoMips_MinTexelLerp_NoMagTexelLerp_Clamp);


	//**************************************
	// Resources
	UINT32 voxelDimension = VoxelGrid::s_voxelGridDimension;

	mVoxelizationRenderTarget = new TextureGPU(*gDriver);
	mVoxelizationRenderTarget->createVoxelizationDummyRenderTexture(voxelDimension * 2, voxelDimension * 2);

	// Voxel Grid GPU structure
	mVoxelGridStructuredBuffer = new ResourceBufferGPU(*gDriver, sizeof(Voxel), voxelDimension * voxelDimension * voxelDimension, true, true);

	mVPL_RedSHCoefficients0  = new TextureGPU(*gDriver);
	mVPL_RedSHCoefficients1  = new TextureGPU(*gDriver);
	mVPL_RedSHCoefficients0->createVPLColors2DTextureArrays(voxelDimension, voxelDimension, voxelDimension);
	mVPL_RedSHCoefficients1->createVPLColors2DTextureArrays(voxelDimension, voxelDimension, voxelDimension);

	mVPL_GreenSHCoefficients0  = new TextureGPU(*gDriver);
	mVPL_GreenSHCoefficients1  = new TextureGPU(*gDriver);
	mVPL_GreenSHCoefficients0->createVPLColors2DTextureArrays(voxelDimension, voxelDimension, voxelDimension);
	mVPL_GreenSHCoefficients1->createVPLColors2DTextureArrays(voxelDimension, voxelDimension, voxelDimension);

	mVPL_BlueSHCoefficients0  = new TextureGPU(*gDriver);
	mVPL_BlueSHCoefficients1  = new TextureGPU(*gDriver);
	mVPL_BlueSHCoefficients0->createVPLColors2DTextureArrays(voxelDimension, voxelDimension, voxelDimension);
	mVPL_BlueSHCoefficients1->createVPLColors2DTextureArrays(voxelDimension, voxelDimension, voxelDimension);

	mVoxelGIFinalTexture = new TextureGPU(*gDriver);
	mVoxelGIFinalTexture->createDrawableIntoColorTextureWithDepth(Renderer::Window_Width, Renderer::Window_Height, SamplerState_NoMips_MinTexelLerp_NoMagTexelLerp_Clamp);

	// Initialize grid data to 0
	std::vector<Voxel> data(voxelDimension * voxelDimension * voxelDimension);
	//ZeroMemory(&data[0], data.size() * sizeof(Voxel));
	for (unsigned i = 0; i < data.size(); ++i)
	{
		data[i] = Voxel();
		data[i].mColorMask = 0;
		data[i].mNormalMask[0] = 0;
		data[i].mNormalMask[1] = 0;
		data[i].mNormalMask[2] = 0;
		data[i].mOcclusion = 0;
	}
	mVoxelGridStructuredBuffer->CreateBuffer(&data[0]);

	// Initialize other necessary objects
	buildFullScreenBoard();
	buildLightVolumeSphere();
}

void EffectManager::setupConstantBuffersAndShaderResources()
{

		/*D3D11Renderer *pD3D11Renderer = static_cast<D3D11Renderer *>(m_pContext->getGPUScreen());
		ID3D11Device *pDevice = pD3D11Renderer->m_pD3DDevice;
		ID3D11DeviceContext *pDeviceContext = pD3D11Renderer->m_pD3DContext;*/

		//ID3D11Buffer *pBuf;
		{
			//cbuffers
			//HRESULT hr;
			D3D11_BUFFER_DESC cbDesc;
			cbDesc.Usage = D3D11_USAGE_DYNAMIC; // can only write to it, can't read
			cbDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
			cbDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE; // make sure we can access it with cpu for writing only
			cbDesc.MiscFlags = 0;
		
			//cbDesc.ByteWidth = sizeof( SetPerFrameConstantsShaderAction::Data );
			//hr = pDevice->CreateBuffer( &cbDesc, NULL, &pBuf );
			//assert(SUCCEEDED(hr));
		
			//SetPerFrameConstantsShaderAction::s_pBuffer = pBuf;
			//m_cbuffers.add("cbPerFrame", pBuf);

			//cbDesc.ByteWidth = sizeof( SetPerObjectGroupConstantsShaderAction::Data );
			//hr = pDevice->CreateBuffer( &cbDesc, NULL, &pBuf);
			//assert(SUCCEEDED(hr));
		
			//SetPerObjectGroupConstantsShaderAction::s_pBuffer = pBuf;
			//m_cbuffers.add("cbPerObjectGroup", pBuf);

			//cbDesc.ByteWidth = sizeof( SetPerObjectConstantsShaderAction::Data );
			//hr = pDevice->CreateBuffer( &cbDesc, NULL, &pBuf);
			//assert(SUCCEEDED(hr));
			//SetPerObjectConstantsShaderAction::s_pBuffer = pBuf;
			//m_cbuffers.add("cbPerObject", pBuf);

			//cbDesc.ByteWidth = sizeof( SetPerMaterialConstantsShaderAction::Data );
			//hr = pDevice->CreateBuffer( &cbDesc, NULL, &pBuf);
			//assert(SUCCEEDED(hr));
			//SetPerMaterialConstantsShaderAction::s_pBuffer = pBuf;
			//m_cbuffers.add("cbPerMaterial", pBuf);

			//cbDesc.ByteWidth = sizeof( SetInstanceControlConstantsShaderAction::Data );
			//hr = pDevice->CreateBuffer( &cbDesc, NULL, &pBuf);
			//assert(SUCCEEDED(hr));
			//SetInstanceControlConstantsShaderAction::s_pBuffer = pBuf;
			//m_cbuffers.add("cbInstanceControlConstants", pBuf);

			//cbDesc.ByteWidth = sizeof(SetPerEmitterConstantsShaderAction::Data);
			//hr = pDevice->CreateBuffer(&cbDesc, NULL, &pBuf);
			//assert(SUCCEEDED(hr));
			//SetPerEmitterConstantsShaderAction::s_pBuffer = pBuf;
			//m_cbuffers.add("cbPerEmitter", pBuf);

			//cbDesc.ByteWidth = sizeof(SetPerLightConstantsShaderAction::Data);
			//hr = pDevice->CreateBuffer(&cbDesc, NULL, &pBuf);
			//assert(SUCCEEDED(hr));
			//SetPerLightConstantsShaderAction::s_pBuffer = pBuf;
			//m_cbuffers.add("cbPerLight", pBuf);
		}

		{
			// cbDesc.Usage = D3D11_USAGE_DEFAULT can't have cpu access flag set

			// if need cpu access for writing (using map) usage must be DYNAMIC or STAGING. resource cant be set as output
			// if need cpu access for reading and writing (using map) usage must be STAGING. resource cant be set as output
			// note, map & DYNAMIC is used for resrouces that get Updated at least once per frame
			// also note that map allows updating part of resource while gpu might be using other part
			// can use UpdateSubresource with DEFAULT or IMMUTABLE, but suggested to use only with DEFAULT
			
			// if usage is default (gpu reads and writes) then UpdateSubresource() is used to write to it from cpu
			
			
			//tbuffers
			D3D11_BUFFER_DESC cbDesc;
			cbDesc.Usage = D3D11_USAGE_DYNAMIC; // can only write to it, can't read
			cbDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE; //means will need shader resource view // D3D11_BIND_CONSTANT_BUFFER;
			cbDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE; // make sure we can access it with cpu for writing only

			cbDesc.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;
			cbDesc.StructureByteStride = sizeof(Matrix4);


			/*cbDesc.ByteWidth = sizeof(SA_SetAndBind_ConstResource_InstancedObjectsAnimationPalettes::Data);
			HRESULT hr = pDevice->CreateBuffer( &cbDesc, NULL, &pBuf);
			assert(SUCCEEDED(hr));
			SA_SetAndBind_ConstResource_InstancedObjectsAnimationPalettes::s_pBuffer = pBuf;
			

			cbDesc.ByteWidth = sizeof(SA_SetAndBind_ConstResource_SingleObjectAnimationPalette::Data);
			hr = pDevice->CreateBuffer( &cbDesc, NULL, &pBuf);
			assert(SUCCEEDED(hr));
			SA_SetAndBind_ConstResource_SingleObjectAnimationPalette::s_pBufferSinglePalette = pBuf;


			cbDesc.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;
			cbDesc.StructureByteStride = sizeof(SA_SetAndBind_ConstResource_PerInstanceData::PerObjectInstanceData);
			cbDesc.ByteWidth = sizeof( SA_SetAndBind_ConstResource_PerInstanceData::Data );
			hr = pDevice->CreateBuffer( &cbDesc, NULL, &pBuf);
			assert(SUCCEEDED(hr));
			SA_SetAndBind_ConstResource_PerInstanceData::s_pBuffer = pBuf;


			D3D11_SHADER_RESOURCE_VIEW_DESC vdesc;
			vdesc.Format = DXGI_FORMAT_UNKNOWN;
			vdesc.Buffer.NumElements = sizeof(SA_SetAndBind_ConstResource_InstancedObjectsAnimationPalettes::Data) / (4*4*4);

			vdesc.ViewDimension = D3D11_SRV_DIMENSION_BUFFER;
			vdesc.Buffer.ElementOffset = 0;
			
			hr = pDevice->CreateShaderResourceView(SA_SetAndBind_ConstResource_InstancedObjectsAnimationPalettes::s_pBuffer, &vdesc, &SA_SetAndBind_ConstResource_InstancedObjectsAnimationPalettes::s_pShaderResourceView);
			assert(SUCCEEDED(hr));
			
			vdesc.Buffer.NumElements = sizeof(SA_SetAndBind_ConstResource_SingleObjectAnimationPalette::Data) / (4*4*4);

			hr = pDevice->CreateShaderResourceView(SA_SetAndBind_ConstResource_SingleObjectAnimationPalette::s_pBufferSinglePalette, &vdesc, &SA_SetAndBind_ConstResource_SingleObjectAnimationPalette::s_pShaderResourceViewSinglePalette);
			assert(SUCCEEDED(hr));


			vdesc.Buffer.NumElements = sizeof(SA_SetAndBind_ConstResource_PerInstanceData::Data) / sizeof(SA_SetAndBind_ConstResource_PerInstanceData::PerObjectInstanceData);
			hr = pDevice->CreateShaderResourceView(SA_SetAndBind_ConstResource_PerInstanceData::s_pBuffer, &vdesc, &SA_SetAndBind_ConstResource_PerInstanceData::s_pShaderResourceView);
			assert(SUCCEEDED(hr));*/
		}

}

void EffectManager::SetShadowMapShaderValue(/*PE::Components::DrawList *pDrawList*/)
{
//	Handle &h = pDrawList->nextGlobalShaderValue();
//	h = Handle("SA_Bind_Resource", sizeof(SA_Bind_Resource));
//	SA_Bind_Resource *p = new(h) SA_Bind_Resource(*m_pContext, m_arena);
//	p->set(SHADOWMAP_TEXTURE_2D_SAMPLER_SLOT, m_shadowMapDepthTexture.m_samplerState, API_CHOOSE_DX11_DX9_OGL_PSVITA(m_shadowMapDepthTexture.m_pDepthShaderResourceView, m_shadowMapDepthTexture.m_pTexture, m_shadowMapDepthTexture.m_texture, m_shadowMapDepthTexture.m_texture));
	GraphicsDriver::Instance()->BindTextureResource(mShadowMapDepthTexture, 1, true);
}

void EffectManager::buildFullScreenBoard()
{
	std::vector<float> verts;
	float fw = (float)(Renderer::Window_Width);
	float fh = (float)(Renderer::Window_Height);
	float fx = -1.0f / fw / 2.0f;
	float fy = 1.0f / fh / 2.0f;

	verts.push_back(-1.0f + fx);	verts.push_back(-1.0f + fy);	verts.push_back(0.0f);
	verts.push_back(1.0f + fx);		verts.push_back(-1.0f + fy);	verts.push_back(0.0f);
	verts.push_back(1.0f + fx);		verts.push_back(1.0f + fy);		verts.push_back(0.0f);
	verts.push_back(-1.0f + fx);	verts.push_back(1.0f + fy);		verts.push_back(0.0f);
	
	std::vector<float> texCoords;
	texCoords.push_back(0.0f); texCoords.push_back(1.0f);// texCoords.push_back(0.0f);
	texCoords.push_back(1.0f); texCoords.push_back(1.0f);// texCoords.push_back(0.0f);
	texCoords.push_back(1.0f); texCoords.push_back(0.0f);// texCoords.push_back(0.0f);
	texCoords.push_back(0.0f); texCoords.push_back(0.0f);// texCoords.push_back(0.0f);
	
	std::vector<UINT32> idxs;
	idxs.push_back(0); idxs.push_back(2); idxs.push_back(1);
	idxs.push_back(2); idxs.push_back(0); idxs.push_back(3);

	ID3D11InputLayout* inputLayout = GetInputLayout(VertexFormat_MinimalMesh);
	mQuadVertexBuffer = new VertexData(verts, idxs, texCoords, inputLayout, 5); // 5, not 8, because normals are not needed

	/*
	m_hFirstGlowPassEffect = getEffectHandle("firstglowpass.fx");
	m_hSecondGlowPassEffect = getEffectHandle("verticalblur.fx");
	m_hGlowSeparationEffect = getEffectHandle("glowseparationpass.fx");
	m_hMotionBlurEffect = getEffectHandle("motionblur.fx");
	m_hColoredMinimalMeshTech = getEffectHandle("ColoredMinimalMesh_Tech");
	*/
	mDeferredLightingEffect = GetEffect("DeferredPointLightingTech");
	
	mVoxelGridEffect = GetEffect("Voxelization_Debug_Tech");
	mVoxelGridClearComputeEffect = GetEffect("VoxelGrid_Clear_Compute_Tech");
	mVPLCreateEffect = GetEffect("VPL_Create_Tech");
	mVPLPropogationNoOccEffect = GetEffect("VoxelGrid_VPL_Propogation_NoOcclusion_Tech");
	mVPLPropogationWithOccEffect = GetEffect("VoxelGrid_VPL_Propogation_WithOcclusion_Tech");
	mVoxelGIIndirectIlluminationApply = GetEffect("VoxelGI_Indirect_Illumination_Apply");
	
}

void EffectManager::buildLightVolumeSphere()
{	// [S1] TODO: uncomment and re-add
	//mLightVolumeSphere = AssetLoader::Instance()->LoadMesh("LightSphere.obj");
}



void EffectManager::SetShadowMapRenderTarget()
{
	if (mCurRenderTarget)
	{
		//assert(!"There should be no active render target when we set shadow map as render target!");
	}

	GraphicsDriver::Instance()->SetDepthStencilOnlyRenderTargetAndViewport(mShadowMapDepthTexture, true);
	mCurRenderTarget = mShadowMapDepthTexture;

}

void EffectManager::SetDeferredShadingRenderTargets()
{
	//((D3D11Renderer*)m_pContext->getGPUScreen())->setDeferredRenderTargetsAndDepths(&mGBufferPositionTexture, &mGBufferNormalsTexture, &mGBufferDiffuseTexture);
	//ID3D11RenderTargetView* deferredRTVs[] = { mGBufferPositionTexture->mRenderTargetView, mGBufferNormalsTexture->mRenderTargetView, mGBufferDiffuseTexture->mRenderTargetView };
	GraphicsDriver::Instance()->SetMultiRenderTargetsWithDepths(mGBufferPositionTexture, mGBufferNormalsTexture, mGBufferDiffuseTexture);
}

void EffectManager::SetVoxelizationRenderTarget()
{
	//((D3D11Renderer*)m_pContext->getGPUScreen())->setRenderTargetsAndViewportWithNoDepth(&mVoxelizationRenderTarget);
}

void EffectManager::SetVoxelGridShaderResource()
{
	//mVoxelGridStructuredBuffer.getObject<ResourceBufferGPU>()->m_resourceBufferLowLevel.m_pShaderResourceUAV
	//Handle &hSV =  DrawList::InstanceReadOnly()->nextGlobalShaderValue();
	//hSV = Handle("StructuredBuffer_VoxelGrid", sizeof(SA_Bind_Resource));
	//SA_Bind_Resource *pSetTextureAction = new(hSV) SA_Bind_Resource(*m_pContext, m_arena);
	//PEASSERT(mVoxelGridStructuredBuffer.getObject<ResourceBufferGPU>()->m_resourceBufferLowLevel.m_pShaderResourceUAV != NULL, "shader uav not set");

	//GraphicsDriver* gDriver = GraphicsDriver::Instance();
	//mCurRenderTarget = mVoxelizationRenderTarget;
	gDriver->SetRenderTargetsAndUAVNoDepth(mVoxelizationRenderTarget, mVoxelGridStructuredBuffer, 3, true);

	// Bind Deferred Lighting texture result as a shader resource for Global Illumination
	gDriver->BindTextureResource(mDeferredLightingResult, 6);
}

void EffectManager::EndCurrentRenderTarget(bool endUAV)
{
	if (endUAV)
	{
		//((D3D11Renderer*)m_pContext->getGPUScreen())->endRenderTargetAndUAV(NULL, true);
	}
	else if(mCurRenderTarget != NULL)
	{
		gDriver->SetRenderTargetAndViewportWithNoDepth(0, false);
		//mCurRenderTarget = NULL;
	}
}

void EffectManager::EndRenderTargets(int numRenderTargets)
{
	GraphicsDriver::Instance()->EndRenderTargets();
}
//
//void EffectManager::setTextureAndDepthTextureRenderTargetForGlow()
//{
//	m_pContext->getGPUScreen()->setRenderTargetsAndViewportWithDepth(m_hGlowTargetTextureGPU.getObject<TextureGPU>(), m_hGlowTargetTextureGPU.getObject<TextureGPU>(), true, true);
//	m_pCurRenderTarget = m_hGlowTargetTextureGPU.getObject<TextureGPU>();
//}
//void EffectManager::setTextureAndDepthTextureRenderTargetForDefaultRendering()
//{
//	// use device back buffer and depth
//	m_pContext->getGPUScreen()->setRenderTargetsAndViewportWithDepth(0, 0, true, true);
//}
//
//
//void EffectManager::set2ndGlowRenderTarget()
//{
//	m_pContext->getGPUScreen()->setRenderTargetsAndViewportWithNoDepth(&m_2ndGlowTargetTextureGPU, true);
//	m_pCurRenderTarget = &m_2ndGlowTargetTextureGPU;
//}
//
//
//void EffectManager::drawFullScreenQuad()
//{
//	Effect &curEffect = *m_hColoredMinimalMeshTech.getObject<Effect>();
//
//	if (!curEffect.m_isReady)
//		return;
//
//	IndexBufferGPU *pibGPU = m_hIndexBufferGPU.getObject<IndexBufferGPU>();
//	pibGPU->setAsCurrent();
//
//	VertexBufferGPU *pvbGPU = m_hVertexBufferGPU.getObject<VertexBufferGPU>();
//	pvbGPU->setAsCurrent(&curEffect);
//
//	curEffect.setCurrent(pvbGPU);
//
//	TextureGPU *pTex = PE::GPUTextureManager::Instance()->getRandomTexture().getObject<TextureGPU>();
//
// 	PE::SA_Bind_Resource setTextureAction(*m_pContext, m_arena, DIFFUSE_TEXTURE_2D_SAMPLER_SLOT, pTex->m_samplerState, API_CHOOSE_DX11_DX9_OGL_PSVITA(pTex->m_pShaderResourceView, pTex->m_pTexture, pTex->m_texture, pTex->m_texture));
// 	setTextureAction.bindToPipeline(&curEffect);
//
//	PE::SetPerFrameConstantsShaderAction sa(*m_pContext, m_arena);
//	sa.m_data.gGameTime = 1.0f;
//
//	sa.bindToPipeline(&curEffect);
//
//	PE::SetPerObjectConstantsShaderAction objSa;
//	objSa.m_data.gW = Matrix4x4();
//	objSa.m_data.gW.loadIdentity();
//
//	static float x = 0;
//	//objSa.m_data.gW.importScale(0.5f, 1.0f, 1.0f);
//	objSa.m_data.gW.setPos(Vector3(x, 0, 1.0f));
//	//x+=0.01;
//	if (x > 1)
//		x = 0;
//	objSa.m_data.gWVP = objSa.m_data.gW;
//
//	objSa.bindToPipeline(&curEffect);
//
//	pibGPU->draw(1, 0);
//
//	pibGPU->unbindFromPipeline();
//	pvbGPU->unbindFromPipeline(&curEffect);
//
//	sa.unbindFromPipeline(&curEffect);
//	objSa.unbindFromPipeline(&curEffect);
//
//	setTextureAction.unbindFromPipeline(&curEffect);
//}

// this function has to be called right after rendering scene into render target
// the reason why mipmaps have to be generated is because we separate glow
// into a smaller texture, so we need to generate mipmaps so that when we sample glow, it is not aliased
// if mipmaps are not generated, but do exist glow might not work at all since a lower mip will be sampled
//void EffectManager::drawGlowSeparationPass()
//{
//	Effect &curEffect = *m_hGlowSeparationEffect.getObject<Effect>();
//
//	if (!curEffect.m_isReady)
//		return;
////todo: generate at least one mipmap on all platforms so that glow downsampling is smoother
//// in case it doesnt look smooth enough. as long as we are nto donwsampling to ridiculous amount it should be ok without mipmaps
///*
//
//		m_hGlowTargetTextureGPU.getObject<TextureGPU>()->m_pTexture->GenerateMipSubLevels();
//
//#if APIABSTRACTION_D3D11
//
//			D3D11Renderer *pD3D11Renderer = static_cast<D3D11Renderer *>(m_pContext->getGPUScreen());
//			ID3D11Device *pDevice = pD3D11Renderer->m_pD3DDevice;
//			ID3D11DeviceContext *pDeviceContext = pD3D11Renderer->m_pD3DContext;
//
//			pDeviceContext->GenerateMips(
//			m_hGlowTargetTextureGPU.getObject<TextureGPU>()->m_pShaderResourceView);
//#endif		
//*/
//	
//	m_pContext->getGPUScreen()->setRenderTargetsAndViewportWithNoDepth(&m_glowSeparatedTextureGPU, true);
//
//	m_pCurRenderTarget = &m_glowSeparatedTextureGPU;
//
//	IndexBufferGPU *pibGPU = m_hIndexBufferGPU.getObject<IndexBufferGPU>();
//	pibGPU->setAsCurrent();
//	
//	VertexBufferGPU *pvbGPU = m_hVertexBufferGPU.getObject<VertexBufferGPU>();
//	pvbGPU->setAsCurrent(&curEffect);
//
//	curEffect.setCurrent(pvbGPU);
//	
//	TextureGPU *t = /*&mDeferredLightingResult;*/ m_hGlowTargetTextureGPU.getObject<TextureGPU>();
//	PE::SA_Bind_Resource setTextureAction(*m_pContext, m_arena, DIFFUSE_TEXTURE_2D_SAMPLER_SLOT, t->m_samplerState, API_CHOOSE_DX11_DX9_OGL(t->m_pShaderResourceView, t->m_pTexture,t->m_texture));
//	setTextureAction.bindToPipeline(&curEffect);
//
//	PE::SetPerObjectConstantsShaderAction objSa;
//	objSa.m_data.gW = Matrix4x4();
//	objSa.m_data.gW.loadIdentity();
//	objSa.m_data.gWVP = objSa.m_data.gW;
//
//	objSa.bindToPipeline(&curEffect);
//	
//	pibGPU->draw(1, 0);
//
//	pibGPU->unbindFromPipeline();
//	pvbGPU->unbindFromPipeline(&curEffect);
//
//	setTextureAction.unbindFromPipeline(&curEffect);
//	objSa.unbindFromPipeline(&curEffect);
//}
//
//void EffectManager::drawFirstGlowPass()
//{
//	Effect &curEffect = *m_hFirstGlowPassEffect.getObject<Effect>();
//
//	if (!curEffect.m_isReady)
//		return;
//	
//	m_pContext->getGPUScreen()->setRenderTargetsAndViewportWithNoDepth(&m_2ndGlowTargetTextureGPU, true);
//
//	m_pCurRenderTarget = &m_2ndGlowTargetTextureGPU;
//
//	IndexBufferGPU *pibGPU = m_hIndexBufferGPU.getObject<IndexBufferGPU>();
//	pibGPU->setAsCurrent();
//	
//	VertexBufferGPU *pvbGPU = m_hVertexBufferGPU.getObject<VertexBufferGPU>();
//	pvbGPU->setAsCurrent(&curEffect);
//
//	curEffect.setCurrent(pvbGPU);
//
//	PE::SA_Bind_Resource setTextureAction(*m_pContext, m_arena, DIFFUSE_TEXTURE_2D_SAMPLER_SLOT, m_glowSeparatedTextureGPU.m_samplerState, API_CHOOSE_DX11_DX9_OGL(m_glowSeparatedTextureGPU.m_pShaderResourceView, m_glowSeparatedTextureGPU.m_pTexture, m_glowSeparatedTextureGPU.m_texture));
//	setTextureAction.bindToPipeline(&curEffect);
//
//	PE::SetPerObjectConstantsShaderAction objSa;
//	objSa.m_data.gW = Matrix4x4();
//	objSa.m_data.gW.loadIdentity();
//	objSa.m_data.gWVP = objSa.m_data.gW;
//
//	objSa.bindToPipeline(&curEffect);
//
//	pibGPU->draw(1, 0);
//
//	pibGPU->unbindFromPipeline();
//	pvbGPU->unbindFromPipeline(&curEffect);
//
//	setTextureAction.unbindFromPipeline(&curEffect);
//	objSa.unbindFromPipeline(&curEffect);
//}
//
//void EffectManager::drawSecondGlowPass()
//{
//	Effect &curEffect = *m_hSecondGlowPassEffect.getObject<Effect>();
//	if (!curEffect.m_isReady)
//		return;
//	
//	m_pContext->getGPUScreen()->setRenderTargetsAndViewportWithNoDepth(m_hFinishedGlowTargetTextureGPU.getObject<TextureGPU>(), true);
//	
//	m_pCurRenderTarget = m_hFinishedGlowTargetTextureGPU.getObject<TextureGPU>();
//	
//	IndexBufferGPU *pibGPU = m_hIndexBufferGPU.getObject<IndexBufferGPU>();
//	pibGPU->setAsCurrent();
//	
//	VertexBufferGPU *pvbGPU = m_hVertexBufferGPU.getObject<VertexBufferGPU>();
//	pvbGPU->setAsCurrent(&curEffect);
//
//	curEffect.setCurrent(pvbGPU);
//
//	PE::SA_Bind_Resource setBlurTextureAction(*m_pContext, m_arena, DIFFUSE_BLUR_TEXTURE_2D_SAMPLER_SLOT, m_2ndGlowTargetTextureGPU.m_samplerState, API_CHOOSE_DX11_DX9_OGL(m_2ndGlowTargetTextureGPU.m_pShaderResourceView, m_2ndGlowTargetTextureGPU.m_pTexture, m_2ndGlowTargetTextureGPU.m_texture));
//	setBlurTextureAction.bindToPipeline(&curEffect);
//
//	TextureGPU *t = /*&mDeferredLightingResult;*/ m_hGlowTargetTextureGPU.getObject<TextureGPU>();
//	PE::SA_Bind_Resource setTextureAction(*m_pContext, m_arena, DIFFUSE_TEXTURE_2D_SAMPLER_SLOT, t->m_samplerState, API_CHOOSE_DX11_DX9_OGL(t->m_pShaderResourceView, t->m_pTexture, t->m_texture));
//	setTextureAction.bindToPipeline(&curEffect);
//
//	PE::SetPerObjectConstantsShaderAction objSa;
//	objSa.m_data.gW = Matrix4x4();
//	objSa.m_data.gW.loadIdentity();
//	objSa.m_data.gWVP = objSa.m_data.gW;
//
//	objSa.bindToPipeline(&curEffect);
//
//
//	// the effect blurs vertically the horizontally blurred glow and adds it to source
//	pibGPU->draw(1, 0);
//
//	pibGPU->unbindFromPipeline();
//	pvbGPU->unbindFromPipeline(&curEffect);
//
//	setBlurTextureAction.unbindFromPipeline(&curEffect);
//	setTextureAction.unbindFromPipeline(&curEffect);
//	objSa.unbindFromPipeline(&curEffect);
//}
//
//void EffectManager::drawMotionBlur()
//{
//	Effect &curEffect = *m_hMotionBlurEffect.getObject<Effect>();
//	if (!curEffect.m_isReady)
//		return;
//	
//	m_pContext->getGPUScreen()->setRenderTargetsAndViewportWithNoDepth(0, true);
//
//	IndexBufferGPU *pibGPU = m_hIndexBufferGPU.getObject<IndexBufferGPU>();
//	pibGPU->setAsCurrent();
//	
//	VertexBufferGPU *pvbGPU = m_hVertexBufferGPU.getObject<VertexBufferGPU>();
//	pvbGPU->setAsCurrent(&curEffect);
//
//	curEffect.setCurrent(pvbGPU);
//
//	TextureGPU *t = m_hFinishedGlowTargetTextureGPU.getObject<TextureGPU>();
//	PE::SA_Bind_Resource setTextureAction(*m_pContext, m_arena, DIFFUSE_TEXTURE_2D_SAMPLER_SLOT,t->m_samplerState, API_CHOOSE_DX11_DX9_OGL(t->m_pShaderResourceView, t->m_pTexture, t->m_texture));
//	setTextureAction.bindToPipeline(&curEffect);
//
//	//todo: enable this to get motion blur working. need to make shader use depth map, not shadow map
//	//PE::SA_Bind_Resource setDepthTextureAction(DEPTHMAP_TEXTURE_2D_SAMPLER_SLOT, API_CHOOSE_DX11_DX9_OGL(m_hGlowTargetTextureGPU.getObject<TextureGPU>()->m_pShaderResourceView, m_hGlowTargetTextureGPU.getObject<TextureGPU>()->m_pTexture, m_hGlowTargetTextureGPU.getObject<TextureGPU>()->m_texture));
//	//setDepthTextureAction.bindToPipeline(&curEffect);
//
//	SetPerObjectGroupConstantsShaderAction cb(*m_pContext, m_arena);
//	cb.m_data.gPreviousViewProjMatrix = m_previousViewProjMatrix;
//	cb.m_data.gViewProjInverseMatrix = m_currentViewProjMatrix.inverse();
//	cb.m_data.gDoMotionBlur = m_doMotionBlur;
//
//	cb.bindToPipeline();
//
//	PE::SetPerObjectConstantsShaderAction objSa;
//	objSa.m_data.gW = Matrix4x4();
//	objSa.m_data.gW.loadIdentity();
//	objSa.m_data.gWVP = objSa.m_data.gW;
//
//	objSa.bindToPipeline(&curEffect);
//
//
//	pibGPU->draw(1, 0);
//	m_previousViewProjMatrix = m_currentViewProjMatrix;
//
//	pibGPU->unbindFromPipeline();
//	pvbGPU->unbindFromPipeline(&curEffect);
//
//	setTextureAction.unbindFromPipeline(&curEffect);
//	cb.unbindFromPipeline(&curEffect);
//	objSa.unbindFromPipeline(&curEffect);
//	//todo: enable this to get motion blur working
//	//setDepthTextureAction.unbindFromPipeline(&curEffect);
//}
//
//void EffectManager::drawFrameBufferCopy()
//{
//	Effect &curEffect = *m_hMotionBlurEffect.getObject<Effect>();
//
//	if (!curEffect.m_isReady)
//		return;
//
//	setFrameBufferCopyRenderTarget();
//
//	IndexBufferGPU *pibGPU = m_hIndexBufferGPU.getObject<IndexBufferGPU>();
//	pibGPU->setAsCurrent();
//	
//	VertexBufferGPU *pvbGPU = m_hVertexBufferGPU.getObject<VertexBufferGPU>();
//	pvbGPU->setAsCurrent(&curEffect);
//
//	curEffect.setCurrent(pvbGPU);
//
//	TextureGPU *t = m_hFinishedGlowTargetTextureGPU.getObject<TextureGPU>();
//	PE::SA_Bind_Resource setTextureAction(*m_pContext, m_arena, DIFFUSE_TEXTURE_2D_SAMPLER_SLOT, t->m_samplerState, API_CHOOSE_DX11_DX9_OGL(t->m_pShaderResourceView, t->m_pTexture, t->m_texture));
//	setTextureAction.bindToPipeline(&curEffect);
//
//	SetPerObjectGroupConstantsShaderAction cb(*m_pContext, m_arena);
//	cb.m_data.gPreviousViewProjMatrix = m_previousViewProjMatrix;
//	cb.m_data.gViewProjInverseMatrix = m_currentViewProjMatrix.inverse();
//	cb.m_data.gDoMotionBlur = m_doMotionBlur;
//
//	cb.bindToPipeline();
//
//	pibGPU->draw(1, 0);
//	m_previousViewProjMatrix = m_currentViewProjMatrix;
//
//	pibGPU->unbindFromPipeline();
//	pvbGPU->unbindFromPipeline(&curEffect);
//
//	setTextureAction.unbindFromPipeline(&curEffect);
//	}

void EffectManager::drawDeferredLightingPass()
{
	GraphicsDriver* gDriver = GraphicsDriver::Instance();
	gDriver->UnbindResourceFromSlot(6);
	
	mCurRenderTarget = mDeferredLightingResult;
	gDriver->SetRenderTargetAndViewportWithNoDepth(mDeferredLightingResult, true);
	Effect* curEffect;
	
	// Bind G-Buffer textures as SRVs
	gDriver->BindTextureResource(mGBufferPositionTexture, 2);
	gDriver->BindTextureResource(mGBufferNormalsTexture, 3);
	gDriver->BindTextureResource(mGBufferDiffuseTexture, 4);

	//// Bind shadow map as SRV
	////SA_Bind_Resource setShadowMapSA(*m_pContext, m_arena, SHADOWMAP_TEXTURE_2D_SAMPLER_SLOT, m_shadowMapDepthTexture.m_samplerState, API_CHOOSE_DX11_DX9_OGL_PSVITA(m_shadowMapDepthTexture.m_pDepthShaderResourceView, m_shadowMapDepthTexture.m_pTexture, m_shadowMapDepthTexture.m_texture, m_shadowMapDepthTexture.m_texture));
	////setShadowMapSA.bindToPipeline(&curEffect);

	// Point lights
	std::vector<PointLightComponent*> mPLs = gRenderer->GetPointLights();
	if(mPLs.size() > 0)
	{
		mLightVolumeSphere->GetVertexData()->SetActive();

		//// Draw each light light volume
		curEffect = GetEffect("DeferredPointLightingTech");
		curEffect->SetActive();
		GraphicsDriver::Instance()->DrawInstanced(mLightVolumeSphere->GetVertexData(), mPLs.size());
		//GraphicsDriver::Instance()->DrawVertexArray(mLightVolumeSphere->GetVertexData());
	}

	// Spot lights
	std::vector<SpotLightComponent*> mSLs = gRenderer->GetSpotLights();
	if (mSLs.size() > 0)
	{
		mLightVolumeSphere->GetVertexData()->SetActive();

		//// Draw each light light volume
		curEffect = GetEffect("DeferredSpotLightingTech");
		curEffect->SetActive();
		GraphicsDriver::Instance()->DrawInstanced(mLightVolumeSphere->GetVertexData(), mSLs.size());
	}

	// Directional lights
	std::vector<DirectionalLightComponent*> mDLs = gRenderer->GetDirectionalLights();
	if (mDLs.size() > 0)
	{
		mQuadVertexBuffer->SetActive();

		//// Draw each light light volume
		curEffect = GetEffect("DeferredDirectionalLightingTech");
		curEffect->SetActive();
		//GraphicsDriver::Instance()->DrawInstanced(mQuadVertexBuffer, mDLs.size());
		GraphicsDriver::Instance()->DrawVertexArray(mQuadVertexBuffer);
	}

	// Unbind GBuffer
	gDriver->UnbindResourceFromSlot(1);
	gDriver->UnbindResourceFromSlot(2);
	gDriver->UnbindResourceFromSlot(3);
	gDriver->UnbindResourceFromSlot(4);
}

void EffectManager::DrawVoxelGrid()
{
	Effect &curEffect = *mVoxelGridEffect;
	GraphicsDriver* gDriver = GraphicsDriver::Instance();

	//gDriver->SetRenderTargetAndViewportWithNoDepth(0, true);

	mQuadVertexBuffer->SetActive();
	curEffect.SetActive();

	// Rebind frame and lighting data to GPU
	//DrawList::InstanceReadOnly()->BindGlobalShaderConstants(&curEffect);
	
	// Bind G-Buffer textures as SRVs
	gDriver->BindTextureResource(mGBufferPositionTexture, 6);

	gDriver->BindTextureResource(mDeferredLightingResult, 2);

	gDriver->SetRenderTargetsAndUAVNoDepth(0, mVoxelGridStructuredBuffer, 1027);

	
	GraphicsDriver::Instance()->DrawVertexArray(mQuadVertexBuffer);

	// Unbind all constants and buffers
	gDriver->UnbindResourceFromSlot(6);
	gDriver->UnbindResourceFromSlot(2);
}

void EffectManager::ClearVoxelGrid()
{
	Effect &curEffect = *mVoxelGridClearComputeEffect;
	curEffect.SetActive(); 
	GraphicsDriver* gDriver = GraphicsDriver::Instance();
	ID3D11DeviceContext *pDeviceContext = gDriver->GetDeviceContext();

	/*PE::SA_Bind_Resource setVoxelGridAction(*m_pContext, m_arena);
	setVoxelGridAction.set(
		GpuResourceSlot_ParticleSpawns_ConsumeBuffer,
		mVoxelGridStructuredBuffer.getObject<ResourceBufferGPU>()->m_resourceBufferLowLevel.m_pShaderResourceUAV, 
		NULL,
		NULL, 
		0);
	setVoxelGridAction.bindToPipeline(&curEffect);*/
	gDriver->BindResourceBuffer(mVoxelGridStructuredBuffer, 1024);

	B_ASSERT(VoxelGrid::s_voxelGridDimension % 8 == 0, "ERROR: voxel grid must be a multiple of 8!");
	UINT32 dispatchDimension = VoxelGrid::s_voxelGridDimension / 8;

	// Clear voxel grid, group size is 8x8x8. Each thread clears a voxel
	pDeviceContext->Dispatch(dispatchDimension, dispatchDimension, dispatchDimension);

	//setVoxelGridAction.unbindFromPipeline(&curEffect);
	gDriver->UnbindResourceFromSlot(1024);
}

void EffectManager::DrawVPLCreationForAllLights()
{
	if (mNumVPLPropogations < 2)
		return;
	
	Effect &curEffect = *mVPLCreateEffect;


	// Set 3 2DTextureArrays as render targets and bind Voxel Grid to register 3
	//GraphicsDriver* gDriver = GraphicsDriver::Instance();
	gDriver->SetMultiRenderTargetsAndUAVNoDepth(mVPL_RedSHCoefficients0,
												mVPL_GreenSHCoefficients0, 
												mVPL_BlueSHCoefficients0, 
												mVoxelGridStructuredBuffer, 
												true);

	mQuadVertexBuffer->SetActive();
	curEffect.SetActive();
	
	//SA_Bind_Resource setTextureAction_posBuffer(*m_pContext, m_arena, DEPTHMAP_TEXTURE_2D_SAMPLER_SLOT, mGBufferPositionTexture.m_samplerState, API_CHOOSE_DX11_DX9_OGL(mGBufferPositionTexture.m_pShaderResourceView, mGBufferPositionTexture.m_pTexture, mGBufferPositionTexture.m_texture));
	//setTextureAction_posBuffer.bindToPipeline(&curEffect);
	gDriver->BindTextureResource(mGBufferPositionTexture, 0);

	// Bind shadow map as SRV
	//SA_Bind_Resource setShadowMapSA(*m_pContext, m_arena, SHADOWMAP_TEXTURE_2D_SAMPLER_SLOT, m_shadowMapDepthTexture.m_samplerState, API_CHOOSE_DX11_DX9_OGL_PSVITA(m_shadowMapDepthTexture.m_pDepthShaderResourceView, m_shadowMapDepthTexture.m_pTexture, m_shadowMapDepthTexture.m_texture, m_shadowMapDepthTexture.m_texture));
	//setShadowMapSA.bindToPipeline(&curEffect); 
	gDriver->BindTextureResource(mShadowMapDepthTexture, 1, true);

	// Rebind frame and lighting data to GPU
	//DrawList::InstanceReadOnly()->BindGlobalShaderConstants(&curEffect);	// Already bound from deferred lighting?

	// Get total number of lights
	ID3D11Buffer* iLightBuf = gRenderer->GetSingleLightBuffer();

	// Draw each light light volume
	int numLights = gRenderer->GetPointLights().size() + gRenderer->GetSpotLights().size() + gRenderer->GetDirectionalLights().size();
	for (int i = 0; i < numLights; ++i)
	{
		/*Light* pLight = RootSceneNode::Instance()->m_lights[i].getObject<Light>();

		SetPerLightConstantsShaderAction lightSA(*m_pContext, m_arena);
		lightSA.m_data.gLightCurrent = pLight->m_cbuffer;
		lightSA.bindToPipeline(&curEffect);*/
		SingleLightConstant lightConst;
		lightConst.gLightCurrent = gRenderer->GetLights()[i]->GetLightData();

		
		// Map the data for GPU use
		gDriver->UploadBuffer(iLightBuf, (void*)&lightConst, sizeof(SingleLightConstant));

		// Set buffer for GPU
		//mGraphicsDriver->SetVSConstantBuffer(mLightConstBuffer, 2);
		//mGraphicsDriver->SetPSConstantBuffer(mLightConstBuffer, 2);
		gDriver->SetConstantBuffer(iLightBuf, PerLightSlot);

		gDriver->DrawInstanced(mQuadVertexBuffer, VoxelGrid::s_voxelGridDimension);

		//lightSA.unbindFromPipeline(&curEffect);
	}

	// Unbind all constants and buffers
	//pibGPU->unbindFromPipeline();
	//pvbGPU->unbindFromPipeline(&curEffect);
	gDriver->UnbindResourceFromSlot(0);
	gDriver->UnbindResourceFromSlot(1);

	// Unbind render targets
	ID3D11RenderTargetView *renderTargets[3] = { 0 };
	gDriver->GetDeviceContext()->OMSetRenderTargets(3, renderTargets, 0);

}

void EffectManager::BindVPLForPropogation(TextureGPU* texture, /*SA_Bind_Resource& bindresource,*/ UINT32 bindSlot)
{
	texture->SetBindSlot(bindSlot);
	gDriver->BindTextureResource(texture, bindSlot);

	//if (bindSlot < 1024)
	//{
	//	//bindresource.set(bindSlot, texture->m_samplerState, texture->m_pShaderResourceView);
	//	//bindresource.bindToPipeline(NULL);
	//	gDriver->BindTextureResource(texture, bindSlot);
	//}
	//else
	//{
	//	//bindresource.set(bindSlot, texture->m_pUnorderedAccessView, NULL, NULL, 0, "",texture->m_samplerState);
	//	//bindresource.bindToPipeline(NULL);
	//	gDriver->BindTe
	//}
}

void EffectManager::SwapVPLPropogationSRV_UAV(TextureGPU* bindResources[6], UINT16 swapType)
{
	if (swapType == 0)
	{
		BindVPLForPropogation(mVPL_RedSHCoefficients0, DEPTHMAP_TEXTURE_2D_SAMPLER_SLOT);
		BindVPLForPropogation(mVPL_GreenSHCoefficients0, SHADOWMAP_TEXTURE_2D_SAMPLER_SLOT);
		BindVPLForPropogation(mVPL_BlueSHCoefficients0, DIFFUSE_TEXTURE_2D_ARRAY_SLOT);
		BindVPLForPropogation(mVPL_RedSHCoefficients1, GpuResourceSlot_FirstUAV);
		BindVPLForPropogation(mVPL_GreenSHCoefficients1, GpuResourceSlot_SecondUAV);
		BindVPLForPropogation(mVPL_BlueSHCoefficients1, GpuResourceSlot_ThirdUAV);
	}
	else
	{
		BindVPLForPropogation(mVPL_RedSHCoefficients0, GpuResourceSlot_FirstUAV);
		BindVPLForPropogation(mVPL_GreenSHCoefficients0, GpuResourceSlot_SecondUAV);
		BindVPLForPropogation(mVPL_BlueSHCoefficients0, GpuResourceSlot_ThirdUAV);
		BindVPLForPropogation(mVPL_RedSHCoefficients1, DEPTHMAP_TEXTURE_2D_SAMPLER_SLOT);
		BindVPLForPropogation(mVPL_GreenSHCoefficients1, SHADOWMAP_TEXTURE_2D_SAMPLER_SLOT);
		BindVPLForPropogation(mVPL_BlueSHCoefficients1, DIFFUSE_TEXTURE_2D_ARRAY_SLOT);
	}
}

void EffectManager::PropogateVPLsAcrossVoxelGrid()
{
	//GraphicsDriver* gDriver = GraphicsDriver::Instance();

	if (mNumVPLPropogations < 2)
		return;

	Effect &curEffect = *mVPLPropogationNoOccEffect;
	curEffect.SetActive();

	// Bind Voxel grid as UAV
	ResourceBufferGPU* voxelGrid = mVoxelGridStructuredBuffer;
	gDriver->BindResourceBuffer(mVoxelGridStructuredBuffer, 3);
	/*PE::SA_Bind_Resource setVoxelGrid_SRV(*m_pContext, m_arena, 
		GLOW_TEXTURE_2D_SAMPLER_SLOT,
		voxelGrid->m_resourceBufferLowLevel.m_samplerState, 
		voxelGrid->m_resourceBufferLowLevel.m_pShaderResourceView
	);
	setVoxelGrid_SRV.bindToPipeline(&curEffect);*/
	
	// Set up dispath dimensions
	ID3D11DeviceContext *pDeviceContext = gDriver->GetDeviceContext();
	BZ_ASSERT(VoxelGrid::s_voxelGridDimension % 8 == 0, "ERROR: voxel grid must be a multiple of 8!");
	UINT32 dispatchDimension = VoxelGrid::s_voxelGridDimension / 8;

	//*********************************************
	// Do first propogation with no occlusion

	// PerFrameCB is already bound with 0 for useOcclusion

	// Bind all SRVs and UAVs
	//SA_Bind_Resource setRedVPL_SRV(*m_pContext, m_arena);
	//SA_Bind_Resource setGreenVPL_SRV(*m_pContext, m_arena);
	//SA_Bind_Resource setBlueVPL_SRV(*m_pContext, m_arena);
	//SA_Bind_Resource setRedVPL_UAV(*m_pContext, m_arena);
	//SA_Bind_Resource setGreenVPL_UAV(*m_pContext, m_arena);
	//SA_Bind_Resource setBlueVPL_UAV(*m_pContext, m_arena);

	TextureGPU* bindResources[6];
	bindResources[0] = mVPL_RedSHCoefficients0;		//&setRedVPL_SRV;
	bindResources[1] = mVPL_GreenSHCoefficients0;	//&setGreenVPL_SRV;
	bindResources[2] = mVPL_BlueSHCoefficients0;	//&setBlueVPL_SRV;
	bindResources[3] = mVPL_RedSHCoefficients1;		//&setRedVPL_UAV;
	bindResources[4] = mVPL_GreenSHCoefficients1;	//&setGreenVPL_UAV;
	bindResources[5] = mVPL_BlueSHCoefficients1;	//&setBlueVPL_UAV;
	SwapVPLPropogationSRV_UAV(bindResources, 0);

	// Propogate VPLs through grid, group size is 8x8x8. Each thread does calculations for 1 voxel
	pDeviceContext->Dispatch(dispatchDimension, dispatchDimension, dispatchDimension);

	// unbind all from pipeline so that we can rebind on next loop
	for (int i = 0; i < 6; ++i)
		gDriver->UnbindResourceFromSlot(bindResources[i]->GetBindSlot());
		//bindResources[i]->unbindFromPipeline(&curEffect);


	//*********************************************
	// Do iterative propogations with occlusion
	Effect &WithOccEffect = *mVPLPropogationWithOccEffect;
	WithOccEffect.SetActive();
	//mGame->GetRenderer().GetCamConsts()->gUseOcclusion = 1;
	//mGame->GetRenderer().UploadCameraConstants();
	
	// Iteratively propogate VPLs with Occlusion in mind
	for (int i = 0; i < mNumVPLPropogations; ++i)
	{
		SwapVPLPropogationSRV_UAV(bindResources, (i + 1) % 2); // start at "swap == 1", want to end with 0 being the final

		// Propogate VPLs through grid, group size is 8x8x8. Each thread does calculations for 1 voxel
		pDeviceContext->Dispatch(dispatchDimension, dispatchDimension, dispatchDimension);

		// unbind all from pipeline so that we can rebind on next loop
		for (int i = 0; i < 6; ++i)
			gDriver->UnbindResourceFromSlot(bindResources[i]->GetBindSlot());
	}
	
	//setVoxelGrid_SRV.unbindFromPipeline(&curEffect);
	gDriver->UnbindResourceFromSlot(3);
}

void EffectManager::ApplyIndirectIlluminationVoxelGI()
{
	
	Effect &curEffect = *mVoxelGIIndirectIlluminationApply;

	mQuadVertexBuffer->SetActive();
	curEffect.SetActive();

	// Set render target
	mCurRenderTarget = mVoxelGIFinalTexture;

	if (mNumVPLPropogations < 2)
		return;

	gDriver->SetRenderTargetAndViewportWithNoDepth(mVoxelGIFinalTexture, true);

	// Bind G-Buffer textures as SRVs
	gDriver->BindTextureResource(mDeferredLightingResult, ADDITIONAL_DIFFUSE_TEXTURE_2D_SAMPLER_SLOT);
	gDriver->BindTextureResource(mGBufferPositionTexture, DEPTHMAP_TEXTURE_2D_SAMPLER_SLOT);
	gDriver->BindTextureResource(mGBufferDiffuseTexture, SHADOWMAP_TEXTURE_2D_SAMPLER_SLOT);
	gDriver->BindTextureResource(mGBufferNormalsTexture, DIFFUSE_TEXTURE_2D_SAMPLER_SLOT);
	/*PE::SA_Bind_Resource setTextureAction_lightBuffer(*m_pContext, m_arena, ADDITIONAL_DIFFUSE_TEXTURE_2D_SAMPLER_SLOT, deferredRenderTarget->m_samplerState, deferredRenderTarget->m_pShaderResourceView);
	PE::SA_Bind_Resource setTextureAction_colBuffer(*m_pContext, m_arena, SHADOWMAP_TEXTURE_2D_SAMPLER_SLOT, mGBufferDiffuseTexture.m_samplerState, mGBufferDiffuseTexture.m_pShaderResourceView);
	PE::SA_Bind_Resource setTextureAction_posBuffer(*m_pContext, m_arena, DEPTHMAP_TEXTURE_2D_SAMPLER_SLOT, mGBufferPositionTexture.m_samplerState, API_CHOOSE_DX11_DX9_OGL(mGBufferPositionTexture.m_pShaderResourceView, mGBufferPositionTexture.m_pTexture, mGBufferPositionTexture.m_texture));
	PE::SA_Bind_Resource setTextureAction_normBuffer(*m_pContext, m_arena, DIFFUSE_TEXTURE_2D_SAMPLER_SLOT, mGBufferNormalsTexture.m_samplerState, API_CHOOSE_DX11_DX9_OGL(mGBufferNormalsTexture.m_pShaderResourceView, mGBufferNormalsTexture.m_pTexture, mGBufferNormalsTexture.m_texture));
	setTextureAction_lightBuffer.bindToPipeline(&curEffect);
	setTextureAction_colBuffer.bindToPipeline(&curEffect);
	setTextureAction_posBuffer.bindToPipeline(&curEffect);
	setTextureAction_normBuffer.bindToPipeline(&curEffect);*/

	// Bind 2DTextureArrays from the result of VPL propogation
	gDriver->BindTextureResource(mVPL_RedSHCoefficients1, GLOW_TEXTURE_2D_SAMPLER_SLOT);
	gDriver->BindTextureResource(mVPL_GreenSHCoefficients1, BUMPMAP_TEXTURE_2D_SAMPLER_SLOT);
	gDriver->BindTextureResource(mVPL_BlueSHCoefficients1, SPECULAR_TEXTURE_2D_SAMPLER_SLOT);
	/*PE::SA_Bind_Resource setTextureAction_redSHCoeff(*m_pContext, m_arena, GLOW_TEXTURE_2D_SAMPLER_SLOT, mVPL_RedSHCoefficients1.m_samplerState, API_CHOOSE_DX11_DX9_OGL(mVPL_RedSHCoefficients1.m_pShaderResourceView, mVPL_RedSHCoefficients1.m_pTexture, mVPL_RedSHCoefficients1.m_texture));
	PE::SA_Bind_Resource setTextureAction_greenSHCoeff(*m_pContext, m_arena, BUMPMAP_TEXTURE_2D_SAMPLER_SLOT, mVPL_GreenSHCoefficients1.m_samplerState, API_CHOOSE_DX11_DX9_OGL(mVPL_GreenSHCoefficients1.m_pShaderResourceView, mVPL_GreenSHCoefficients1.m_pTexture, mVPL_GreenSHCoefficients1.m_texture));
	PE::SA_Bind_Resource setTextureAction_blueSHCoeff(*m_pContext, m_arena, SPECULAR_TEXTURE_2D_SAMPLER_SLOT, mVPL_BlueSHCoefficients1.m_samplerState, API_CHOOSE_DX11_DX9_OGL(mVPL_BlueSHCoefficients1.m_pShaderResourceView, mVPL_BlueSHCoefficients1.m_pTexture, mVPL_BlueSHCoefficients1.m_texture));
	bindVPLForPropogation(&mVPL_RedSHCoefficients1, setTextureAction_redSHCoeff, GLOW_TEXTURE_2D_SAMPLER_SLOT);
	bindVPLForPropogation(&mVPL_GreenSHCoefficients1, setTextureAction_greenSHCoeff, BUMPMAP_TEXTURE_2D_SAMPLER_SLOT);
	bindVPLForPropogation(&mVPL_BlueSHCoefficients1, setTextureAction_blueSHCoeff, SPECULAR_TEXTURE_2D_SAMPLER_SLOT);*/


	gDriver->DrawVertexArray(mQuadVertexBuffer);

	// Unbind resources
	gDriver->UnbindResourceFromSlot(0);
	gDriver->UnbindResourceFromSlot(1);
	gDriver->UnbindResourceFromSlot(2);
	gDriver->UnbindResourceFromSlot(3);
	gDriver->UnbindResourceFromSlot(4);
	gDriver->UnbindResourceFromSlot(5);
	gDriver->UnbindResourceFromSlot(6);
}

//void EffectManager::debugDrawVPLResult()
//{
//	Effect &curEffect = *getEffectHandle("VPL_Propagation_Debug").getObject<Effect>();;// *mVoxelGIIndirectIlluminationApply.getObject<Effect>();
//
//	IndexBufferGPU *pibGPU = m_hIndexBufferGPU.getObject<IndexBufferGPU>();
//	pibGPU->setAsCurrent();
//	VertexBufferGPU *pvbGPU = m_hVertexBufferGPU.getObject<VertexBufferGPU>();
//	pvbGPU->setAsCurrent(&curEffect);
//	curEffect.setCurrent(pvbGPU);
//
//	// Set render target
//	m_pContext->getGPUScreen()->setRenderTargetsAndViewportWithNoDepth(0, true);
//
//	// Bind G-Buffer textures as SRVs
//	TextureGPU* deferredRenderTarget = &mDeferredLightingResult;// .getObject<TextureGPU>();
//	PE::SA_Bind_Resource setTextureAction_colBuffer(*m_pContext, m_arena, SHADOWMAP_TEXTURE_2D_SAMPLER_SLOT, deferredRenderTarget->m_samplerState, deferredRenderTarget->m_pShaderResourceView);
//	PE::SA_Bind_Resource setTextureAction_posBuffer(*m_pContext, m_arena, DEPTHMAP_TEXTURE_2D_SAMPLER_SLOT, mGBufferPositionTexture.m_samplerState, API_CHOOSE_DX11_DX9_OGL(mGBufferPositionTexture.m_pShaderResourceView, mGBufferPositionTexture.m_pTexture, mGBufferPositionTexture.m_texture));
//	PE::SA_Bind_Resource setTextureAction_normBuffer(*m_pContext, m_arena, DIFFUSE_TEXTURE_2D_SAMPLER_SLOT, mGBufferNormalsTexture.m_samplerState, API_CHOOSE_DX11_DX9_OGL(mGBufferNormalsTexture.m_pShaderResourceView, mGBufferNormalsTexture.m_pTexture, mGBufferNormalsTexture.m_texture));
//	setTextureAction_colBuffer.bindToPipeline(&curEffect);
//	setTextureAction_posBuffer.bindToPipeline(&curEffect);
//	setTextureAction_normBuffer.bindToPipeline(&curEffect);
//
//	// Bind 2DTextureArrays from the result of VPL propogation
//	PE::SA_Bind_Resource setTextureAction_redSHCoeff(*m_pContext, m_arena, GLOW_TEXTURE_2D_SAMPLER_SLOT, mVPL_RedSHCoefficients1.m_samplerState, API_CHOOSE_DX11_DX9_OGL(mVPL_RedSHCoefficients1.m_pShaderResourceView, mVPL_RedSHCoefficients1.m_pTexture, mVPL_RedSHCoefficients1.m_texture));
//	PE::SA_Bind_Resource setTextureAction_greenSHCoeff(*m_pContext, m_arena, BUMPMAP_TEXTURE_2D_SAMPLER_SLOT, mVPL_GreenSHCoefficients1.m_samplerState, API_CHOOSE_DX11_DX9_OGL(mVPL_GreenSHCoefficients1.m_pShaderResourceView, mVPL_GreenSHCoefficients1.m_pTexture, mVPL_GreenSHCoefficients1.m_texture));
//	PE::SA_Bind_Resource setTextureAction_blueSHCoeff(*m_pContext, m_arena, SPECULAR_TEXTURE_2D_SAMPLER_SLOT, mVPL_BlueSHCoefficients1.m_samplerState, API_CHOOSE_DX11_DX9_OGL(mVPL_BlueSHCoefficients1.m_pShaderResourceView, mVPL_BlueSHCoefficients1.m_pTexture, mVPL_BlueSHCoefficients1.m_texture));
//	bindVPLForPropogation(&mVPL_RedSHCoefficients1, setTextureAction_redSHCoeff, GLOW_TEXTURE_2D_SAMPLER_SLOT);
//	bindVPLForPropogation(&mVPL_GreenSHCoefficients1, setTextureAction_greenSHCoeff, BUMPMAP_TEXTURE_2D_SAMPLER_SLOT);
//	bindVPLForPropogation(&mVPL_BlueSHCoefficients1, setTextureAction_blueSHCoeff, SPECULAR_TEXTURE_2D_SAMPLER_SLOT);
//
//	pibGPU->draw(1, 0);
//
//	// Unbind resources
//	setTextureAction_posBuffer.unbindFromPipeline(&curEffect);
//	setTextureAction_colBuffer.unbindFromPipeline(&curEffect);
//	setTextureAction_normBuffer.unbindFromPipeline(&curEffect);
//	setTextureAction_redSHCoeff.unbindFromPipeline(&curEffect);
//	setTextureAction_greenSHCoeff.unbindFromPipeline(&curEffect);
//	setTextureAction_blueSHCoeff.unbindFromPipeline(&curEffect);
//}

//Effect* EffectManager::operator[] (const char *pEffectFilename)
//{
//	//return m_map.findHandle(pEffectFilename).getObject<Effect>();
//	return GetEffect(pEffectFilename);
//}
//

void EffectManager::DebugDrawRenderTarget(/*bool drawGlowRenderTarget, bool drawSeparatedGlow, bool drawGlow1stPass, bool drawGlow2ndPass,*/ bool drawShadowRenderTarget, 
											bool drawDeferredPositionTex, bool drawDeferredNormalTex, bool drawDeferredDiffuseTex, bool drawDeferredLightResult)
{
	// use motion blur for now since it doesnt do anything but draws the surface
	Effect *curEffect = GetEffect("BasicQuadTech");
	curEffect->SetActive();

	GraphicsDriver* gDriver = GraphicsDriver::Instance();
	gDriver->SetRenderTargetAndViewportWithNoDepth(0, true);

	mQuadVertexBuffer->SetActive();

	/*if (drawGlowRenderTarget)
		setTextureAction.set(DIFFUSE_TEXTURE_2D_SAMPLER_SLOT, m_hGlowTargetTextureGPU.getObject<TextureGPU>()->m_samplerState,  API_CHOOSE_DX11_DX9_OGL(m_hGlowTargetTextureGPU.getObject<TextureGPU>()->m_pShaderResourceView, m_hGlowTargetTextureGPU.getObject<TextureGPU>()->m_pTexture, m_hGlowTargetTextureGPU.getObject<TextureGPU>()->m_texture));
	if (drawSeparatedGlow)
		setTextureAction.set(DIFFUSE_TEXTURE_2D_SAMPLER_SLOT, m_glowSeparatedTextureGPU.m_samplerState, API_CHOOSE_DX11_DX9_OGL(m_glowSeparatedTextureGPU.m_pShaderResourceView, m_glowSeparatedTextureGPU.m_pTexture, m_glowSeparatedTextureGPU.m_texture));
	if (drawGlow1stPass)
		setTextureAction.set(DIFFUSE_TEXTURE_2D_SAMPLER_SLOT, m_2ndGlowTargetTextureGPU.m_samplerState, API_CHOOSE_DX11_DX9_OGL(m_2ndGlowTargetTextureGPU.m_pShaderResourceView, m_2ndGlowTargetTextureGPU.m_pTexture, m_2ndGlowTargetTextureGPU.m_texture));
	if (drawGlow2ndPass)
		setTextureAction.set(DIFFUSE_TEXTURE_2D_SAMPLER_SLOT, m_hFinishedGlowTargetTextureGPU.getObject<TextureGPU>()->m_samplerState, API_CHOOSE_DX11_DX9_OGL(m_hFinishedGlowTargetTextureGPU.getObject<TextureGPU>()->m_pShaderResourceView, m_hFinishedGlowTargetTextureGPU.getObject<TextureGPU>()->m_pTexture, m_hFinishedGlowTargetTextureGPU.getObject<TextureGPU>()->m_texture));
	*/if (drawShadowRenderTarget)
		SetShadowMapShaderValue();
	if (drawDeferredPositionTex)
		gDriver->BindTextureResource(mGBufferPositionTexture, 1);
	if (drawDeferredNormalTex)
		gDriver->BindTextureResource(mGBufferNormalsTexture, 1);
	if (drawDeferredDiffuseTex)
		gDriver->BindTextureResource(mGBufferDiffuseTexture, 1);
	if(drawDeferredLightResult)
		gDriver->BindTextureResource(mDeferredLightingResult, 1);


	gDriver->DrawVertexArray(mQuadVertexBuffer);

	gDriver->UnbindResourceFromSlot(1);
}

void EffectManager::DrawToBackBuffer()
{
	Effect* finalEffect = GetEffect("BasicQuadTech");

	GraphicsDriver* gDriver = GraphicsDriver::Instance();
	gDriver->SetRenderTargetAndViewportWithNoDepth(0, true);
	
	gDriver->BindTextureResource(mVoxelGIFinalTexture, 1);
	mQuadVertexBuffer->SetActive();
	finalEffect->SetActive();

	gDriver->DrawVertexArray(mQuadVertexBuffer);

	gDriver->UnbindResourceFromSlot(1);
}

void EffectManager::DrawPreviousRenderTargetToBackBuffer()
{
	Effect* finalEffect = GetEffect("BasicQuadTech");

	GraphicsDriver* gDriver = GraphicsDriver::Instance();
	gDriver->SetRenderTargetAndViewportWithNoDepth(0, false);

	gDriver->BindTextureResource(mCurRenderTarget, 1);
	mQuadVertexBuffer->SetActive();
	finalEffect->SetActive();

	gDriver->DrawVertexArray(mQuadVertexBuffer);

	gDriver->UnbindResourceFromSlot(1);
}

void EffectManager::UploadPerFrameBuffer(float frameTime, float gameTime)
{
	/*PE::SetPerFrameConstantsShaderAction sa(*m_pContext, m_arena);
	sa.m_data.gFrameTime = frameTime;
	sa.m_data.gGameTime = gameTime;
	sa.bindToPipeline();*/
}
