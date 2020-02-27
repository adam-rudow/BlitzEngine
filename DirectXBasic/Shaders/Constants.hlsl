#ifndef HLSL_CONSTANTS
#define HLSL_CONSTANTS

// row major matrices
#pragma pack_matrix(row_major)

#define Deferred_Render 1

#define PI 3.14159265f

cbuffer PerCameraConstants: register(b0) 
{
	float4x4 mViewProjMatrix;
	float3 mCamWorldPos;
	float pad;
	float4x4 gVoxelViewMatrices[3];
	float4x4 gVoxelOrthoProjection;
	float4 gxyzVoxelGridCenter_wUseOcclusion;
	float4 gxVoxelCellSize_yVoxelDimension_zw;
}

cbuffer PerObjectConstants: register(b1) 
{
	float4x4 mWorldTransformMatrix;
}

struct LightData
{
	float3 mDiffuseColor;
	uint mIsShadowCaster;
	float3 mSpecularColor;
	float mSpecularPower;
	float3 mPosition;
	float mRange;
	float3 mDirection;
	float mSpotDotProduct;
	float3 mAttenuation;
	uint mEnabled;
	uint mType;
	float3 pad;
};

struct PointLightData 
{
	float3 mDiffuseColor;
	float pad1;
	float3 mSpecularColor;
	float mSpecularPower;
	float3 mPosition;
	float mRange;
	float3 mAttenuation;
	uint mEnabled;
};

struct DirectionalLightData
{
	float3 mDiffuseColor;
	int mIsShadowCaster;
	float3 mSpecularColor;
	float mSpecularPower;
	float3 mDirection;
	bool mEnabled;
};

struct SpotLightData
{
	float3 mDiffuseColor;
	float pad1;
	float3 mSpecularColor;
	float mSpecularPower;
	float3 mPosition;
	float mRange;
	float3 mDirection;
	float mSpotDotProduct;
	float3 mAttenuation;
	bool mEnabled;
};


cbuffer LightingConstants : register(b2) 
{
	float3 mAmbientLight;
	float padding;
	PointLightData mPointLights[8];
	DirectionalLightData mDirectionalLights[2];
	SpotLightData mSpotLights[4];
	LightData mLights[14];
	float4x4 mLightWVP;
}

struct Particle
{
	float3 mPos;
	float3 mVelocity;
	float2 mSize;
	float mLifespan;
};

struct PerlinSineTriple
{
	float A;
	float a;
	float B;
	float b;
	float C;
	float c;
	float2 PST_Pad;
}; // 32 bytes

struct EmitterPerlinData
{
	PerlinSineTriple mAngleWave;
	PerlinSineTriple mZDistWave;
	PerlinSineTriple mSizeWave;
	PerlinSineTriple mLifespanWave;
}; // 32 * 4 = 128 bytes

cbuffer PerParticleEmitterConstants : register(b3)
{
	float4x4 gEmitterWorldTransform;
	float2 gParticleLifeSpans;
	float2 gParticleSizes;
	float2 gParticleForces;
	float gGravitMagnitude;
	float gThetaClamp;
	float gRandomSeed;
	uint gToSpawnCount;
	EmitterPerlinData gParticlesRandomWaves;
	float2 PPEC_pad;
};


cbuffer PostProcessConstants : register(b4)
{
	float mExposure;
	bool mHDR;
	float pad1;
	float pad2;
}

cbuffer SingleLightConstant : register(b5)
{
	LightData	gLightCurrent;
	float4		gxIsShadowCaster_yzw;
};

cbuffer PerFrameConstants : register(b6)
{
	float gFrameTime;
	float gGameTime;
	float2 PFC_pad;
};


struct PS_Out_GBuffer
{
	float4 position : SV_TARGET0;
	float4 normal	: SV_TARGET1;
	float4 diffuse	: SV_TARGET2;
	//float4 shadow	: SV_TARGET3;
};

//cbuffer TimeConstants: register(b4) {
//	float mTime;
//	bool mNightVisionOn;
//	bool mPixelated;
//	bool mHDR;
//	float mExposure;
//	bool isHorizontal;
//	float pad2;
//	float pad3;
//}

SamplerState DefaultSampler : register(s2);

Texture2D DiffuseTexture : register(t2);

//Texture2D PostProcessTexture : register(t1);
//
//Texture2D NightVisionNoiseTexture : register(t2);
//
//Texture2D GaussianBlurTexture : register(t3);
//
//Texture2D BinocularTexture : register(t4);

////////////////////////////
// Per object group
////////////////////////////

Texture2D gDepthMap : register(t0); // for post processing this is the zbuffer after all objects have been drawn
SamplerState gDepthMapSampler : register(s0);
Texture2D gShadowMap : register(t1); // shadow (depth) map from light sources POV
SamplerState gShadowMapSampler : register(s1);


////////////////////////////
// Per object
////////////////////////////

Texture2D gDiffuseMap : register(t2);
SamplerState gDiffuseMapSampler : register(s2);

Texture2DArray gDiffuseMapArray : register(t2);
SamplerState gDiffuseMapArraySampler : register(s2);

TextureCube gCubeMap : register(t2);
SamplerState gCubeMapSampler : register(s2);

Texture2D gGlowMap : register(t3);
SamplerState gGlowMapSampler : register(s3);

Texture2D gBumpMap : register(t4);
SamplerState gBumpMapSampler : register(s4);

Texture2D gSpecularMap : register(t5);
SamplerState gSpecularMapSampler : register(s5);

// special maps
Texture2D gAdditionalDiffuseMap : register(t6);
SamplerState gAdditionalDiffuseMapSampler : register(s6);

Texture2D gHorizontallyBlurredGlowMap : register(t6);
SamplerState gBlurredGlowMapSampler : register(s6);

Texture2D gWindMap : register(t7);
SamplerState gWindMapSampler : register(s7);

Texture1D gRandomMap : register(t7);
SamplerState gRandomMapSampler : register(s7);

#endif