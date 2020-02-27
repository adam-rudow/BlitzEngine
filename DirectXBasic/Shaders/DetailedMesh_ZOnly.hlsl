#include "Constants.hlsl"
#include "DetailedMesh_Structs.hlsl"


DetailedMesh_PS_In VShader(DetailedMesh_VS_In input /*float3 position : POSITION, float4 color : COLOR*/)
{
	DetailedMesh_PS_In output;

	// Positions
	output.mPos = float4(input.mPosition, 1.0f);
	output.mPos = mul(output.mPos, mWorldTransformMatrix);
	output.mWorldPos = output.mPos.xyz;
	output.mPos = mul(output.mPos, mLightWVP);// mViewProjMatrix);

	// Normal + Tangent + Binormal
	output.mWorldNorm = mul(float4(input.mNormal, 0.0f), mWorldTransformMatrix).xyz;
	//output.mWorldTangent = mul(float4(input.mTangent, 0.0f), mWorldTransformMatrix).xyz;
	//output.mWorldBinorm = cross(output.mWorldTangent, output.mWorldNorm);

	// Texture Coordinate
	output.mTex = output.mPos.zw;

	return output;
}

float4 PShader(DetailedMesh_PS_In pIn) : SV_TARGET
{
	float v = pIn.mTex.x / pIn.mTex.y;
	float4 combinedColor = float4(v,v,v,v);

	return combinedColor;

}
