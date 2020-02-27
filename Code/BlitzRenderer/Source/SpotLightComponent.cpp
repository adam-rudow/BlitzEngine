#include "SpotLightComponent.h"
//#include "Game.h"
#include "MeshComponent.h"

SpotLightComponent::SpotLightComponent(/*Game& game*/) : LightComponent(/*game*/)
{
	mData.mEnabled = true;
	mData.mType = Spot;
	Register();

	mMeshRep = new MeshComponent(/*mGame*/);
	mMeshRep->SetMesh(ST_BasicMesh, "sphere.obj");
	mMeshRep->SetScale(0.7f);

	AttachComponent(mMeshRep);
}

SpotLightData::SpotLightData()
{
	// Set some defaults so that the light works when places in level
	mDiffuseColor = Color::White;
	mSpecularColor = Color::White;
	mSpecularPower = 10.0f;
	mDirection = Vector3(0, 0, -1);
	mSpotDotProduct = cosf(30.0f);
	mAttenuation = Vector3(150.0f, 350.0f, 0.0f);
	mEnabled = true;
}

void SpotLightComponent::Register()
{
	Renderer* gRenderer = reinterpret_cast<Renderer*>(IRenderer::Instance());
	gRenderer->AddSpotLight(this);
}

void SpotLightComponent::Unregister()
{
	Renderer* gRenderer = reinterpret_cast<Renderer*>(IRenderer::Instance());
	gRenderer->RemoveSpotLight(this);
}

//void SpotLightComponent::SetLightPosition(Vector3 position)
//{
//	mData.mPosition = position;
//	SetPosition(position);
//}
//
//void SpotLightComponent::SetLightDirection(Vector3 direction)
//{
//	direction.Normalize();
//	//direction.y *= -1;
//	mData.mDirection = direction;
//}

void SpotLightComponent::SetSpotAngle(float angleDegrees)
{

	mData.mSpotDotProduct = cosf(ToRadians(angleDegrees));
}

void SpotLightComponent::CalculateShadowTransforms()
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
