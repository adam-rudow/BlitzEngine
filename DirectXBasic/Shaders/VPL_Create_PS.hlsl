#ifndef VPL_Create_PS_cg
#define VPL_Create_PS_cg

#include "Constants.hlsl"
#include "VoxelGI_Structs.hlsl"
#include "lighthelper.fx"
#include "shadowmaphelper.fx"



RWStructuredBuffer<Voxel> voxelGridBuffer : register(u3);

VPL_MRT_PS_OUT main(VPLCreate_PS_IN pIn, float4 screenSpace : SV_Position)
{
	VPL_MRT_PS_OUT pOut;

	// Get voxel grid position
	int3 voxelIdx = int3(pIn.iPosH.xy, pIn.iInstanceID);
	int bufferIdx = voxelIdx.x
		+ (voxelIdx.y * gxVoxelCellSize_yVoxelDimension_zw.y)
		+ (voxelIdx.z * gxVoxelCellSize_yVoxelDimension_zw.y * gxVoxelCellSize_yVoxelDimension_zw.y);

	Voxel vox = voxelGridBuffer[bufferIdx];

	// If voxel doesn't contain geometry info, skip it
	if (vox.occlusion == 0)
		discard;

	int halfDimension = gxVoxelCellSize_yVoxelDimension_zw.y / 2;
	int3 gridOffset = voxelIdx - int3(halfDimension, halfDimension, halfDimension);
	float3 posW = (float3(gridOffset)* gxVoxelCellSize_yVoxelDimension_zw.x) + gxyzVoxelGridCenter_wUseOcclusion.xyz;		// Use voxel center pos or from posBuffer? (might be weird with small render target)

	float3 toLight = normalize(gLightCurrent.mPosition - posW);

	// Get projected shadow projection
	float4 projectedTexCoord = mul(float4(posW, 1.0f), mLightWVP);
    float shadowFactor = CalcShadowFactor(projectedTexCoord); // no shadow factor while trying to find normals bug


	float nDotL;
	//float3 normal = DecodeNormalFromUInt(vox.normalMask[0]);
    float3 normal = normalize(GetClosestNormalToDirection(vox.normalMask, -gLightCurrent.mDirection, nDotL));

	// Do hacky stuff
	//if(dot(normal, normal) == 0)
	//	normal = DecodeNormalFromUInt(vox.normalMask[1]);
	//if(dot(normal, normal) == 0)
	//	normal = DecodeNormalFromUInt(vox.normalMask[2]);
	//if(dot(normal, normal) == 0)
	//	normal = DecodeNormalFromUInt(vox.normalMask[3]);
	//if(dot(normal, normal) == 0)
	//	normal = float3(1, 1, 1);

	float3 albedo = DecodeColorFromUInt(vox.colorMask);
	float4 combinedColor = float4(albedo, 1.0f);

	/*SurfaceInfo defaultSurface;
	defaultSurface.pos = posW;
	defaultSurface.normal = normal;
	defaultSurface.diffuse = combinedColor;
	defaultSurface.spec = float4(0.1, 0.1, 0.1, 20.0);*/




	//Combine lighting from all lights
	//float4 diffuse = float4(RenderLight(defaultSurface, gLightCurrent, gLightCurrent.xyzPos_wEnabled.xyz, shadowFactor), 1);
	float4 diffuse = float4(0,0,0,0);
	if (gLightCurrent.mType == 0 && gLightCurrent.mEnabled == 1)
	{
		diffuse += shadowFactor * combinedColor * ComputePointLight(gLightCurrent, posW, normal);
	}
	else if (gLightCurrent.mType == 1 && gLightCurrent.mEnabled == 1)
	{
		diffuse += shadowFactor * combinedColor * ComputeDirectionalLight(gLightCurrent, posW, normal);
	}
	else if (gLightCurrent.mType == 2 && gLightCurrent.mEnabled == 1)
	{
		diffuse += shadowFactor * combinedColor * ComputeSpotLight(gLightCurrent, posW, normal);
	}

	diffuse = saturate(diffuse);

	float4 SHCoefficients = ClampedCosineSHCoeffs(normal);
	pOut.redColor	= SHCoefficients * diffuse.r;
	pOut.greenColor = SHCoefficients * diffuse.g;
	pOut.blueColor	= SHCoefficients * diffuse.b;

	// testing
	uint temp;																	// diffuse.xyz?
	InterlockedExchange(voxelGridBuffer[bufferIdx].colorMask, EncodeColorIntoUInt(abs(albedo.xyz), 0), temp);

	return pOut;
}

#endif
