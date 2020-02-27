#pragma once
#include "LightComponent.h"
//#include "MeshComponent.h"

//class MeshComponent;

struct RENDERER_EXPORT PointLightData
{
	PointLightData();

	Vector3 mDiffuseColor;
	float pad1;
	Vector3 mSpecularColor;
	float mSpecularPower;
	Vector3 mPosition;
	float mRange;
	Vector3 mAttenuation;
	uint32_t mEnabled;
}; // 64 Bytes

class RENDERER_EXPORT PointLightComponent : public LightComponent
{
public:
	PointLightComponent(/*class Game& game*/);

	void Register() override;
	void Unregister() override;
	
	void CalculateShadowTransforms() override;

	//const PointLightData& GetLightData() { return mData; }
	//
	//void SetDiffuseColor(Vector3 diffuseColor) { mData.mDiffuseColor = diffuseColor; }
	//void SetSpecularColor(Vector3 specularColor) { mData.mSpecularColor = specularColor; }
	//void SetSpecularPower(float specularPower) { mData.mSpecularPower = specularPower; }
	//void SetLightPosition(Vector3 position);
	//void SetAttenuation(Vector3 attenuation) { mData.mAttenuation = attenuation; }
	//void SetRange(float range) { mData.mRange = range; }
	//void SetEnabled(bool enable) { mData.mEnabled = enable; }

private:
	//PointLightData mData;

	//class MeshComponent* mMeshRep;
};