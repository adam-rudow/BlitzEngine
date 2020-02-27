#include "Constants.hlsl"
#include "Std_Structs.hlsl"


//struct BasicQuad_VS_In
//{
//	float3 mPosition : POSITION;
//	float2 mTexCoord : TEXCOORD0;
//	//float3 mNormal	 : NORMAL;
//	//float4 mColor : COLOR;
//};

struct PS_In
{
	float4 mPos : SV_POSITION;
	float2 mTex : TEXCOORD0;
	//float4 mCol : COLOR;
};

PS_In VShader(BasicQuad_VS_In input)
{
	BasicMesh_PS_In output;

	output.mPos = float4(input.mPosition, 1.0f);
	output.mTex = input.mTexCoord;

	return output;
}

float4 PShader(BasicMesh_PS_In input) : SV_TARGET
{

	return gShadowMap.Sample(gShadowMapSampler, input.mTex);
}

