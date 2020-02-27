#ifndef Voxelization_Debug_PS
#define Voxelization_Debug_PS

#include "Constants.hlsl"
#include "VoxelGI_Structs.hlsl"
#include "Std_Structs.hlsl"


RWStructuredBuffer<Voxel> voxelGridBuffer : register(u1);

float4 main(BasicMesh_PS_In pIn) : SV_TARGET
{
	float3 posW = gAdditionalDiffuseMap.Sample(gAdditionalDiffuseMapSampler, pIn.mTex).xyz;// sample2D(gAdditionalDiffuseMapSampler, pIn.mTex, gAdditionalDiffuseMap).xyz;

	float3 deferredColor = gDiffuseMap.Sample(gDiffuseMapSampler, pIn.mTex).xyz;// sample2D(gDiffuseMapSampler, pIn.mTex, gDiffuseMap).xyz;
	//return float4(deferredColor, 0.51f);

	if (dot(posW, posW) == 0)
		return float4(deferredColor, 1.0f);

	// Get voxel grid entry of world position
	float3 voxelGridOffset = (posW - gxyzVoxelGridCenter_wUseOcclusion.xyz) / gxVoxelCellSize_yVoxelDimension_zw.x;
	voxelGridOffset = round(voxelGridOffset);

	int gridHalfDimension = gxVoxelCellSize_yVoxelDimension_zw.y / 2;
	int3 voxelIdx = int3(gridHalfDimension, gridHalfDimension, gridHalfDimension) + int3(voxelGridOffset);


	// Cull out voxels outside of grid
	if (voxelIdx.x > -1 && voxelIdx.x < gxVoxelCellSize_yVoxelDimension_zw.y &&
		voxelIdx.y > -1 && voxelIdx.y < gxVoxelCellSize_yVoxelDimension_zw.y &&
		voxelIdx.z > -1 && voxelIdx.z < gxVoxelCellSize_yVoxelDimension_zw.y)
	{
		int bufferIdx = voxelIdx.x
			+ (voxelIdx.y * gxVoxelCellSize_yVoxelDimension_zw.y)
			+ (voxelIdx.z * gxVoxelCellSize_yVoxelDimension_zw.y * gxVoxelCellSize_yVoxelDimension_zw.y);

		//return float4(voxelIdx.x / gxVoxelCellSize_yVoxelDimension_zw.y, voxelIdx.y / gxVoxelCellSize_yVoxelDimension_zw.y, voxelIdx.z / gxVoxelCellSize_yVoxelDimension_zw.y, 1);
		
		uint colorMask = voxelGridBuffer[bufferIdx].colorMask;
		return float4(DecodeColorFromUInt(colorMask), 1);

		// testing normal decode	
		/*float nDotL;
		float3 toCam = normalize(mCamWorldPos - posW);
		float3 normal = GetClosestNormalToDirection(voxelGridBuffer[bufferIdx].normalMask, toCam, nDotL);
		
		return float4(toCam, 1);*/

	}

	return float4(deferredColor, 1.0f);
}

#endif
