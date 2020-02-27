#include "System.h"
#include "Game/IGame.h"
#include "Game/IInputManager.h"
#include "Rendering/IRenderer.h"

//extern void ConstructInternalIGame();

#ifdef _SYSTEMDLL
ImplAbstractSingleton_Base(IGame)
ImplAbstractSingleton_Base(IRenderer)
ImplAbstractSingleton_Base(IInputManager)
/*IGame* IGame::sInstance = 0;										
IGame* IGame::Instance() 
{ 
	return IGame::sInstance; 
}*/					
//void IGame::Construct() 
//{ 
//	int i = 0;
//	int* p = &i;
//	//ConstructInternalIGame(); 
//}	
//void IGame::ResetInstance(IGame* newInstance) 
//{ 
//	//IGame::sInstance = newInstance; 
//}
#endif

