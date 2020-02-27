#ifndef DetailedMesh_Voxelization_VS
#define DetailedMesh_Voxelization_VS

#include "Constants.hlsl"
#include "DetailedMesh_Structs.hlsl"
#include "VoxelGI_Structs.hlsl"

Voxel_GS_IN main(DetailedMesh_VS_In vIn)
{
	Voxel_GS_IN vOut;
	vOut.iPosH = float4(vIn.mPosition, 1.0);
	vOut.iNormal = mul(float4(vIn.mNormal, 0.0f), mWorldTransformMatrix).xyz;
	vOut.iTexCoord = vIn.mTexCoord;
	return vOut;
}

#endif
