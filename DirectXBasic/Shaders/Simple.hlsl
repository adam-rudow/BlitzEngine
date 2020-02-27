#include "Constants.hlsl"

struct VS_In
{
	float3 mPosition : SV_POSITION;
	float4 mColor : COLOR;
};

struct PS_In
{
	float4 mPos : SV_POSITION;
	float4 mCol : COLOR;
};

PS_In VShader(VS_In input /*float3 position : POSITION, float4 color : COLOR*/)
{
	PS_In output;

	output.mPos = float4(input.mPosition, 1.0f);
	output.mPos = mul(output.mPos, mWorldTransformMatrix);
	output.mPos = mul(output.mPos, mViewProjMatrix);
	output.mCol = input.mColor;

	return output;
}

float4 PShader(PS_In input) : SV_TARGET
{
	return input.mCol;
}










//struct VOut
//{
//	float4 position : SV_POSITION;
//	float4 color : COLOR;
//};
//
//VOut VShader(float4 position : POSITION, float4 color : COLOR)
//{
//	VOut output;
//
//	output.position = position;
//	output.color = color;
//
//	return output;
//}
//
//float4 PShader(float4 position : SV_POSITION, float4 color : COLOR) : SV_TARGET
//{
//	return color;
//}