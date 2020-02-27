#include "DirectionalLightComponent.h"
#include "MeshComponent.h"

DirectionalLightComponent::DirectionalLightComponent(/*Game& game*/) : LightComponent(/*game*/)
{
	Register();
	mData.mType = Directional;

	/*mMeshRep = new MeshComponent(mGame);
	mMeshRep->SetMesh(ST_BasicMesh, "sphere.obj");
	mMeshRep->SetScale(10.7f);
	mCastsShadows = false;

	AttachComponent(mMeshRep);*/
}

DirectionalLightData::DirectionalLightData()
{
	// Set some defaults so that the light works when places in level
	mDiffuseColor   = Vector3(0.8f, 0.8f, 0.8f);
	mSpecularColor  = Vector3(0.8f, 0.8f, 0.8f);
	mSpecularPower  = 2.0f;
	mDirection		= Vector3(0.0f, 0.0f, -1.0f);
	mEnabled		= true;
}

void DirectionalLightComponent::Register()
{
	Renderer* gRenderer = reinterpret_cast<Renderer*>(IRenderer::Instance());
	gRenderer->AddDirectionalLight(this);
}

void DirectionalLightComponent::Unregister()
{
	Renderer* gRenderer = reinterpret_cast<Renderer*>(IRenderer::Instance());
	gRenderer->RemoveDirectionalLight(this);
}

//void DirectionalLightComponent::SetLightDirection(Vector3 direction)
//{
//	direction.Normalize();
//	mData.mDirection = direction;
//}

void DirectionalLightComponent::CalculateShadowTransforms()
{
	Vector3 pos = mWorldTransform.GetTranslation();
	Vector3 n = mData.mDirection;
	Vector3 target = pos + n;
	Vector3 up = mWorldTransform.GetUp();
	//mLightViewTransform = Matrix4::CreateViewMatrix(pos, target, up);
	mLightViewTransform = Matrix4::CreateLookAtMatrix(pos, target, up);

	static float verticleFOV = /*0.162f*/0.45f * PI; // 0.25 * PI
	static float aspectratio = static_cast<float>(Renderer::Window_Width) / static_cast<float>(Renderer::Window_Height);// 1.3f;
	//mLightProjectionTransform = Matrix4::CreatePerspectiveFOV(ToRadians(70.0f), Renderer::Window_Width, Renderer::Window_Height, 5.0f, 10000.0f);
	mLightProjectionTransform = Matrix4::CreateOrtho(4.0f * Renderer::Window_Width, 4.0f * Renderer::Window_Height, 5.0f, 10000.0f);
}
