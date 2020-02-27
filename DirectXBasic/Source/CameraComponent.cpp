//#include "PreCompiledHeader.h"
#include "CameraComponent.h"
#include "Game/InputManager.h"
#include "Renderer.h"

const float TranslateSpeed = 2000.0f;
const float RotationAngleSpeed = 0.25f;

CameraComponent::CameraComponent(/*class Game& game*/) : Component(/*game*/), mTargetOffset(0.0f)
{
	SetSpringConstant(128.0f);
	mCamDistFromOrigin = 100.0f;
	mHeightOffset = 25;
	mFocusPos = Vector3(0, 0, -200);
	SetPosition(mFocusPos - Vector3(0, -mHeightOffset, mCamDistFromOrigin));

	mCameraMat = Matrix4::CreateLookAtMatrix(mPosition, mFocusPos, Vector3::UnitY);

	mNear = 5.0f;
	mFar = 10000.0f;
	
	float orthoDist = (float)VoxelGrid::s_voxelGridRealSize;// / 2.0f;
	mOrthoViewToProjectionTransform = Matrix4::CreateOrtho(orthoDist, orthoDist, mNear, orthoDist + mNear);

	// Trigger voxel grid to snap to position on first frame
	mMovedThisFrame = true;
}

void CameraComponent::SetAsActive()
{
	// Tell the renderer
	Renderer::Instance()->SetViewMatrix(mCameraMat, mWorldTransform.GetTranslation());
}

void CameraComponent::Update(float deltaTime)
{
	// Move camera based on set velocity
	if (mCameraVelocity.LengthSq() > 0)
	{
		mCameraVelocity.Normalize();
		mPosition += mCameraVelocity * TranslateSpeed * deltaTime;
		mCameraVelocity = Vector3::Zero;
	}

	Renderer* gRenderer = reinterpret_cast<Renderer*>(IRenderer::Instance());

	// Rotate camera based on screen/mouse rotation
	Matrix4 xRotMatrix = Matrix4::CreateRotationY(-mCameraRotation_Euler.x);
	Matrix4 yRotMatrix = Matrix4::CreateRotationX(mCameraRotation_Euler.y);
	mWorldTransform = yRotMatrix * xRotMatrix * Matrix4::CreateTranslation(mPosition/*mWorldTransform.GetTranslation()*/);

	// Upload data to GPU
	mFocusPos = mPosition + mCamDistFromOrigin * mWorldTransform.GetForward();
	mCameraMat = Matrix4::CreateLookAtMatrix(mPosition, mFocusPos, Vector3::UnitY);
	gRenderer->SetViewMatrix(mCameraMat, mWorldTransform.GetTranslation());

	// Update Voxel grid
	if (mMovedThisFrame)
	{
		mVoxelGrid.SnapVoxelGridCenter(mPosition, GetWorldTransform().GetForward());
		CreateViewMatricesForVoxelization(mPosition);
		FillCamConst(gRenderer->GetCamConsts());
	}

	mMovedThisFrame = false;

	//BEPrint((mWorldTransform.GetTranslation().ToString() + "\n").c_str());
}

void CameraComponent::CheckInputMovement()
{
	/*if (InputManager::Instance()->GetTriggerState(char('C')))
	{
		RotateRight();
	}
	if (InputManager::Instance()->GetTriggerState(char('Z')))
	{
		RotateLeft();
	}*/
	if (InputManager::Instance()->GetTriggerState(char('W')))
	{
		MoveForward();
	}
	if (InputManager::Instance()->GetTriggerState(char('S')))
	{
		MoveBack();
	}
	if (InputManager::Instance()->GetTriggerState(char('E')))
	{
		MoveUp();
	}
	if (InputManager::Instance()->GetTriggerState(char('Q')))
	{
		MoveDown();
	}
	if (InputManager::Instance()->GetTriggerState(char('D')))
	{
		MoveRight();
	}
	if (InputManager::Instance()->GetTriggerState(char('A')))
	{
		MoveLeft();
	}
}

void CameraComponent::FillCamConst(PerCameraConstants* camConst)
{
	// Fill constant buffer with camera data, pre-multiply viewProj matrix for voxelization
	camConst->gVoxelViewMatrices[0] = mFrontSideWorldToViewTransform * mOrthoViewToProjectionTransform;
	camConst->gVoxelViewMatrices[1] = mRightSideWorldToViewTransform * mOrthoViewToProjectionTransform;
	camConst->gVoxelViewMatrices[2] = mTopSideWorldToViewTransform * mOrthoViewToProjectionTransform;
	camConst->gVoxelOrthoProjection = mOrthoViewToProjectionTransform;
	camConst->gVoxelCellSize		= mVoxelGrid.s_voxelSize;
	camConst->gVoxelDimension		= static_cast<float>(mVoxelGrid.s_voxelGridDimension);
	camConst->gVoxelGridCenter		= mVoxelGrid.mGridCenterWS;
	camConst->gUseOcclusion			= 0;

}

void CameraComponent::CreateViewMatricesForVoxelization(Vector3 pos)
{
	float halfDimension = (float)VoxelGrid::s_voxelGridDimension / 2.0f;
	float cellSize = VoxelGrid::s_voxelSize;

	// Front view matrix
	Vector3 viewPos = mVoxelGrid.mGridCenterWS - halfDimension * Vector3(0, 0, cellSize) - Vector3(0, 0, mNear);
	mFrontSideWorldToViewTransform = Matrix4::CreateLookAtMatrix(viewPos, mVoxelGrid.mGridCenterWS, Vector3(0, 1, 0));

	// Right side view matrix
	viewPos = mVoxelGrid.mGridCenterWS + halfDimension * Vector3(cellSize, 0, 0) + Vector3(mNear, 0, 0);
	mRightSideWorldToViewTransform = Matrix4::CreateLookAtMatrix(viewPos, mVoxelGrid.mGridCenterWS, Vector3(0, 1, 0));

	// Top view matrix
	viewPos = mVoxelGrid.mGridCenterWS + halfDimension * Vector3(0, cellSize, 0) + Vector3(0, mNear, 0);
	mTopSideWorldToViewTransform = Matrix4::CreateLookAtMatrix(viewPos, mVoxelGrid.mGridCenterWS, Vector3(0, 0, 1));
}

void CameraComponent::RotateRight()
{
	//tempAngle += 2.0f;// 0.05f;
	//float rot = tempAngle;
	//float x = cos(rot * (3.14159265f / 180.0f));
	//float y = sin(rot * (3.14159265f / 180.0f));
	//mCameraPos = mFocusPos + mCamDistFromOrigin * Vector3(x, y, mCameraPos.z);
	//mCameraMat = Matrix4::CreateLookAtMatrix(mCameraPos, mFocusPos, Vector3::UnitZ);
	//// Tell the renderer
	//mGame.GetRenderer().SetViewMatrix(mCameraMat);

	AddRotation(Quaternion(GetWorldTransform().GetUp() /*Vector3(0,0,1)*/, RotationAngleSpeed));
	mFocusPos = mPosition + mCamDistFromOrigin * GetWorldTransform().GetForward();// -Vector3(0, 0, mHeightOffset);
	mCameraMat = Matrix4::CreateLookAtMatrix(mPosition, mFocusPos, Vector3::UnitY);

	///std::string s = "mFocusPos = " + mFocusPos.ToString() + "\n";
	//OutputDebugString(s.c_str());

	// Tell the renderer
	Renderer::Instance()->SetViewMatrix(mCameraMat, mWorldTransform.GetTranslation());
	mMovedThisFrame = true;
}

void CameraComponent::RotateLeft()
{
	//tempAngle -= 2.0f;// 0.05f;
	//float rot = tempAngle;
	//float x = cos(rot * (3.14159265f / 180.0f));
	//float y = sin(rot * (3.14159265f / 180.0f));
	//mPosition = mFocusPos + mCamDistFromOrigin * Vector3(x, y, mPosition.z);
	//mCameraMat = Matrix4::CreateLookAtMatrix(mPosition, mFocusPos, Vector3::UnitZ);
	//// Tell the renderer
	//mGame.GetRenderer().SetViewMatrix(mCameraMat);

	AddRotation(Quaternion(GetWorldTransform().GetUp(), -RotationAngleSpeed));
	mFocusPos = mPosition + mCamDistFromOrigin * GetWorldTransform().GetForward();// -Vector3(0, 0, mHeightOffset);
	mCameraMat = Matrix4::CreateLookAtMatrix(mPosition, mFocusPos, Vector3::UnitY);

	// Tell the renderer
	Renderer::Instance()->SetViewMatrix(mCameraMat, mWorldTransform.GetTranslation());
	mMovedThisFrame = true;
}

void CameraComponent::RotateHorizontal(float deltaX)
{
	mCameraRotation_Euler.x += (deltaX * RotationAngleSpeed);
	mMovedThisFrame = true;

	//Quaternion rot(GetWorldTransform().GetUp(), RAD2DEG(asinf(deltaX)));
	//AddRotation(rot);
	//mFocusPos = mPosition + mCamDistFromOrigin * GetWorldTransform().GetForward();// -Vector3(0, 0, mHeightOffset);
	//mCameraMat = Matrix4::CreateLookAtMatrix(mPosition, mFocusPos, Vector3::UnitY);

	//// Tell the renderer
	//mGame.GetRenderer().SetViewMatrix(mCameraMat, mWorldTransform.GetTranslation());
	//mMovedThisFrame = true;
}

void CameraComponent::RotateVerticle(float deltaY)
{
	mCameraRotation_Euler.y += (deltaY * RotationAngleSpeed);
	mMovedThisFrame = true;
}

void CameraComponent::RotateUp()
{

}

void CameraComponent::RotateDown()
{

}

void CameraComponent::MoveForward()
{
	mCameraVelocity.x += -sin(DEG2RAD(mCameraRotation_Euler.x));
	mCameraVelocity.y += -tan(DEG2RAD(mCameraRotation_Euler.y));
	mCameraVelocity.z +=  cos(DEG2RAD(mCameraRotation_Euler.x));
	mMovedThisFrame = true;
}

void CameraComponent::MoveBack()
{
	mCameraVelocity.x +=  sin(DEG2RAD(mCameraRotation_Euler.x));
	mCameraVelocity.y +=  tan(DEG2RAD(mCameraRotation_Euler.y));
	mCameraVelocity.z += -cos(DEG2RAD(mCameraRotation_Euler.x));
	mMovedThisFrame = true;
}

void CameraComponent::MoveUp()
{
	mCameraVelocity.y += 1.0f;
	mMovedThisFrame = true;
}

void CameraComponent::MoveDown()
{
	mCameraVelocity.y += -1.0f;
	mMovedThisFrame = true;
}

void CameraComponent::MoveRight()
{
	mCameraVelocity.x += cos(DEG2RAD(mCameraRotation_Euler.x));
	mCameraVelocity.z += sin(DEG2RAD(mCameraRotation_Euler.x));
	mMovedThisFrame = true;
}

void CameraComponent::MoveLeft()
{
	mCameraVelocity.x += -cos(DEG2RAD(mCameraRotation_Euler.x));
	mCameraVelocity.z += -sin(DEG2RAD(mCameraRotation_Euler.x));
	mMovedThisFrame = true;
}

void CameraComponent::SetHorizontalDist(float min, float max)
{
	mHDist = Vector2(min, max);
}

void CameraComponent::SetVerticalDist(float min, float max)
{
	mVDist = Vector2(min, max);
}

void CameraComponent::SetTargetOffset(float offset)
{
	mTargetOffset = offset;
}

void CameraComponent::SetSpringConstant(float value)
{
	mSpringConstant = value;
	mDampConstant = 2.0f * sqrtf(mSpringConstant);
}

void CameraComponent::SnapToIdealPosition()
{
	mPosition = ComputeIdealPosition();
}


Vector3 CameraComponent::ComputeIdealPosition()
{
	float horizDist = mHDist.x;
	float vertDist = mVDist.x;

	/*if (mMove)
	{
		horizDist = Math::Lerp(mHDist.x, mHDist.y, mMove->GetLinearAxis());
		vertDist = Math::Lerp(mVDist.x, mVDist.y, mMove->GetLinearAxis());
	}*/

	Vector3 shipPos = mOwner->GetWorldTransform().GetTranslation();
	Vector3 mPosition = shipPos - Vector3::UnitX * horizDist +
		Vector3::UnitZ * vertDist;

	return mPosition;
}
