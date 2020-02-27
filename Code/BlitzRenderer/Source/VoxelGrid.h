#pragma once

// Inter-Engine includes
#include "Math/math.h"
#include "RendererMacros.h"

struct Voxel
{
	uint32_t mNormalMask[4];
	uint32_t mColorMask;
	uint32_t mOcclusion;
};

class RENDERER_EXPORT VoxelGrid
{
public:
	// Constructor -------------------------------------------------------------
	VoxelGrid();
	virtual ~VoxelGrid(){}

	void SnapVoxelGridCenter(Vector3 pCamPos, Vector3 pCamDir);
	Vector3 GetClosestVoxelCellCenterWS(Vector3 pPositionWS);
	//Vector3 ComputeVoxelCenterWorldSpace(Vector3 pVoxelIndex);
	Vector3 GetVoxelIndexFromPoint(Vector3 pWorldSpacePoint, Vector3 pVoxelGridCenterWS);

	static float s_voxelSize;
	static uint32_t s_voxelGridDimension; // ex: 32 x 32 x 32
	static float s_voxelGridRealSize;

	Vector3 mGridCenterWS;
	Vector3 mGridCenterIdx;
};
