#include "Constants.hlsl"
#include "DetailedMesh_Structs.hlsl"
#include "lighthelper.fx"
#include "shadowmaphelper.fx"


DetailedMesh_PS_In VShader(DetailedMesh_VS_In input)
{
	DetailedMesh_PS_In output;

	// Positions
	output.mPos = float4(input.mPosition, 1.0f);
	output.mPos = mul(output.mPos, mWorldTransformMatrix);
	output.mWorldPos = output.mPos.xyz;
	output.mPos = mul(output.mPos, mViewProjMatrix);

	// Normal + Tangent + Binormal
	output.mWorldNorm = mul(float4(input.mNormal, 0.0f), mWorldTransformMatrix).xyz;
	output.mWorldTangent = mul(float4(input.mTangent, 0.0f), mWorldTransformMatrix).xyz;
	//output.mWorldBinorm = cross(output.mWorldTangent, output.mWorldNorm);

	// Texture Coordinate
	output.mTex = input.mTexCoord;

	return output;
}

#if Deferred_Render

PS_Out_GBuffer PShader(DetailedMesh_PS_In input)
{
	PS_Out_GBuffer pOut;

	// Get position and albedo/diffuse color at this pixel
	float4 posW = float4(input.mWorldPos, 1.0f);
	float4 diffuse = gDiffuseMap.Sample(gDiffuseMapSampler, input.mTex);

	// Generate normal from normal map (in tangent space)
	float3 normFromMap = gBumpMap.Sample(gBumpMapSampler, input.mTex).xyz;
	normFromMap = 2.0f * normFromMap - 1.0f;
	float3 N = normalize(input.mWorldNorm);
	float3 T = normalize(input.mWorldTangent - dot(input.mWorldTangent, N) * N); // Ensure that interpolated tangent is perpendicular to normal
	float3 B = cross(N, T); //  Dont need to renormalize since N and T are perpendicularunit vectors

	float3x3 TBN = float3x3(T, B, N);
	float4 nNorm = float4( normalize( mul(normFromMap, TBN)), 0.0f);

	// Output values to multi render targets for deferred light pass later
	pOut.position = posW;
	pOut.normal = nNorm;
	pOut.diffuse = diffuse;
	return pOut;

#else

float4 PShader(DetailedMesh_PS_In input) : SV_TARGET
{

	float4 texCol = DiffuseTexture.Sample(DefaultSampler, input.mTex);

	// Generate normal from normal map (in tangent space)
	float4 normFromMap = gBumpMap.Sample(gBumpMapSampler, input.mTex);
	normFromMap = 2.0f * normFromMap - 1.0f;
	float3 N = normalize(input.mWorldNorm);
	float3 T = normalize(input.mWorldTangent - dot(input.mWorldTangent, N) * N); // Ensure that interpolated tangent is perpendicular to normal
	float3 B = cross(N, T);

	float3x3 TBN = float3x3(T, B, N);
	float4 nNorm = float4(normalize(mul(normFromMap, TBN), 0.0f);

	// Init lighting col with ambient
	float4 phongVal = float4(mAmbientLight, 0.0f);

	int i;
	for (i = 0; i < 8; ++i)
	{
		if (mPointLights[i].mEnabled > 0)
		{
			phongVal += ComputePointLight(mPointLights[i], input.mWorldPos, nNorm);
		}
	}

	for (i = 0; i < 2; ++i)
	{
		if (mDirectionalLights[i].mEnabled > 0)
		{
			phongVal += ComputeDirectionalLight(mDirectionalLights[i], input.mWorldPos, nNorm);
		}
	}

	for (i = 0; i < 4; ++i)
	{
		if (mSpotLights[i].mEnabled > 0)
		{
			phongVal += ComputeSpotLight(mSpotLights[i], input.mWorldPos, nNorm);
		}
	}

	// Do HDR stuffs
	if (mHDR)
	{
		//float mExposure = 0.5;
		phongVal = phongVal * texCol;
		float4 mapped = float4(1.0, 1.0, 1.0, 1.0) - exp(-phongVal * mExposure);
		return saturate(mapped);
	}

	phongVal = saturate(phongVal);
	return (phongVal * texCol);

#endif

}
