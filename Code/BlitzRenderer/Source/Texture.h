#pragma once

#include "Renderer.h"

enum ESamplerState;

enum TextureFamily
{
	COLOR_MAP,
	NORMAL_MAP,
	SPECULAR_MAP,
	GLOW_MAP,
	COLOR_MAP_ARRAY,
	COLOR_CUBE,
};

enum TextureGPU_BindSlot
{
	TextureSlot_DepthMap,
	TextureSlot_ShadowMap,
	TextureSlot_ColorMap,
	TextureSlot_GlowMap,
	TextureSlot_NormalMap,
	TextureSlot_SpecularMap,
};

enum TextureType
{
	TextureType_Albedo,
	TextureType_Normal,
	TextureType_Depth,
	TextureType_Specular,
	TextureType_LightMap,
	TextureType_Roughness,
	TextureType_Metalness,
	TextureType_Microfacet

};

struct TextureFileSet
{

};

struct RENDERER_EXPORT TextureGPU
{
	TextureGPU();
	TextureGPU(GraphicsDriver& gDriver);
	~TextureGPU();
	
	void createTextureNoFamily(const char* textureFilename, const char* package = NULL);
	void createColorTextureGPU(const char* textureFilename, const char* package = NULL);// , ESamplerState samplerState = SamplerState_Count);
	void createBumpTextureGPU(const char* textureFilename, const char* package = NULL);
	void createSpecularTextureGPU(const char* textureFilename, const char* package = NULL);
	void createGlowTextureGPU(const char* textureFilename, const char* package = NULL);

	void createDrawableIntoColorTexture(UINT32 w, UINT32 h, ESamplerState sampler);
	void createDrawableIntoColorTextureWithDepth(UINT32 w, UINT32 h, ESamplerState sampler, bool enableMSAA = false,  bool use32BitRedForDepth = false);
	void createDrawableIntoDepthTexture(UINT32 w, UINT32 h, ESamplerState sampler);

	// Deferred Rendering
	void createDeferredRenderTexture(UINT32 w, UINT32 h);

	// Voxel GI
	void createVoxelizationDummyRenderTexture(UINT32 w, UINT32 h);
	void createVPLColors2DTextureArrays(UINT32 w, UINT32 h, UINT32 d);
	
	void createRandom2DTexture();

	void createColorCubeTextureGPU(const char* textureFilename, const char* package = NULL);

	// not implemented for DX9
	void createColorTextureArrayGPU(const char* textureFilenames[], UINT32 nTextures, const char* package = NULL);

	inline UINT32 GetBindSlot() const { return mBindSlot; }
	inline void SetBindSlot(UINT32 slot) { mBindSlot = slot; }

	//GraphicsDriver& mGraphics;

	ESamplerState mSamplerState;
	TextureFamily mFamily;

	ShaderResourceView* mShaderResourceView; // a standard texture view
	RenderTargetView* mRenderTargetView; // a view for D3D to draw into this texture
	UnorderedAccessView* mUnorderedAccessView;

	ID3D11DepthStencilView* mDepthStencilView; // a view for D3D to draw into this texture as depth buffer (is different from normal texture)
	ShaderResourceView* mDepthShaderResourceView; // a view of depth as texture
	
	Viewport mViewport;
	TextureD3D* mTexture;
	TextureD3D* mDepthTexture;

	UINT32 mBindSlot;


	char mname[256];
};

