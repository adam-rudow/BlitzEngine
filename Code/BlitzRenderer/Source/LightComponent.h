#pragma once
#include "Components/Component.h"
#include "RendererMacros.h"

class MeshComponent;

struct RENDERER_EXPORT LightData
{
	LightData();

	Vector3 mDiffuseColor;
	uint32_t mIsShadowCaster;
	Vector3 mSpecularColor;
	float mSpecularPower;
	Vector3 mPosition;
	float mRange;
	Vector3 mDirection;
	float mSpotDotProduct;
	Vector3 mAttenuation;
	uint32_t mEnabled;
	uint32_t mType;
	float pad[3];
}; // 84 Bytes

enum RENDERER_EXPORT LightType
{
	Point,
	Directional,
	Spot,
	Count
};


class RENDERER_EXPORT LightComponent : public Component
{
public:
	LightComponent(/*class Game& game*/) : Component(/*game*/) { }

	void Register() override;
	void Unregister() override;

	virtual void CalculateShadowTransforms() = 0;

	const LightData& GetLightData() { return mData; }
	Matrix4 GetLightViewTransform() const { return mLightViewTransform; }
	Matrix4 GetLightProjectionTransform() const { return mLightProjectionTransform; }
	bool IsShadowCaster() const { return mCastsShadows; }

	void SetLightPosition(Vector3 position);
	void SetDiffuseColor(Vector3 diffuseColor) { mData.mDiffuseColor = diffuseColor; }
	void SetSpecularColor(Vector3 specularColor) { mData.mSpecularColor = specularColor; }
	void SetSpecularPower(float specularPower) { mData.mSpecularPower = specularPower; }
	void SetLightDirection(Vector3 direction);
	void SetAttenuation(Vector3 attenuation) { mData.mAttenuation = attenuation; }
	void SetRange(float range) { mData.mRange = range; }
	void SetEnabled(bool enable) { mData.mEnabled = enable; }
	void SetCastShadows(bool castShadows) { mCastsShadows = castShadows; }

protected:
	LightData mData;
	Matrix4 mLightViewTransform;
	Matrix4 mLightProjectionTransform;

	bool mCastsShadows;

	class MeshComponent* mMeshRep;
};