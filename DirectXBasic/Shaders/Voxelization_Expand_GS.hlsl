#ifndef Voxelization_Expand_GS
#define Voxelization_Expand_GS

#include "Constants.hlsl"
#include "VoxelGI_Structs.hlsl"
#include "Std_Structs.hlsl"

int GetBestViewDirectionIdx(float3 surfaceNormal)
{
	float3 frontToBack = float3(0, 0, 1);
	float3 leftToRight = float3(1, 0, 0);
	float3 bottomToTop = float3(0, 1, 0);

	float dotDirFB = abs(dot(surfaceNormal, frontToBack));
	float dotDirLR = abs(dot(surfaceNormal, leftToRight));
	float dotDirBT = abs(dot(surfaceNormal, bottomToTop));

	float maxDot = max(dotDirFB, max(dotDirBT, dotDirLR));
	if (maxDot == dotDirFB)
		return 0;
	else if (maxDot == dotDirLR)
		return 1;
	else
		return 2;
}

[maxvertexcount(3)]
void main(triangle Voxel_GS_IN input[3], inout TriangleStream<Voxel_PS_IN> triangleStream)
{
	/*Voxel_PS_IN gOut[3];
	for (int i = 0; i < 3; ++i)
	{
		gOut[i].iPosH = input[i].iPosH;
		gOut[i].iPosW = input[i].iPosH.xyz;
		gOut[i].iNormal = input[i].iNormal;
		gOut[i].iTexCoord = input[i].iTexCoord;
	}

	triangleStream.Append(gOut[0]);
	triangleStream.Append(gOut[1]);
	triangleStream.Append(gOut[2]);

	triangleStream.RestartStrip();*/

	//***********************************************
	float3 surfaceNormal = normalize(input[0].iNormal + input[1].iNormal + input[2].iNormal);
	int maxViewDirIdx = GetBestViewDirectionIdx(surfaceNormal);

	Voxel_PS_IN gOut[3];
	[unroll]
	for (int i = 0; i < 3; ++i)
	{
		// gVoxelViewMatrices are pre-multiplied by ortho proj matrix on cpp side when mapping the buffers
		float4x4 wvp = mul(mWorldTransformMatrix, gVoxelViewMatrices[maxViewDirIdx]);
		gOut[i].iPosH = mul(input[i].iPosH, wvp);
		gOut[i].iPosW = input[i].iPosH.xyz;
		gOut[i].iNormal = input[i].iNormal;
		gOut[i].iTexCoord = input[i].iTexCoord;
	}

	// Edge shift triangles with respect to orthographic view projection -> conservative rasterization
	float2 dir1 = normalize(gOut[1].iPosH.xy - gOut[0].iPosH.xy);
	float2 dir2 = normalize(gOut[2].iPosH.xy - gOut[1].iPosH.xy);
	float2 dir3 = normalize(gOut[0].iPosH.xy - gOut[2].iPosH.xy);

	// divide by dimension of voxel grid
	float expandScale = 1.0f / (2.0f * gxVoxelCellSize_yVoxelDimension_zw.y);
	gOut[0].iPosH.xy += normalize(dir3 - dir1) * expandScale;
	gOut[1].iPosH.xy += normalize(dir1 - dir2) * expandScale;
	gOut[2].iPosH.xy += normalize(dir2 - dir3) * expandScale;

	triangleStream.Append(gOut[0]);
	triangleStream.Append(gOut[1]);
	triangleStream.Append(gOut[2]);

	triangleStream.RestartStrip();
}

#endif
