#pragma once
#include "LightComponent.h"

//class MeshComponent;

struct RENDERER_EXPORT DirectionalLightData
{
	DirectionalLightData();

	Vector3 mDiffuseColor;
	int mIsShadowCaster;
	Vector3 mSpecularColor;
	float mSpecularPower;
	Vector3 mDirection;
	uint32_t mEnabled;
}; // 48 bytes

class RENDERER_EXPORT DirectionalLightComponent : public LightComponent
{
public:
	DirectionalLightComponent(/*class Game& game*/);

	void Register() override;
	void Unregister() override;
	
	void CalculateShadowTransforms() override;

	//const DirectionalLightData& GetLightData() { return mData; }
	//Matrix4 GetLightViewTransform() const { return mLightViewTransform; }
	//Matrix4 GetLightProjectionTransform() const { return mLightProjectionTransform; }
	//bool IsShadowCaster() const { return mCastsShadows;  }
	//
	//void SetDiffuseColor(Vector3 diffuseColor) { mData.mDiffuseColor = diffuseColor; }
	//void SetSpecularColor(Vector3 specularColor) { mData.mSpecularColor = specularColor; }
	//void SetSpecularPower(float specularPower) { mData.mSpecularPower = specularPower; }
	//void SetLightDirection(Vector3 direction);
	//void SetEnabled(bool enable) { mData.mEnabled = enable; }
	//void SetCastShadows(bool castShadows) { mCastsShadows = castShadows; }

private:
	//DirectionalLightData mData;
	//Matrix4 mLightViewTransform;
	//Matrix4 mLightProjectionTransform;

	//bool mCastsShadows;

	class MeshComponent* mMeshRep;
};