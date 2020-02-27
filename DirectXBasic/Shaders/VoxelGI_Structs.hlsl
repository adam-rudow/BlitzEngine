#ifndef VOXEL_GI_STRUCTS
#define VOXEL_GI_STRUCT

#define PI 3.14159265f

//*****************************************************************************
// Vertex Shader  Input Structure //////////
// Geo Shader input for voxel creation
struct Voxel_GS_IN
{
	float4 iPosH		: POSITION;
	float2 iTexCoord	: TEXCOORD;
	float3 iNormal		: NORMAL;
};

// Fragment shader input for voxel creation
struct Voxel_PS_IN
{
	float4 iPosH		: SV_POSITION;
	float3 iPosW		: PSIN_EXTRA_POSITION;
	float2 iTexCoord	: PSIN_PROJECTION;
	float3 iNormal		: PIPELINE_NORMAL;
};

// Geometry shader input for creating virtual point lights
struct VPLCreate_GS_IN
{
	float4 iPosH		: POSITION;
	float2 iColor		: COLOR;
	uint iInstanceID	: SV_InstanceID;
};

// Fragment shader input for creating virtual point lights
struct VPLCreate_PS_IN
{
	float4 iPosH            : SV_POSITION;
	float2 iColor           : COLOR;
	//float4 iProjTexCoord    API_SEMANTIC(PSIN_PROJECTION);
	uint iInstanceID        : SV_RenderTargetArrayIndex;
};

// Multi render target fragment shader output for virtual point light creation
struct VPL_MRT_PS_OUT
{
	float4 redColor     : SV_TARGET0;
	float4 greenColor   : SV_TARGET1;
	float4 blueColor    : SV_TARGET2;
};

struct Voxel
{
	uint4 normalMask;
	uint colorMask;
	uint occlusion;
};

//*****************************************************************************
// Helper Function Definition //////////

// Some values taken from the internets
static float3 tetrahedronFaceNormals[4] =
{
	float3(0.0f, -0.57735026f, 0.81649661f),
	float3(0.0f, -0.57735026f, -0.81649661f),
	float3(-0.81649661f, 0.57735026f, 0.0f),
	float3(0.81649661f, 0.57735026f, 0.0f)
};

static float3 cubeDirections[6] =
{
	float3(0, 0, 1.0f), float3(1.0f, 0, 0), float3(0, 0, -1.0f),
	float3(-1.0f, 0, 0), float3(0, 1.0f, 0), float3(0, -1.0f, 0),
};

// offsets to six neighbor cell centers 
static int3 gridOffsets[6] =
{
	int3(0,0,1), int3(1,0,0), int3(0,0,-1),
	int3(-1,0,0), int3(0,1,0), int3(0,-1,0)
};

// Some values taken from the internets
static float4 cubeFaceSHCoefficients[6] =
{
	/*float4(PI / (2 * sqrt(PI)), 0.0f, ((2 * PI) / 3.0f) * sqrt(3.0f / (4 * PI)), 0.0f),
	float4(PI / (2 * sqrt(PI)), 0.0f, 0.0f, -((2 * PI) / 3.0f) * sqrt(3.0f / (4 * PI))),
	float4(PI / (2 * sqrt(PI)), 0.0f, -((2 * PI) / 3.0f) * sqrt(3.0f / (4 * PI)), 0.0f),
	float4(PI / (2 * sqrt(PI)), 0.0f, 0.0f, ((2 * PI) / 3.0f) * sqrt(3.0f / (4 * PI))),
	float4(PI / (2 * sqrt(PI)), -((2 * PI) / 3.0f) * sqrt(3.0f / (4 * PI)), 0.0f, 0.0f),
	float4(PI / (2 * sqrt(PI)), ((2 * PI) / 3.0f) * sqrt(3.0f / (4 * PI)), 0.0f, 0.0f)*/
	float4(0.8862269521f, 0.0f, 1.0233267546f, 0.0f),  // ClampedCosineCoeffs(directions[0])
	float4(0.8862269521f, 0.0f, 0.0f, -1.0233267546f), // ClampedCosineCoeffs(directions[1])
	float4(0.8862269521f, 0.0f, -1.0233267546f, 0.0f), // ClampedCosineCoeffs(directions[2])
	float4(0.8862269521f, 0.0f, 0.0f, 1.0233267546f),  // ClampedCosineCoeffs(directions[3])
	float4(0.8862269521f, -1.0233267546f, 0.0f, 0.0f), // ClampedCosineCoeffs(directions[4])
	float4(0.8862269521f, 1.0233267546, 0.0f, 0.0f)    // ClampedCosineCoeffs(directions[5])
};


int GetBestTetrahedronNormalIdx(float3 normal, out float dotMax)
{
	float dot0 = dot(tetrahedronFaceNormals[0], normal);
	float dot1 = dot(tetrahedronFaceNormals[1], normal);
	float dot2 = dot(tetrahedronFaceNormals[2], normal);
	float dot3 = dot(tetrahedronFaceNormals[3], normal);

	dotMax = max(dot0, max(dot1, max(dot2, dot3)));
	if (dotMax == dot0)
		return 0;
	else if (dotMax == dot1)
		return 1;
	else if (dotMax == dot2)
		return 2;
	else
		return 3;
}

uint EncodeColorIntoUInt(float3 color, float contrast)
{
	uint result = 0;

	// Encode blue in last 8 bits
	uint blue = uint(color.z * 255.0f);
	result |= (blue & 0xff);

	// Encode green in bits 16-9
	uint green = uint(color.y * 255.0f);
	green = (green << 8);
	result |= (green & 0xff00);

	// Encode red in bits 24-17
	uint red = uint(color.x * 255.0f);
	red = (red << 16);
	result |= (red & 0xff0000);

	// Encode contrast into highest 8 bits so that it is the most important contributor to the InterlockedMax later on
	uint contrastInt = uint(contrast * 255.0f);
	contrastInt = (contrastInt << 24);
	result |= (contrastInt & 0xff000000);

	return result;
}

float3 DecodeColorFromUInt(uint colorMask)
{
	float3 voxelColor;
	voxelColor.z = float(colorMask & 0xFF) / 255.0f;

	voxelColor.y = float((colorMask & 0xFF00) >> 8) / 255.0f;

	voxelColor.x = float((colorMask & 0xFF0000) >> 16) / 255.0f;

	return voxelColor;
}

uint EncodeNormalAsUInt(float3 normal, float dotWithBest)
{
	// uint encoded format: [bestDot: 31-26][xSign: 25][xDir: 24-17][ySign: 16][yDir: 15-9][zSign: 8][zdir: 8-0]
	uint result = 0;

	// Encode z in last 9 bits
	uint z = uint(abs(normal.z * 255.0f));
	result |= (z & 0xFF);
	if (normal.z < 0)
		result |= ((1 << 8) & 0x100); // Encode sign in first bit of section (9th bit)

									  // Encode y in bits 18-10
	uint y = uint(abs(normal.y * 255.0f));
	y = ((y << 9) & 0x1FE00);
	result |= y;
	if (normal.y < 0)
		result |= ((1 << 17) & 0x20000); // Encode sign in first bit of section

										 // Encode x in bits 27-19
	uint x = uint(abs(normal.x * 255.0f));
	x = ((x << 18) & 0x3FC0000);
	result |= x;
	if (normal.x < 0)
		result |= ((1 << 26) & 0x4000000); // Encode sign in first bit of section

										   // Encode dot product with best tetrahedron face into top 5 bits of result
	uint dot = uint(saturate(dotWithBest) * 31.0f);
	result |= ((dot << 27) & 0xF8000000);

	return result;
}

float3 DecodeNormalFromUInt(uint inNorm/*, out float bestDot*/)
{
	// uint encoded format: [bestDot: 31-26][xSign: 25][xDir: 24-17][ySign: 16][yDir: 15-9][zSign: 8][zdir: 8-0]
	float3 normal;

	normal.z = float(inNorm & 0xFF) / 255.0f;
	normal.z *= (inNorm & 0x100) > 0 ? -1 : 1;

	uint y = (inNorm & 0x1FE00);
	normal.y = float(y >> 9) / 255.0f;
	normal.y *= (inNorm & 0x20000) > 0 ? -1 : 1;

	uint x = (inNorm & 0x3FC0000);
	normal.x = float(x >> 18) / 255.0f;
	normal.x *= (inNorm & 0x4000000) > 0 ? -1 : 1;

	// Encode dot product with best tetrahedron face into top 5 bits of result
	//uint dot = uint(saturate(dotWithBest) * 31);
	//result |= (dot << 27);

	return normal;
}



// VPL creation functions

// Some math taken from the internets
float4 ClampedCosineSHCoeffs(float3 dir)
{
	float4 coeffs;
	coeffs.x = .8862269262f;	//PI / (2.0f * sqrt(PI));
	coeffs.y = -1.0233267079f;	//-((2.0f * PI) / 3.0f) * sqrt(3.0f / (4.0f * PI));
	coeffs.z = 1.0233267079f;	//((2.0f * PI) / 3.0f) * sqrt(3.0f / (4.0f * PI));
	coeffs.w = -1.0233267079f;	//-((2.0f * PI) / 3.0f) * sqrt(3.0f / (4.0f * PI));
	coeffs.wyz *= dir;
	return coeffs;
}

float3 GetClosestNormalToDirection(uint4 normalMask, float3 direction, out float dotMax)
{
	float3 normal0 = DecodeNormalFromUInt(normalMask.x);
	float3 normal1 = DecodeNormalFromUInt(normalMask.y);
	float3 normal2 = DecodeNormalFromUInt(normalMask.z);
	float3 normal3 = DecodeNormalFromUInt(normalMask.w);

	float dot0 = dot(direction, normal0);
	float dot1 = dot(direction, normal1);
	float dot2 = dot(direction, normal2);
	float dot3 = dot(direction, normal3);

	dotMax = max(dot0, max(dot1, max(dot2, dot3)));
    if ((dotMax == dot0 && dotMax != 0) || (dotMax == 0 && dot(normal0, normal0) != 0))
		return normal0;
    else if ((dotMax == dot1 && dotMax != 0) || (dotMax == 0 && dot(normal1, normal1) != 0))
		return normal1;
    else if ((dotMax == dot2 && dotMax != 0) || (dotMax == 0 && dot(normal2, normal2) != 0))
		return normal2;
	else
		return normal3;
		

	/*float4x3 normalMat;
	normalMat[0] = DecodeNormalFromUInt(normalMask.x);
	normalMat[1] = DecodeNormalFromUInt(normalMask.y);
	normalMat[2] = DecodeNormalFromUInt(normalMask.z);
	normalMat[3] = DecodeNormalFromUInt(normalMask.w);

	float4 dots = mul(normalMat, direction);

	float maximum = max(max(dots.x, dots.y),
					max(dots.z, dots.w));

	int idx;
	if (maximum == dots.x)
		idx = 0;
	else if (maximum == dots.y)
		idx = 1;
	else if (maximum == dots.z)
		idx = 2;
	else 
		idx = 3;

	dotMax = dots[idx];
	return normalMat[idx];*/
}


#endif // file guard
