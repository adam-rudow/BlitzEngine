#ifndef HLSL_LIGHTHELPER
#define HLSL_LIGHTHELPER

#include "Constants.hlsl"

// Light and material structs
struct SurfaceInfo
{
	float3 pos;
	float3 normal;
	float4 diffuse;
	float4 spec;
};

//------------------------------------------------------------------------------
// Constants
//------------------------------------------------------------------------------

// calculation functions for each type of light
//float3 PEParallelLight(SurfaceInfo v, Light L, float3 eyePos)
//{
//	float3 litColor = make_float3(0, 0, 0);
//	
//	//Light vector (opposite photon direction) //?: + and now works
//	float3 lightVec = -L.xyzDir_w.xyz;
//	
//	//Add the ambient term.
//    if (gxVoxelCellSize_yVoxelDimension_zw.z > 0.5f)
//        litColor += v.diffuse.xyz * L.ambient.xyz;
//	
//	//Add diffuse and spec term, if surface has los to the light
//	float diffuseFactor = max(dot(lightVec, v.normal), 0.0);
//	
//	float specPower = max(v.spec.a, 1.0);
//	float3 toEye	= -normalize(eyePos - v.pos);//?: + and now works
//	float3 R		= reflect(lightVec, v.normal);
//	float specFactor = pow(max(dot(R, toEye), 0.0), specPower);
//		
//	//diffuse and sepcular terms
//	litColor += diffuseFactor * v.diffuse.xyz * L.diffuse.xyz;
//	litColor += specFactor * v.spec.xyz * L.spec.xyz;
//	
//	return litColor;
//}
//
//float3 PEParallelLightWithShadow(SurfaceInfo v, Light L, float3 eyePos, float shadowFactor)
//{
//	float3 litColor = make_float3(0, 0, 0);
//
//	//Light vector (opposite photon direction) //?: + and now works
//	float3 lightVec = -L.xyzDir_w.xyz;
//
//	//Add the ambient term.
//    if (gxVoxelCellSize_yVoxelDimension_zw.z > 0.5f)
//	    litColor += v.diffuse.xyz * L.ambient.xyz;
//
//	//Add diffuse and spec term, if surface has los to the light
//	float diffuseFactor = max(dot(lightVec, v.normal), 0.0);
//
//	float specPower = max(v.spec.a, 1.0);
//	float3 toEye = -normalize(eyePos - v.pos);//?: + and now works
//	float3 R = reflect(lightVec, v.normal);
//	float specFactor = pow(max(dot(R, toEye), 0.0), specPower);
//
//	//diffuse and sepcular terms
//	litColor += shadowFactor * diffuseFactor * v.diffuse.xyz * L.diffuse.xyz;
//	litColor += shadowFactor * specFactor * v.spec.xyz * L.spec.xyz;
//
//	return litColor;
//}
//
float3 PEPointLight(SurfaceInfo v, PointLightData l, float3 eyePos, float shadowFactor)
{

	//return v.diffuse.xyz;

	float3 litColor = float3(0, 0, 0);
	
	// Surface to light vector
	float3 lightVec =  l.mPosition - v.pos;


	float3 norm = v.normal;
	//if (norm.x < 0) norm.x *= -1;
	//if (norm.y < 0) norm.y *= -1;
	//if (norm.z < 0) norm.z *= -1;
	//return normalize(norm);
	
	// Distance from surface to light
	float d = length(lightVec);
	
	
	if( d > l.mRange )
		return litColor;
		
	// Normalize the light vector
	lightVec /= d;
	
	
	
	float diffuseFactor = max(dot(lightVec, v.normal), 0.01);
	float specPower		= max(v.spec.a, 1.0);
	float3 toEye		= normalize(eyePos - v.pos);
	float3 R			= reflect( -lightVec, v.normal);
	float specFactor	= pow(max(dot(R, toEye), 0.0), specPower);
	
	litColor += shadowFactor * diffuseFactor * v.diffuse.xyz * l.mDiffuseColor.xyz;
	litColor += shadowFactor * specFactor * v.spec.xyz * l.mSpecularColor.xyz;

	
	// attenuate
	litColor /= dot(l.mAttenuation, float3(1.0, d, d*d));
	
	// Add ambient light
	//litColor += v.diffuse.xyz * l.ambient.xyz;
	
	return litColor;
	
}
//
//float3 PESpotLight(SurfaceInfo v, Light L, float3 eyePos, float shadowFactor)
//{
//	float3 litColor = PointLight(v, L, eyePos, shadowFactor);
//	
//	//The vector from the surface to the light
//	float3 lightVec = normalize(L.xyzPos_wEnabled.xyz- v.pos);
//	
//	//Scale color by spotlight factor
//	float s = pow(max(dot(-lightVec, L.xyzDir_w.xyz), 0.0), L.xyzAtt_wSpotPwr.w);
//	
//	return litColor * s;
//}

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


float4 ComputePointLight(LightData light, float3 pos, float3 normal)
{
	float4 retColor = float4(0.0f, 0.0f, 0.0f, 0.0f);
	float3 LVal = normalize(light.mPosition - pos);
	float N_dot_L = dot(normal, LVal);
	if (N_dot_L > 0.0f)
	{
		//Diffuse Light
		float distLightToPix = distance(light.mPosition, pos);
		float smoothStep = smoothstep(light.mAttenuation.x, light.mAttenuation.y, distLightToPix);
		float3 diffuseColor = lerp(light.mDiffuseColor, float3(0.0f, 0.0f, 0.0f), smoothStep);

		//Specular Light
		float3 VVal = normalize(mCamWorldPos - pos);
		float3 RVal = reflect(-1 * LVal, normal);
		RVal = normalize(RVal);
		float R_dot_V = dot(RVal, VVal);
		float specFactor = pow(max(0.0f, R_dot_V), light.mSpecularPower);
		float3 specColor = lerp(light.mSpecularColor, float3(0.0f, 0.0f, 0.0f), smoothStep);

		// Add light components to total
		retColor += float4(diffuseColor, 1.0f) * N_dot_L;
		retColor += float4(specColor, 1.0f) * specFactor;// pow(max(0.0f, R_dot_V), light.mSpecularPower);
	}

	return retColor;
}

float4 ComputeDirectionalLight(LightData light, float3 pos, float3 normal)
{
	float4 retColor = float4(0.0f, 0.0f, 0.0f, 0.0f);
	float3 LVal = -1 * light.mDirection;
	float N_dot_L = dot(normal, LVal);
	if (N_dot_L > 0.0f)
	{
		//Diffuse Light
		float3 diffuseColor = light.mDiffuseColor;

		//Specular Light
		float3 VVal = normalize(mCamWorldPos - pos);
		float3 RVal = reflect(-1 * LVal, normal);
		RVal = normalize(RVal);
		float R_dot_V = dot(RVal, VVal);
		float specFactor = pow(max(0.0f, R_dot_V), light.mSpecularPower);
		float3 specColor = light.mSpecularColor;

		// Add light components to total
		retColor += float4(diffuseColor, 1.0f) * N_dot_L;
		retColor += float4(specColor, 1.0f) * specFactor;// pow(max(0.0f, R_dot_V), pl.mSpecularPower);
	}

	return retColor;
}

float4 ComputeSpotLight(LightData light, float3 pos, float3 normal)
{
	float4 retColor = float4(0.0f, 0.0f, 0.0f, 0.0f);
	float3 LVal = normalize(light.mPosition - pos);
	float N_dot_L = dot(normal, LVal);
	if (N_dot_L > 0.0f)
	{
		//Diffuse Light
		float dotDirs = dot(light.mDirection, -LVal);
		// modify based on distance
		//float distLightToPix = distance(light.mPosition, pos);
		//float smoothStep = smoothstep(light.mAttenuation.x, light.mAttenuation.y, distLightToPix);
		//float3 diffuseColor = lerp(light.mDiffuseColor, float3(0.0f, 0.0f, 0.0f), smoothStep);

		// modify based on angle
		if (dotDirs < light.mSpotDotProduct)
			return retColor;
		float smoothStep = smoothstep(1.0f, light.mSpotDotProduct, dotDirs);
		float3 diffuseColor = lerp(light.mDiffuseColor, float3(0.0f, 0.0f, 0.0f), smoothStep);

		//Specular Light
		float3 VVal = normalize(mCamWorldPos - pos);
		float3 RVal = reflect(-1 * LVal, normal);
		RVal = normalize(RVal);
		float R_dot_V = dot(RVal, VVal);
		float specFactor = pow(max(0.0f, R_dot_V), light.mSpecularPower);
		float3 specColor = lerp(light.mSpecularColor, float3(0.0f, 0.0f, 0.0f), smoothStep);

		// Add light components to total
		retColor += float4(diffuseColor, 1.0f) * N_dot_L;
		retColor += float4(specColor, 1.0f) * specFactor;
	}

	return retColor;
}


//float3 RenderLight(LightData l, float3 eyePos, float3 normal, float shadowFactor)
//{
//	if (l.xRange_yType_zw.y < 0.1) // == 0)
//		return PointLight(v, l, eyePos, shadowFactor);
//	else if (l.xRange_yType_zw.y < 1.1) // == 1)
//		return ParallelLightWithShadow(v, l, eyePos, shadowFactor); //ParallelLight(v, l, eyePos);
//	else if (l.xRange_yType_zw.y < 2.1) // == 2)
//		return SpotLight(v, l, eyePos, shadowFactor);
//	else
//		return PointLight(v, l, eyePos, shadowFactor);
//}

//float3 RenderLight(SurfaceInfo v, Light l, float3 eyePos, float shadowFactor)
//{
//    if (l.xRange_yType_zw.y < 0.1) // == 0)
//		return PointLight(v, l, eyePos, shadowFactor);
//	else if (l.xRange_yType_zw.y < 1.1) // == 1)
//        return ParallelLightWithShadow(v, l, eyePos, shadowFactor); //ParallelLight(v, l, eyePos);
//	else if (l.xRange_yType_zw.y < 2.1) // == 2)
//		return SpotLight(v, l, eyePos, shadowFactor);
//	else
//		return PointLight(v, l, eyePos, shadowFactor);
//}
//
//float4 RenderAllLights(SurfaceInfo defaultSurface, float3 eyePos, float shadowFactor)
//{
//	float4 combinedLighting = make_float4(0, 0, 0, 0);
//
//	combinedLighting += make_float4(RenderLight(defaultSurface, gLights[0], xyzgEyePosW_wDoMotionBlur.xyz, shadowFactor), 1.0);
//
//	int i;
//	for (i = 1; i < NUM_LIGHT_SOURCES_DATAS; ++i)
//	{
//		//if(gLights[i].xyzPos_wEnabled.w == 1)
//			combinedLighting += make_float4(RenderLight(defaultSurface, gLights[i], eyePos, 1.0), 1.0);
//	}
//
//	return combinedLighting;
//}

#endif
