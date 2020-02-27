#pragma once

#ifdef _ASSET_LOADER_DLL
#define ASSET_LOADER_EXPORT	//__declspec(dllexport)
#define ASSET_LOADER_EXTERN extern
#else
#define ASSET_LOADER_EXPORT	//__declspec(dllimport)
#define ASSET_LOADER_EXTERN
#endif

#ifdef _ASSET_PROCESSOR
#define ASSET_PROCESSOR
#endif
