#include "Constants.hlsl"

struct VS_In
{
	float3 mPosition : POSITION;
	float2 mTexCoord : TEXCOORD0;
	float3 mNormal	 : NORMAL;
};

struct PS_In
{
	float4 mPos			: SV_POSITION;
	float3 mWorldNorm	: W_NORM;
	float3 mWorldPos	: W_POSITION;
	float2 mTex			: TEXCOORD1;
};

PS_In VShader(VS_In input /*float3 position : POSITION, float4 color : COLOR*/)
{
	PS_In output;

	// Positions
	output.mPos = float4(input.mPosition, 1.0f);
	output.mPos = mul(output.mPos, mWorldTransformMatrix);
	output.mWorldPos = output.mPos.xyz;
	output.mPos = mul(output.mPos, mLightWVP);// mViewProjMatrix);

	// Normal
	float4 tempNorm = mul(float4(input.mNormal, 0.0f), mWorldTransformMatrix);
	output.mWorldNorm = tempNorm.xyz;

	// Texture Coordinate
	output.mTex = output.mPos.zw;

	return output;
}

float4 PShader(PS_In pIn) : SV_TARGET
{
	float v = pIn.mTex.x / pIn.mTex.y;
	float4 combinedColor = float4(v,v,v,v);

	return combinedColor;

}
