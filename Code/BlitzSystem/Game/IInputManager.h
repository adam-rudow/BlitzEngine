#pragma once
#include "../System.h"
#include "Utils/Utility.h"

class SYSTEM_EXPORT IInputManager
{
	DeclAbstractSingleton(IInputManager);

public:
	// gets windows input messages
	virtual bool GetInputMessages(UINT uMsg, WPARAM wParam) = 0;

	// updates keyInfos per frame
	virtual void Update() = 0;

	// returns true, as long as key pressed 
	virtual bool GetTriggerState(unsigned char keyCode) = 0;

	// returns true, exactly once, when key is pressed
	virtual bool GetSingleTriggerState(unsigned char keyCode) = 0;

	// gets mouse-position in screen-space
	virtual POINT GetMousePos(bool windowSpace = false) const = 0;

	// gets mouse-position in screen-space
	virtual POINT GetPrevMousePos() const = 0;

	// sets mouse-position in screen-space
	virtual void SetMousePos(POINT position, bool windowSpace = false) const = 0;

	// sets previous mouse-position in screen-space
	virtual inline void SetPrevMousePos(POINT prevPos) = 0;

	// toggle mouse-cursor
	virtual void ShowMouseCursor(bool show) = 0;
};