//#include "Game.h"
//#include "AssertDbg.h"
//#include <windows.h>
//#include <windowsx.h>
//#include <iostream>
//#include "PreCompiledHeader.h"
#include "Game.h"
#include "EffectManager.h"
#include "MeshComponent.h"
#include "ParticleSystem.h"
//#include "Profiler.h"
#include "Game/InputManager.h"

//[S1] Fix Dependecy
#include "AssetLoader.h"

#include <ctime>

//ImplAbstractSingleton(IGame, Game)
//void ConstructInternalIGame() 
//{
//	IGame* curInstance = IGame::Instance();
//	IGame::ResetInstance(new Game());
//	if (curInstance != nullptr)	
//	{ 						
//		delete(curInstance); 							
//	}													
//}

Game::Game() : IGame()
//, mRenderer(), //, mAssetLoader(*this)
{
	//GameInstance = this;
	mFrameTime = 0;
	mFrameRate = 0;
}

Game::~Game()
{
	//[S1] Fix Dependecy
	//delete(AssetLoader::Instance());
	//delete(System::Instance());

	//delete(Profiler::Instance());
}

bool Game::Init()
{
	//System::Construct();
	//[S1] Fix Dependecy
	AssetLoader::Construct();

	// Init Renderer
	//if (!mRenderer.Init(hInstance, nCmdShow))
	//	return false;

	//mRenderer = Renderer::Instance();

	LoadGame();

	return true;
}

bool is_big_endian(void)
{
	/*union {
		uint32_t i;
		char c[4];
	} bint = { 0x01020304 };

	return bint.c[0] == 1;*/
	int num = 1;
	if (*(char *)&num == 1)
	{
		OutputDebugString("\nLittle-Endian\n");
		return false;
	}
	else
	{
		OutputDebugString("Big-Endian\n");
		return true;
	}
}

void Game::Update()
{
	//TickGameClock();

	mFrameTime = System::Instance()->GetFrameTime();
	cam->Update(S_CAST(float, mFrameTime));

	for (size_t i = 0; i < mUpdateObjects.size(); ++i)
	{
		mUpdateObjects[i]->Update(S_CAST(float, mFrameTime));
	}
}

//typedef unsigned uint;
//uint EncodeNormalAsUInt(Vector3 normal, float dotWithBest)
//{
//	// uint encoded format: [bestDot: 31-26][xSign: 25][xDir: 24-17][ySign: 16][yDir: 15-9][zSign: 8][zdir: 8-0]
//	uint result = 0;
//
//	// Encode z in last 9 bits
//	uint z = uint(abs(normal.z * 255.0f));
//	result |= (z & 0xFF);
//	if (normal.z < 0)
//		result |= ((1 << 8) & 0x100); // Encode sign in first bit of section (9th bit)
//
//									  // Encode y in bits 18-10
//	uint y = uint(abs(normal.y * 255.0f));
//	y = ((y << 9) & 0x1FE00);
//	result |= y;
//	if (normal.y < 0)
//		result |= ((1 << 17) & 0x20000); // Encode sign in first bit of section
//
//										 // Encode x in bits 27-19
//	uint x = uint(abs(normal.x * 255.0f));
//	x = ((x << 18) & 0x3FC0000);
//	result |= x;
//	if (normal.x < 0)
//		result |= ((1 << 26) & 0x4000000); // Encode sign in first bit of section
//
//										   // Encode dot product with best tetrahedron face into top 5 bits of result
//	if (dotWithBest < 0) dotWithBest = 0; if (dotWithBest > 1) dotWithBest = 1;
//	uint dot = uint(dotWithBest * 31.0f);
//	result |= ((dot << 27) & 0xF8000000);
//
//	return result;
//}
//Vector3 DecodeNormalFromUInt(uint inNorm/*, out float bestDot*/)
//{
//	// uint encoded format: [bestDot: 31-26][xSign: 25][xDir: 24-17][ySign: 16][yDir: 15-9][zSign: 8][zdir: 8-0]
//	Vector3 normal;
//
//	normal.z = float(inNorm & 0xFF) / 255.0f;
//	normal.z *= (inNorm & 0x100) > 0 ? -1 : 1;
//
//	uint y = (inNorm & 0x1FE00);
//	normal.y = float(y >> 9) / 255.0f;
//	normal.y *= (inNorm & 0x20000) > 0 ? -1 : 1;
//
//	uint x = (inNorm & 0x3FC0000);
//	normal.x = float(x >> 18) / 255.0f;
//	normal.x *= (inNorm & 0x4000000) > 0 ? -1 : 1;
//
//	// Encode dot product with best tetrahedron face into top 5 bits of result
//	//uint dot = uint(saturate(dotWithBest) * 31);
//	//result |= (dot << 27);
//
//	return normal;
//}
//
//
//static Vector3 tetrahedronFaceNormals[4] =
//{
//	Vector3(0.0f, -0.57735026f, 0.81649661f),
//	Vector3(0.0f, -0.57735026f, -0.81649661f),
//	Vector3(-0.81649661f, 0.57735026f, 0.0f),
//	Vector3(0.81649661f, 0.57735026f, 0.0f)
//};
//
//int GetBestTetrahedronNormalIdx(Vector3 normal, float& dotMax)
//{
//	float dot0 = Vector3::Dot(tetrahedronFaceNormals[0], normal);
//	float dot1 = Vector3::Dot(tetrahedronFaceNormals[1], normal);
//	float dot2 = Vector3::Dot(tetrahedronFaceNormals[2], normal);
//	float dot3 = Vector3::Dot(tetrahedronFaceNormals[3], normal);
//
//	dotMax = max(dot0, max(dot1, max(dot2, dot3)));
//	if (dotMax == dot0)
//		return 0;
//	else if (dotMax == dot1)
//		return 1;
//	else if (dotMax == dot2)
//		return 2;
//	else
//		return 3;
//}


void Game::LoadGame()
{
	/*
	Vector3 dir(0, 1, 1);
	dir.Normalize();
	float dot = -100;
	Vector3 res = tetrahedronFaceNormals[GetBestTetrahedronNormalIdx(dir, dot)];
	*/

	//LoadWorldAxis();

	cam = new CameraComponent();
	cam->SetAsActive();

	static int SceneToLoad = 2;
	if (SceneToLoad == 1)
	{
		LoadSponzaScene();
	}
	else if (SceneToLoad == 2)
	{
		LoadSandsScene();
	}
	else if (SceneToLoad == 3)
	{
		LoadCharizardScene();
	}
	
	return;
	// Camera Light
	//PointLightComponent* pl1 = new PointLightComponent();
	////pl1->SetLightPosition(Vector3(-50, -30, 50));
	//pl1->SetLightPosition(Vector3(60, 0, -30));
	//pl1->SetDiffuseColor(Color::Red);
	//pl1->SetSpecularColor(Color::LightYellow);
	//pl1->SetSpecularPower(20.0f);
	//pl1->SetAttenuation(Vector3(200.0f, 500.0f, 500.0f));
	//pl1->SetRange(500);
	//pl1->SetEnabled(false);
	//cam->AttachComponent(pl1);

	float startY = -VoxelGrid::s_voxelSize / 2;
	//
	MeshComponent* mc = AssetLoader::Instance()->LoadMesh("Charizard.mesh");
	mc->SetScale(0.1f);
	mc->SetPosition(Vector3(-80, startY, 0));

	DirectionalLightComponent* sun = new DirectionalLightComponent();
	sun->SetLightPosition(Vector3(0, 4050, 0));
	sun->SetRotation(Concatenate(Quaternion(Vector3(1, 0, 0), 90.0f), Quaternion(sun->GetWorldTransform().GetForward(), 18.0f)));
	sun->SetRotation(Quaternion(Vector3(1, 0, 0), 90.0f));
	sun->AddRotation(Quaternion(sun->GetWorldTransform().GetRight(), 18.0f));
	sun->SetLightDirection(sun->GetWorldTransform().GetForward());
	sun->SetDiffuseColor(Vector3(0.9f, 0.75f, 0.6f));
	//sun->SetDiffuseColor(Vector3(1.8f, 1.5f, 1.2f));
	sun->SetSpecularColor(Vector3(0.3f, 0.3f, 0.3f));
	sun->SetSpecularPower(10.0f);
	sun->SetCastShadows(true);
	sun->SetEnabled(true);
	

	MeshComponent* crate = AssetLoader::Instance()->LoadMesh("crate.mesh");
	//crate->SetRotation(Quaternion(Vector3::UnitY, 15));
	crate->SetScale(0.0001f);
	//crate->SetPosition(Vector3(100, 0, 0));

	//MeshComponent* floor = AssetLoader::Instance()->LoadMesh("cobblePlane.obj");
	////floor->SetMesh("cobblePlane.obj");
	//floor->SetPosition(Vector3(0, startY, -50));
	//
	//MeshComponent* house = AssetLoader::Instance()->LoadMesh("GothicChurch.fbx"); // new MeshComponent(*this);
	////house->SetMesh("gothicChurch.obj");
	//house->SetScale(0.5f);
	//house->SetRotation(Quaternion(Vector3::UnitY, 30.0f));
	//house->SetPosition(Vector3(750, startY, 350));

	/*MeshComponent* crate = AssetLoader::Instance()->LoadMesh("sand_location.mesh");
	crate->SetRotation(Quaternion(Vector3::UnitY, 15));
	crate->SetScale(0.01f);
	crate->SetPosition(Vector3(100, 0, 0));*/

	//MeshComponent* crate2 = AssetLoader::Instance()->LoadMesh("sand_location.fbx");
	//crate2->SetRotation(Quaternion(Vector3::UnitY, 15));
	//crate2->SetPosition(Vector3(-100, 0, 0));	

	
	/*
	MeshComponent* ivy = new MeshComponent(*this);
	ivy->SetMesh("Ivy2.obj");
	ivy->SetScale(0.5f);
	ivy->SetRotation(Quaternion(Vector3::UnitY, 180.0f));
	ivy->SetPosition(Vector3(0, -50, 20));

	SpotLightComponent* sl2 = new SpotLightComponent(*this);
	sl2->SetLightPosition(Vector3(-150, -50, 170));
	sl2->SetLightDirection(ivy->GetPosition() - sl2->GetPosition()); // Point at Charizard
	sl2->SetSpotAngle(30.0f);
	sl2->SetDiffuseColor(Color::Green);
	sl2->SetSpecularColor(Color::White);
	sl2->SetSpecularPower(20.0f);
	sl2->SetAttenuation(Vector3(500.0f, 600.0f, 0));
	sl2->SetEnabled(true);*/

	//*****************************
	// Particles
	//ParticleSystem* ps = new ParticleSystem(*this);
	//ps->SetPosition(Vector3(200, 0, 0));
	//ps->Initialize();

}

void Game::LoadSponzaScene()
{
	//[S1] Fix Dependecy
	MeshComponent* world = AssetLoader::Instance()->LoadMesh("Sponza_Scene.mesh");
	world->SetScale(0.02f);

	DirectionalLightComponent* sun = new DirectionalLightComponent();
	sun->SetLightPosition(Vector3(0, 4050, 0));
	sun->SetRotation(Concatenate(Quaternion(Vector3(1, 0, 0), 90.0f), Quaternion(sun->GetWorldTransform().GetForward(), 18.0f)));
	sun->SetRotation(Quaternion(Vector3(1, 0, 0), 90.0f));
	sun->AddRotation(Quaternion(sun->GetWorldTransform().GetRight(), 18.0f));
	sun->SetLightDirection(sun->GetWorldTransform().GetForward());
	sun->SetDiffuseColor(Vector3(0.9f, 0.75f, 0.6f));
	//sun->SetDiffuseColor(Vector3(1.8f, 1.5f, 1.2f));
	sun->SetSpecularColor(Vector3(0.3f, 0.3f, 0.3f));
	sun->SetSpecularPower(10.0f);
	sun->SetCastShadows(true);
	sun->SetEnabled(true);
}

void Game::LoadSandsScene()
{
	//[S1] Fix Dependecy
	MeshComponent* world = AssetLoader::Instance()->LoadMesh("sand_location.mesh");

	DirectionalLightComponent* sun = new DirectionalLightComponent();
	sun->SetLightPosition(Vector3(0, 1950, -1500));
	sun->SetRotation(Quaternion(Vector3(1, 0, 0), 35.0f));
	sun->SetLightDirection(sun->GetWorldTransform().GetForward());
	sun->SetDiffuseColor(Vector3(1.8f, 1.5f, 1.2f));
	sun->SetSpecularColor(Vector3(0.6f, 0.6f, 0.6f));
	sun->SetSpecularPower(10.0f);
	sun->SetCastShadows(true);
	sun->SetEnabled(true);
}

void Game::LoadCharizardScene()
{
	float startY = -VoxelGrid::s_voxelSize / 2;
	//
	MeshComponent* mc = AssetLoader::Instance()->LoadMesh("Charizard.mesh");
	mc->SetScale(0.1f);
	mc->SetPosition(Vector3(-80, startY, 0));

	DirectionalLightComponent* sun = new DirectionalLightComponent();
	sun->SetLightPosition(Vector3(0, 4050, 0));
	sun->SetRotation(Concatenate(Quaternion(Vector3(1, 0, 0), 90.0f), Quaternion(sun->GetWorldTransform().GetForward(), 18.0f)));
	sun->SetRotation(Quaternion(Vector3(1, 0, 0), 90.0f));
	sun->AddRotation(Quaternion(sun->GetWorldTransform().GetRight(), 18.0f));
	sun->SetLightDirection(sun->GetWorldTransform().GetForward());
	sun->SetDiffuseColor(Vector3(0.9f, 0.75f, 0.6f));
	//sun->SetDiffuseColor(Vector3(1.8f, 1.5f, 1.2f));
	sun->SetSpecularColor(Vector3(0.3f, 0.3f, 0.3f));
	sun->SetSpecularPower(10.0f);
	sun->SetCastShadows(true);
	sun->SetEnabled(true);


	MeshComponent* crate = AssetLoader::Instance()->LoadMesh("crate.mesh");
	//crate->SetRotation(Quaternion(Vector3::UnitY, 15));
	crate->SetScale(0.0001f);
}

void Game::LoadWorldAxis()
{
	//[S1] Fix Dependecy
	MeshComponent* mc = nullptr;// AssetLoader::Instance()->LoadMesh("axis.obj"); //new MeshComponent(*this);
	mc->SetScale(2.0f);
	mc->SetPosition(Vector3(0, 0, 0));	
}

bool Game::HandleMouseEvent(MSG* msg)
{
	//WPARAM fwKeys = GET_KEYSTATE_WPARAM(msg->wParam);
	if (msg->message == WM_MOUSEWHEEL) // mouse wheel
	{
		//SetFocus(msg->hwnd);
		////GET_KEYSTATE_WPARAM(msg->wParam);
		//short z = (short)HIWORD(msg->wParam);
		//std::string s = "" + z;
		//printf(s.c_str());
		//if (z / 120 > 0)
		//{
		//	cam->MoveForward();
		//}
		//if(((short)HIWORD(msg->wParam)) / 120 < 0)
		//{
		//	cam->MoveBack();
		//}
	}
	else 
	{
		Vector2 rotation2D;
		POINT currentMousePos = InputManager::Instance()->GetMousePos();
		POINT prevMousePos = InputManager::Instance()->GetPrevMousePos();
		if (InputManager::Instance()->GetTriggerState(VK_RBUTTON))
		{
			//OutputDebugString("Right Mouse Button");
			
			//if (InputManager::Instance()->GetTriggerState(VK_RBUTTON))
			{
				//InputManager::Instance()->ShowMouseCursor(false);
				rotation2D.x -= (currentMousePos.x - prevMousePos.x); //* 0.25f;
				rotation2D.y += (currentMousePos.y - prevMousePos.y); //* 0.25f;
				CLAMP(rotation2D.y, -80.0f, 80.0f);
				InputManager::Instance()->SetMousePos(prevMousePos);
			}

			if (rotation2D.LengthSq() > 0)
			{
				cam->RotateHorizontal(rotation2D.x);// / (Renderer::Window_Width));// *0.5f));
				cam->RotateVerticle(rotation2D.y);
			}
		}
		else
		{
			InputManager::Instance()->ShowMouseCursor(true);
			InputManager::Instance()->SetPrevMousePos(currentMousePos);
		}
	}
	

	return true;
}

bool Game::HandleKeyEvent(MSG* msg)
{
	// Check camera-specific movement
	cam->CheckInputMovement();

	TCHAR c = (TCHAR)msg->wParam;
	
	if (c == VK_UP)
	{
		IRenderer::Instance()->ModifyExposure(0.2f);
	}
	else if (c == VK_DOWN)
	{
		IRenderer::Instance()->ModifyExposure(-0.2f);
	}
	else if (c == '.')
	{
		EffectManager::Instance()->AdjustVPLPropogationInterations(2);
	}
	else if (c == ',')
	{
		EffectManager::Instance()->AdjustVPLPropogationInterations(-2);
	}
	else if (c == 0x31) // 1 key
	{
		IRenderer::Instance()->SetRenderMode(1);
	}
	else if (c == 0x32) // 2 key
	{
		IRenderer::Instance()->SetRenderMode(2);
	}
	else if (c == 0x33) // 3 key
	{
		IRenderer::Instance()->SetRenderMode(3);
	}
	else if (c == 0x34) // 4 key
	{
		IRenderer::Instance()->SetRenderMode(4);
	}
	else if (c == 0x35) // 5 key
	{
		IRenderer::Instance()->SetRenderMode(5);
	}
	else if (c == 0x36) // 6 key
	{
		IRenderer::Instance()->SetRenderMode(6);
	}
	else if (c == 0x37) // 7 key
	{
		//mRenderer.SetRenderMode(1);
	}
	else if (c == 0x38) // 8 key
	{
		//mRenderer.SetRenderMode(1);
	}
	else if (c == 0x39) // 9 key
	{
		//mRenderer.SetRenderMode(1);
	}
	else if (c == 0x30) // 0 key
	{
		IRenderer::Instance()->SetRenderMode(0);
	}
	else if (c == 'l' && cam)
	{
		// toggle cam light
		PointLightComponent* light = dynamic_cast<PointLightComponent*>(cam->GetChildComponent(0));
		if (light)
		{
			bool e = light->GetLightData().mEnabled != 0;
			light->SetEnabled(!e);
		}
	}
	else if (msg->wParam == 0x1B) // Escape key
	{
		Quit();
	}
	else if (msg->wParam == 0x08) // Backspace key
	{

	}
	else if (msg->wParam == 0x09) // Tab key
	{

	}

	return true;
}

void Game::TickGameClock()
{
	//SYSTEMTIME time;
	//GetSystemTime(&time);
	//
	//unsigned short ms = time.wMilliseconds;
	//if (ms < mPrevSystemTime)
	//	ms += 1000;
	//
	//mFrameTime = S_CAST(double, ms - mPrevSystemTime ) / 1000.0f;
	//mGameTime += mFrameTime;
	//mPrevSystemTime = ms;
	//mFrameRate = S_CAST(float, 1.0f / mFrameTime);

	// Moved to System::Tick()
	/*mFrameTime = Profiler::Instance()->QueryCPUFrameTime();
	mFrameRate = S_CAST(float, 1.0f / mFrameTime);
	mGameTime += mFrameTime;*/
	//[S1]
	System::Instance()->Tick();
	//[S1]
	float avgFrameRate = S_CAST(float, 1.0 / System::Instance()->GetAverageFrameTime());

	//char str[256];
	//sprintf_s(str, "Frame Time =%f ms\n", mFrameTime * 1000);
	//OutputDebugString(str);
	std::string frameTimeStr = "Frame Time: " + std::to_string(System::Instance()->GetAverageFrameTime() * 1000).substr(0, 5) + "ms";
	std::string fpsStr = "FPS: \t  " + std::to_string(avgFrameRate).substr(0, 5);

	IRenderer::Instance()->DrawString(frameTimeStr.c_str(), 20.0f, 50.0f);
	IRenderer::Instance()->DrawString(fpsStr.c_str(), 20.0f, 75.0f);
}

void Game::AddCameraRotation(float xRot, float yRot)
{
	cam->RotateHorizontal(xRot);// / (Renderer::Window_Width));// *0.5f));
	cam->RotateVerticle(yRot);
}

void Game::Quit()
{
	//mQuitGame = true;
}