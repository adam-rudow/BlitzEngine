//#include "GraphicsDriver.h"
//#include "Renderer.h"
//#include "AssertDbg.h"
//#include "PreCompiledHeader.h"
#include "ResourceBufferGPU.h"
#include "Effect.h"
#include "Texture.h"
#include "RasterizerStateManager.h"
#include "AlphaBlendStateManager.h"
#include "DepthStencilStateManager.h"
#include "SamplerState.h"

#include "Utils/AssertDbg.h"
#include "Logging/Log.h"

#include <iostream>
#include <sstream>

#include <d3dx11.h>

// include Direct3D Library files
//#pragma comment (lib, "d3d11.lib")
//#pragma comment (lib, "d3dx11.lib")

ImplSingleton(GraphicsDriver)

GraphicsDriver::GraphicsDriver()
{
#if HDR
	mRTV_Format = DXGI_FORMAT_R16G16B16A16_FLOAT;
#else
	mRTV_Format = DXGI_FORMAT_R8G8B8A8_UNORM;
#endif
}


GraphicsDriver::~GraphicsDriver()
{
	// Destroy Managers
	delete(AlphaBlendStateManager::Instance());
	delete(RasterizerStateManager::Instance());
	delete(DepthStencilStateManager::Instance());
	delete(SamplerStateManager::Instance());


	// Release Core D3D objects
	if (g_SwapChain)
	{
		g_SwapChain->SetFullscreenState(FALSE, NULL);	// switch to windowed mode; 
		g_SwapChain->Release();
	}
	if (m_BackBuffer)		m_BackBuffer->Release();
	if (g_DeviceContext)	g_DeviceContext->Release();
	if (pVBuffer)			pVBuffer->Release();
	//if (mCurrentDepthStencil) mCurrentDepthStencil->Release();;
	if (mMeshDepthStencil)	mMeshDepthStencil->Release();
	if (mMeshDepthState)	mMeshDepthState->Release();

#if _DEBUG
	ID3D11Debug* debugDevice = NULL;
	HRESULT hr = g_Device->QueryInterface(__uuidof(ID3D11Debug), reinterpret_cast<void**>(&debugDevice));
	assert(hr == S_OK);

	debugDevice->ReportLiveDeviceObjects(D3D11_RLDO_DETAIL);
	debugDevice->Release();
#endif

	if (g_Device) g_Device->Release();

}

void GraphicsDriver::InitGraphics(HWND hWnd)
{
	InitD3D(hWnd);

	RasterizerStateManager::Construct();
	AlphaBlendStateManager::Construct();
	DepthStencilStateManager::Construct();
	SamplerStateManager::Construct();

	mMeshDepthStencil = CreateDepthStencil();
	SetDepthStencil(mMeshDepthStencil);

	mMeshDepthState = CreateDepthStencilState(true);
	SetDepthStencilState(mMeshDepthState);
}

void GraphicsDriver::InitD3D(HWND hWnd)
{
	DWORD createDeviceFlags = 0;
#ifdef _DEBUG
	createDeviceFlags |= D3D11_CREATE_DEVICE_DEBUG;
#endif
	// create a struct to hold info about the swap chain
	DXGI_SWAP_CHAIN_DESC scd;

	// clear out the struct for use
	ZeroMemory(&scd, sizeof(DXGI_SWAP_CHAIN_DESC));

	// fill the swap chain description struct
	scd.BufferCount = 1;									// one back buffer, 2 if triple buffering
	scd.BufferDesc.Format = mRTV_Format;					// use 32-bit colors, or 64-bit if HDR
	scd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;		// how swap chain is to be used 
	scd.OutputWindow = hWnd;								// the window to be used
	scd.SampleDesc.Count = 1;								// how many multisamples
	scd.Windowed = TRUE;									// windowed/fullscreen mode
	scd.BufferDesc.Width = Renderer::Window_Width;
	scd.BufferDesc.Height = Renderer::Window_Height;
	//scd.SampleDesc.Count = 4;
	//scd.SampleDesc.Quality = 1;
	scd.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;		// Allow full-screen switching

	//******************************************
	// Create Device
	D3D_FEATURE_LEVEL level;
	D3D_FEATURE_LEVEL levelsWanted[] =
	{
		//TODO: Update directX
		D3D_FEATURE_LEVEL_11_1,
		D3D_FEATURE_LEVEL_11_0,
		D3D_FEATURE_LEVEL_10_1,
		D3D_FEATURE_LEVEL_10_0
	};
	UINT numLevelsWanted = sizeof(levelsWanted) / sizeof(levelsWanted[0]);

															// create a device, device context, and swap chain using the info in the scd struct
	HRESULT hr = D3D11CreateDeviceAndSwapChain(
		NULL,
		D3D_DRIVER_TYPE_HARDWARE,
		NULL,
		createDeviceFlags,
		levelsWanted,//NULL,
		numLevelsWanted,//NULL,
		D3D11_SDK_VERSION,
		&scd,
		&g_SwapChain,
		&g_Device,
		&level,//NULL,
		&g_DeviceContext);

	if (SUCCEEDED(hr))
	{
		BZ_CORE_TRACE("\nBlitzEngine: Created D3D Device:");
		if (level == D3D_FEATURE_LEVEL_11_0)
		{
			BZ_CORE_TRACE("D3D_FEATURE_LEVEL_11_0\n");
		}
		else if (level == D3D_FEATURE_LEVEL_10_1)
		{
			BZ_CORE_TRACE("D3D_FEATURE_LEVEL_10_1\n");
		}
		else if (level == D3D_FEATURE_LEVEL_10_0)
		{
			BZ_CORE_TRACE("D3D_FEATURE_LEVEL_10_0\n");
		}
	}
	/*else
	{
		if (driverTypes[iDrv] == D3D_DRIVER_TYPE_HARDWARE)
		{
			OutputDebugStringA("PyEgnine2.0: WARNING: Could not created D3D device with HW driver\n");
		}
	}*/

	BZ_ASSERT(hr == S_OK, "Failed to init directX stuffs\n");
	
	// get the address of the back buffer
	ID3D11Texture2D *pBackBuffer;
	hr = g_SwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)&pBackBuffer);
	BZ_ASSERT(hr == S_OK, "Failed to get the back buffer\n");

	// temp try
	D3D11_RENDER_TARGET_VIEW_DESC rtvd;
	ZeroMemory(&rtvd, sizeof(D3D11_RENDER_TARGET_VIEW_DESC));
	rtvd.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
	rtvd.Format = mRTV_Format;	// 32-bit colors, or 64-bit if HDR

	// use the back buffer address to create the render target
	hr = g_Device->CreateRenderTargetView(pBackBuffer, &rtvd, &m_BackBuffer); // desc was NULL before
	BZ_ASSERT(hr == S_OK, "Failed to create the back buffer\n");
	pBackBuffer->Release();

	// set the render target as the back buffer
	g_DeviceContext->OMSetRenderTargets(1, &m_BackBuffer, NULL);


	// Set the Viewport
	D3D11_VIEWPORT viewport;
	ZeroMemory(&viewport, sizeof(D3D11_VIEWPORT));
	viewport.TopLeftX = 0;
	viewport.TopLeftY = 0;
	viewport.Width = static_cast<float>(Renderer::Window_Width);
	viewport.Height = static_cast<float>(Renderer::Window_Height);
	viewport.MinDepth = 0.0f;
	viewport.MaxDepth = 1.0f;

	g_DeviceContext->RSSetViewports(1, &viewport);

	///
	
}

std::wstring ToWideString(const std::string& str)
{
	int stringLength = MultiByteToWideChar(CP_ACP, 0, str.data(), str.length(), 0, 0);
	std::wstring wstr(stringLength, 0);
	MultiByteToWideChar(CP_ACP, 0,  str.data(), str.length(), &wstr[0], stringLength);
	return wstr;
}

ID3D11VertexShader* GraphicsDriver::CreateVertexShader(ID3D10Blob** vsBlob, const char* shaderFileName, const char* shaderEntryName, D3D_SHADER_MACRO* macros)
{
	DWORD compileFlags = 0;
#if _DEBUG
	compileFlags = D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION;
#endif
	// compile hlsl shader file
	ID3D10Blob* pErrorBlob;
	HRESULT hr = D3DX11CompileFromFile(shaderFileName, macros, 0, shaderEntryName, "vs_5_0", compileFlags, 0, 0, vsBlob, &pErrorBlob, 0);
	if (FAILED(hr))
	{
		if (pErrorBlob != NULL)
		{
			std::string s((char*)pErrorBlob->GetBufferPointer());
			s = "Shader Compile Error :: " + s;
			OutputDebugString(s.c_str());
			//printf((char*)pErrorBlob->GetBufferPointer());
		}
	}
	if (pErrorBlob != NULL)
		pErrorBlob->Release();
	BZ_ASSERT(hr == S_OK, "Failed to compile vertex shader.\n");

	// encapsulate both shaders into shader objects
	ID3D11VertexShader* pVS;
	hr = g_Device->CreateVertexShader((*vsBlob)->GetBufferPointer(), (*vsBlob)->GetBufferSize(), NULL, &pVS);
	BZ_ASSERT(hr == S_OK, "Failed to create vertex shader.\n");

	return pVS;
}

ID3D11PixelShader* GraphicsDriver::CreatePixelShader(ID3D10Blob** psBlob, const char* shaderFileName, const char* shaderEntryName, D3D_SHADER_MACRO* macros)
{
	DWORD compileFlags = 0;
#if _DEBUG
	compileFlags = D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION;
#endif
	// compile hlsl shader file
	ID3D10Blob* pErrorBlob;
	HRESULT hr = D3DX11CompileFromFile(shaderFileName, macros, 0, shaderEntryName, "ps_5_0", compileFlags, 0, 0, psBlob, &pErrorBlob, 0);
	if (FAILED(hr))
	{
		if (pErrorBlob != NULL)
		{
			std::string s((char*)pErrorBlob->GetBufferPointer());
			s = "Shader Compile Error :: " + s;
			OutputDebugString(s.c_str());
			//printf((char*)pErrorBlob->GetBufferPointer());
		}
	}
	if (pErrorBlob != NULL)
		pErrorBlob->Release();
	BZ_ASSERT(hr == S_OK, "Failed to compile pixel shader.\n");

	// encapsulate both shaders into shader objects
	ID3D11PixelShader* pPS;
	g_Device->CreatePixelShader((*psBlob)->GetBufferPointer(), (*psBlob)->GetBufferSize(), NULL, &pPS);

	return pPS;
}

GeometryShader* GraphicsDriver::CreateGeometryShader(ID3D10Blob** gsBlob, const char* shaderFileName, const char* shaderEntryName, D3D_SHADER_MACRO* macros)
{
	DWORD compileFlags = 0;
#if _DEBUG
	compileFlags = D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION;
#endif
	// compile hlsl shader file
	ID3D10Blob* pErrorBlob;
	HRESULT hr = D3DX11CompileFromFile(shaderFileName, macros, 0, shaderEntryName, "gs_5_0", compileFlags, 0, 0, gsBlob, &pErrorBlob, 0);
	if (FAILED(hr))
	{
		if (pErrorBlob != NULL)
		{
			std::string s((char*)pErrorBlob->GetBufferPointer());
			s = "Shader Compile Error :: " + s;
			OutputDebugString(s.c_str());
			//printf((char*)pErrorBlob->GetBufferPointer());
		}
	}
	if (pErrorBlob != NULL)
		pErrorBlob->Release();
	BZ_ASSERT(hr == S_OK, "Failed to compile pixel shader.\n");

	// encapsulate both shaders into shader objects
	ID3D11GeometryShader* pGS;
	g_Device->CreateGeometryShader((*gsBlob)->GetBufferPointer(), (*gsBlob)->GetBufferSize(), NULL, &pGS);

	return pGS;
}

ComputeShader* GraphicsDriver::CreateComputeShader(ID3D10Blob** csBlob, const char* shaderFileName, const char* shaderEntryName, D3D_SHADER_MACRO* macros)
{
	DWORD compileFlags = 0;
#if _DEBUG
	compileFlags = D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION;
#endif
	// compile hlsl shader file
	ID3D10Blob* pErrorBlob;
	HRESULT hr = D3DX11CompileFromFile(shaderFileName, macros, 0, shaderEntryName, "cs_5_0", compileFlags, 0, 0, csBlob, &pErrorBlob, 0);
	if (FAILED(hr))
	{
		if (pErrorBlob != NULL)
		{
			std::string s((char*)pErrorBlob->GetBufferPointer());
			s = "Shader Compile Error :: " + s;
			OutputDebugString(s.c_str());
			//printf((char*)pErrorBlob->GetBufferPointer());
		}
	}
	if (pErrorBlob != NULL)
		pErrorBlob->Release();
	BZ_ASSERT(hr == S_OK, "Failed to compile pixel shader.\n");

	// encapsulate both shaders into shader objects
	ID3D11ComputeShader* pCS;
	g_Device->CreateComputeShader((*csBlob)->GetBufferPointer(), (*csBlob)->GetBufferSize(), NULL, &pCS);

	return pCS;
}

//ID3DX11Effect* GraphicsDriver::CreateShaderEffect(const char* shaderFileName)
//{
////	DWORD compileFlags = 0;
////#if _DEBUG
////	compileFlags = D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION;
////#endif
//
//	// compile hlsl shader file
//	ID3DBlob* blobEffects = NULL;
//	ID3DBlob* blobErrors = NULL;
//	HRESULT hr = D3DX11CompileFromFile(shaderFileName, NULL, NULL, NULL, "fx_5_0", D3DCOMPILE_ENABLE_STRICTNESS, 0, NULL, &blobEffects, &blobErrors, NULL);
//	BZ_ASSERT(hr == S_OK, "Failed to compile pixel shader.\n");
//
//	// encapsulate both shaders into shader objects
//	ID3DX11Effect* pFX;
//	//hr = D3DX11CreateEffectFromMemory(blobEffects->GetBufferPointer(), blobEffects->GetBufferSize(), D3DCOMPILE_ENABLE_STRICTNESS, g_Device, &pFX);
//
//	return NULL;
//}

ID3D11ShaderResourceView* GraphicsDriver::CreateSRVTextureFromFile(std::string fileName)
{
	ID3D11ShaderResourceView* texView;
	HRESULT hr = D3DX11CreateShaderResourceViewFromFile(g_Device, (LPCSTR)fileName.c_str(), NULL, NULL, &texView, NULL);
	BZ_ASSERT(hr == S_OK, "Failed to load texture file.");

	std::stringstream strm;
	strm << std::hex << (unsigned)texView;
	std::string msg("\nCreated SRV texture " + fileName + " at location " + strm.str());
	BEPrint(msg.c_str());

	return texView;
}

ID3D11SamplerState* GraphicsDriver::CreateSamplerState()
{
	// Create the sample state
	D3D11_SAMPLER_DESC desc;
	ZeroMemory(&desc, sizeof(desc));
	desc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	desc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	desc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	desc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	desc.ComparisonFunc = D3D11_COMPARISON_NEVER;
	desc.MinLOD = 0;
	desc.MaxLOD = D3D11_FLOAT32_MAX;
	ID3D11SamplerState* samplerState = nullptr;
	HRESULT hr = g_Device->CreateSamplerState(&desc, &samplerState);
	BZ_ASSERT(hr == S_OK, "Failure Creating Sampler State");

	return samplerState;
}

//void GraphicsDriver::CreateInputLayout(const D3D11_INPUT_ELEMENT_DESC* desc, int numParams, ID3D10Blob* compiledVSCode, ID3D11InputLayout* layoutRef)

ID3D11InputLayout* GraphicsDriver::CreateInputLayout(const D3D11_INPUT_ELEMENT_DESC* desc, int numParams, ID3D10Blob** compiledVSCode)
{
	// Create the input layout
	ID3D11InputLayout* layoutRef = nullptr;
	HRESULT hr = g_Device->CreateInputLayout(desc, numParams, (*compiledVSCode)->GetBufferPointer(), (*compiledVSCode)->GetBufferSize(), &layoutRef);
	BZ_ASSERT(hr == S_OK, "Failed to create input layout.");
	
	g_DeviceContext->IASetInputLayout(layoutRef);

	return layoutRef;


	//////////////////////////////////////////////////

	// temp triangle list using vertex struct
	//Vertex vertices[] =
	//{
	//	{ 0.0f, 0.5f, 0.0f, D3DXCOLOR(1.0f, 0.0f, 0.0f, 1.0f) },
	//	{ 0.45f, -0.5f, 0.0f, D3DXCOLOR(0.0f, 1.0f, 0.0f, 1.0f) },
	//	{ -0.45f, -0.5f, 0.0f, D3DXCOLOR(0.0f, 0.0f, 1.0f, 1.0f) }
	//};

	//// create the vertex buffer
	//D3D11_BUFFER_DESC bd;
	//ZeroMemory(&bd, sizeof(bd));

	//bd.Usage = D3D11_USAGE_DYNAMIC;					// write access by CPU and GPU
	//bd.ByteWidth = sizeof(Vertex) * 3;				// size is the Vertex struct * 3
	//bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;		// use as a vertex buffer
	//bd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;		// allow CPU to write in buffer

	// hr = g_Device->CreateBuffer(&bd, NULL, &pVBuffer);	// create the buffer
	//BZ_ASSERT(hr == S_OK, "Failed to create vertex buffer.");

	//												// copy the vertices into the buffer
	//D3D11_MAPPED_SUBRESOURCE ms;
	//hr = g_DeviceContext->Map(pVBuffer, NULL, D3D11_MAP_WRITE_DISCARD, NULL, &ms);
	//BZ_ASSERT(hr == S_OK, "Failed to map vertex buffer to GPU.");

	//memcpy(ms.pData, vertices, sizeof(vertices));
	//g_DeviceContext->Unmap(pVBuffer, NULL);
}

ID3D11Buffer* GraphicsDriver::CreateVertexBuffer(std::vector<float>& vertData, bool allowIndirectDraw)
{

	//float vertices[] =
	//{
	//	0.0f, 0.5f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f,  // top left
	//	0.45f, -0.5f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f,   // top right
	//	-0.45f, -0.5f, 0.0f, 0.0f, 0.0f, 1.0f, 1.0f  // bottom right
	//											   //-0.5f, -0.5f, 0.0f, 0.0f, 0.0f, 1.0f, // bottom left
	//};
	// create the vertex buffer
	//const void* data = vertData.data();
	ID3D11Buffer* buffer = nullptr;

	D3D11_BUFFER_DESC bd;
	ZeroMemory(&bd, sizeof(bd));

	bd.Usage = D3D11_USAGE_DEFAULT;// D3D11_USAGE_DYNAMIC;					// write access by CPU and GPU
	bd.ByteWidth = sizeof(float) * vertData.size();	// size is the Vertex struct * 3
	bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;		// use as a vertex buffer

	bd.MiscFlags = allowIndirectDraw ? D3D11_RESOURCE_MISC_DRAWINDIRECT_ARGS : 0;

	//bd.CPUAccessFlags = 0;/*D3D11_CPU_ACCESS_WRITE;	*/	// allow CPU to write in 

	// Fill buffer with data to avoid having to map it seperately
	D3D11_SUBRESOURCE_DATA initialData;
	ZeroMemory(&initialData, sizeof(initialData));
	initialData.pSysMem = (void*)vertData.data();

	HRESULT hr = g_Device->CreateBuffer(&bd, &initialData/*NULL*/, &buffer);	// create the buffer
	BZ_ASSERT(hr == S_OK, "Failed to create vertex buffer.");

	// copy the vertices into the buffer
	/*D3D11_MAPPED_SUBRESOURCE ms;
	hr = g_DeviceContext->Map(buffer, NULL, D3D11_MAP_WRITE_DISCARD, NULL, &ms);
	BZ_ASSERT(hr == S_OK, "Failed to map vertex buffer to GPU.");
	memcpy(ms.pData, vertData.data(), vertData.size() * sizeof(float));
	g_DeviceContext->Unmap(buffer, NULL);*/

	return buffer;
}

BufferGPU* GraphicsDriver::CreateVertexBufferFromSrc_Minimal(std::vector<float>& verts, std::vector<float>& texCoords)
{
	// Weave vertices adn texCoords together into one list
	std::vector<float> vertexBuffer(verts.size() + texCoords.size());// +verts.size()/*normals*/);
	for (unsigned i = 0, k = 0, j = 0; i < verts.size() && k < texCoords.size(); i += 3, k += 2, j += 5)
	{
		vertexBuffer[j] = verts[i];
		vertexBuffer[j + 1] = verts[i + 1];
		vertexBuffer[j + 2] = verts[i + 2];
		vertexBuffer[j + 3] = texCoords[k];
		vertexBuffer[j + 4] = texCoords[k + 1];
	}

	return CreateVertexBuffer(vertexBuffer);
}

ID3D11Buffer* GraphicsDriver::CreateIndexBuffer(std::vector<uint32_t>& idxData)
{
	ID3D11Buffer* buffer = nullptr;

	D3D11_BUFFER_DESC bd;
	ZeroMemory(&bd, sizeof(bd));

	bd.Usage = /*D3D11_USAGE_IMMUTABLE;*/ D3D11_USAGE_DYNAMIC;	// write access by CPU and GPU
	bd.ByteWidth = sizeof(float) * idxData.size();				// size is the Vertex struct * 3
	bd.BindFlags = D3D11_BIND_INDEX_BUFFER;						// use as a vertex buffer
	bd.CPUAccessFlags = /*0;*/D3D11_CPU_ACCESS_WRITE;			// allow CPU to write in buffer

																// Fill buffer with data to avoid having to map it seperately
	D3D11_SUBRESOURCE_DATA initialData;
	ZeroMemory(&initialData, sizeof(initialData));
	initialData.pSysMem = (void*)idxData.data();

	HRESULT hr = g_Device->CreateBuffer(&bd, &initialData/*NULL*/, &buffer);	// create the buffer
	BZ_ASSERT(hr == S_OK, "Failed to create index buffer.");

	return buffer;
}

BufferGPU* GraphicsDriver::CreateResourceBuffer(ResourceBufferGPU* rbgpu, void *pData, UINT32 structSize, UINT32 numStructs, bool needsAppendFlag /*false*/)
{
	HRESULT hr;
	ID3D11Buffer* buffer = nullptr;

	D3D11_BUFFER_DESC sbDesc;
	sbDesc.BindFlags = D3D11_BIND_UNORDERED_ACCESS | D3D11_BIND_SHADER_RESOURCE;
	sbDesc.CPUAccessFlags = 0;
	sbDesc.MiscFlags = needsAppendFlag ? D3D11_RESOURCE_MISC_BUFFER_STRUCTURED : 0;
	sbDesc.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;
	sbDesc.StructureByteStride = structSize;
	sbDesc.ByteWidth = structSize * numStructs;
	sbDesc.Usage = D3D11_USAGE_DEFAULT;
	if (pData != NULL)
	{
		D3D11_SUBRESOURCE_DATA  InitData;
		InitData.pSysMem = pData;
		hr = g_Device->CreateBuffer(&sbDesc, &InitData, &buffer);
	}
	else
	{
		hr = g_Device->CreateBuffer(&sbDesc, 0, &buffer);
	}

	// UAV
	ID3D11UnorderedAccessView** uav = rbgpu->GetUAV();
	if (*uav != NULL)
	{
		D3D11_UNORDERED_ACCESS_VIEW_DESC sbUAVDesc;
		sbUAVDesc.Buffer.FirstElement = 0;
		sbUAVDesc.Buffer.Flags = needsAppendFlag ? D3D11_BUFFER_UAV_FLAG_APPEND : 0;
		sbUAVDesc.Buffer.NumElements = numStructs;
		sbUAVDesc.Format = DXGI_FORMAT_UNKNOWN;
		sbUAVDesc.ViewDimension = D3D11_UAV_DIMENSION_BUFFER;
		hr = g_Device->CreateUnorderedAccessView(buffer, &sbUAVDesc, uav);
		BZ_ASSERT(hr == S_OK, "");
	}

	// SRV
	ID3D11ShaderResourceView** srv = rbgpu->GetSRV();
	if (*srv != NULL)
	{
		D3D11_SHADER_RESOURCE_VIEW_DESC sbSRVDesc;
		sbSRVDesc.Buffer.ElementOffset = 0;
		sbSRVDesc.Buffer.ElementWidth = structSize;
		sbSRVDesc.Buffer.FirstElement = 0;
		sbSRVDesc.Buffer.NumElements = numStructs;
		sbSRVDesc.Format = DXGI_FORMAT_UNKNOWN;
		sbSRVDesc.ViewDimension = D3D11_SRV_DIMENSION_BUFFER;
		hr = g_Device->CreateShaderResourceView(buffer, &sbSRVDesc, srv);
		BZ_ASSERT(hr == S_OK, "");
	}

	rbgpu->SetSamplerState(&SamplerStateManager::Instance()->GetSamplerState(SamplerState_NoMips_NoMinTexelLerp_NoMagTexelLerp_Clamp));
	return buffer;
}

BufferGPU* GraphicsDriver::CreateIndirectArgumentsBuffer(void* pData)
{

	D3D11_BUFFER_DESC bd;
	ZeroMemory(&bd, sizeof(bd));

	bd.Usage			= D3D11_USAGE_DEFAULT;					// write access by GPU
	bd.ByteWidth		= sizeof(UINT32) * 4;
	bd.BindFlags		= 0;		
	bd.CPUAccessFlags	= 0;
	bd.StructureByteStride = 0;
	bd.MiscFlags		= D3D11_RESOURCE_MISC_DRAWINDIRECT_ARGS;


	// Fill buffer with data to avoid having to map it seperately
	D3D11_SUBRESOURCE_DATA initialData;
	ZeroMemory(&initialData, sizeof(initialData));
	initialData.pSysMem = pData;

	ID3D11Buffer* buffer = nullptr;
	HRESULT hr = g_Device->CreateBuffer(&bd, &initialData, &buffer);	// create the buffer
	BZ_ASSERT(hr == S_OK, "Failed to create vertex buffer.");

	return buffer;
}

ID3D11Buffer* GraphicsDriver::CreateConstantBuffer(unsigned size)
{
	ID3D11Buffer* buffer = nullptr;

	D3D11_BUFFER_DESC bd;
	ZeroMemory(&bd, sizeof(bd));

	bd.Usage = D3D11_USAGE_DYNAMIC;					// write access by CPU and GPU
	bd.ByteWidth = size;				// size is the Vertex struct * 3
	bd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;		// use as a vertex buffer
	bd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;		// allow CPU to write in buffer

	HRESULT hr = g_Device->CreateBuffer(&bd, NULL, &buffer);	// create the buffer
	BZ_ASSERT(hr == S_OK, "Failed to create vertex buffer.");

	return buffer;
}

void GraphicsDriver::CreateRasterizerState(RasterizerState* rasterizerState)
{
	//let's set CCW as the front face, since right hand rule is nice...
	//D3D11_RASTERIZER_DESC rastDesc;
	//ZeroMemory(&rastDesc, sizeof(rastDesc));
	//rastDesc.FillMode = D3D11_FILL_SOLID;
	//rastDesc.CullMode = D3D11_CULL_NONE; //D3D11_CULL_BACK;
	//rastDesc.FrontCounterClockwise = false;
	D3D11_RASTERIZER_DESC rastDesc;
	ZeroMemory(&rastDesc, sizeof(rastDesc));
	rastDesc.CullMode = rasterizerState->mCullMode;
	rastDesc.FillMode = rasterizerState->mFillMode;
	rastDesc.MultisampleEnable = rasterizerState->mMSAAEnabled;
	rastDesc.FrontCounterClockwise = false;
	//rastDesc.DepthClipEnable = true;

	//ID3D11RasterizerState* rastState;
	auto hr = g_Device->CreateRasterizerState(&rastDesc, &rasterizerState->mRasterizerStateObject);
	BZ_ASSERT(hr == S_OK, "Problem Creating Rasterizer State");

	//g_DeviceContext->RSSetState(rastState);

	// select which primitive type we are using
	g_DeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

}

ID3D11DepthStencilView* GraphicsDriver::CreateDepthStencil()
{
	ID3D11Texture2D* depthStencilTexture;
	// Create depth stencil texture
	D3D11_TEXTURE2D_DESC descDepth;
	ZeroMemory(&descDepth, sizeof(descDepth));
	descDepth.Width = Renderer::Window_Width;
	descDepth.Height = Renderer::Window_Height;
	descDepth.MipLevels = 1;
	descDepth.ArraySize = 1;
	descDepth.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	descDepth.SampleDesc.Count = 1;
	descDepth.SampleDesc.Quality = 0;
	descDepth.Usage = D3D11_USAGE_DEFAULT;
	descDepth.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	descDepth.CPUAccessFlags = 0;
	descDepth.MiscFlags = 0;
	HRESULT hr = g_Device->CreateTexture2D(&descDepth, nullptr, &depthStencilTexture);
	BZ_ASSERT(hr == S_OK, "Problem Creating Depth Stencil");

	ID3D11DepthStencilView* depthStencil = nullptr;

	// Create the depth stencil view
	D3D11_DEPTH_STENCIL_VIEW_DESC descDSV;
	ZeroMemory(&descDSV, sizeof(descDSV));
	descDSV.Format = descDepth.Format;
	descDSV.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
	descDSV.Texture2D.MipSlice = 0;
	hr = g_Device->CreateDepthStencilView(depthStencilTexture, &descDSV, &depthStencil);
	BZ_ASSERT(hr == S_OK, "Problem Creating Depth Stencil");

	return depthStencil;
}

ID3D11DepthStencilState* GraphicsDriver::CreateDepthStencilState(bool depthTestEnabled)
{
	D3D11_DEPTH_STENCIL_DESC dsDesc;

	// Depth test parameters
	dsDesc.DepthEnable = depthTestEnabled;
	dsDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
	dsDesc.DepthFunc = D3D11_COMPARISON_LESS;

	// Stencil test parameters
	dsDesc.StencilEnable = false;
	dsDesc.StencilReadMask = 0xFF;
	dsDesc.StencilWriteMask = 0xFF;

	// Stencil operations if pixel is front-facing
	dsDesc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	dsDesc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_INCR;
	dsDesc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	dsDesc.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

	// Stencil operations if pixel is back-facing
	dsDesc.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	dsDesc.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_DECR;
	dsDesc.BackFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	dsDesc.BackFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

	// Create depth stencil state
	ID3D11DepthStencilState* ss = nullptr;
	HRESULT hr = g_Device->CreateDepthStencilState(&dsDesc, &ss);
	BZ_ASSERT(hr == S_OK, "Problem Creating Depth Stencil");

	return ss;
}

void GraphicsDriver::UploadBuffer(ID3D11Buffer* buffer, void* dataRet, int size)
{
	D3D11_MAPPED_SUBRESOURCE ms;
	HRESULT hr = g_DeviceContext->Map(buffer, NULL, D3D11_MAP_WRITE_DISCARD, NULL, &ms);
	BZ_ASSERT(hr == S_OK, "Failed to map vertex buffer to GPU.");

	memcpy(ms.pData, dataRet, size);
	g_DeviceContext->Unmap(buffer, NULL);
}

void GraphicsDriver::ClearRenderTargets()
{
	// clear the back buffer to black
	FLOAT color[4] = { 0.0f, 0.0f, 0.0f, 1.0f };
	g_DeviceContext->ClearRenderTargetView(m_BackBuffer, color);
}

void GraphicsDriver::ClearDepthStencils(/*ID3D11DepthStencilView* depthStencil, */float depth)
{
	// Clear the back buffer 
	g_DeviceContext->ClearDepthStencilView(mMeshDepthStencil, D3D11_CLEAR_DEPTH, depth, 0);
}

void GraphicsDriver::DrawVertexArray(VertexData* vertData)
{
	if (vertData->GetDrawCallType() != VBUF_INDIRECT)
		g_DeviceContext->DrawIndexed(vertData->GetIndexCount(), 0, 0);
	else
		//g_DeviceContext->Draw(8, 0);
		g_DeviceContext->DrawInstancedIndirect(vertData->GetVertexBuffer(), 0);
}

void GraphicsDriver::DrawInstanced(class VertexData* vertData, UINT32 instanceCount)
{
	g_DeviceContext->DrawIndexedInstanced(vertData->GetIndexCount(), instanceCount, 0, 0, 0);
}

void GraphicsDriver::BindEffect(Effect* pEffect)
{
	//if (pEffect->mVS)	
		g_DeviceContext->VSSetShader(pEffect->mVS, NULL, NULL);
	//if (pEffect->mPS)	
		g_DeviceContext->PSSetShader(pEffect->mPS, NULL, NULL);
	//if (pEffect->mGS)	
		g_DeviceContext->GSSetShader(pEffect->mGS, NULL, NULL);
	//if (pEffect->mCS)	
		g_DeviceContext->CSSetShader(pEffect->mCS, NULL, NULL);
}

void GraphicsDriver::BindVertexAndPixelShaders(ShaderType shader)
{
	ShaderPair p = mShaderMap.find(shader)->second;

	g_DeviceContext->VSSetShader(p.mVertexShader, 0, 0);
	g_DeviceContext->PSSetShader(p.mPixelShader, 0, 0);
}

void GraphicsDriver::BindVertexBuffer(ID3D11Buffer* vertData, int size)
{
	// select vertex bufer to display
	UINT stride = size * sizeof(float);
	UINT offset = 0;
	g_DeviceContext->IASetVertexBuffers(0, 1, &vertData, &stride, &offset);
}

void GraphicsDriver::BindIndexBuffer(ID3D11Buffer* vertData, int size)
{
	g_DeviceContext->IASetIndexBuffer(vertData, DXGI_FORMAT_R32_UINT, 0);
}

void GraphicsDriver::BindTextureResource(TextureGPU* mResource, UINT32 pSlot, bool isDepthTexture)
{
	if (pSlot < 1024)	//SRV
	{
		ID3D11ShaderResourceView* srv = isDepthTexture ? mResource->mDepthShaderResourceView : mResource->mShaderResourceView;
		g_DeviceContext->VSSetShaderResources(pSlot, 1, &srv);
		g_DeviceContext->PSSetShaderResources(pSlot, 1, &srv);
		g_DeviceContext->GSSetShaderResources(pSlot, 1, &srv);
		g_DeviceContext->CSSetShaderResources(pSlot, 1, &srv);

		if (mResource->mSamplerState != SamplerState_INVALID && mResource->mSamplerState != SamplerState_NotNeeded)
		{
			SamplerState& ss = SamplerStateManager::Instance()->GetSamplerState(mResource->mSamplerState);
			g_DeviceContext->VSSetSamplers(pSlot, 1, &ss.mSamplerStateObject);
			g_DeviceContext->PSSetSamplers(pSlot, 1, &ss.mSamplerStateObject);
		}
	}
	else				// UAV
	{
		UINT32 initialCount = 1;
		ID3D11UnorderedAccessView* uav = mResource->mUnorderedAccessView;
		g_DeviceContext->CSSetUnorderedAccessViews(pSlot - 1024, 1, &uav, &initialCount);
	}
}

void GraphicsDriver::UnbindResourceFromSlot(UINT32 pSlot)
{
	if (pSlot < 1024)	//SRV
	{
		ID3D11ShaderResourceView* srv = NULL;
		g_DeviceContext->VSSetShaderResources(pSlot, 1, &srv);
		g_DeviceContext->PSSetShaderResources(pSlot, 1, &srv);
		g_DeviceContext->GSSetShaderResources(pSlot, 1, &srv);
		g_DeviceContext->CSSetShaderResources(pSlot, 1, &srv);
	}
	else				// UAV
	{
		UINT32 initialCount = 1;
		ID3D11UnorderedAccessView* uav = NULL;
		g_DeviceContext->CSSetUnorderedAccessViews(pSlot - 1024, 1, &uav, &initialCount);
	}
}

void GraphicsDriver::BindResourceBuffer(ResourceBufferGPU* pBuffer, UINT32 pSlot, bool bindSamplerState, bool resetCount)
{
	if (pSlot < 1024)	//SRV
	{
		ID3D11ShaderResourceView** srv = pBuffer->GetSRV();
		g_DeviceContext->VSSetShaderResources(pSlot, 1, srv);
		g_DeviceContext->PSSetShaderResources(pSlot, 1, srv);
		g_DeviceContext->GSSetShaderResources(pSlot, 1, srv);
		g_DeviceContext->CSSetShaderResources(pSlot, 1, srv);

		if (bindSamplerState)
			BindSamplerState(pBuffer->GetSamplerState()->mSamplerStateObject, pSlot);
	}
	else				// UAV
	{
		UINT32 initialCount = resetCount ? 0 : (UINT32)-1;// pBuffer->GetStructCount();
		ID3D11UnorderedAccessView** uav = pBuffer->GetUAV();
		g_DeviceContext->CSSetUnorderedAccessViews(pSlot - 1024, 1, uav, /*(UINT32*)*/&initialCount);
	}
}

void GraphicsDriver::UnbindResourceBuffer(UINT32 pSlot)
{
	if (pSlot < 1024)	//SRV
	{
		ID3D11ShaderResourceView* srv = NULL;
		g_DeviceContext->VSSetShaderResources(pSlot, 1, &srv);
		g_DeviceContext->PSSetShaderResources(pSlot, 1, &srv);
		g_DeviceContext->GSSetShaderResources(pSlot, 1, &srv);
		g_DeviceContext->CSSetShaderResources(pSlot, 1, &srv);
	}
	else				// UAV
	{
		//UINT32 initialCount = 1;
		int initialCount = -1;
		ID3D11UnorderedAccessView* uav = NULL;
		g_DeviceContext->CSSetUnorderedAccessViews(pSlot - 1024, 1, &uav, (UINT32*)&initialCount);
	}
}

void GraphicsDriver::BindInputLayout(ID3D11InputLayout* inputLayout)
{
	// set this input layout
	g_DeviceContext->IASetInputLayout(inputLayout);
}

void GraphicsDriver::BindPSTexture(ID3D11ShaderResourceView* texture, unsigned slot)
{
	g_DeviceContext->PSSetShaderResources(slot, 1, &texture);
}

void GraphicsDriver::BindPSSamplerState(ID3D11SamplerState* sampler, unsigned slot)
{
	g_DeviceContext->PSSetSamplers(slot, 1, &sampler);
}

void GraphicsDriver::BindSamplerState(ID3D11SamplerState* sampler, unsigned slot)
{
	g_DeviceContext->VSSetSamplers(slot, 1, &sampler);
	g_DeviceContext->GSSetSamplers(slot, 1, &sampler);
	g_DeviceContext->PSSetSamplers(slot, 1, &sampler);
	g_DeviceContext->CSSetSamplers(slot, 1, &sampler);
}

void GraphicsDriver::SetConstantBuffer(ID3D11Buffer* inBuffer, int inStartSlot)
{
	g_DeviceContext->VSSetConstantBuffers(inStartSlot, 1, &inBuffer);
	g_DeviceContext->GSSetConstantBuffers(inStartSlot, 1, &inBuffer);
	g_DeviceContext->PSSetConstantBuffers(inStartSlot, 1, &inBuffer);
	g_DeviceContext->CSSetConstantBuffers(inStartSlot, 1, &inBuffer);
}

void GraphicsDriver::SetVSConstantBuffer(ID3D11Buffer* inBuffer, int inStartSlot)
{
	auto buffer = inBuffer;
	g_DeviceContext->VSSetConstantBuffers(inStartSlot, 1, &buffer);
}

void GraphicsDriver::SetPSConstantBuffer(ID3D11Buffer* inBuffer, int inStartSlot)
{
	auto buffer = inBuffer;
	g_DeviceContext->PSSetConstantBuffers(inStartSlot, 1, &buffer);
}

void GraphicsDriver::SetDepthStencil(ID3D11DepthStencilView* depthStencil)
{
	mCurrentDepthStencil = depthStencil;

	auto renderTarget = m_BackBuffer;// mCurrentRenderTarget.get();
	g_DeviceContext->OMSetRenderTargets(1, &renderTarget, mCurrentDepthStencil);
}

void GraphicsDriver::SetDepthStencilState(ID3D11DepthStencilState* depthStencilState)
{
	g_DeviceContext->OMSetDepthStencilState(depthStencilState, 1);
}

void GraphicsDriver::SetInputTopology(D3D11_PRIMITIVE_TOPOLOGY topology)
{
	g_DeviceContext->IASetPrimitiveTopology(topology);
}

void GraphicsDriver::Present()
{
	// switch the back buffer and the front buffer
	g_SwapChain->Present(0, 0);
}

void GraphicsDriver::RegisterShaders(ShaderType type, ID3D11VertexShader* vs, ID3D11PixelShader* ps, ID3D11GeometryShader* gs, ID3D11ComputeShader* cs)
{
	ShaderPair p;
	p.mVertexShader = vs;
	p.mPixelShader = ps;

	mShaderMap.emplace(type, p);
}

void GraphicsDriver::EndRenderTargets()
{
	g_DeviceContext->OMSetRenderTargets(0, NULL, NULL);
}

void GraphicsDriver::SetMultiRenderTargetsWithDepths(TextureGPU* pPositionTex, TextureGPU* pNormalTex, TextureGPU* pDiffuseTex)
{
	ID3D11RenderTargetView* RTVs[] = { pPositionTex->mRenderTargetView, pNormalTex->mRenderTargetView, pDiffuseTex->mRenderTargetView };
	g_DeviceContext->OMSetRenderTargets(3, RTVs, mMeshDepthStencil);

	// Clear render targets
	float black[] = { 0, 0, 0, 0 };
	for (int i = 0; i < 3; ++i)
		g_DeviceContext->ClearRenderTargetView(RTVs[i], black);

	g_DeviceContext->ClearDepthStencilView(mMeshDepthStencil, D3D11_CLEAR_DEPTH, 1.0f, 0);
}

void GraphicsDriver::SetRenderTargetAndViewportWithNoDepth(TextureGPU *pDestColorTex, bool clear)
{
	ID3D11RenderTargetView *renderTargets[1] = { 0 };
	if (pDestColorTex != 0)
	{
		renderTargets[0] = pDestColorTex->mRenderTargetView;
	}
	else
	{
		renderTargets[0] = m_BackBuffer;
	}

	g_DeviceContext->OMSetRenderTargets(1, renderTargets, 0);

	if (pDestColorTex != 0)
	{
		g_DeviceContext->RSSetViewports(1, &pDestColorTex->mViewport);

		float color[] = { 0.0f, 0.0f, 0.0f, 1.0f };
		if (clear)
		{
			g_DeviceContext->ClearRenderTargetView(pDestColorTex->mRenderTargetView, color);
		}
	}
	else
	{
		D3D11_VIEWPORT vp;
		vp.TopLeftX = 0;
		vp.TopLeftY = 0;
		vp.Width = (float)(Renderer::Window_Width);
		vp.Height = (float)(Renderer::Window_Height);
		vp.MinDepth = 0.0f;
		vp.MaxDepth = 1.0f;

		g_DeviceContext->RSSetViewports(1, &vp);
		//float color[] = { 1.0f * (rand() % 100) / 100.0f, 0.0f, 0.0f, 1.0f };
		float color[] = { 0.f, 0.f, 0.f, 1.f };
		if (clear)
		{
			g_DeviceContext->ClearRenderTargetView(m_BackBuffer, color);
		}
	}
}

void GraphicsDriver::SetDepthStencilOnlyRenderTargetAndViewport(TextureGPU *pDestDepthTex, bool clear)
{
	g_DeviceContext->OMSetRenderTargets(0, NULL, pDestDepthTex->mDepthStencilView);
	g_DeviceContext->RSSetViewports(1, &pDestDepthTex->mViewport);

	if (clear)
	{
		g_DeviceContext->ClearDepthStencilView(pDestDepthTex->mDepthStencilView, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
	}
}

void GraphicsDriver::SetRenderTargetsAndUAVNoDepth(TextureGPU* pDestColorTex, ResourceBufferGPU* pUAV, unsigned bindSlot, bool clear)
{
	ID3D11RenderTargetView *renderTargets[1] = { 0 };
	if (pDestColorTex != 0)
	{
		renderTargets[0] = pDestColorTex->mRenderTargetView;
	}
	else
	{
		renderTargets[0] = m_BackBuffer;
	}

	ID3D11UnorderedAccessView* uav[1] = { 0 };
	if (pUAV != 0)
	{
		uav[0] = *pUAV->GetUAV();
	}

	UINT offset = -1;
	g_DeviceContext->OMSetRenderTargetsAndUnorderedAccessViews(1, renderTargets, 0, 1, 1, uav, &offset);
	//g_DeviceContext->OMSetRenderTargets(1, renderTargets, NULL);

	if (pDestColorTex != 0)
	{
		g_DeviceContext->RSSetViewports(1, &pDestColorTex->mViewport);

		float color[] = { 0.0f, 0.0f, 0.0f, 0.0f };
		if (clear)
		{
			g_DeviceContext->ClearRenderTargetView(pDestColorTex->mRenderTargetView, color);
		}
	}
	else
	{
		D3D11_VIEWPORT vp;
		vp.TopLeftX = 0;
		vp.TopLeftY = 0;
		vp.Width = (float)(Renderer::Window_Width);
		vp.Height = (float)(Renderer::Window_Height);
		vp.MinDepth = 0.0f;
		vp.MaxDepth = 1.0f;

		g_DeviceContext->RSSetViewports(1, &vp);
		float color[] = { 1.0f * (rand() % 100) / 100.0f, 0.0f, 0.0f, 1.0f };
		if (clear)
		{
			g_DeviceContext->ClearRenderTargetView(m_BackBuffer, color);
		}
	}
}

void GraphicsDriver::SetMultiRenderTargetsAndUAVNoDepth(TextureGPU* pRenderTarget0, TextureGPU* pRenderTarget1, TextureGPU* pRenderTarget2, ResourceBufferGPU* pUAV, bool clear)
{
	int numRTs = 0;
	numRTs += pRenderTarget0 == NULL ? 0 : 1;
	numRTs += pRenderTarget1 == NULL ? 0 : 1;
	numRTs += pRenderTarget2 == NULL ? 0 : 1;

	ID3D11RenderTargetView *renderTargets[3] = { 0 };
	if (pRenderTarget0 != 0)
	{
		renderTargets[0] = pRenderTarget0->mRenderTargetView;
	}
	else
	{
		renderTargets[0] = m_BackBuffer;
	}
	renderTargets[1] = pRenderTarget1->mRenderTargetView;
	renderTargets[2] = pRenderTarget2->mRenderTargetView;

	ID3D11UnorderedAccessView* uav[1] = { 0 };
	if (pUAV != 0)
	{
		uav[0] = *pUAV->GetUAV();
	}

	UINT offset = -1;
	g_DeviceContext->OMSetRenderTargetsAndUnorderedAccessViews(numRTs, renderTargets, NULL, numRTs, 1, uav, &offset);
	//g_DeviceContext->OMSetRenderTargets(1, renderTargets, NULL);

	if (pRenderTarget0 != 0)
	{
		g_DeviceContext->RSSetViewports(1, &pRenderTarget0->mViewport);

		float color[] = { 0.0f, 0.0f, 0.0f, 0.0f };
		if (clear)
		{
			g_DeviceContext->ClearRenderTargetView(pRenderTarget0->mRenderTargetView, color);
			if (pRenderTarget1 != 0)
				g_DeviceContext->ClearRenderTargetView(pRenderTarget1->mRenderTargetView, color);
			if (pRenderTarget2 != 0)
				g_DeviceContext->ClearRenderTargetView(pRenderTarget2->mRenderTargetView, color);
		}
	}
	else
	{
		D3D11_VIEWPORT vp;
		vp.TopLeftX = 0;
		vp.TopLeftY = 0;
		vp.Width = (float)(Renderer::Window_Width);
		vp.Height = (float)(Renderer::Window_Height);
		vp.MinDepth = 0.0f;
		vp.MaxDepth = 1.0f;

		g_DeviceContext->RSSetViewports(1, &vp);
		float color[] = { 1.0f * (rand() % 100) / 100.0f, 0.0f, 0.0f, 1.0f };
		if (clear)
		{
			g_DeviceContext->ClearRenderTargetView(m_BackBuffer, color);
			if (pRenderTarget1 != 0)
				g_DeviceContext->ClearRenderTargetView(pRenderTarget1->mRenderTargetView, color);
			if (pRenderTarget2 != 0)
				g_DeviceContext->ClearRenderTargetView(pRenderTarget2->mRenderTargetView, color);
		}
	}
}
