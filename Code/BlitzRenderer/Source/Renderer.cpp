//#include "Renderer.h"
//#include "AssertDbg.h"
//#include "Game.h"
///#include "PreCompiledHeader.h"
#include "Renderer.h"
#include "Effect.h"
#include "GPUProfiler.h"
#include "EffectManager.h"
#include "AlphaBlendStateManager.h"
#include "DepthStencilStateManager.h"
#include "RasterizerStateManager.h"
#include "Utils/Utility.h"
#include "Utils/AssertDbg.h"
#include "System.h"
#include "Application.h"

LRESULT CALLBACK WindowProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

int Renderer::Window_Width	= GWindowWidth;
int Renderer::Window_Height = GWindowHeight;

//ImplSingleton(Renderer)

IRenderer* ConstructRenderer(HINSTANCE hInstance, int nCmdShow)
{
	IRenderer::StaticConstruct<Renderer>();
	return IRenderer::Instance();
}


Renderer::Renderer(/*Game& game*/)
	///: mGame(game)
	: m_doFrustumCulling(true)
	, m_doVoxelGI(true)
{
	//m_IsFullScreen = false;
#if HDR
	mPPConsts.mHDR = 1;
#else
	mPPConsts.mHDR = 0;
	//mRenderMode = RenderMode_DebugVoxelGrid;// RenderMode_Default;
	mRenderMode = RenderMode_Default;
#endif

	mNumTrianglesInScene = 0;
}


Renderer::~Renderer()
{
	CleanD3D();

	delete(EffectManager::Instance());

	GpuProfiler::Instance()->Shutdown();
	delete(GpuProfiler::Instance());

	for (auto d : mDrawComponents)
	{
		delete(d);
	}
	
	mFontWrapper->Release();

	// Destroy GraphicsDriver
	delete(GraphicsDriver::Instance());

}

bool Renderer::Init(HINSTANCE hInstance, int nCmdShow)
{
	GraphicsDriver::Construct();
	mGraphicsDriver = GraphicsDriver::Instance();

	HWND hWnd;
	if (!Application::Instance()->CreateGameWindow(hInstance, hWnd))
		DbgAssert(false, "Failed to create Game window!\n");

	mGraphicsDriver->InitGraphics(hWnd);

	EffectManager::Construct();
	//EffectManager::Instance()->SetGame(System::Instance()->GetGame());

	if(!InitShaders())
		DbgAssert(false, "Failed to create Shaders!\n");
	
	EffectManager::Instance()->LoadGPUResources();

	if(!InitFontEngine())
		DbgAssert(false, "Failed to create Font Engine!\n");

	GpuProfiler::Construct();
	GpuProfiler::Instance()->Init();

	// Display the window if all initialization succeeds
	ShowWindow(hWnd, nCmdShow);

	// Setup constant buffers
	mFrameConstBuffer	= mGraphicsDriver->CreateConstantBuffer(sizeof(PerFrameConstants));
	mCamConstBuffer		= mGraphicsDriver->CreateConstantBuffer(sizeof(PerCameraConstants));
	mLightConstBuffer	= mGraphicsDriver->CreateConstantBuffer(sizeof(LightingConstants));
	//mPPConstBuffer	= mGraphicsDriver->CreateConstantBuffer(sizeof(PostProcessConstants));
	mSingleLightBuffer	= mGraphicsDriver->CreateConstantBuffer(sizeof(SingleLightConstant));
	mEmitterConstBuffer = mGraphicsDriver->CreateConstantBuffer(sizeof(PerParticleEmitterConstants));

	return true;
}

void Renderer::RenderFrame()
{
	ClearFrame();
	DoRenderEffects();
	Present();
}

void Renderer::ClearFrame()
{
	mGraphicsDriver->ClearRenderTargets();
	mGraphicsDriver->ClearDepthStencils(1.0f);
}

bool Renderer::InitFontEngine()
{
	IFW1Factory *pFW1Factory;
	HRESULT hResult = FW1CreateFactory(FW1_VERSION, &pFW1Factory);

	hResult = pFW1Factory->CreateFontWrapper(mGraphicsDriver->GetDevice(), L"Arial", &mFontWrapper);
	//assert(hResult == S_OK);

	pFW1Factory->Release();

	return true;
}

void Renderer::DrawAllVertexBuffers(DrawPassType drawPassType)
{	
	mCurDrawPassType = drawPassType;

	for (unsigned i = 0; i < mDrawComponents.size(); ++i)
	{
		mDrawComponents[i]->Draw(*this);
	}
}

void Renderer::DrawFowardRenderedBuffers()
{
	mCurDrawPassType = DrawPass_Default;

	for (unsigned i = 0; i < mForwardDrawComponents.size(); ++i)
	{
		//DrawVertexBuffer(mForwardDrawComponents[i]->GetVertexData());
		mForwardDrawComponents[i]->Draw(*this);
	}
}

//void Renderer::DrawVertexBuffer(VertexData* vertexArray)
//{
//	vertexArray->SetActive();
//	if (vertexArray->GetMainEffect() != NULL)
//	{
//		mGraphicsDriver->DrawVertexArray(vertexArray);
//	}
//}


void Renderer::DrawVertexBuffer(VertexData* vertexArray, ShaderType shaderType)
{
	vertexArray->SetActive();

	// Choose proper shader to render vertex array with
	Effect* curEffect;
	if (mCurDrawPassType == DrawPass_DepthOnly)
		curEffect = EffectManager::Instance()->GetZOnlyEffect(shaderType);
	else
		curEffect = EffectManager::Instance()->GetEffect(shaderType);
	
	// For voxelization, get the shader based on the normal shader
	if (mCurDrawPassType == DrawPass_Voxelization)
		curEffect = EffectManager::Instance()->GetVoxelizationEffect(curEffect);


	if (curEffect != NULL)
	{
		curEffect->SetActive();
		mGraphicsDriver->DrawVertexArray(vertexArray);
	}
}

void Renderer::DrawVertexBuffer(VertexData* vertexArray)
{
	vertexArray->SetActive();

	VertexFormatGPU vertFormat;
	if (vertexArray->GetVertexSize() == 11)
	{
		vertFormat = VertexFormat_DetailedMesh;
	}
	else if (vertexArray->GetVertexSize() == 8)
	{
		vertFormat = VertexFormat_StdMesh;
	}
	else
	{
		vertFormat = VertexFormat_MinimalMesh;
	}

	// Choose proper shader to render vertex array with
	Effect* curEffect;
	if (mCurDrawPassType == DrawPass_DepthOnly)
		curEffect = EffectManager::Instance()->GetZOnlyEffect(vertFormat);
	else if (mCurDrawPassType == DrawPass_Voxelization)
		curEffect = EffectManager::Instance()->GetVoxelizationEffect(vertFormat);
	else
		curEffect = EffectManager::Instance()->GetEffect(vertFormat);

	if (curEffect != NULL)
	{
		curEffect->SetActive();
		mGraphicsDriver->DrawVertexArray(vertexArray);
	}
}

void Renderer::DrawString(const char* text, float xPos, float yPos)
{
	mTextToPrint.push_back(ScreenText(text, xPos, yPos));
}

void Renderer::DrawStrings()
{
	for (unsigned i = 0; i < mTextToPrint.size(); ++i)
	{
		//wchar_t* text = mTextToPrint[i].mText.c_str();
		int size_needed = MultiByteToWideChar(CP_UTF8, 0, &mTextToPrint[i].mText[0], (int)mTextToPrint[i].mText.size(), NULL, 0);
		std::wstring wstrTo(size_needed, 0);
		MultiByteToWideChar(CP_UTF8, 0, &mTextToPrint[i].mText[0], (int)mTextToPrint[i].mText.size(), &wstrTo[0], size_needed);
		mFontWrapper->DrawString(mGraphicsDriver->GetDeviceContext(), wstrTo.c_str(), 24.0f, mTextToPrint[i].mXPos, mTextToPrint[i].mYPos, 0xffffffff, 0);
	}

	mTextToPrint.clear();
}

void Renderer::Present()
{
	mGraphicsDriver->Present();
	GpuProfiler::Instance()->EndFrame();
}

void Renderer::DoRenderEffects()
{
	GpuProfiler* gProfiler = GpuProfiler::Instance();
	gProfiler->BeginFrame();

	UploadConstantBuffers();
	gProfiler->Timestamp(GTS_UploadCBuffers);

	static bool shadowMapping = true;
	if (shadowMapping)
	{
		//  Set shadow map as the render target
		EffectManager::Instance()->SetShadowMapRenderTarget();
		gProfiler->Timestamp(GTS_ShadowClear);

		// Draw geometry as depth-only
		DrawAllVertexBuffers(DrawPass_DepthOnly);
		gProfiler->Timestamp(GTS_ShadowObjects);
		EffectManager::Instance()->EndCurrentRenderTarget();

		EffectManager::Instance()->SetShadowMapShaderValue();
	}

	if (m_doFrustumCulling)
	{

	}

	static bool deferredRender = true;
	if (deferredRender)
	{
		// Set G-Buffers as render targets
		EffectManager::Instance()->SetDeferredShadingRenderTargets();
		gProfiler->Timestamp(GTS_MainClear);
	}

	//***************************************************************
	// Draw geometry normally
	DrawAllVertexBuffers(DrawPass_Default);
	gProfiler->Timestamp(GTS_MainObjects);
	EffectManager::Instance()->EndCurrentRenderTarget();

	// Draw light volumes to apply lighting
	if (deferredRender)
	{
		EffectManager::Instance()->drawDeferredLightingPass();
		gProfiler->Timestamp(GTS_DeferredLightPass);
		//EffectManager::Instance()->EndCurrentRenderTarget();
	}

	//***************************************************************
	// Draw geometry to be voxelized for Global Illumination
	if (m_doVoxelGI)
	{
		// Clear the voxel grid to all black
		EffectManager::Instance()->ClearVoxelGrid();

		// Fill Voxel grid with geometry data
		EffectManager::Instance()->SetVoxelGridShaderResource();

		DrawAllVertexBuffers(DrawPass_Voxelization);
		EffectManager::Instance()->EndCurrentRenderTarget();

		if (EffectManager::Instance()->GetNumVPLPropogations() > 0)
		{
			// Create VPLs for each light in game
			EffectManager::Instance()->DrawVPLCreationForAllLights();

			// Propogate VPLs through the voxel grid
			EffectManager::Instance()->PropogateVPLsAcrossVoxelGrid();

			// Add indirect light to original geometry (screen space)
			EffectManager::Instance()->ApplyIndirectIlluminationVoxelGI();
		}

		gProfiler->Timestamp(GTS_VoxelGITotal);
	}

	//***************************************************************
	// Draw Forward pass for UI elements and particles
	DrawFowardRenderedBuffers();
	gProfiler->Timestamp(GTS_FinalForwardDraw);

	if (mRenderMode == RenderMode_Default && deferredRender)
	{
		// Draw final results to the backbuffer
		EffectManager::Instance()->DrawPreviousRenderTargetToBackBuffer();
		EffectManager::Instance()->EndCurrentRenderTarget();
	}
	else
	{
		bool drawShadowMap		= mRenderMode == RenderMode_DebugShadowMap;
		bool drawPosGBuffer		= mRenderMode == RenderMode_DebugPosBuffer;
		bool drawNormGBuffer	= mRenderMode == RenderMode_DebugNormBuffer;
		bool drawDiffuseGBuffer = mRenderMode == RenderMode_DebugDiffuseBuffer;
		bool drawDeferredResult = mRenderMode == RenderMode_DeferredLightResult;
		bool drawVoxelGrid		= mRenderMode == RenderMode_DebugVoxelGrid;
		if (drawVoxelGrid)
		{
			EffectManager::Instance()->DrawVoxelGrid();
			//EffectManager::Instance()->EndCurrentRenderTarget();
		}
		else
		{
			EffectManager::Instance()->DebugDrawRenderTarget(drawShadowMap, drawPosGBuffer, drawNormGBuffer, drawDiffuseGBuffer, drawDeferredResult);
		}
	}

	// Draw text last
	DrawStrings();

	gProfiler->WaitForDataAndUpdate();
	gProfiler->DrawQueryText(this);
	

	//EffectManager::Instance()->EndCurrentRenderTarget();
}

void Renderer::SetViewProjMatrix(const Matrix4& view, const Matrix4& proj, const Vector3& pos)
{
	PerCameraConstants camConsts;
	camConsts.mViewProjMatrix = view * proj;
	camConsts.mCamWorldPos = pos;

	mGraphicsDriver->UploadBuffer(mCamConstBuffer, (void*)&camConsts, sizeof(PerCameraConstants));
	mGraphicsDriver->SetVSConstantBuffer(mCamConstBuffer, 0);
	mGraphicsDriver->SetPSConstantBuffer(mCamConstBuffer, 0);
}

void Renderer::SetViewMatrix(const Matrix4& view, const Vector3& pos)
{
	//mViewMatrix = view; // store later for Unproject
	mCamConsts.mViewProjMatrix = view * mProjMatrix;
	mCamConsts.mCamWorldPos = pos;

	/*char str[256];
	sprintf_s(str, "CamPosition: <%f, %f. %f> \n", mCamConsts.mCamWorldPos.x, mCamConsts.mCamWorldPos.y, mCamConsts.mCamWorldPos.z);
	OutputDebugString(str);*/	
}

void Renderer::UploadConstantBuffers()
{
	UploadPerFrameConstants();
	UploadCameraConstants();
	UploadLightingConstants();

	// Map PostProcessing Data for GPU
	//mGraphicsDriver->UploadBuffer(mPPConstBuffer, (void*)&mPPConsts, sizeof(PostProcessConstants));
	//mGraphicsDriver->SetPSConstantBuffer(mPPConstBuffer, 4);
}

void Renderer::UploadPerFrameConstants()
{
	mFrameConsts.mFrameTime = static_cast<float>(Application::Instance()->GetFrameTime());
	mFrameConsts.mGameTime	= static_cast<float>(Application::Instance()->GetSystemTime());

	mGraphicsDriver->UploadBuffer(mFrameConstBuffer, (void*)&mFrameConsts, sizeof(PerFrameConstants));
	mGraphicsDriver->SetConstantBuffer(mFrameConstBuffer, PerFrameSlot);
}

void Renderer::UploadCameraConstants()
{
	mGraphicsDriver->UploadBuffer(mCamConstBuffer, (void*)&mCamConsts, sizeof(PerCameraConstants));
	mGraphicsDriver->SetConstantBuffer(mCamConstBuffer, PerCameraSlot);
	//mGraphicsDriver->SetVSConstantBuffer(mCamConstBuffer, 0);
	//mGraphicsDriver->SetPSConstantBuffer(mCamConstBuffer, 0);
}

void Renderer::UploadLightingConstants()
{
	// Zero out memory (sets enabled to false for all unless specified to true while storing data)
	ZeroMemory(&mLightingConsts, sizeof(LightingConstants));

	mLightingConsts.mAmbientLight = Vector3::Zero;

	// Store lights in their respective lists (for deferred rendering light volumes, different lights have different meshes)
	{
		// Add PointLight info
		for (unsigned i = 0; i < mPointLights.size(); ++i)
		{
			mLightingConsts.mPointLights[i].mDiffuseColor	= mPointLights[i]->GetLightData().mDiffuseColor;
			mLightingConsts.mPointLights[i].mSpecularColor	= mPointLights[i]->GetLightData().mSpecularColor;
			mLightingConsts.mPointLights[i].mSpecularPower	= mPointLights[i]->GetLightData().mSpecularPower;
			mLightingConsts.mPointLights[i].mPosition		= mPointLights[i]->GetPosition();
			mLightingConsts.mPointLights[i].mRange			= mPointLights[i]->GetLightData().mRange;
			mLightingConsts.mPointLights[i].mAttenuation	= mPointLights[i]->GetLightData().mAttenuation;
			mLightingConsts.mPointLights[i].mEnabled		= mPointLights[i]->GetLightData().mEnabled;

			// Invert Y pos if no parent
			//if(mPointLights[i]->GetOwner() == NULL)
			//	mLightingConsts.mPointLights[i].mPosition.y *= -1;
		}

		// Add DirectionalLight info
		for (unsigned i = 0; i < mDirectionalLights.size(); ++i)
		{
			mLightingConsts.mDirectionalLights[i].mDiffuseColor		= mDirectionalLights[i]->GetLightData().mDiffuseColor;
			mLightingConsts.mDirectionalLights[i].mSpecularColor	= mDirectionalLights[i]->GetLightData().mSpecularColor;
			mLightingConsts.mDirectionalLights[i].mSpecularPower	= mDirectionalLights[i]->GetLightData().mSpecularPower;
			mLightingConsts.mDirectionalLights[i].mDirection		= mDirectionalLights[i]->GetLightData().mDirection;
			mLightingConsts.mDirectionalLights[i].mIsShadowCaster	= mDirectionalLights[i]->IsShadowCaster() ? 1 : 0;
			mLightingConsts.mDirectionalLights[i].mEnabled			= mDirectionalLights[i]->GetLightData().mEnabled;

			// Invert Y for LHS
			//mLightingConsts.mDirectionalLights[i].mDirection.y *= -1;

			// Set up data for shadow resources
			if (mDirectionalLights[i]->IsShadowCaster())
			{
				mDirectionalLights[i]->CalculateShadowTransforms();
				mLightingConsts.mLightWVP = mDirectionalLights[i]->GetLightViewTransform() * mDirectionalLights[i]->GetLightProjectionTransform();
			}
		}

		// Add SpotLight info
		for (unsigned i = 0; i < mSpotLights.size(); ++i)
		{
			mLightingConsts.mSpotLights[i].mDiffuseColor	= mSpotLights[i]->GetLightData().mDiffuseColor;
			mLightingConsts.mSpotLights[i].mSpecularColor	= mSpotLights[i]->GetLightData().mSpecularColor;
			mLightingConsts.mSpotLights[i].mSpecularPower	= mSpotLights[i]->GetLightData().mSpecularPower;
			mLightingConsts.mSpotLights[i].mPosition		= mSpotLights[i]->GetPosition();
			mLightingConsts.mSpotLights[i].mRange			= mSpotLights[i]->GetLightData().mRange;
			mLightingConsts.mSpotLights[i].mDirection		= mSpotLights[i]->GetLightData().mDirection;
			mLightingConsts.mSpotLights[i].mSpotDotProduct	= mSpotLights[i]->GetLightData().mSpotDotProduct;
			mLightingConsts.mSpotLights[i].mAttenuation		= mSpotLights[i]->GetLightData().mAttenuation;
			mLightingConsts.mSpotLights[i].mEnabled			= mSpotLights[i]->GetLightData().mEnabled;

			// Invert Y pos if no parent
			//if(mSpotLights[i]->GetOwner() == NULL)
			//	mLightingConsts.mSpotLights[i].mPosition.y *= -1;
			// Invert Y for LHS
			//mLightingConsts.mSpotLights[i].mDirection.y *= -1;
		}
	}

	// Store lights in single list as well for cleaner light loops in GPU as well as for Voxel GI creating VPLs
	// Add DirectionalLight info
	for (unsigned i = 0; i < mLights.size(); ++i)
	{
		mLightingConsts.mLights[i].mDiffuseColor	= mLights[i]->GetLightData().mDiffuseColor;
		mLightingConsts.mLights[i].mSpecularColor	= mLights[i]->GetLightData().mSpecularColor;
		mLightingConsts.mLights[i].mSpecularPower	= mLights[i]->GetLightData().mSpecularPower;
		mLightingConsts.mLights[i].mPosition		= mLights[i]->GetPosition();
		mLightingConsts.mLights[i].mRange			= mLights[i]->GetLightData().mRange;
		mLightingConsts.mLights[i].mDirection		= mLights[i]->GetLightData().mDirection;
		mLightingConsts.mLights[i].mSpotDotProduct	= mLights[i]->GetLightData().mSpotDotProduct;
		mLightingConsts.mLights[i].mAttenuation		= mLights[i]->GetLightData().mAttenuation;
		mLightingConsts.mLights[i].mIsShadowCaster	= mLights[i]->IsShadowCaster() ? 1 : 0;
		mLightingConsts.mLights[i].mEnabled			= mLights[i]->GetLightData().mEnabled;
		mLightingConsts.mLights[i].mType			= mLights[i]->GetLightData().mType;

		// Set up data for shadow resources
		if (mLights[i]->IsShadowCaster())
		{
			mLights[i]->CalculateShadowTransforms();
			mLightingConsts.mLightWVP = mLights[i]->GetLightViewTransform() * mLights[i]->GetLightProjectionTransform();
		}
	}

	// Map the data for GPU use
	mGraphicsDriver->UploadBuffer(mLightConstBuffer, (void*)&mLightingConsts, sizeof(LightingConstants));

	// Set buffer for GPU
	//mGraphicsDriver->SetVSConstantBuffer(mLightConstBuffer, 2);
	//mGraphicsDriver->SetPSConstantBuffer(mLightConstBuffer, 2);
	mGraphicsDriver->SetConstantBuffer(mLightConstBuffer, AllLightsSlot);
}

void Renderer::AdjustForRHCoords(Matrix4 viewMat)
{
	// invert Z axis
	viewMat.matrix[2][0] *= -1;
	viewMat.matrix[2][1] *= -1;
	viewMat.matrix[2][2] *= -1;
	viewMat.matrix[2][3] *= -1;
}

//bool Renderer::CreateGameWindow(HINSTANCE hInstance)
//{
//	// struct that holds into for the window class
//	WNDCLASSEX wc;
//
//	// clear out the window class for use
//	ZeroMemory(&wc, sizeof(WNDCLASSEX));
//
//	// fill in the struct with the needed info
//	wc.cbSize = sizeof(WNDCLASSEX);
//	wc.style = CS_HREDRAW | CS_VREDRAW;
//	wc.lpfnWndProc = WindowProc;
//	wc.hInstance = hInstance;
//	wc.hCursor = LoadCursor(NULL, IDC_ARROW);
//	wc.lpszClassName = "WindowClass1";
//	//if(!m_IsFullScreen)
//	//	wc.hbrBackground = (HBRUSH)COLOR_WINDOW;
//
//	// register the window class
//	RegisterClassEx(&wc);
//
//	// Resize client vs window sizes
//	RECT wr = { 0, 0, Window_Width, Window_Height };		// set size but not pos
//	AdjustWindowRect(&wr, WS_OVERLAPPEDWINDOW, FALSE);	// adjust the size
//
//														// create the window and use the result as the handle
//	hWnd = CreateWindowEx(NULL,
//		"WindowClass1",		// name of the window class
//		"BlitzRenderer",	// title of the window
//		WS_OVERLAPPEDWINDOW,		// window style
//		300,				// x-pos
//		200,				// y-pos
//		Window_Width,	// width
//		Window_Height,	// height
//		NULL,				// no parent window
//		NULL,				// no menus
//		hInstance,			// application handle
//		NULL);				// used with multiple windows
//
//	return true;
//}
//
//// Main message handler for the program
//LRESULT CALLBACK WindowProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
//{
//	// sort through and find what code to run for the message given
//	switch (message)
//	{
//		// this message is read when the window is closed
//	case WM_DESTROY:
//	{
//		// close the application entirely
//		PostQuitMessage(0);
//		return 0;
//	}
//	break;
//	}
//
//	InputManager::Instance()->GetInputMessages(message, wParam);
//
//	// Handle any messages the switch statement didnt 
//	return DefWindowProc(hWnd, message, wParam, lParam);
//}

bool Renderer::InitShaders()
{
	// Init projection matrix
	float radAngle = ToRadians(70.0f);// *(PI / 180.0f);
	mProjMatrix = Matrix4::CreatePerspectiveFOV(radAngle, static_cast<float>(Window_Width), static_cast<float>(Window_Height), 25.0f, 10000.0f);
	//mProjMatrix = Matrix4::CreateOrtho(static_cast<float>(Window_Width), static_cast<float>(Window_Height), 10.0f, 1000.0f);

	// Ortho for 2D elements - Don't currently need
	mCamConsts.mViewProjMatrix = Matrix4::CreateOrtho(static_cast<float>(Window_Width), static_cast<float>(Window_Height), 1000.0f, -1000.0f);
	
	D3D_SHADER_MACRO commonDefines[] = { NULL, NULL };

	// Plain Quad
	{
		// Plain Quad shader - Load and Compile
		Effect* effect = new Effect(*mGraphicsDriver);
		effect->mVSVertexFormat = VertexFormat_MinimalMesh;
		effect->loadTechnique(
			"Shaders/BasicQuad.hlsl", "VShader",
			NULL, NULL,
			"Shaders/BasicQuad.hlsl", "PShader",
			NULL, NULL,
			RasterizerState_SolidTriNoCull,
			DepthStencilState_NoZBuffer,
			AlphaBlendState_NoBlend,
			"BasicQuadTech",
			commonDefines);

		//EffectManager::Instance()->RegisterEffect(ST_BasicQuad, effect);
		EffectManager::Instance()->RegisterEffect(VertexFormat_MinimalMesh, effect);
	}

	// Basic Unlit Meshes
	{
		// Plain Mesh shader - Load and Compile
		Effect* effect = new Effect(*mGraphicsDriver);
		effect->mVSVertexFormat = VertexFormat_StdMesh;
		effect->loadTechnique(
			"Shaders/BasicMesh.hlsl", "VShader",
			NULL, NULL,
			"Shaders/BasicMesh.hlsl", "PShader",
			NULL, NULL,
			RasterizerState_SolidTriNoCull,
			DepthStencilState_ZBuffer,
			AlphaBlendState_NoBlend,
			"BasicMeshTech",
			commonDefines);

		EffectManager::Instance()->RegisterEffect(ST_BasicMesh, effect);
		//EffectManager::Instance()->RegisterEffect(VertexFormat_StdMesh, effect);

	}

	{
		// Phong lighting shader - Load and Compile
		Effect* effect = new Effect(*mGraphicsDriver);
		effect->mVSVertexFormat = VertexFormat_StdMesh;
		effect->loadTechnique(
			"Shaders/Phong.hlsl", "VShader",
			NULL, NULL,
			"Shaders/Phong.hlsl", "PShader",
			NULL, NULL,
			RasterizerState_SolidTriNoCull,
			DepthStencilState_ZBuffer,
			AlphaBlendState_NoBlend,
			"BasicMesh_Lit_Tech",
			commonDefines);

		//EffectManager::Instance()->RegisterEffect(ST_Lit, effect);
		EffectManager::Instance()->RegisterEffect(VertexFormat_StdMesh, effect);
	}

	// Detailed Lit/Shadowed
	{
		// Phong lighting shader - Load and Compile
		Effect* effect = new Effect(*mGraphicsDriver);
		effect->mVSVertexFormat = VertexFormat_DetailedMesh;
		effect->loadTechnique(
			"Shaders/DetailedMesh_ShadowedPhong.hlsl", "VShader",
			NULL, NULL,
			"Shaders/DetailedMesh_ShadowedPhong.hlsl", "PShader",
			NULL, NULL,
			RasterizerState_SolidTriNoCull,
			DepthStencilState_ZBuffer,
			AlphaBlendState_NoBlend,
			"DetailedMesh_Lit_Tech",
			commonDefines);

		//EffectManager::Instance()->RegisterEffect(ST_DetailedLit, effect);
		EffectManager::Instance()->RegisterEffect(VertexFormat_DetailedMesh, effect);
	}

	// Basic Shadowed Meshes
	{
		// Basic Shadowed shader - Load and Compile
		Effect* effect = new Effect(*mGraphicsDriver);
		effect->mVSVertexFormat = VertexFormat_StdMesh;
		effect->loadTechnique(
			"Shaders/StdMesh_ZOnly.hlsl", "VShader",
			NULL, NULL,
			NULL, NULL,
			//"Shaders/StdMesh_ZOnly.hlsl", "PShader",	// Do not need pixel shader because we are writing depth only for shadows. 
			NULL, NULL,									// Use pixel shader for debug only
			RasterizerState_SolidTriNoCull,
			DepthStencilState_ZBuffer,
			AlphaBlendState_NoBlend,
			"BasicShadowedMeshTech",
			commonDefines);

		//EffectManager::Instance()->RegisterZOnlyEffect(ST_Lit, effect);
		EffectManager::Instance()->RegisterZOnlyEffect(VertexFormat_StdMesh, effect);
	}

	// Detailed Shadowed Meshes
	{
		// Detailed Shadowed shader - Load and Compile
		Effect* effect = new Effect(*mGraphicsDriver);
		effect->mVSVertexFormat = VertexFormat_DetailedMesh;
		effect->loadTechnique(
			"Shaders/DetailedMesh_ZOnly.hlsl", "VShader",
			NULL, NULL,
			NULL, NULL,
			//"Shaders/StdMesh_ZOnly.hlsl", "PShader",	// Do not need pixel shader because we are writing depth only for shadows. 
			NULL, NULL,									// Use pixel shader for debug only
			RasterizerState_SolidTriNoCull,
			DepthStencilState_ZBuffer,
			AlphaBlendState_NoBlend,
			"DetailedShadowedMeshTech",
			commonDefines);

		//EffectManager::Instance()->RegisterZOnlyEffect(ST_DetailedLit, effect);
		EffectManager::Instance()->RegisterZOnlyEffect(VertexFormat_DetailedMesh, effect);
	}

	// Deferred Lighting application pass - point light
	{
		// Deferred Lighting - Load and Compile
		Effect* effect = new Effect(*mGraphicsDriver);
		effect->mVSVertexFormat = VertexFormat_MinimalMesh;
		effect->loadTechnique(
			"Shaders/DeferredLightingPass.hlsl", "VShaderPointLight",
			NULL, NULL,
			"Shaders/DeferredLightingPass.hlsl", "PShader",
			NULL, NULL,
			RasterizerState_SolidTriBackCull,
			DepthStencilState_NoZBuffer,
			AlphaBlendState_Additive,
			//AlphaBlendState_NoBlend,
			"DeferredPointLightingTech",
			commonDefines);

		EffectManager::Instance()->RegisterEffect(ST_DeferredLightEffectPL, effect);
	}

	// Deferred Lighting application pass - directional light
	{
		// Deferred Lighting - Load and Compile
		Effect* effect = new Effect(*mGraphicsDriver);
		effect->mVSVertexFormat = VertexFormat_MinimalMesh;
		effect->loadTechnique(
			"Shaders/DeferredLightingPass.hlsl", "VShaderDirectionalLight",
			NULL, NULL,
			"Shaders/DeferredLightingPass.hlsl", "PShader",
			NULL, NULL,
			RasterizerState_SolidTriBackCull,
			DepthStencilState_NoZBuffer,
			AlphaBlendState_Additive,
			"DeferredDirectionalLightingTech",
			commonDefines);

		EffectManager::Instance()->RegisterEffect(ST_DeferredLightEffectDL, effect);
	}

	// Deferred Lighting application pass - spot light
	{
		// Deferred Lighting - Load and Compile
		Effect* effect = new Effect(*mGraphicsDriver);
		effect->mVSVertexFormat = VertexFormat_MinimalMesh;
		effect->loadTechnique(
			"Shaders/DeferredLightingPass.hlsl", "VShaderSpotLight",
			NULL, NULL,
			"Shaders/DeferredLightingPass.hlsl", "PShader",
			NULL, NULL,
			RasterizerState_SolidTriBackCull,
			DepthStencilState_NoZBuffer,
			AlphaBlendState_Additive,
			"DeferredSpotLightingTech",
			commonDefines);

		EffectManager::Instance()->RegisterEffect(ST_DeferredLightEffectSL, effect);
	}

	//******************************************************
	// Voxel GI
	//******************************************************

	// Voxel grid clear compute shader
	{
		Effect *pEffect = new Effect(*mGraphicsDriver);
		pEffect->loadTechnique(
			NULL, NULL,
			NULL, NULL, // geometry shader
			NULL, NULL,
			"Shaders/VoxelGrid_Clear_CS.hlsl", "main", // compute shader
			RasterizerState_SolidTriNoCull,
			DepthStencilState_ZBuffer, 
			AlphaBlendState_NoBlend, // depth stencil, blend states
			"VoxelGrid_Clear_Compute_Tech",
			commonDefines);

		EffectManager::Instance()->RegisterVoxelEffect(pEffect);
	}

	// Std Mesh voxelization
	{
		Effect *pEffect = new Effect(*mGraphicsDriver);
		pEffect->mVSVertexFormat = VertexFormat_StdMesh;
		pEffect->loadTechnique(
			"Shaders/StdMesh_Voxelization_VS.hlsl", "main",
			"Shaders/Voxelization_Expand_GS.hlsl", "main", // geometry shader
			"Shaders/Voxelization_PS.hlsl", "main",
			NULL, NULL, // compute shader
			RasterizerState_SolidTriNoCull,
			DepthStencilState_NoZBuffer, 
			AlphaBlendState_NoBlend, // depth stencil, blend states
			"StdMesh_Voxelization_Tech",
			commonDefines);

		//EffectManager::Instance()->RegisterVoxelEffect(pEffect);
		EffectManager::Instance()->RegisterVoxelEffect(VertexFormat_StdMesh, pEffect);
	}

	// Detailed Mesh voxelization
	{
		Effect *pEffect = new Effect(*mGraphicsDriver);
		pEffect->mVSVertexFormat = VertexFormat_DetailedMesh;
		pEffect->loadTechnique(
			"Shaders/DetailedMesh_Voxelization_VS.hlsl", "main",
			"Shaders/Voxelization_Expand_GS.hlsl", "main", // geometry shader
			"Shaders/Voxelization_PS.hlsl", "main",
			NULL, NULL, // compute shader
			RasterizerState_SolidTriNoCull,
			DepthStencilState_NoZBuffer,
			AlphaBlendState_NoBlend, // depth stencil, blend states
			"DetailedMesh_Voxelization_Tech",
			commonDefines);

		//EffectManager::Instance()->RegisterVoxelEffect(pEffect);
		EffectManager::Instance()->RegisterVoxelEffect(VertexFormat_DetailedMesh, pEffect);
	}

	// Debug Voxelization stage
	{
		Effect *pEffect = new Effect(*mGraphicsDriver);
		pEffect->mVSVertexFormat = VertexFormat_MinimalMesh;
		pEffect->loadTechnique(
			"Shaders/BasicQuad.hlsl", "VShader",
			NULL, NULL, // geometry shader
			"Shaders/Voxelization_Debug_PS.hlsl", "main",
			NULL, NULL, // compute shader
			RasterizerState_SolidTriNoCull,
			DepthStencilState_NoZBuffer, 
			AlphaBlendState_NoBlend, // depth stencil, blend states
			"Voxelization_Debug_Tech",
			commonDefines);

		EffectManager::Instance()->RegisterVoxelEffect(pEffect);
	}

	// Create VPLs by rendering to 3 2D texture arrays
	{
		Effect *pEffect = new Effect(*mGraphicsDriver);
		pEffect->mVSVertexFormat = VertexFormat_MinimalMesh;
		pEffect->loadTechnique(
			"Shaders/VPL_Create_VS.hlsl", "main",
			"Shaders/VPL_Create_GS.hlsl", "GS", // geometry shader
			"Shaders/VPL_Create_PS.hlsl", "main",
			NULL, NULL, // compute shader
			RasterizerState_SolidTriNoCull,
			DepthStencilState_NoZBuffer,
			AlphaBlendState_NoBlend, // depth stencil, blend states
			"VPL_Create_Tech",
			commonDefines);

		EffectManager::Instance()->RegisterVoxelEffect(pEffect);
	}

	// Propagate Indirect light through voxel grid
	{
		D3D_SHADER_MACRO defines[] =
		{
			"USE_OCCLUSION", "0",
			NULL, NULL
		};
		
		Effect *pEffect = new Effect(*mGraphicsDriver);
		pEffect->loadTechnique(
			NULL, NULL,
			NULL, NULL, 
			NULL, NULL, // geometry shader
			"Shaders/VoxelGrid_VPL_Propogation_CS.hlsl", "main", // compute shader
			RasterizerState_SolidTriNoCull,
			DepthStencilState_NoZBuffer,
			AlphaBlendState_NoBlend, // depth stencil, blend states
			"VoxelGrid_VPL_Propogation_NoOcclusion_Tech",
			commonDefines);

		EffectManager::Instance()->RegisterVoxelEffect(pEffect);
	}

	// Propagate Indirect light through voxel grid
	{
		D3D_SHADER_MACRO defines[] =
		{
			"USE_OCCLUSION", "1",
			NULL, NULL
		};

		Effect *pEffect = new Effect(*mGraphicsDriver);
		pEffect->loadTechnique(
			NULL, NULL,
			NULL, NULL,
			NULL, NULL, // geometry shader
			"Shaders/VoxelGrid_VPL_Propogation_CS.hlsl", "main", // compute shader
			RasterizerState_SolidTriNoCull,
			DepthStencilState_NoZBuffer,
			AlphaBlendState_NoBlend, // depth stencil, blend states
			"VoxelGrid_VPL_Propogation_WithOcclusion_Tech",
			defines);

		EffectManager::Instance()->RegisterVoxelEffect(pEffect);
	}

	{
		Effect *pEffect = new Effect(*mGraphicsDriver);
		pEffect->mVSVertexFormat = VertexFormat_MinimalMesh;
		pEffect->loadTechnique(
			"Shaders/BasicQuad.hlsl", "VShader",
			NULL, NULL, // geometry shader
			"Shaders/VoxelGI_ApplyIndirectLight_PS.hlsl", "main",
			NULL, NULL, // compute shader
			RasterizerState_SolidTriNoCull,
			DepthStencilState_NoZBuffer, 
			AlphaBlendState_NoBlend, // depth stencil, blend states
			"VoxelGI_Indirect_Illumination_Apply",
			commonDefines);

		EffectManager::Instance()->RegisterVoxelEffect(pEffect);
	}

	//******************************************************
	// Particle Systems
	//******************************************************

	// Normal additive blended particle
	{
		Effect *pEffect = new Effect(*mGraphicsDriver);
		pEffect->mVSVertexFormat = VertexFormat_StdParticle;
		pEffect->loadTechnique(
			"Shaders/StdParticle_VS.hlsl", "main",
			"Shaders/PointToSprite_GS.hlsl", "GS", // geometry shader
			"Shaders/StdParticle_PS.hlsl", "main",
			NULL, NULL, // compute shader
			RasterizerState_SolidTriNoCull,
			DepthStencilState_ZBuffer, 
			AlphaBlendState_Additive, // depth stencil, blend states
			"StdParticle_Tech");

		//pEffect->m_effectDrawOrder = EffectDrawOrder::Last;
		EffectManager::Instance()->RegisterEffect(pEffect);
	}

	// Spawn particles and initialize them into the append/consume buffers
	{
		Effect *pEffect = new Effect(*mGraphicsDriver);
		pEffect->loadTechnique(
			NULL, NULL,
			NULL, NULL,
			NULL, NULL, // geometry shader
			"Shaders/StdParticle_Spawn_CS.hlsl", "main", // compute shader
			RasterizerState_SolidTriNoCull,
			DepthStencilState_NoZBuffer,
			AlphaBlendState_NoBlend, // depth stencil, blend states
			"Particle_Spawn_Tech",
			commonDefines);

		EffectManager::Instance()->RegisterEffect(pEffect);
	}

	// Update/move particles in a compute shader
	{
		Effect *pEffect = new Effect(*mGraphicsDriver);
		pEffect->loadTechnique(
			NULL, NULL,
			NULL, NULL,
			NULL, NULL, // geometry shader
			"Shaders/StdParticle_Update_CS.hlsl", "main", // compute shader
			RasterizerState_SolidTriNoCull,
			DepthStencilState_NoZBuffer,
			AlphaBlendState_NoBlend, // depth stencil, blend states
			"Particle_Update_Tech",
			commonDefines);

		EffectManager::Instance()->RegisterEffect(pEffect);
	}

	return true;
}

void Renderer::CleanD3D()
{
	if (meshVS) meshVS->Release();
	if (meshPS) meshPS->Release();
	if (mBasicMeshIL) mBasicMeshIL->Release();
	if (mFrameConstBuffer) mFrameConstBuffer->Release();
	if (mCamConstBuffer) mCamConstBuffer->Release();
	if (mLightConstBuffer) mLightConstBuffer->Release();
	if (mSingleLightBuffer) mSingleLightBuffer->Release(); 
	if (mEmitterConstBuffer) mEmitterConstBuffer->Release();
}

void Renderer::AddComponent(DrawComponent* comp, UINT8 drawOrder)
{
	if ((drawOrder & DrawOrder_Default) > 0)
		mDrawComponents.push_back(comp);
	else if ((drawOrder & DrawOrder_Last) > 0)
		mForwardDrawComponents.push_back(comp);
}

void Renderer::RemoveComponent(DrawComponent* comp, UINT8 drawOrder)
{

}

void Renderer::AddLight(LightComponent* light)
{
	mLights.push_back(light);
}

void Renderer::RemoveLight(LightComponent* light)
{

}

void Renderer::AddPointLight(PointLightComponent* comp)
{
	DbgAssert(mPointLights.size() <= MAX_POINT_LIGHTS, "Adding light would exceed max size in buffer!");
	mPointLights.push_back(comp);
	mLights.push_back(comp);
}

void Renderer::RemovePointLight(PointLightComponent* comp)
{

}

void Renderer::AddDirectionalLight(DirectionalLightComponent* comp)
{
	DbgAssert(mDirectionalLights.size() <= MAX_DIRECTIONAL_LIGHTS, "Adding light would exceed max size in buffer!");
	mDirectionalLights.push_back(comp);
	mLights.push_back(comp);
}

void Renderer::RemoveDirectionalLight(DirectionalLightComponent* comp)
{

}

void Renderer::AddSpotLight(SpotLightComponent* comp)
{
	DbgAssert(mSpotLights.size() <= MAX_SPOT_LIGHTS, "Adding light would exceed max size in buffer!");
	mSpotLights.push_back(comp);
	mLights.push_back(comp);
}

void Renderer::RemoveSpotLight(SpotLightComponent* comp)
{

}

void Renderer::SetRenderMode(unsigned mode)
{
#if HDR
	if (mode == 1)
	{
		// Toggle HDR
		if (mPPConsts.mHDR == 0)
			mPPConsts.mHDR = 1;
		else
			mPPConsts.mHDR = 0;
	}
#endif
	if (mode == 0)
	{
		mRenderMode = RenderMode_Default;
	}
	else if (mode == 1)
	{
		mRenderMode = RenderMode_DebugPosBuffer;
	}
	else if (mode == 2)
	{
		mRenderMode = RenderMode_DebugNormBuffer;
	}
	else if (mode == 3)
	{
		mRenderMode = RenderMode_DebugDiffuseBuffer;
	}
	else if (mode == 4)
	{
		mRenderMode = RenderMode_DebugShadowMap;
	}
	else if (mode == 5)
	{
		mRenderMode = RenderMode_DebugVoxelGrid;
	}
	else if (mode == 6)
	{
		mRenderMode = RenderMode_DeferredLightResult;
	}
}

void Renderer::ModifyExposure(float delta)
{
#if HDR
	if(mPPConsts.mHDR == 1)
		mPPConsts.mExposure += delta;
#endif
}

//void Renderer::AdjustNumVPLPropogations(int val)
//{
//	mNumVPLPropogations += 2 * val;
//
//	if (mNumVPLPropogations < 2)
//		mNumVPLPropogations = 2;
//}
