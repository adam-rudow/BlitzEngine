#pragma once
#include "../SystemDefines.h"
#include "Utils/Utility.h"
#include "Math/math.h"

class DrawComponent;
enum DrawOrder
{
	DrawOrder_First = 1,
	DrawOrder_Default = 2,
	DrawOrder_Last = 4
};

class SYSTEM_EXPORT IRenderer
{
	DeclAbstractSingleton(IRenderer)

public:
	//IRenderer(/*class Game& game*/);
	//~IRenderer();

	// Startup functions
	virtual bool Init() = 0;
	//bool CreateGameWindow(HINSTANCE hInstance);

	// Core Rendering
	virtual void RenderFrame() = 0;
	//void ClearFrame();
	//void DoRenderEffects();
	//void DrawAllVertexBuffers(DrawPassType drawPassType);
	//void DrawFowardRenderedBuffers();
	//void Present();

	//// Release D3D data
	//void CleanD3D();

	//// Add/Remove components
	virtual void AddComponent(DrawComponent* comp, UINT8 drawOrder = DrawOrder_Default) = 0;
	virtual void RemoveComponent(DrawComponent* comp, UINT8 drawOrder = DrawOrder_Default) = 0;

	//virtual void AddLight(LightComponent* light) = 0;
	//virtual void RemoveLight(LightComponent* light) = 0;
	//virtual void AddPointLight(PointLightComponent* comp) = 0;
	//virtual void RemovePointLight(PointLightComponent* comp) = 0;
	//virtual void AddDirectionalLight(DirectionalLightComponent* comp) = 0;
	//virtual void RemoveDirectionalLight(DirectionalLightComponent* comp) = 0;
	//virtual void AddSpotLight(SpotLightComponent* comp) = 0;
	//virtual void RemoveSpotLight(SpotLightComponent* comp) = 0;

	//// Prepare data for GPU
	virtual void SetViewProjMatrix(const Matrix4& view, const Matrix4& proj, const Vector3& pos) = 0;
	virtual void SetViewMatrix(const Matrix4& view, const Vector3& pos) = 0;

	//void UploadPerFrameConstants();
	//void UploadLightingConstants();
	//void UploadConstantBuffers();
	//void UploadCameraConstants();

	//void DrawVertexBuffer(VertexData* vertexArray, ShaderType shaderType);
	//void DrawVertexBuffer(VertexData* vertexArray);
	virtual void DrawString(const char* text, float xPos, float yPos) = 0;

	//// Modify constant buffer data and Render Modes
	virtual void SetRenderMode(unsigned mode) = 0;
	virtual void ModifyExposure(float delta) = 0;
	////void AdjustNumVPLPropogations(int val);
	//inline PerCameraConstants* GetCamConsts() { return &mCamConsts; }

	//// Getters
	//inline GraphicsDriver& GetGraphicsDriver() const { return *mGraphicsDriver; }
	//inline ID3D11Buffer* GetSingleLightBuffer() const { return mSingleLightBuffer; }
	//inline ID3D11Buffer* GetPerEmitterBuffer() const { return mEmitterConstBuffer; }

	//inline std::vector<LightComponent*> GetLights() { return mLights; }
	//inline std::vector<PointLightComponent*> GetPointLights() { return mPointLights; }
	//inline std::vector<SpotLightComponent*> GetSpotLights() { return mSpotLights; }
	//inline std::vector<DirectionalLightComponent*> GetDirectionalLights() { return mDirectionalLights; }

	//inline DrawPassType GetCurrentDrawPassType() const { return mCurDrawPassType; }

	virtual void AdjustNumTrianglesInScene(int deltaTriangles) = 0;
	virtual UINT32 GetNumTrianglesInScene() const = 0;
};

extern IRenderer* ConstructRenderer();
