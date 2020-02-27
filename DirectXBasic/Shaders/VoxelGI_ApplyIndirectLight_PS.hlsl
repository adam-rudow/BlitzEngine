#ifndef Voxel_GI_Final_PS
#define Voxel_GI_Final_PS

#include "Constants.hlsl"
#include "Std_Structs.hlsl"
#include "VoxelGI_Structs.hlsl"
#include "lighthelper.fx"

// Source geometry and lighting data
Texture2DArray redSHCoeffTextureArrayIn		: register(t3);
Texture2DArray greenSHCoeffTextureArrayIn	: register(t4);
Texture2DArray blueSHCoeffTextureArrayIn	: register(t5);
SamplerState redSampler 					: register(s3);
SamplerState greenSampler 					: register(s4);
SamplerState blueSampler 					: register(s5);

// G-Buffers from deferred rendering need to be bound also
// t0 -> position buffer
// t1 -> diffuse (or deferred color?)
// t2 -> normal


float4 main(BasicMesh_PS_In pIn) : SV_TARGET
{
	float4 outColor = float4(0, 0, 0, 0);

	// Get position from G-Buffer
	float3 position = gDepthMap.Sample(gDepthMapSampler, pIn.mTex).xyz; // sample2D(gDepthMapSampler, pIn.iColor.xy, gDepthMap).xyz;

	// Get diffuse/albedo?
	float3 baseColor = gShadowMap.Sample(gShadowMapSampler, pIn.mTex).xyz; // sample2D(gShadowMapSampler, pIn.iColor.xy, gShadowMap).rgb;

	// Get deferred lighting Color
	float3 litColor = gAdditionalDiffuseMap.Sample(gAdditionalDiffuseMapSampler, pIn.mTex).xyz; // sample2D(gAdditionalDiffuseMapSampler, pIn.iColor.xy, gAdditionalDiffuseMap).rgb;

	// Get normal from G-Buffer
	float3 normal = normalize(gDiffuseMap.Sample(gDiffuseMapSampler, pIn.mTex).xyz); //; sample2D(gDiffuseMapSampler, pIn.iColor.xy, gDiffuseMap).xyz);


	// Get voxel grid offset
	float3 gridOffset = (position - gxyzVoxelGridCenter_wUseOcclusion.xyz) / gxVoxelCellSize_yVoxelDimension_zw.x;

	// Get 3D texture coordinate for 2DTextureArrays
	int gridHalfDimension = gxVoxelCellSize_yVoxelDimension_zw.y / 2.0;
	float3 texCoord = float3(gridHalfDimension, gridHalfDimension, gridHalfDimension) + float3(0.5f, 0.5f, 0) +gridOffset;
	texCoord.xy /= gxVoxelCellSize_yVoxelDimension_zw.y;
	//float3 texCoord = float3(16.5f, 16.5f, 16.0f) + gridOffset;
	//texCoord.xy /= 32.0f;

	// Get texCoords for trilinear sampling
	int lowZ = floor(texCoord.z);
	int highZ = min(lowZ + 1, gxVoxelCellSize_yVoxelDimension_zw.y - 1);
	float highZWeight = texCoord.z - lowZ;
	float lowZWeight = 1.0f - highZWeight;
	float3 tcLow = float3(texCoord.xy, lowZ);
	float3 tcHigh = float3(texCoord.xy, highZ);

	// Do trilinear sampling
	float4 redSHCoeffs	= lowZWeight	* redSHCoeffTextureArrayIn.Sample(redSampler, tcLow)	//sample2D(redSampler, tcLow, redSHCoeffTextureArrayIn)
						+ highZWeight	* redSHCoeffTextureArrayIn.Sample(redSampler, tcHigh);//sample2D(redSampler, tcHigh, redSHCoeffTextureArrayIn); 

	float4 greenSHCoeffs = lowZWeight	* greenSHCoeffTextureArrayIn.Sample(greenSampler, tcLow)	//sample2D(greenSampler, tcLow, greenSHCoeffTextureArrayIn)
						+ highZWeight	* greenSHCoeffTextureArrayIn.Sample(greenSampler, tcHigh);//sample2D(greenSampler, tcHigh, greenSHCoeffTextureArrayIn); 

	float4 blueSHCoeffs = lowZWeight	* blueSHCoeffTextureArrayIn.Sample(blueSampler, tcLow)		//sample2D(blueSampler, tcLow, blueSHCoeffTextureArrayIn)
						+ highZWeight	* blueSHCoeffTextureArrayIn.Sample(blueSampler, tcHigh);//sample2D(blueSampler, tcHigh, blueSHCoeffTextureArrayIn);


																				// Calculate clamped cosine lobe SH-coefficients for surface normal. 
	float4 surfaceNormalLobe = ClampedCosineSHCoeffs(normal/*float3(1,0,0)*/);

	// Perform diffuse SH-lighting. 
	float3 diffuseGlobalIllum;
	diffuseGlobalIllum.r = dot(redSHCoeffs, surfaceNormalLobe);
	diffuseGlobalIllum.g = dot(greenSHCoeffs, surfaceNormalLobe);
	diffuseGlobalIllum.b = dot(blueSHCoeffs, surfaceNormalLobe);

	diffuseGlobalIllum = max(diffuseGlobalIllum, float3(0.0f, 0.0f, 0.0f));
	diffuseGlobalIllum /= PI;

	diffuseGlobalIllum = pow(diffuseGlobalIllum, 0.25f);

	outColor = float4(diffuseGlobalIllum* baseColor, 1.0f) + float4(litColor, 0);
	//outColor = float4(diffuseGlobalIllum * baseColor, 1.0f) + float4(litColor, 0);// / 2.0f;
	return outColor;


}

#endif
