#ifndef VoxelGrid_Clear_CS
#define VoxelGrid_Clear_CS

#include "Constants.hlsl"
#include "VoxelGI_Structs.hlsl"

RWStructuredBuffer<Voxel> voxelGridBuffer : register(u0);


[numthreads(8, 8, 8)]
void main(uint3 DTid : SV_DispatchThreadID)
{
	int idx = DTid.x + (DTid.y * gxVoxelCellSize_yVoxelDimension_zw.y) + (DTid.z * gxVoxelCellSize_yVoxelDimension_zw.y * gxVoxelCellSize_yVoxelDimension_zw.y);

	voxelGridBuffer[idx].normalMask = uint4(0, 0, 0, 0);
	voxelGridBuffer[idx].colorMask = 0;
	voxelGridBuffer[idx].occlusion = 0;
}

#endif
