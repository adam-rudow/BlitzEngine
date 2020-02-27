#pragma once
#include "Components/Component.h"
#include "Renderer.h"
#include "VoxelGrid.h"
//#include "Math.h"


class CameraComponent : public Component
{
public:
	CameraComponent(/*class Game& game*/);

	void Update(float deltaTime) override;

	void CheckInputMovement();
	
	const Matrix4& GetCameraMatrix() const { return mCameraMat; }
	Vector3 GetCamPos() { return mCameraPos; }
	inline Vector3 GetEulerRotation() const { return mCameraRotation_Euler; }

	void SetHorizontalDist(float min, float max);
	void SetVerticalDist(float min, float max);
	void SetTargetOffset(float offset);
	
	//void SetMoveComponent(MoveComponentPtr move) { mMove = move; }

	void SetAsActive();
	void SetSpringConstant(float value);
	void SnapToIdealPosition();

	void CreateViewMatricesForVoxelization(Vector3 pos);
	
	void RotateRight();
	void RotateLeft();
	void RotateHorizontal(float deltaX);
	void RotateUp();
	void RotateDown();
	void RotateVerticle(float deltaY);
	void MoveForward();
	void MoveBack();
	void MoveUp();
	void MoveDown();
	void MoveRight();
	void MoveLeft();

	void FillCamConst(PerCameraConstants* camConst);

private:
	struct PerCameraConstants mCamConst;

	Vector3 ComputeIdealPosition();
	Matrix4 mCameraMat;

	Vector3 mFocusPos;
	Vector3 mCameraPos;
	Vector3 mCameraVelocity;
	Vector3 mCameraRotation_Euler;

	float mCamDistFromOrigin;
	float mHeightOffset;
	bool mMovedThisFrame;

	Vector2 mHDist;
	Vector2 mVDist;

	VoxelGrid mVoxelGrid;

	// View matrices for Orthographic voxelation
	Matrix4 mFrontSideWorldToViewTransform;
	Matrix4 mRightSideWorldToViewTransform;
	Matrix4 mTopSideWorldToViewTransform;

	// Orthographic projection matrix
	Matrix4 mOrthoViewToProjectionTransform;

	//MoveComponentPtr mMove;

	float mTargetOffset;
	float mSpringConstant;
	float mDampConstant;

	float mNear;
	float mFar;

	float tempAngle = 0;
};
