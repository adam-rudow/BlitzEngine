#include "VoxelGrid.h"


uint32_t VoxelGrid::s_voxelGridDimension = 32;
float  VoxelGrid::s_voxelGridRealSize = 3840;// 3072;
float  VoxelGrid::s_voxelSize = VoxelGrid::s_voxelGridRealSize / (float)VoxelGrid::s_voxelGridDimension;


VoxelGrid::VoxelGrid()
{
	float gridHalfDimension = static_cast<float>(s_voxelGridDimension / 2);
	mGridCenterIdx = Vector3(gridHalfDimension, gridHalfDimension, gridHalfDimension);
}

void VoxelGrid::SnapVoxelGridCenter(Vector3 pCamPos, Vector3 pCamDir)
{
	// Snap based on rotation
	Vector3 realCenterWS = pCamPos + pCamDir * mGridCenterIdx.x * s_voxelSize;
	mGridCenterWS = GetClosestVoxelCellCenterWS(realCenterWS);
}

Vector3 VoxelGrid::GetClosestVoxelCellCenterWS(Vector3 pPositionWS)
{
	//std::string s1 = "VoxelGridCenter: " + voxelIdx2.ToString() + "\n";
	//OutputDebugString(s.c_str());

	Vector3 voxelIdx = pPositionWS;// / s_voxelSize;
	Vector3 voxelIdx2 = Vector3((static_cast<int>(floor(voxelIdx.x)) / static_cast<int>(s_voxelSize)) * s_voxelSize,
								(static_cast<int>(floor(voxelIdx.y)) / static_cast<int>(s_voxelSize)) * s_voxelSize,
								(static_cast<int>(floor(voxelIdx.z)) / static_cast<int>(s_voxelSize)) * s_voxelSize);

	float halfCellSize = s_voxelSize / 2.0f + 0.0001f;
	Vector3 voxelCenterWS = voxelIdx2 + Vector3(halfCellSize, halfCellSize, halfCellSize);

	//std::string s = "VoxelGridCenter: " + voxelIdx2.ToString() + "\n";
	//OutputDebugString(s.c_str());
	return voxelCenterWS;
}

Vector3 VoxelGrid::GetVoxelIndexFromPoint(Vector3 pWorldSpacePoint, Vector3 pVoxelGridCenterWS)
{
		
	Vector3 voxelIdx;
	voxelIdx = (pWorldSpacePoint - mGridCenterWS) / s_voxelSize;
	voxelIdx = Vector3(round(voxelIdx.x), round(voxelIdx.y), round(voxelIdx.z));

	voxelIdx = mGridCenterIdx + voxelIdx;
		
	return voxelIdx;
}



