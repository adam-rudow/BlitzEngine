#include "Constants.hlsl"
#include "lighthelper.fx"
#include "shadowmaphelper.fx"


struct VS_In
{
	float3 mPosition : POSITION;
	float2 mTexCoord : TEXCOORD0;
	float3 mNormal	 : NORMAL;
};

struct PS_In
{
	float4 mPos			: SV_POSITION;
	float3 mWorldNorm	: W_NORM;
	float3 mWorldPos	: W_POSITION;
	float2 mTex			: TEXCOORD0;
};

struct MinimalMesh_VS_In
{
	float3 iPosL        : POSITION;
	float2 iColor       : TEXCOORD0;
	//float3 mNormal		: NORMAL;
};

struct MinimalMesh_PS_In
{
	float4 iPosH		: SV_POSITION;
	float3 iPosW		: POSITION;
	float2 iColor		: TEXCOORD0;
	nointerpolation uint3 iInstanceID : INSTANCE_ID;
};


MinimalMesh_PS_In VShaderPointLight(MinimalMesh_VS_In vIn)//, uint instanceID : SV_InstanceID)
{
	MinimalMesh_PS_In vOut;
	uint instanceID = 0;
	float scaleRange = mPointLights[instanceID].mRange * mPointLights[instanceID].mEnabled;
	float3 lPos = mPointLights[instanceID].mPosition;

	// Build world transform of light
	matrix <float, 4, 4> world = { scaleRange, 0, 0, 0,
									0, scaleRange, 0, 0,
									0, 0, scaleRange, 0,
									lPos.x, lPos.y, lPos.z, 1};

	vOut.iPosW = mul(float4(vIn.iPosL, 1), world).xyz;
	if (mPointLights[instanceID].mEnabled > 0)
		vOut.iPosH = mul(float4(vOut.iPosW, 1), mViewProjMatrix);
	else
		vOut.iPosH = float4(vIn.iPosL, 1);

	vOut.iColor = vIn.iColor;
	vOut.iInstanceID.x = instanceID;
	vOut.iInstanceID.y = 0; // Point
	return vOut;
}

MinimalMesh_PS_In VShaderDirectionalLight(MinimalMesh_VS_In vIn/*, uint instanceID : SV_InstanceID*/)
{
	uint instanceID = 0;
	MinimalMesh_PS_In vOut;
	if (mDirectionalLights[instanceID].mEnabled)
		vOut.iPosH = float4(vIn.iPosL, 1);
	else
		vOut.iPosH = float4(0, 0, 0, 0); // Will get skipped by rasterizer

	vOut.iPosW = vIn.iPosL;
	vOut.iColor = vIn.iColor;
	vOut.iInstanceID.x = instanceID;
	vOut.iInstanceID.y = 1; // Directional
	return vOut;
}

MinimalMesh_PS_In VShaderSpotLight(MinimalMesh_VS_In vIn, uint instanceID : SV_InstanceID)
{
	float scaleRange = mSpotLights[instanceID].mRange;
	float3 lPos = mSpotLights[instanceID].mPosition;

													 // Build world transform of light
	Matrix world = { scaleRange, 0, 0, 0,
					0, scaleRange, 0, 0,
					0, 0, scaleRange, 0,
					lPos.x, lPos.y, lPos.z, 1 };


	MinimalMesh_PS_In vOut;
	if (mSpotLights[instanceID].mEnabled)
		vOut.iPosH = mul(mul(float4(vIn.iPosL, 1), world), mViewProjMatrix);
	else
		vOut.iPosH = float4(vIn.iPosL, 1);

	vOut.iPosW = vIn.iPosL;
	vOut.iColor = vIn.iColor;
	vOut.iInstanceID.x = instanceID;
	vOut.iInstanceID.y = 2; // Spot
	return vOut;
}

float4 PShader(MinimalMesh_PS_In pIn/*, float4 screenSpace : SV_Position*/) : SV_TARGET
{
	float4 screenSpace = pIn.iPosH;
	float2 texCoord = float2(screenSpace.x / 1280.0f, screenSpace.y / 720.0f); //  TODO: pass these vars through cb or something

	float4 dPosW	= gDiffuseMap.Sample(gDiffuseMapSampler, texCoord);
	float4 dNormW	= gGlowMap.Sample(gGlowMapSampler, texCoord);
	float4 dDiffuse	= gBumpMap.Sample(gBumpMapSampler, texCoord);
	
	//return dDiffuse;

	if(dot(dNormW.xyz, dNormW.xyz) < 0.01f)// || dot(dPosW.xyz, dPosW.xyz) < 0.01f)
		return dDiffuse;

	dNormW = normalize(dNormW);

	int instanceID = pIn.iInstanceID.x;
	int type = pIn.iInstanceID.y;

	float4 combinedColor = float4(0, 0, 0, 0);// dDiffuse;
	//SurfaceInfo defaultSurface;
	//defaultSurface.pos		= dPosW.xyz;
	//defaultSurface.normal	= dNormW.xyz;
	//defaultSurface.diffuse	= combinedColor;
	//defaultSurface.spec		= float4(0.1, 0.1, 0.1, 20.0);
	float4 projectedTexCoord_shadow = mul(float4(dPosW.xyz, 1.0), mLightWVP);
	float shadowFactor = 1.0f;
	if (type == 1 && mDirectionalLights[instanceID].mIsShadowCaster == 1)
		shadowFactor = CalcShadowFactor(projectedTexCoord_shadow);

	//Combine lighting from all lights
	float4 combinedLighting = float4(0, 0, 0, 0);
	if (type == 0 && mPointLights[instanceID].mEnabled > 0.5f)
		combinedLighting = dDiffuse * ComputePointLight(mPointLights[instanceID], dPosW, dNormW);
	else if (type == 1 && mDirectionalLights[instanceID].mEnabled > 0.5f)
		combinedLighting = shadowFactor * dDiffuse * ComputeDirectionalLight(mDirectionalLights[instanceID], dPosW, dNormW);
	else if (type == 2 && mSpotLights[instanceID].mEnabled > 0.5f)
		combinedLighting = dDiffuse * ComputeSpotLight(mSpotLights[instanceID], dPosW, dNormW);
		/*	combinedLighting = make_float4(ParallelLightWithShadow(defaultSurface, gLightCurrent, xyzgEyePosW_wDoMotionBlur.xyz, shadowFactor), 1.0f);
		else
			combinedLighting = make_float4(RenderLight(defaultSurface, gLightCurrent, xyzgEyePosW_wDoMotionBlur.xyz, shadowFactor), 1.0f);*/

	combinedColor = combinedLighting;
	combinedColor.a = 1;
	// change target alpha based on glow
	//combinedColor.a = 0.51f;
	//float perPixelGlow = dDiffuse.a;//sample2D(gGlowMapSampler, pIn.iTexCoord, gGlowMap).r;
	//if (dDiffuse.a > 0.1)
	//{
	//	combinedColor = lerp(combinedColor, dDiffuse, perPixelGlow);
	//	combinedColor.a = perPixelGlow;
	//}


	// PBR test
	/*float3 kd = dDiffuse *  float3(0, 1, 0.5f);
	float3 ks = float3(1, 1, 1) * float3(0, 0.999f, 0.028f);

	float3 L = mDirectionalLights[instanceID].mDirection;
	float3 V = normalize(mCamWorldPos - dPosW);
	float3 N = dNormW;

	float3 H = normalize(L + V);
	float NdotL = saturate(dot(N, L));
	float NdotV = dot(N, V);
	float NdotH = dot(N, H);
	float LdotH = dot(L, H);
	float Roughness = 0.2f;
	
	float a_2 = Roughness * Roughness;
	float NdotL_2 = NdotL * NdotL;
	float NdotV_2 = NdotV * NdotV;
	float NdotH_2 = NdotH * NdotH;
	float OneMinusNdotL_2 = 1.0f - NdotL_2;
	float OneMinusNdotV_2 = 1.0f - NdotV_2;

	float3 Fd = float3(1, 1, 1) - ks;

	float gamma = saturate(dot(V - N * NdotV, L - N * NdotL));
	float A = 1.0f - 0.5f * (a_2 / (a_2 + 0.33f));
	float B = 0.45f * (a_2 / (a_2 + 0.09f));
	float C = sqrt(OneMinusNdotL_2 * OneMinusNdotV_2) / max(NdotL, NdotV);

	float3 Rd = kd * Fd * (A + B * gamma * C) * NdotL;

	float D = NdotH_2 * (a_2 - 1.0f) + 1.0f;

	float Fs = ks + Fd * exp(-6 * LdotH);

	float G1_1 = 1.0f + sqrt(1.0f + a_2 * (OneMinusNdotL_2 / NdotL_2));
	float G1_2 = 1.0f + sqrt(1.0f + a_2 * (OneMinusNdotV_2 / NdotV_2));
	float G = G1_1 * G1_2;

	float3 Rs = (a_2 * Fs) / (D * D * G * NdotV);

	return float4(Rd + Rs, 1.0f);*/

	return combinedColor;
}
