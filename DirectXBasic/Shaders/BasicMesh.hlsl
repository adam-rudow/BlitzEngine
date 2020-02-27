#include "Constants.hlsl"
#include "Std_Structs.hlsl"

struct VS_In
{
	float3 mPosition : POSITION;
	float2 mTexCoord : TEXCOORD0;
	float3 mNormal	 : NORMAL;
	//float4 mColor : COLOR;
};

struct PS_In
{
	float4 mPos : SV_POSITION;
	float2 mTex : TEXCOORD0;
	//float4 mCol : COLOR;
};

PS_In VShader(BasicMesh_VS_In input /*float3 position : POSITION, float4 color : COLOR*/)
{
	BasicMesh_PS_In output;

	output.mPos = float4(input.mPosition, 1.0f);
	output.mPos = mul(output.mPos, mWorldTransformMatrix);
	output.mPos = mul(output.mPos, mViewProjMatrix);
	output.mTex = input.mTexCoord;
	//output.mCol = input.mColor;

	return output;
}

#if Deferred_Render

PS_Out_GBuffer PShader(BasicMesh_PS_In input)
{
	PS_Out_GBuffer pOut;
	float4 posW = input.mPos;
	float4 nNorm = float4(0, 0, 0, 0);
	float4 diffuse = DiffuseTexture.Sample(DefaultSampler, input.mTex);

	pOut.position = posW;
	pOut.normal = nNorm;
	pOut.diffuse = diffuse;
	return pOut;
#else

float4 PShader(BasicMesh_PS_In input) : SV_TARGET
{
	//return float4(input.mTex, (input.mTex.x +  input.mTex.y) / 2, 1.0f);// input.mCol;
	return DiffuseTexture.Sample(DefaultSampler, input.mTex);
#endif

}
