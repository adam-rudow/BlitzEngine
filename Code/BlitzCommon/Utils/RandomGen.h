#pragma once

#include "../Math/math.h"
#include "Utility.h"

class BDLLEXP PerlinSineTriple
{
public:
	float A;
	float a;
	float B;
	float b;
	float C;
	float c;
	Vector2 PST_Pad;
}; // 32 bytes


class BDLLEXP RandomGen
{
	DeclSingleton(RandomGen)

public:
	RandomGen();

	static void Init();

	static PerlinSineTriple CreateSineTriple();

	/***************************************************************
	* float randInRange(float min, float max)
	*
	* Description:
	*	This generates a random value within a range
	*
	* Returns: A random float X where min <= X <= max
	***************************************************************/
	template<typename type>
	static type randInRange(type min, type max)
	{
		if (min == max)
			return min;
		return min + S_CAST(type, rand()) / S_CAST(type, RAND_MAX / (max - min));
	}

private:
	int temp;

};