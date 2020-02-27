#pragma once

#ifdef _SYSTEMDLL
	#define SYSTEM_EXPORT	//__declspec(dllexport)
	#define SYSTEM_EXTERN extern
#else
	#define SYSTEM_EXPORT	//__declspec(dllimport)
	#define SYSTEM_EXTERN
#endif
