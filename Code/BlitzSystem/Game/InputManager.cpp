#include "InputManager.h"
#include "../System.h"
#include "../Application.h"
#include "IGame.h"

#define KEY_PRESSED 1       // flag, indicating that key was pressed
#define KEY_QUERIED 2       // flag, indicating that key was queried
#define KEY_MULTI_PRESSED 4 // flag, indicating that key was pressed more than once

//ImplSingleton(InputManager)

IInputManager* ConstructInputManager()
{
	IInputManager::StaticConstruct<InputManager>();
	return IInputManager::Instance();
}

InputManager::InputManager() : cursorVisible(true)
{
	memset(keyInfos, 0, NUM_KEY_INFOS*sizeof(unsigned int));

	mPrevMousePos = GetMousePos();
}

bool InputManager::GetInputMessages(UINT uMsg, WPARAM wParam)
{
	switch(uMsg)
	{
		// keyboard key is pressed down  
		case WM_KEYDOWN:
		{
			SetTriggerState(wParam,true);
			return true;
		}

		// keyboard key is released
		case WM_KEYUP:
		{
			SetTriggerState(wParam,false);
			return true;
		}

		// left mouse-button is pressed down
		case WM_LBUTTONDOWN:
		{
			SetTriggerState(VK_LBUTTON,true);
			return true;
		}		
    
		// left mouse-button is released
		case WM_LBUTTONUP:
		{
			SetTriggerState(VK_LBUTTON,false);
			return true;
		}

		// right mouse-button is pressed down
		case WM_RBUTTONDOWN:
		{
			SetTriggerState(VK_RBUTTON,true);
			return true;
		}

		// right mouse-button is released
		case WM_RBUTTONUP:
		{
			SetTriggerState(VK_RBUTTON,false);
			return true;
		}
	}

	return false;
}

void InputManager::Update()
{
	CheckForWindowsMessages();

	for(unsigned int i=0; i<NUM_KEY_INFOS; i++)
	{
		if(keyInfos[i] & KEY_QUERIED)
			keyInfos[i] |= KEY_MULTI_PRESSED;
	}
}

void InputManager::SetTriggerState(unsigned char keyCode, bool pressed)
{
	if(pressed)
	{
		keyInfos[keyCode] |= KEY_PRESSED;
	}
	else
	{
		keyInfos[keyCode] &= ~KEY_PRESSED;
		keyInfos[keyCode] &= ~KEY_QUERIED;
		keyInfos[keyCode] &= ~KEY_MULTI_PRESSED;
	}
}

bool InputManager::GetTriggerState(unsigned char keyCode)
{
	return (keyInfos[keyCode] & KEY_PRESSED);
}

bool InputManager::GetSingleTriggerState(unsigned char keyCode)
{
	if((keyInfos[keyCode] & KEY_PRESSED) && (!(keyInfos[keyCode] & KEY_MULTI_PRESSED)))
	{
		keyInfos[keyCode] |= KEY_QUERIED;
		return true;
	}
	return false; 
}

POINT InputManager::GetMousePos(bool windowSpace) const
{ 
	POINT pos;
	GetCursorPos(&pos);
	if(windowSpace)
		MapWindowPoints(NULL, Application::Instance()->GetHWnd(), &pos, 1);
	return pos;
}

void InputManager::SetMousePos(POINT position, bool windowSpace) const
{
  if(windowSpace)
    MapWindowPoints(Application::Instance()->GetHWnd(), NULL, &position, 1);
  SetCursorPos(position.x, position.y);
}

void InputManager::ShowMouseCursor(bool show) 
{
	if(cursorVisible == show)
		return;

	int counter;
	if(show)
	{
		do
		{
			counter = ShowCursor(true);
		}
		while(counter<0);
	}
	else
	{
		do
		{
		  counter = ShowCursor(false);
		}
		while(counter>=0);
	}

	cursorVisible = show;
}

void InputManager::CheckForWindowsMessages()
{
	// this struct holds Windows event messsages
	MSG msg = { 0 };

	if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
	{
		bool fHandled = false;
		//if (msg.message >= WM_MOUSEFIRST && msg.message <= WM_MOUSELAST)
		//	fHandled = HandleMouseEvent(&msg);
		//if(msg.message >= WM_KEYFIRST && msg.message <= WM_KEYLAST)
		fHandled = Application::Instance()->HandleKeyEvent(&msg);

		//if (InputManager::Instance()->GetTriggerState(VK_RBUTTON) || InputManager::Instance()->GetTriggerState(VK_LBUTTON))
		{
			//OutputDebugString("Right Mouse Button\n");
			Application::Instance()->HandleMouseEvent(&msg);
		}

		// translate keystroke messages into the right format
		TranslateMessage(&msg);

		// send the message to the WindowProc function
		DispatchMessage(&msg);

		if (msg.message == WM_QUIT)
		{
			Application::Instance()->Quit(msg.wParam);

			// return this part of the WM_QUIT message to Windows
			//retParam = msg.wParam;
		}
	}
}
