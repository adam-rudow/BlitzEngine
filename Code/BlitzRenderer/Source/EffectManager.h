#pragma once

#include <map>
#include <set>

#include "Math/Matrix4.h"
#include "Utils/Utility.h"
#include "RenderUtility.h"
#include "RendererMacros.h"
#include "Renderer.h"

struct Effect;
struct Light;
struct TextureGPU;
class GraphicsDriver;
class ResourceBufferGPU;
class VertexData;
//class IGame;
class MeshComponent;

//struct Voxel
//{
//	UINT32 mNormalMask[4];
//	UINT32 mColorMask;
//	//Vector3 m_colorMask;
//	UINT32 mOcclusion;
//};

class RENDERER_EXPORT EffectManager
{
	DeclSingleton(EffectManager)

	EffectManager();
	~EffectManager();

	//void SetGame(IGame* game) { mGame = game; }

	void LoadGPUResources();
	void setupConstantBuffersAndShaderResources();
	
	Effect *operator[] (const char *pEffectFilename);

	void RegisterEffect(Effect* pEffect);
	void RegisterEffect(ShaderType pType, Effect* pEffect);
	void RegisterEffect(VertexFormatGPU vertFormat, Effect* pEffect);
	void RegisterZOnlyEffect(ShaderType pType, Effect* pEffect);
	void RegisterZOnlyEffect(VertexFormatGPU vertFormat, Effect* pEffect);
	void RegisterVoxelEffect(Effect* pEffect);
	void RegisterVoxelEffect(VertexFormatGPU vertFormat, Effect* pEffect);
	void RegisterInputLayout(VertexFormatGPU eFormat, ID3D11InputLayout* layout);

	Effect* GetEffect(const char *pEffectFilename);
	Effect* GetEffect(ShaderType pType);
	Effect* GetEffect(VertexFormatGPU vertFormat);
	Effect* GetZOnlyEffect(ShaderType pType);
	Effect* GetZOnlyEffect(VertexFormatGPU vertFormat);
	Effect* GetVoxelizationEffect(Effect* normalEffect);
	Effect* GetVoxelizationEffect(VertexFormatGPU vertFormat);
	ID3D11InputLayout* GetInputLayout(VertexFormatGPU eFormat);

	//void setTextureAndDepthTextureRenderTargetForGlow();
	//void setTextureAndDepthTextureRenderTargetForDefaultRendering();

	//void set2ndGlowRenderTarget();
	//void setFrameBufferCopyRenderTarget();

	void SetShadowMapRenderTarget();
	void SetDeferredShadingRenderTargets();

	void SetVoxelizationRenderTarget();
	void SetVoxelGridShaderResource();

	void ClearVoxelGrid();

	void EndCurrentRenderTarget(bool endUAV = false);
	void EndRenderTargets(int numRenderTargets = 1);

	void SetShadowMapShaderValue();

	void buildFullScreenBoard();
	void buildLightVolumeSphere();

	void DebugDrawRenderTarget(/*bool drawGlowRenderTarget, bool drawSeparatedGlow, bool drawGlow1stPass, bool drawGlow2ndPass,*/ bool drawShadowRenderTarget,
								bool drawDeferredPositionTex, bool drawDeferredNormalTex, bool drawDeferredDiffuseTex, bool drawDeferredLightResult);
	//void debugDrawVPLResult();

	void UploadPerFrameBuffer(float frameTime, float gameTime);
	
	//****************************
	// Post Processing (Bloom + MotionBlur)
	/*void drawFullScreenQuad();
	void drawFirstGlowPass();
	void drawGlowSeparationPass();
	void drawSecondGlowPass();
	void drawMotionBlur();
	void drawFrameBufferCopy();*/
	void DrawToBackBuffer();
	void DrawPreviousRenderTargetToBackBuffer();

	//****************************
	// Deferred Rendering
	void drawDeferredLightingPass();

	//****************************
	// Voxel GI
	void DrawVoxelGrid();
	void DrawVPLCreationForAllLights();
	void BindVPLForPropogation(TextureGPU* texture, /*SA_Bind_Resource& bindresource,*/ UINT32 bindSlot);
	void SwapVPLPropogationSRV_UAV(TextureGPU* bindResources[6], UINT16 swapType);
	void PropogateVPLsAcrossVoxelGrid();
	void ApplyIndirectIlluminationVoxelGI();
	inline void AdjustVPLPropogationInterations(int numChanged) 
	{
		mNumVPLPropogations = (mNumVPLPropogations + numChanged < 0) ? 0 : mNumVPLPropogations + numChanged;
	}
	inline int GetNumVPLPropogations() const { return mNumVPLPropogations; }

private:
	Renderer* gRenderer;

	// Member vars
	std::map<VertexFormatGPU, ID3D11InputLayout*> mInputLayoutMap;

	std::map<std::string, Effect*> mNameToEffectMap;
	std::map<ShaderType, Effect*> mTypeToEffectMap;
	std::map<VertexFormatGPU, Effect*> mFormatToEffectMap;

	std::map<ShaderType, Effect*> mTypeToZOnlyEffectMap;
	std::map<VertexFormatGPU, Effect*> mFormatToZOnlyEffectMap;

	std::map<std::string, Effect*> mVoxelEffectMap;
	std::map<VertexFormatGPU, Effect*> mFormatToVoxelEffectMap;
	
	TextureGPU* mCurRenderTarget;
	//Texture *m_pCurDepthTarget;

	//****************************
	// Deferred Rendering
	Effect* mDeferredLightingEffect;
	TextureGPU* mGBufferPositionTexture;
	TextureGPU* mGBufferNormalsTexture;
	TextureGPU* mGBufferDiffuseTexture;
	TextureGPU* mDeferredLightingResult;
	VertexData* mLightVolumeVertexBuffer;

	//****************************
	// Voxel GI
	Effect* mVoxelGridEffect;
	TextureGPU* mVoxelizationRenderTarget;
	ResourceBufferGPU* mVoxelGridStructuredBuffer;
	Effect* mVPLCreateEffect;
	Effect* mVPLPropogationNoOccEffect;
	Effect* mVPLPropogationWithOccEffect;
	TextureGPU* mVPL_RedSHCoefficients0;
	TextureGPU* mVPL_GreenSHCoefficients0;
	TextureGPU* mVPL_BlueSHCoefficients0;
	TextureGPU* mVPL_RedSHCoefficients1;
	TextureGPU* mVPL_GreenSHCoefficients1;
	TextureGPU* mVPL_BlueSHCoefficients1;
	TextureGPU* mVoxelGIFinalTexture;
	Effect* mVoxelGridClearComputeEffect;
	Effect* mVoxelGIIndirectIlluminationApply;

private:

	//IGame* mGame;
	GraphicsDriver* gDriver;

	TextureGPU* mShadowMapDepthTexture;
	
	//Texture m_frameBufferCopyTexture;
	
	Matrix4 m_currentViewProjMatrix;
	Matrix4 m_previousViewProjMatrix;

	// Verts/Indxs for a quad
	VertexData* mQuadVertexBuffer;
	MeshComponent* mLightVolumeSphere;
	//Handle m_hVertexBufferGPU;
	//Handle m_hIndexBufferGPU;
	


	std::set<ID3D11PixelShader *> m_pixelShaders;
	std::set<ID3D11VertexShader *> m_vertexShaders;

	std::set<ID3D11Buffer *> m_cbuffers; // only DX 11 has constant buffers. DX9 just has constant registers
		
	bool m_doMotionBlur;

	// Debug/Demo variables
	int mNumVPLPropogations;
	float m_useAmbientLight = 1;

}; // class EffectManager

