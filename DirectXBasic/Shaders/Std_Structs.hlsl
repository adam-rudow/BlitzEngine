#ifndef STD_STRUCTS
#define STD_STRUCTS

struct BasicQuad_VS_In
{
	float3 mPosition : POSITION;
	float2 mTexCoord : TEXCOORD0;
};

struct StdMesh_VS_In
{
	float3 mPosition : POSITION;
	float2 mTexCoord : TEXCOORD0;
	float3 mNormal	 : NORMAL;
};

struct StdMesh_PS_In
{
	float4 mPos : SV_POSITION;
	float3 mWorldNorm : W_NORM;
	float3 mWorldPos : W_POSITION;
	float2 mTex : TEXCOORD0;
};

struct BasicMesh_VS_In
{
	float3 mPosition : POSITION;
	float2 mTexCoord : TEXCOORD0;
	float3 mNormal	 : NORMAL;
	//float4 mColor : COLOR;
};

struct BasicMesh_PS_In
{
	float4 mPos : SV_POSITION;
	float2 mTex : TEXCOORD0;
	//float4 mCol : COLOR;
};

struct Std_Particle_VS_IN//STD_PARTICLE_VS_IN
{
	float3 mPos				: POSITION;
	float3 mVelocity		: VELOCITY;
	float2 mSize			: SIZE;
	float mLifespan			: LIFESPAN;
};

struct Std_Particle_PS_IN
{
	float4 mPos				: SV_POSITION;
	float2 mTexCoord		: TEXCOORD0;
	nointerpolation float mLifespan : LIFESPAN;
};

#endif // file guard
