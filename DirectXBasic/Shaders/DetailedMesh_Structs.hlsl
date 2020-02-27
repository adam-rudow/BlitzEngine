#ifndef DETAILEDMESH_STRUCTS
#define DETAILEDMESH_STRUCTS

struct DetailedMesh_VS_In
{
	float3 mPosition : POSITION;
	float2 mTexCoord : TEXCOORD0;
	float3 mNormal	 : NORMAL;
	float3 mTangent	 : TANGENT;
};

struct DetailedMesh_PS_In
{
	float4 mPos			: SV_POSITION;
	float3 mWorldNorm	: W_NORM;
	float3 mWorldTangent: W_TANG;
	//float3 mWorldBinorm	: W_BINORM;
	float3 mWorldPos	: W_POSITION;
	float2 mTex			: TEXCOORD0;
};

#endif
