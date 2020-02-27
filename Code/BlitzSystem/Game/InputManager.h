#pragma once
#include <windows.h>
#include "Utils/Utility.h"
#include "IInputManager.h"
#include "../SystemDefines.h"

#define NUM_KEY_INFOS 256

// InputManager
//
// Simple manager for keyboard-/ mouse-input (based on windows input).
class SYSTEM_EXPORT InputManager : public IInputManager
{
	//DeclSingleton(InputManager)

public:
	InputManager();

	// gets windows input messages
	bool GetInputMessages(UINT uMsg, WPARAM wParam) override;

	// updates keyInfos per frame
	void Update() override;

	// returns true, as long as key pressed 
	bool GetTriggerState(unsigned char keyCode) override;

	// returns true, exactly once, when key is pressed
	bool GetSingleTriggerState(unsigned char keyCode) override;

	// gets mouse-position in screen-space
	POINT GetMousePos(bool windowSpace=false) const override;

	// gets mouse-position in screen-space
	POINT GetPrevMousePos() const override { return mPrevMousePos; }

	// sets mouse-position in screen-space
	void SetMousePos(POINT position, bool windowSpace=false) const override;

	// sets previous mouse-position in screen-space
	inline void SetPrevMousePos(POINT prevPos) override { mPrevMousePos = prevPos; }

	// toggle mouse-cursor
	void ShowMouseCursor(bool show) override;

private:
	// sets key-state 
	void SetTriggerState(unsigned char keyCode, bool pressed);

	void CheckForWindowsMessages();

	// array of keyInfos (each representing a bitmask, holding flags) 
	unsigned int keyInfos[NUM_KEY_INFOS];

	POINT mPrevMousePos;

	bool cursorVisible;

};
