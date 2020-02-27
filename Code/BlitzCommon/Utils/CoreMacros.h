#pragma once

//**********************************************
//		Core Macros
//**********************************************
#ifdef _COMMON_DLL
#define BDLLEXP //__declspec(dllexport)
#else
#define BDLLEXP //__declspec(dllimport)
#endif

