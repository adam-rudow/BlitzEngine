#ifndef Voxelization_PS
#define Voxelization_PS


#include "Constants.hlsl"
#include "VoxelGI_Structs.hlsl"
//#include "samplerhelper.hlsl
//#include "StandardTextureResources.hlsl"

// Values and strategy from Hawar Doghramachi

RWStructuredBuffer<Voxel> voxelGridBuffer : register(u1);

float4 main(Voxel_PS_IN pIn/*, float4 screenSpace : SV_Position*/) : SV_TARGET
{
	// if sampling diffuse texture
	float3 baseColor = gDiffuseMap.Sample(gDiffuseMapSampler, pIn.iTexCoord).xyz; // should sample from screenSpace instead?

		// encode color in linear space into unsigned integer
		float3 baseLinear = lerp(baseColor / 12.92f, pow((baseColor + 0.055f) / 1.055f, 2.4f), baseColor>0.04045f);

	// if sampling deferred rendering texture
	//float3 baseColor = sample2D(gAdditionalDiffuseMapSampler, screenSpace.xy, gAdditionalDiffuseMap).xyz;	

	//if(dot(pIn.iNormal, pIn.iNormal) == 0 || dot(pIn.iPosW.xyz, pIn.iPosW.xyz) == 0)
	//	return float4(baseColor, 1.0f);

	float contrast = length(baseColor.rrg - baseColor.gbb) / (sqrt(2.0f) + baseColor.r + baseColor.g + baseColor.b);
	uint colorMask = EncodeColorIntoUInt(baseColor, contrast);

	float dotProd;
	float3 normal = (normalize(pIn.iNormal));
	int normalIdx = GetBestTetrahedronNormalIdx(normal, dotProd);
	uint normalMask = EncodeNormalAsUInt(normal, dotProd);

	float3 posW = mul(float4(pIn.iPosW, 1.0f), mWorldTransformMatrix);

	float3 voxelGridOffset = (posW - gxyzVoxelGridCenter_wUseOcclusion.xyz) / (float)gxVoxelCellSize_yVoxelDimension_zw.x;
	voxelGridOffset = round(voxelGridOffset);

	int gridHalfDimension = gxVoxelCellSize_yVoxelDimension_zw.y / 2;
	int3 voxelIdx = int3(gridHalfDimension, gridHalfDimension, gridHalfDimension) + int3(voxelGridOffset);

	// Cull out voxels outside of grid
	if (voxelIdx.x > -1 && voxelIdx.x < gxVoxelCellSize_yVoxelDimension_zw.y &&
		voxelIdx.y > -1 && voxelIdx.y < gxVoxelCellSize_yVoxelDimension_zw.y &&
		voxelIdx.z > -1 && voxelIdx.z < gxVoxelCellSize_yVoxelDimension_zw.y)
	{
		int bufferIdx = voxelIdx.x + voxelIdx.y * gxVoxelCellSize_yVoxelDimension_zw.y + voxelIdx.z * gxVoxelCellSize_yVoxelDimension_zw.y * gxVoxelCellSize_yVoxelDimension_zw.y;

		InterlockedMax(voxelGridBuffer[bufferIdx].colorMask, colorMask);
		InterlockedMax(voxelGridBuffer[bufferIdx].normalMask[normalIdx], normalMask);
		InterlockedMax(voxelGridBuffer[bufferIdx].occlusion, 1);

		return float4(baseColor, 1.0f);

		//return float4(normal, 1.0f);
	}

	return float4(baseColor, 1.0f);
	return float4(normal, 1);
}

#endif
