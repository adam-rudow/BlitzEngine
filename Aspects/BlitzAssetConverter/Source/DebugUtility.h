#pragma once
#include <string>
#include <iostream>

//**********************************************
//		Casts
//**********************************************

#ifndef S_CAST
#define S_CAST(type, val) static_cast<type>(val)
#endif

static void Print(std::string text)
{
	std::cout << text;
}