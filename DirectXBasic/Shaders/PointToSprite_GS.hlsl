#ifndef PointToSprite_GS
#define PointToSprite_GS

#include "Constants.hlsl"
#include "Std_Structs.hlsl"

[maxvertexcount(4)]
void GS(point Std_Particle_VS_IN input[1], inout TriangleStream<Std_Particle_PS_IN> triangleStream)
{
	if (input[0].mLifespan > 0)
	{
		float3 eyePos = mCamWorldPos;
		float3 vertsWS[4];
		float2 texCoords[4] = { float2(0, 0), float2(1, 0), float2(0, 1), float2(1, 1) };

		// Create look-at from particle to camera
		float3 toCam = normalize(eyePos - input[0].mPos);
		float3 right = normalize(cross(float3(0, 1, 0), toCam));
		float3 up = normalize(cross(toCam, right));

		// Top left corner (from camera's perspective)
		vertsWS[0] = input[0].mPos + (input[0].mSize.x / 2) * right + (input[0].mSize.y / 2) * up;

		// Top right corner (from camera's perspective)
		vertsWS[1] = input[0].mPos - (input[0].mSize.x / 2) * right + (input[0].mSize.y / 2) * up;

		// Bottom left corner (from camera's perspective)
		vertsWS[2] = input[0].mPos + (input[0].mSize.x / 2) * right - (input[0].mSize.y / 2) * up;

		// Bottom right corner (from camera's perspective)
		vertsWS[3] = input[0].mPos - (input[0].mSize.x / 2) * right - (input[0].mSize.y / 2) * up;

		// Add generated points to out-stream and pass on properties for pixel shader
		Std_Particle_PS_IN psIN;
		for (int i = 0; i < 4; ++i)
		{
			psIN.mPos = mul(float4(vertsWS[i], 1.0f), mViewProjMatrix);
			psIN.mTexCoord = texCoords[i];
			psIN.mLifespan = input[0].mLifespan;
			triangleStream.Append(psIN);
		}
	}
}

#endif
