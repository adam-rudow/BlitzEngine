#pragma once
#include <windows.h>
#include <Utils/Utility.h>

#define NUM_KEY_INFOS 256

// InputManager
//
// Simple manager for keyboard-/ mouse-input (based on windows input).
class InputManager
{
	DeclSingleton(InputManager)

public:
	InputManager();

	// gets windows input messages
	bool GetInputMessages(UINT uMsg, WPARAM wParam);

	// updates keyInfos per frame
	void Update();

	// returns true, as long as key pressed 
	bool GetTriggerState(unsigned char keyCode);

	// returns true, exactly once, when key is pressed
	bool GetSingleTriggerState(unsigned char keyCode);

	// gets mouse-position in screen-space
	POINT GetMousePos(bool windowSpace=false) const;

	// gets mouse-position in screen-space
	POINT GetPrevMousePos() const { return mPrevMousePos; }

	// sets mouse-position in screen-space
	void SetMousePos(POINT position, bool windowSpace=false) const;

	// sets previous mouse-position in screen-space
	inline void SetPrevMousePos(POINT prevPos) { mPrevMousePos = prevPos; }

	// toggle mouse-cursor
	void ShowMouseCursor(bool show);

private:
	// sets key-state 
	void SetTriggerState(unsigned char keyCode, bool pressed);

	// array of keyInfos (each representing a bitmask, holding flags) 
	unsigned int keyInfos[NUM_KEY_INFOS];

	POINT mPrevMousePos;

	bool cursorVisible;

};
