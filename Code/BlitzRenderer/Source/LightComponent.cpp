#include "LightComponent.h"
#include "MeshComponent.h"

LightData::LightData()
{
	// Set some defaults so that the light works when places in level
	mDiffuseColor   = Vector3(0.8f, 0.8f, 0.8f);
	mSpecularColor  = Vector3(0.8f, 0.8f, 0.8f);
	mSpecularPower  = 2.0f;
	mDirection		= Vector3(0.0f, 0.0f, -1.0f);
	mEnabled		= true;
}

void LightComponent::Register()
{
	Renderer* gRenderer = reinterpret_cast<Renderer*>(IRenderer::Instance());
	gRenderer->AddLight(this);
}

void LightComponent::Unregister()
{
	Renderer* gRenderer = reinterpret_cast<Renderer*>(IRenderer::Instance());
	gRenderer->RemoveLight(this);
}

void LightComponent::SetLightPosition(Vector3 position)
{
	mData.mPosition = position;
	SetPosition(position);
}

void LightComponent::SetLightDirection(Vector3 direction)
{
	direction.Normalize();
	mData.mDirection = direction;
}
