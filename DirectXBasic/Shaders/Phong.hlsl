#include "Constants.hlsl"
#include "Std_Structs.hlsl"
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
	float4 mPos : SV_POSITION;
	float3 mWorldNorm : W_NORM;
	float3 mWorldPos : W_POSITION;
	float2 mTex : TEXCOORD0;
};


/*
float4 ComputePointLight(PointLightData pl, float3 pos, float3 normal)
{
	float4 retColor = float4(0.0f, 0.0f, 0.0f, 0.0f);
	float3 LVal = normalize(pl.mPosition - pos);
	float N_dot_L = dot(normal, LVal);
	if (N_dot_L > 0.0f) 
	{
		//Diffuse Light
		float distLightToPix = distance(pl.mPosition, pos);
		float smoothStep = smoothstep(pl.mAttenuation.x, pl.mAttenuation.y, distLightToPix);
		float3 diffuseColor = lerp(pl.mDiffuseColor, float3(0.0f, 0.0f, 0.0f), smoothStep);

		//Specular Light
		float3 VVal = normalize(mCamWorldPos - pos);
		float3 RVal = reflect(-1 * LVal, normal);
		RVal = normalize(RVal);
		float R_dot_V = dot(RVal, VVal);
		float specFactor = pow(max(0.0f, R_dot_V), pl.mSpecularPower);
		float3 specColor = lerp(pl.mSpecularColor, float3(0.0f, 0.0f, 0.0f), smoothStep);

		// Add light components to total
		retColor += float4(diffuseColor, 1.0f) * N_dot_L;
		retColor += float4(specColor, 1.0f) * specFactor;// pow(max(0.0f, R_dot_V), pl.mSpecularPower);
	}

	return retColor;
}

float4 ComputeDirectionalLight(DirectionalLightData dl, float3 pos, float3 normal)
{
	float4 retColor = float4(0.0f, 0.0f, 0.0f, 0.0f);
	float3 LVal = -1 * dl.mDirection;
	float N_dot_L = dot(normal, LVal);
	if (N_dot_L > 0.0f)
	{
		//Diffuse Light
		float3 diffuseColor = dl.mDiffuseColor;

		//Specular Light
		float3 VVal = normalize(mCamWorldPos - pos);
		float3 RVal = reflect(-1 * LVal, normal);
		RVal = normalize(RVal);
		float R_dot_V = dot(RVal, VVal);
		float specFactor = pow(max(0.0f, R_dot_V), dl.mSpecularPower);
		float3 specColor = dl.mSpecularColor;

		// Add light components to total
		retColor += float4(diffuseColor, 1.0f) * N_dot_L;
		retColor += float4(specColor, 1.0f) * specFactor;// pow(max(0.0f, R_dot_V), pl.mSpecularPower);
	}

	return retColor;
}

float4 ComputeSpotLight(SpotLightData sl, float3 pos, float3 normal)
{
	float4 retColor = float4(0.0f, 0.0f, 0.0f, 0.0f);
	float3 LVal = normalize(sl.mPosition - pos);
	float N_dot_L = dot(normal, LVal);
	if (N_dot_L > 0.0f)
	{
		//Diffuse Light
		float dotDirs = dot(sl.mDirection, -LVal);
		// modify based on distance
		//float distLightToPix = distance(sl.mPosition, pos);
		//float smoothStep = smoothstep(sl.mAttenuation.x, sl.mAttenuation.y, distLightToPix);
		//float3 diffuseColor = lerp(sl.mDiffuseColor, float3(0.0f, 0.0f, 0.0f), smoothStep);

		// modify based on angle
		if (dotDirs < sl.mSpotDotProduct)
			return retColor;
		float smoothStep = smoothstep(1.0f, sl.mSpotDotProduct, dotDirs);
		float3 diffuseColor = lerp(sl.mDiffuseColor, float3(0.0f, 0.0f, 0.0f), smoothStep);

		//Specular Light
		float3 VVal = normalize(mCamWorldPos - pos);
		float3 RVal = reflect(-1 * LVal, normal);
		RVal = normalize(RVal);
		float R_dot_V = dot(RVal, VVal);
		float specFactor = pow(max(0.0f, R_dot_V), sl.mSpecularPower);
		float3 specColor = lerp(sl.mSpecularColor, float3(0.0f, 0.0f, 0.0f), smoothStep);

		// Add light components to total
		retColor += float4(diffuseColor, 1.0f) * N_dot_L;
		retColor += float4(specColor, 1.0f) * specFactor;
	}

	return retColor;
}
*/

StdMesh_PS_In VShader(StdMesh_VS_In input)
{
	StdMesh_PS_In output;

	// Positions
	output.mPos = float4(input.mPosition, 1.0f);
	output.mPos = mul(output.mPos, mWorldTransformMatrix);
	output.mWorldPos = output.mPos.xyz;
	output.mPos = mul(output.mPos, mViewProjMatrix);

	// Normal
	float4 tempNorm = mul(float4(input.mNormal, 0.0f), mWorldTransformMatrix);
	output.mWorldNorm = tempNorm.xyz;

	// Texture Coordinate
	output.mTex = input.mTexCoord;

	return output;
}

#if Deferred_Render

PS_Out_GBuffer PShader(StdMesh_PS_In input)
{
	PS_Out_GBuffer pOut;
	float4 posW = float4(input.mWorldPos, 1.0f);
	float4 nNorm = float4(normalize(input.mWorldNorm), 0.0f);
	float4 diffuse = DiffuseTexture.Sample(DefaultSampler, input.mTex);

	pOut.position	= posW;
	pOut.normal		= nNorm;
	pOut.diffuse	= diffuse;
	return pOut;

#else

float4 PShader(StdMesh_PS_In input) : SV_TARGET
{

	float4 texCol = DiffuseTexture.Sample(DefaultSampler, input.mTex);
	float3 nNorm = normalize(input.mWorldNorm);
	float4 phongVal = float4(mAmbientLight, 0.0f);

	int i;
	for (i = 0; i < 8; ++i)
	{
		if (mPointLights[i].mEnabled > 0)
		{
			phongVal += ComputePointLight(mPointLights[i], input.mWorldPos, nNorm);
		}
	}

	for (i = 0; i < 2; ++i)
	{
		if (mDirectionalLights[i].mEnabled > 0)
		{
			phongVal += ComputeDirectionalLight(mDirectionalLights[i], input.mWorldPos, nNorm);
		}
	}

	for (i = 0; i < 4; ++i)
	{
		if (mSpotLights[i].mEnabled > 0)
		{
			phongVal += ComputeSpotLight(mSpotLights[i], input.mWorldPos, nNorm);
		}
	}

	// Do HDR stuffs
	if (mHDR)
	{
		//float mExposure = 0.5;
		phongVal = phongVal * texCol;
		float4 mapped = float4(1.0, 1.0, 1.0, 1.0) - exp(-phongVal * mExposure);
		return saturate(mapped);
	}

	phongVal = saturate(phongVal);
	return (phongVal * texCol);

#endif

}
