#pragma once
#include <windows.h>

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
#define DeclSingleton(C)		\
private:						\
		static C* sInstance;	\
public:							\
	static C* Instance(){		\
		return C::sInstance;	\
	}							\
	static void Construct();	

#define ImplSingleton(C)		\
	C* C::sInstance = 0;		\
	void C::Construct(){ C::sInstance = new C(); } 	


//**********************************************
//		Values
//**********************************************
//#ifndef PI
//#define PI 3.14159265358979323846
//#endif

/***************************************************************
* float randInRange(float min, float max)
*
* Description:
*	This generates a random value within a range
*
* Returns: A random float X where min <= X <= max
***************************************************************/
template<typename type>
type randInRange(type min, type max)
{
	if (min == max)
		return min;
	return min + S_CAST(type, rand()) / S_CAST(type, RAND_MAX / (max - min));
}





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
