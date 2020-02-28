//#pragma once
//
//#include "../System.h"
//#include "Utils/Utility.h"
//
//
//
//class SYSTEM_EXPORT IGame
//{
//	DeclAbstractSingleton(IGame);
//
//public:
//	virtual bool Init()		= 0;
//	virtual void Quit()		= 0;
//	virtual void Update() = 0;
//	//virtual WPARAM GetQuitParam() const = 0;
//
//	virtual bool HandleMouseEvent(MSG* msg) = 0;
//	virtual bool HandleKeyEvent(MSG* msg) = 0;
//
//	// TODO: move this to a camera manager
//	virtual void AddCameraRotation(float xROt, float yRot) = 0;
//
//	virtual void LoadGame() = 0;
//};
//
//class SYSTEM_EXPORT BlitzGame : public IGame
//{
//public:
//
//	virtual bool Init() override { return true; }
//	virtual void Quit() override {}
//	virtual void Update() {}
//	//virtual WPARAM GetQuitParam() const = 0;
//
//	virtual bool HandleMouseEvent(MSG* msg) override { return false; }
//	virtual bool HandleKeyEvent(MSG* msg) override { return false; }
//
//	// TODO: move this to a camera manager
//	virtual void AddCameraRotation(float xROt, float yRot) override {}
//
//	virtual void LoadGame() override {}
//};