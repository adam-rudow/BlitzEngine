#pragma once
#include "LightComponent.h"

//class MeshComponent;

struct RENDERER_EXPORT SpotLightData
{
	SpotLightData();

	Vector3 mDiffuseColor;
	float pad1;
	Vector3 mSpecularColor;
	float mSpecularPower;
	Vector3 mPosition;
	float mRange;
	Vector3 mDirection;
	float mSpotDotProduct;
	Vector3 mAttenuation;
	uint32_t mEnabled;
}; // 80 Bytes

class RENDERER_EXPORT SpotLightComponent : public LightComponent
{
public:
	SpotLightComponent(/*class Game& game*/);

	void Register() override;
	void Unregister() override;

	void CalculateShadowTransforms() override;

	//const SpotLightData& GetLightData() { return mData; }
	//
	//void SetDiffuseColor(Vector3 diffuseColor) { mData.mDiffuseColor = diffuseColor; }
	//void SetSpecularColor(Vector3 specularColor) { mData.mSpecularColor = specularColor; }
	//void SetSpecularPower(float specularPower) { mData.mSpecularPower = specularPower; }
	//void SetLightPosition(Vector3 position);
	//void SetRange(float range) { mData.mRange = range; }
	//void SetLightDirection(Vector3 direction);
	void SetSpotAngle(float angleDegrees);
	//void SetAttenuation(Vector3 attenuation) { mData.mAttenuation = attenuation; }
	//void SetEnabled(bool enable) { mData.mEnabled = enable; }

private:
	//SpotLightData mData;

	//class MeshComponent* mMeshRep;
};