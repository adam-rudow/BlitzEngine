#pragma once

#include "../System.h"
#include "Utils/Utility.h"



class SYSTEM_EXPORT IGame
{
	DeclAbstractSingleton(IGame);

public:
	//IGame() {}
	//virtual ~IGame() {}

	virtual bool Init()		= 0;
	virtual void Quit()		= 0;
	virtual void Update() = 0;
	//virtual WPARAM GetQuitParam() const = 0;

	virtual bool HandleMouseEvent(MSG* msg) = 0;
	virtual bool HandleKeyEvent(MSG* msg) = 0;

	// TODO: move this to a camera manager
	virtual void AddCameraRotation(float xROt, float yRot) = 0;

	virtual void LoadGame() = 0;
};
