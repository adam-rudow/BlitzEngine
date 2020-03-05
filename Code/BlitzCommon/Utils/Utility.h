#pragma once
#include <windows.h>
#include "CoreMacros.h"
#include "Logging/Log.h"

//*********************************************
// TODO:
// Abstract this file out in an organized format 
//

//**********************************************
//		Assets
//**********************************************
enum AssetType
{
	AT_Mesh,
	AT_Diffuse,
	AT_Normal
};


//**********************************************
//		Lighting
//**********************************************
const unsigned MAX_POINT_LIGHTS = 8;
const unsigned MAX_DIRECTIONAL_LIGHTS = 2;
const unsigned MAX_SPOT_LIGHTS = 4;

//**********************************************
//		Print
//**********************************************
static void BEPrint(const char* info)
{
	//char str[256];
	//sprintf_s(str, info);
	OutputDebugString(info);
	BZ_CORE_TRACE(info);
}

//**********************************************
//		Casts
//**********************************************

#ifndef S_CAST
#define S_CAST(type, val) static_cast<type>(val)
#endif

#ifndef R_CAST
#define R_CAST(type, val) reinterpret_cast<type>(val)
#endif


//**********************************************
//		Singleton Defines
//**********************************************

#define DeclExternClassFunction(Type, FunctionName, ClassNameSuffix)\
{																	\
	extern Type FunctionName ##C();									\
}

#define DeclSingleton(C)		\
private:						\
	static C* sInstance;		\
public:							\
	static C* Instance(){		\
		return C::sInstance;	\
	}							\
	static void Construct();

//#define DeclAbstractSingleton(C)	\
//private:						\
//	static C* sInstance;		\
//public:							\
//	static C* Instance(){		\
//		return C::sInstance;	\
//	}							\
//	static void Construct() { extern void ConstructInternal##C(); }\
//	static void ResetInstance(C* newInstance) { C::sInstance = newInstance; }

#define DeclAbstractSingleton(C)	\
private:							\
	static C* sInstance;			\
public:								\
	static C* Instance();			\
	template <typename T>			\
	static void StaticConstruct()	\
	{								\
		C::sInstance = new T();		\
	}

/**
* Declare a singleton where only the Instance members are exported in to a dll
*/
//#define DeclSingletonDllExport(C)		\
//private:								\
//	static C* sInstance;				\
//public:									\
//	BDLLEXP static C* Instance(){		\
//		return C::sInstance;			\
//	}									\
//	BDLLEXP static void Construct();	

#define ImplSingleton(C)							\
	C* C::sInstance = 0;							\
	void C::Construct(){ C::sInstance = new C(); } 	
/*
#define ImplAbstractSingleton_Base(C)							\
	C* C::sInstance = 0;										\
	C* C::Instance() { return C::sInstance; }					\
	void C::Construct() { extern void ConstructInternal##C(); }	\
	void C::ResetInstance(C* newInstance) { C::sInstance = newInstance; }
*/	

#define ImplAbstractSingleton_Base(C)	\
	C* C::sInstance = 0;				\
	C* C::Instance()					\
	{									\
		return C::sInstance;			\
	}


#define ImplAbstractSingleton(ParentClass, ChildClass)		\
	void ConstructInternal##ParentClass() {					\
		ParentClass* curInstance = ParentClass::Instance();	\
		ParentClass::ResetInstance(new ChildClass());		\
		if (curInstance != nullptr)	{ 						\
			delete(curInstance); 							\
		}													\
	}
	///ParentClass* ParentClass::sInstance = 0;				\



//**********************************************
//		Values
//**********************************************
//#ifndef PI
//#define PI 3.14159265358979323846
//#endif



//**********************************************
//		Asserts
//**********************************************
#if _DEBUG
#include <cassert>
#define B_ASSERT(condition, log)												\
{																				\
	if (!(condition))															\
	{																			\
		std::cerr << "Assertion failed at " << __FILE__ << ":" << __LINE__;		\
		std::cerr << " inside " << __FUNCTION__ << std::endl;					\
		std::cerr << "Condition: " << #condition;								\
		std::cerr << log << "\n";												\
		abort();																\
	}																			\
}
#else
#define B_ASSERT(condition) (condition)
#endif
