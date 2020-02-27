#pragma once

#ifdef _MATHDLL
	#define MATH_EXPORT //__declspec(dllexport)
#else
	#define MATH_EXPORT //__declspec(dllimport)
#endif
