#include "RandomGen.h"
#include <ctime>

ImplSingleton(RandomGen)

RandomGen::RandomGen()
{
	
}

void RandomGen::Init()
{
	srand(static_cast<unsigned>(time(NULL)));
}

PerlinSineTriple RandomGen::CreateSineTriple()
{
	PerlinSineTriple triple;
	triple.A = randInRange<float>(0, 0.65f);
	triple.B = randInRange<float>(0, 1 - triple.A);
	triple.C = 1.0f - triple.A - triple.B;
	triple.a = randInRange<float>(0.0f, 0.1f);
	triple.b = randInRange<float>(0.0f, 0.1f);
	triple.c = randInRange<float>(0.0f, 0.1f);

	return triple;
}