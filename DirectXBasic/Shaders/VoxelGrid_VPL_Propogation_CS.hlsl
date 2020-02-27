#ifndef VoxelGrid_VPL_Propogation_CS
#define VoxelGrid_VPL_Propogation_CS

#include "Constants.hlsl"
#include "VoxelGI_Structs.hlsl"

// Source geometry and lighting data
Texture2DArray<float4> redSHCoeffTextureArrayIn : register(t0);
Texture2DArray greenSHCoeffTextureArrayIn : register(t1);
Texture2DArray blueSHCoeffTextureArrayIn : register(t2);
StructuredBuffer<Voxel> voxelGridBuffer : register(t3);

// Output textures
RWTexture2DArray<float4> redSHCoeffTextureArrayOut : register(u0);
RWTexture2DArray<float4> greenSHCoeffTextureArrayOut : register(u1);
RWTexture2DArray<float4> blueSHCoeffTextureArrayOut : register(u2);


[numthreads(8, 8, 8)]
void main(uint3 DTid : SV_DispatchThreadID)
{
	// Get useful indices for sampling
	int idx = DTid.x + (DTid.y * gxVoxelCellSize_yVoxelDimension_zw.y) + (DTid.z * gxVoxelCellSize_yVoxelDimension_zw.y * gxVoxelCellSize_yVoxelDimension_zw.y);
	int3 gridPos = DTid.xyz;

	// Get SH coefficients from current voxel
	uint status;
	float4 sumRedSHCoefficient		= redSHCoeffTextureArrayIn.Load(int4(gridPos, 0));
	float4 sumGreenSHCoefficient	= greenSHCoeffTextureArrayIn.Load(int4(gridPos, 0));
	float4 sumBlueSHCoefficient		= blueSHCoeffTextureArrayIn.Load(int4(gridPos, 0));

	[unroll]
	for (int i = 0; i < 6; ++i) // For each neighbor voxel
	{
		// Get neighbor's grid pos
		int3 neighborGridPos = gridPos + gridOffsets[i];

		// Make sure we are still inside of grid
		if (neighborGridPos.x < 0 || neighborGridPos.x > gxVoxelCellSize_yVoxelDimension_zw.y ||
			neighborGridPos.y < 0 || neighborGridPos.y > gxVoxelCellSize_yVoxelDimension_zw.y ||
			neighborGridPos.z < 0 || neighborGridPos.z > gxVoxelCellSize_yVoxelDimension_zw.y)
		{
			continue;
		}

		// Get SH coefficients from neighbor voxel
		float4 neighborRedSHCoefficient = redSHCoeffTextureArrayIn.Load(int4(neighborGridPos, 0));
		float4 neighborGreenSHCoefficient = greenSHCoeffTextureArrayIn.Load(int4(neighborGridPos, 0));
		float4 neighborBlueSHCoefficient = blueSHCoeffTextureArrayIn.Load(int4(neighborGridPos, 0));

		//sumRedSHCoefficient += neighborRedSHCoefficient;
		//sumGreenSHCoefficient += neighborGreenSHCoefficient;
		//sumBlueSHCoefficient += neighborBlueSHCoefficient;
		//
		//continue;


		// Check if we should consider occlusion (true for all except first round)
#if USE_OCCLUSION
		float4 occlusionCoefficients = float4(0, 0, 0, 0);
		//if (gxyzVoxelGridCenter_wUseOcclusion.w > 0.5f)
		//{
		int neighborGridIdx = neighborGridPos.x + neighborGridPos.y * gxVoxelCellSize_yVoxelDimension_zw.y + neighborGridPos.z * gxVoxelCellSize_yVoxelDimension_zw.y * gxVoxelCellSize_yVoxelDimension_zw.y;
		Voxel nVox = voxelGridBuffer[neighborGridIdx];

		if (nVox.occlusion == 1)
		{
			// Get best normal to use and calculate SH Coefficient for that direction
			float dot;
			float3 occNormal = GetClosestNormalToDirection(nVox.normalMask, -cubeDirections[i], dot);
			occlusionCoefficients = ClampedCosineSHCoeffs(occNormal);
		}
		//}
#endif

		[unroll]
		for (int k = 0; k < 6; ++k) // For each direction/face of neighbor voxel
		{
			// Dir from current voxel center to neighbor voxel center
			float3 neighborCenter = cubeDirections[i];

			// Dir from neighbor voxel center to neighbor current face center
			float3 facePos = cubeDirections[k] * 0.5f;

			// Get vector from current voxel center to neighbor voxel's current face center
			float3 dirToFaceCenter = facePos - neighborCenter;

			float dirLength = length(dirToFaceCenter);
			dirToFaceCenter /= dirLength;

			// Determine angle that encompasses a face of the neighbor voxel
			float angleToFace = 0;

			// Exclude face coming back towards current voxel
			if ( dirLength > 0.51f)
			{
				// Precomputed angles since we know that dirToFaceCenter is either straight in the direction from current to 
				// neighbor (angle = 0), or it turns in a new perpendicular direction (angle is same for each)
				angleToFace = (dirLength >= 1.49f) ? 0.0318842778f /*(22.95668f / (4 * 180.0f))*/ : 0.0336955972f/*(24.26083f / (4 * 180.0f))*/;
			}

			// Calculate SH coefficient for direction
			float4 directionSH;
			directionSH.x = 0.2820947918f;	//1.0f / (2 * sqrt(PI));
			directionSH.y = -0.4886025119f;	//-sqrt(3.0f / (4 * PI));
			directionSH.z = 0.4886025119f;	//sqrt(3.0f / (4 * PI));
			directionSH.w = -0.4886025119f; //-sqrt(3.0f / (4 * PI));
			directionSH.wyz *= dirToFaceCenter;

			// Calculate flux from neighbor to face of current
			float3 flux;
			flux.r = dot(neighborRedSHCoefficient, directionSH);
			flux.g = dot(neighborGreenSHCoefficient, directionSH);
			flux.b = dot(neighborBlueSHCoefficient, directionSH);
			flux = max(0.0f, flux) * angleToFace * 2.55f;

#if USE_OCCLUSION
			// Is using occlusion, apply it to results
			//if (gxyzVoxelGridCenter_wUseOcclusion.w > 0.5f)
			//{
			float occlusion = 1.0f - saturate(1.25f * dot(occlusionCoefficients, directionSH));
			flux *= occlusion;
			//}
#endif
			//flux = float3(1, 1, 1);
			// Add contribution to SH sums
			float4 coefficient		= cubeFaceSHCoefficients[k];
			sumRedSHCoefficient		+= coefficient * flux.r;
			sumGreenSHCoefficient	+= coefficient * flux.g;
			sumBlueSHCoefficient	+= coefficient * flux.b;
		}
	}

	// Store results in other 2DTextureArrays, make sure to swap source/destination on CPU side
	redSHCoeffTextureArrayOut[gridPos]		= sumRedSHCoefficient;	// min(2.5f, sumRedSHCoefficient);
	greenSHCoeffTextureArrayOut[gridPos]	= sumGreenSHCoefficient;	// min(2.5f, sumGreenSHCoefficient);
	blueSHCoeffTextureArrayOut[gridPos]		= sumBlueSHCoefficient;	// min(2.5f, sumBlueSHCoefficient);
}

#endif
