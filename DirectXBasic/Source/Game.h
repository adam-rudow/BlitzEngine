#pragma once
#include "BlitzSystem/Game/IGame.h"
#include "BlitzSystem/System.h"
#include "BlitzSystem/Application.h"
#include "BlitzSystem/Rendering/IRenderer.h"

#include "CameraComponent.h"


class FeatureTestGame : public Application
{

public:
	FeatureTestGame();
	~FeatureTestGame();

	void Init_Internal()		override;

	void LoadGame();

	inline void AddUpdateObject(Component* obj) { mUpdateObjects.push_back(obj); }

	//WPARAM GetQuitParam() const override { return retParam; }
	//Renderer* GetRenderer() { return mRenderer; }
	//AssetLoader& GetAssetLoader() { return mAssetLoader; }
	CameraComponent* GetMainCamera(){ return cam; }

	//inline double GetFrameTime() const { return 0/*System::Instance()->GetFrameTime()*/; }
	//inline float GetFrameRate() const { return 0/*System::Instance()->GetFrameRate()*/; }
	//inline double GetGameTime() const { return 0/*System::Instance()->GetSystemTime()*/; }

	bool HandleMouseEvent(MSG* msg) override;
	bool HandleKeyEvent(MSG* msg) override;

	void AddCameraRotation(float xRot, float yRot) override;

	void TickGameClock();

	//static Game& GetGameInstance() { return Game::mGameInstance; }

private:
	//void ProcessInput();
	void Update() override;
	//void GenerateOutput();

	void LoadWorldAxis();

	void LoadSponzaScene();
	void LoadSandsScene();
	void LoadCharizardScene();

	//void CheckForWindowsMessages();

	CameraComponent* cam;
	float mMouseWheelZ = 0;

	//Renderer* mRenderer;
	//AssetLoader mAssetLoader;

	std::vector<Component*> mUpdateObjects;

	//bool mQuitGame = false;
	//WPARAM retParam;

	double	mFrameTime;
	float	mFrameRate;
	double	mGameTime;
};
