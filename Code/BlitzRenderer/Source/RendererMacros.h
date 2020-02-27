#pragma once

#ifdef _RENDERERDLL
#define RENDERER_EXPORT	//__declspec(dllexport)
#else
#define RENDERER_EXPORT	//__declspec(dllimport)
#endif
