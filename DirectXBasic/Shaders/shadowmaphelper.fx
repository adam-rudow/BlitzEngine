#ifndef HLSL_SHADOWMAPHELPER
#define HLSL_SHADOWMAPHELPER

#define SHADOW_EPSILON 0.001
#define SMAP_SIZE 2048.0//4096.0
#define SMAP_DX (1.0 / SMAP_SIZE)

#define USE_SHADOW_MAP_FILTERING 1

float CalcShadowFactor(float4 iProjTexCoord)
{
	// Complete projection by doing division by w
	iProjTexCoord.xyz /= iProjTexCoord.w;
	
	// Points outside the light volume are in shadow - commented out for now to eliminate harsh light edges
	if (iProjTexCoord.x < -1.0 || iProjTexCoord.x > 1.0 ||
		iProjTexCoord.y < -1.0 || iProjTexCoord.y > 1.0 ||
		iProjTexCoord.z < 0.0)
		return 0.0;
		
	// Transform from NDC space to texture space
	iProjTexCoord.x = +0.5 * iProjTexCoord.x + 0.5;
	iProjTexCoord.y = -0.5 * iProjTexCoord.y + 0.5;

	// depth in NDC space
	float depth = iProjTexCoord.z;
	
	// filtering
	#if USE_SHADOW_MAP_FILTERING
		float s0 = gShadowMap.Sample(gShadowMapSampler, iProjTexCoord.xy + float2(0, 0)).r;
		float s1 = gShadowMap.Sample(gShadowMapSampler, iProjTexCoord.xy + float2(SMAP_DX, 0)).r;
		float s2 = gShadowMap.Sample(gShadowMapSampler, iProjTexCoord.xy + float2(0, SMAP_DX)).r;
		float s3 = gShadowMap.Sample(gShadowMapSampler, iProjTexCoord.xy + float2(SMAP_DX, SMAP_DX)).r;
	#else
		float s0 = gShadowMap.Sample(gShadowMapSampler, iProjTexCoord.xy).r;
	#endif
	
	float result0 = (depth <= s0 + SHADOW_EPSILON) ? 1.0 : 0.0;
	
	#if USE_SHADOW_MAP_FILTERING
		// to texel space: 0..1 per texel so fraction part is how much to blend between pixels
		float2 texelPos = SMAP_SIZE * iProjTexCoord.xy;
		
		float2 t = frac(texelPos);
		
		float result1 = (depth <= s1 + SHADOW_EPSILON) ? 1.0 : 0.0;
		float result2 = (depth <= s2 + SHADOW_EPSILON) ? 1.0 : 0.0;
		float result3 = (depth <= s3 + SHADOW_EPSILON) ? 1.0 : 0.0;
	
		return lerp(lerp(result0, result1, t.x),
					lerp(result2, result3, t.x), t.y);
	#else
		return result0;
	#endif

}

#endif
