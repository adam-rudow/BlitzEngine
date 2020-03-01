#pragma once
#include <windows.h>
#include <windowsx.h>
#include <vector>

#include "Rendering/IRenderer.h"

#include "Utils/RandomGen.h"

#include "DrawComponent.h"
#include "GraphicsDriver.h"
#include "VertexData.h"

//#include "LightComponent.h"
#include "SpotLightComponent.h"
#include "PointLightComponent.h"
#include "DirectionalLightComponent.h"

class GpuProfiler;

struct PerFrameConstants
{
	float mFrameTime;
	float mGameTime;
	Vector2 pad;
};

struct RENDERER_EXPORT PerCameraConstants
{
	Matrix4 mViewProjMatrix;
	Vector3 mCamWorldPos;
	float pad;
	Matrix4 gVoxelViewMatrices[3];
	Matrix4 gVoxelOrthoProjection;
	Vector3 gVoxelGridCenter;
	float gUseOcclusion;
	float gVoxelCellSize;
	float gVoxelDimension;
	Vector2 pad2;
};

struct RENDERER_EXPORT PerObjectConstants
{
	Matrix4 mWorldTransformMatrix;
};

struct LightingConstants
{
	Vector3 mAmbientLight;
	float pad;
	PointLightData mPointLights[MAX_POINT_LIGHTS];
	DirectionalLightData mDirectionalLights[MAX_DIRECTIONAL_LIGHTS];
	SpotLightData mSpotLights[MAX_SPOT_LIGHTS];
	LightData mLights[MAX_POINT_LIGHTS + MAX_DIRECTIONAL_LIGHTS + MAX_SPOT_LIGHTS];
	Matrix4 mLightWVP;
};

struct SingleLightConstant
{
	LightData	gLightCurrent;
	float		gIsShadowCaster;
	float		pad[3];
};


struct RENDERER_EXPORT EmitterPerlinData
{
	PerlinSineTriple mAngleWave;
	PerlinSineTriple mZDistWave;
	PerlinSineTriple mSizeWave;
	PerlinSineTriple mLifespanWave;
}; // 32 * 4 = 128 bytes


struct PerParticleEmitterConstants
{
	Matrix4 mWorldTransform;
	Vector2 mLifeSpans;
	Vector2 mSizes;
	Vector2 mForces;
	float mGravitMagnitude;
	float mThetaClamp;
	float mRandomSeed;
	UINT32 mToSpawnCount;
	EmitterPerlinData mRandomWaves;
	Vector2 pad;
};

struct PostProcessConstants
{
	float mExposure = 2.5f;
	uint32_t mHDR = 1;
	Vector2 pad;
};

struct ScreenText
{
	ScreenText(std::string str, float x, float y) { mText = str; mXPos = x; mYPos = y; }

	std::string mText;
	float mXPos;
	float mYPos;
};

enum ERenderMode
{
	RenderMode_Default,
	RenderMode_DebugShadowMap,
	RenderMode_DebugPosBuffer,
	RenderMode_DebugNormBuffer,
	RenderMode_DebugDiffuseBuffer,
	RenderMode_DeferredLightResult,
	RenderMode_DebugVoxelGrid
};

class RENDERER_EXPORT Renderer : public IRenderer
{
	//DeclSingleton(Renderer)

public:
	Renderer(/*class Game& game*/);
	~Renderer();

	// Startup functions
	bool Init();
	//bool CreateGameWindow(HINSTANCE hInstance);

	// Core Rendering
	void RenderFrame();
	void ClearFrame();
	void DoRenderEffects();
	void DrawAllVertexBuffers(DrawPassType drawPassType);
	void DrawFowardRenderedBuffers();
	void Present();

	// Release D3D data
	void CleanD3D();

	// Add/Remove components
	void AddComponent(DrawComponent* comp, UINT8 drawOrder = DrawOrder_Default);
	void RemoveComponent(DrawComponent* comp, UINT8 drawOrder = DrawOrder_Default);

	void AddLight(LightComponent* light);
	void RemoveLight(LightComponent* light);
	void AddPointLight(PointLightComponent* comp);
	void RemovePointLight(PointLightComponent* comp);
	void AddDirectionalLight(DirectionalLightComponent* comp);
	void RemoveDirectionalLight(DirectionalLightComponent* comp);
	void AddSpotLight(SpotLightComponent* comp);
	void RemoveSpotLight(SpotLightComponent* comp);

	// Prepare data for GPU
	void SetViewProjMatrix(const Matrix4& view, const Matrix4& proj, const Vector3& pos);
	void SetViewMatrix(const Matrix4& view, const Vector3& pos);

	void UploadPerFrameConstants();
	void UploadLightingConstants();
	void UploadConstantBuffers();
	void UploadCameraConstants();

	void DrawVertexBuffer(VertexData* vertexArray, ShaderType shaderType);
	void DrawVertexBuffer(VertexData* vertexArray);
	void DrawString(const char* text, float xPos, float yPos);

	// Modify constant buffer data and Render Modes
	void SetRenderMode(unsigned mode) override;
	void ModifyExposure(float delta) override;
	//void AdjustNumVPLPropogations(int val);
	inline PerCameraConstants* GetCamConsts() { return &mCamConsts; }

	// Getters
	inline GraphicsDriver& GetGraphicsDriver() const { return *mGraphicsDriver; }
	inline ID3D11InputLayout* GetBasicMeshIL() const { return mBasicMeshIL; }
	inline ID3D11InputLayout* GetPhongIL() const { return mPhongIL; }
	inline ID3D11Buffer* GetSingleLightBuffer() const { return mSingleLightBuffer; }
	inline ID3D11Buffer* GetPerEmitterBuffer() const { return mEmitterConstBuffer; }

	inline std::vector<LightComponent*> GetLights() { return mLights; }
	inline std::vector<PointLightComponent*> GetPointLights() { return mPointLights; }
	inline std::vector<SpotLightComponent*> GetSpotLights() { return mSpotLights; }
	inline std::vector<DirectionalLightComponent*> GetDirectionalLights() { return mDirectionalLights; }
	
	inline DrawPassType GetCurrentDrawPassType() const { return mCurDrawPassType; }

	inline void AdjustNumTrianglesInScene(int deltaTriangles) { mNumTrianglesInScene += deltaTriangles; }
	inline UINT32 GetNumTrianglesInScene() const { return mNumTrianglesInScene; }

	static int Window_Width;
	static int Window_Height;

	inline HWND GetHWnd() const { return hWnd;	}

private:
	bool InitShaders();
	bool InitFontEngine();
	void AdjustForRHCoords(Matrix4 viewMat);

	void DrawStrings();

	// Core variables
	HWND hWnd;
	GraphicsDriver* mGraphicsDriver;
	///class Game& mGame;
	ERenderMode mRenderMode;

	IFW1FontWrapper* mFontWrapper;
	std::vector<ScreenText> mTextToPrint;

	GpuProfiler* mGpuProfiler;

	// Shaders
	ID3D11VertexShader *meshVS;
	ID3D11PixelShader *meshPS;
	ID3D11VertexShader *phongVS;
	ID3D11PixelShader *phongPS;

	// Input Layouts
	ID3D11InputLayout* mBasicMeshIL;
	ID3D11InputLayout* mPhongIL;

	// Lists of components
	std::vector<DrawComponent*> mDrawComponents;
	std::vector<DrawComponent*> mForwardDrawComponents;
	std::vector<LightComponent*> mLights;
	std::vector<SpotLightComponent*> mSpotLights;
	std::vector<PointLightComponent*> mPointLights;
	std::vector<DirectionalLightComponent*> mDirectionalLights;

	// Views to draw from
	//std::vector<View*> mViews;

	// Saved matrices for 3D
	Matrix4 mSavedCamViewMatrix;
	Matrix4 mViewMatrix;
	Matrix4 mProjMatrix;

	// Constants and Constant Buffers
	PerFrameConstants	mFrameConsts;
	ID3D11Buffer*		mFrameConstBuffer;
	PerCameraConstants	mCamConsts;
	ID3D11Buffer*		mCamConstBuffer;
	LightingConstants	mLightingConsts;
	ID3D11Buffer*		mLightConstBuffer;
	PostProcessConstants mPPConsts;
	//ID3D11Buffer* mPPConstBuffer;
	PerParticleEmitterConstants mEmitterConsts;
	ID3D11Buffer*		mEmitterConstBuffer;
	ID3D11Buffer*		mSingleLightBuffer;

	DrawPassType mCurDrawPassType;

	UINT32 mNumTrianglesInScene;

	//UINT16 mNumVPLPropogations;
	// Effects
	//ID3DXEffect* mFurEffect;

	///////////////////////////
	// Feature Toggles
	bool m_doVoxelGI;
	bool m_doFrustumCulling;
	//bool m_do 
};

