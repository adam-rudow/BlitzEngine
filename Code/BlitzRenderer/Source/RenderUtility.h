#pragma once

#include <d3d11.h>
//#include <D3Dx11.h>
//#include <D3Dx10.h>
//#include <d3dx11effect.h>

#include <string>


// Shader Types
typedef ID3D11PixelShader			PixelShader;
typedef ID3D11VertexShader			VertexShader;
typedef ID3D11GeometryShader		GeometryShader;
typedef ID3D11ComputeShader			ComputeShader;

// Other Render settings
typedef ID3D11InputLayout			VertexInputLayout;

// Resource Views
typedef ID3D11RenderTargetView		RenderTargetView;
typedef ID3D11ShaderResourceView	ShaderResourceView;
typedef ID3D11UnorderedAccessView	UnorderedAccessView;

// Resources
typedef ID3D11Buffer				BufferGPU;
typedef ID3D11Texture2D				TextureD3D;
typedef D3D11_VIEWPORT				Viewport;

enum VertexFormatGPU
{
	VertexFormat_MinimalMesh,
	VertexFormat_StdMesh,
	VertexFormat_DetailedMesh,
	VertexFormat_StdParticle,
	VertexFormat_Count
};

enum ShaderType
{
	ST_NoDefault,
	ST_BasicQuad,
	ST_BasicMesh,
	ST_Lit,
	ST_DetailedLit,
	ST_DeferredLightEffectPL,
	ST_DeferredLightEffectDL,
	ST_DeferredLightEffectSL
};

enum DrawPassType
{
	DrawPass_Default,
	DrawPass_DepthOnly,
	DrawPass_Voxelization
};

enum DrawCallType
{
	VBUF_INDBUF,
	VBUF,
	COMPUTE,
	VBUF_INDIRECT,
};

//enum DrawOrder
//{
//	DrawOrder_First = 1,
//	DrawOrder_Default = 2,
//	DrawOrder_Last = 4
//};

enum ConstantBufferBindSlot
{
	PerCameraSlot,
	PerObjectSlot,
	AllLightsSlot,
	PerParticleEmitterSlot,
	PostProcessSlot,
	PerLightSlot,
	PerFrameSlot,
	ConstantBufferBindSlot_Count
};

enum GPUBindSlot
{
	// per object group
	DEPTHMAP_TEXTURE_2D_SAMPLER_SLOT = 0, // register(t0)
	SHADOWMAP_TEXTURE_2D_SAMPLER_SLOT = 1, // register(t1)

	// per object/draw call
	DIFFUSE_TEXTURE_2D_SAMPLER_SLOT = 2, // register(t2)
	DIFFUSE_TEXTURE_2D_ARRAY_SLOT = 2, // register(t2)
	DIFFUSE_TEXTURE_CUBE_SLOT = 2, // register(t2)

	GLOW_TEXTURE_2D_SAMPLER_SLOT = 3,
	BUMPMAP_TEXTURE_2D_SAMPLER_SLOT = 4,
	SPECULAR_TEXTURE_2D_SAMPLER_SLOT = 5,

	ADDITIONAL_DIFFUSE_TEXTURE_2D_SAMPLER_SLOT = 6,
	DIFFUSE_BLUR_TEXTURE_2D_SAMPLER_SLOT = 6,

	WIND_TEXTURE_2D_SAMPLER_SLOT = 7,
	RANDOM_TEXTURE_1D_SAMPLER_SLOT = 7,

	NUM_GPU_SAMPLERS, // how many samplers can be bound at the same time

	// we can surpass NUM_GPU_SAMPLERS since below buffer ids dont require samplers

	GpuResourceSlot_ParticleDataCS_SRV_ConstResource = 70,

	GpuResourceSlot_FirstUAV = 1024, // all values past thsi will be subreacted 1024
	GpuResourceSlot_ParticleDataCSUAV = GpuResourceSlot_FirstUAV,	// register(u0)

	GpuResourceSlot_SecondUAV = GpuResourceSlot_FirstUAV + 1,					// register(u1)
	GpuResourceSlot_ParticleSpawns_ConsumeBuffer = GpuResourceSlot_SecondUAV,	// register(u1)

	GpuResourceSlot_ThirdUAV = GpuResourceSlot_SecondUAV + 1,					// register(u2)
	GpuResourceSlot_VoxelGridStructuredBuffer_UAV = GpuResourceSlot_FirstUAV + 2, // register(u2)

	GpuResourceSlot_VoxelGridStructuredBuffer_3RT_UAV = GpuResourceSlot_FirstUAV + 3 // register(u3)
};

struct ShaderPair
{
	ID3D11VertexShader* mVertexShader;
	ID3D11PixelShader* mPixelShader;
};

static VertexFormatGPU GetFormatFromVSName(const char *vsFilename)
{
	std::string s(vsFilename);

	if (s.substr(0, 12) == "MinimalMesh_"	|| s == "Shaders/BasicMesh.hlsl" || s == "Shaders/BasicQuad.hlsl")
		return VertexFormat_MinimalMesh;
	if (s.substr(8, 8) == "StdMesh_"		|| s == "Shaders/Phong.hlsl")
		return VertexFormat_StdMesh;
	if (s.substr(0, 13) == "DetailedMesh_")
		return VertexFormat_DetailedMesh;
	if (s.substr(0, 12) == "StdParticle_")
		return VertexFormat_StdParticle;

	return VertexFormat_Count;
}
