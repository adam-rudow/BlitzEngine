#include "PointLightComponent.h"
#include "MeshComponent.h"

PointLightComponent::PointLightComponent(/*Game& game*/) : LightComponent(/*game*/)
{
	mData.mEnabled = true;
	mData.mType = Point;
	mMeshRep = NULL;
	Register();

	/*mMeshRep = new MeshComponent(mGame);
	mMeshRep->SetMesh("sphere.obj");
	mMeshRep->SetScale(0.7f);

	AttachComponent(mMeshRep);*/
}

PointLightData::PointLightData()
{
	// Set some defaults so that the light works when places in level
}

void PointLightComponent::Register()
{
	Renderer* gRenderer = reinterpret_cast<Renderer*>(IRenderer::Instance());
	gRenderer->AddPointLight(this);
}

void PointLightComponent::Unregister()
{
	Renderer* gRenderer = reinterpret_cast<Renderer*>(IRenderer::Instance());
	gRenderer->RemovePointLight(this);
}

void PointLightComponent::CalculateShadowTransforms()
{
	Vector3 pos = mWorldTransform.GetTranslation();
	Vector3 n = mData.mDirection;
	Vector3 target = pos + n;
	Vector3 up = mWorldTransform.GetUp();
	//mLightViewTransform = Matrix4::CreateViewMatrix(pos, target, up);
	mLightViewTransform = Matrix4::CreateLookAtMatrix(pos, target, up);

	//static float verticleFOV = /*0.162f*/0.25f * PI;
	//static float aspectratio = static_cast<float>(Renderer::Window_Width) / static_cast<float>(Renderer::Window_Height);// 1.3f;

	mLightProjectionTransform = Matrix4::CreatePerspectiveFOV(ToRadians(70.0f),
											static_cast<float>(Renderer::Window_Width), static_cast<float>(Renderer::Window_Height), 5.0f, 10000.0f);
}

//void PointLightComponent::SetLightPosition(Vector3 position)
//{
//	mData.mPosition = position; 
//	SetPosition(position);
//}
