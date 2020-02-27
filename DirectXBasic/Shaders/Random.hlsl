#include "Constants.hlsl"


#if 0

#define RANDOM_IA 16807
#define RANDOM_IM 2147483647
#define RANDOM_AM (1.0f/float(RANDOM_IM))
#define RANDOM_IQ 127773u
#define RANDOM_IR 2836
#define RANDOM_MASK 123459876

	struct NumberGenerator {
		int seed; // Used to generate values.


				  // Generates the next number in the sequence.
		void Cycle() {
			seed ^= RANDOM_MASK;
			int k = seed / RANDOM_IQ;
			seed = RANDOM_IA * (seed - k * RANDOM_IQ) - RANDOM_IR * k;

			if (seed < 0)
				seed += RANDOM_IM;

			seed ^= RANDOM_MASK;
		}

		// Returns the current random float.
		float GetCurrentFloat() {
			Cycle();
			return RANDOM_AM * seed;
		}

		// Returns the current random int.
		int GetCurrentInt() {
			Cycle();
			return seed;
		}

		// Cycles the generator based on the input count. Useful for generating a thread unique seed.
		// PERFORMANCE - O(N)
		void Cycle(const uint _count) {
			for (uint i = 0; i < _count; ++i)
				Cycle();
		}

		// Returns a random float within the input range.
		float GetRandomFloat(const float low, const float high) {
			float v = GetCurrentFloat();
			return low * (1.0f - v) + high * v;
		}

		// Sets the seed
		void SetSeed(const uint value) {
			seed = int(value);
			Cycle();
		}
	};

#endif


float SampleSineTriple(PerlinSineTriple wave, float seed)
{
	float sampleWave = wave.A * sin(seed / wave.a)
		+ wave.B * sin(seed / wave.b)
		+ wave.C * sin(seed / wave.c);

	return 0.5f + 0.5f * sampleWave;
}

float randomInRange(float min, float max, PerlinSineTriple wave, int id)
{
	return min + (max - min) * SampleSineTriple(wave, gGameTime + gFrameTime * id * id);
}

float3 GetRandomDirection(int id, PerlinSineTriple phiWave, PerlinSineTriple zWave)
{

	float phi = randomInRange(0, 2 * PI, phiWave, id);
	float minTheta = cos(gThetaClamp);
	float zDistribution = randomInRange(minTheta, 1.0f, zWave, id); //generateRandomNumberInRange(minTheta, 1.0f, frameTime, id);
	float theta = acos(zDistribution);

	float3 randomizedVec = sin(theta)
		* (cos(phi) * float3(gEmitterWorldTransform[0][0], gEmitterWorldTransform[0][1], gEmitterWorldTransform[0][2])
			+ sin(phi) * float3(gEmitterWorldTransform[1][0], gEmitterWorldTransform[1][1], gEmitterWorldTransform[1][2]))
		+ cos(theta) * float3(gEmitterWorldTransform[2][0], gEmitterWorldTransform[2][1], gEmitterWorldTransform[2][2]);

	return normalize(randomizedVec);

}