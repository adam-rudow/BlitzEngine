#include "Utils/AssertDbg.h"
#include "Texture.h"
#include "SamplerState.h"


// Inter-Engine includes
//#include "../../Utils/ErrorHandling.h"
//#include "PrimeEngine/Utils/PEString.h"
//#include "PrimeEngine/Math/CameraOps.h"
//#include "PrimeEngine/Utils/StringOps.h"
//#include "PrimeEngine/FileSystem/FileReader.h"

TextureGPU::TextureGPU()
{
	mShaderResourceView = NULL;
	mRenderTargetView = NULL;
	mUnorderedAccessView = NULL;
	mDepthStencilView = NULL;
	mDepthShaderResourceView = NULL;
	mTexture = NULL;
	mDepthTexture = NULL;
	//mGraphics = *GraphicsDriver::Instance();
}

TextureGPU::TextureGPU(GraphicsDriver& gDriver)// : mGraphics(gDriver)
{
	//msamplerState = SamplerState_INVALID;
	mShaderResourceView			= NULL;
	mRenderTargetView			= NULL;
	mUnorderedAccessView		= NULL;
	mDepthStencilView			= NULL;
	mDepthShaderResourceView	= NULL;
	mTexture					= NULL;
	mDepthTexture				= NULL;
}

TextureGPU::~TextureGPU()
{
	if (mShaderResourceView)		mShaderResourceView->Release();
	if (mRenderTargetView)			mRenderTargetView->Release();
	if (mUnorderedAccessView)		mUnorderedAccessView->Release();
	if (mDepthStencilView)			mDepthStencilView->Release();
	if (mDepthShaderResourceView)	mDepthShaderResourceView->Release();
	if (mTexture)					mTexture->Release();
	if (mDepthTexture)				mDepthTexture->Release();
}

void TextureGPU::createDeferredRenderTexture(UINT32 w, UINT32 h)
{
	mViewport.TopLeftX = 0;
	mViewport.TopLeftY = 0;
	mViewport.Width = static_cast<float>(w);
	mViewport.Height = static_cast<float>(h);
	mViewport.MinDepth = 0.0f;
	mViewport.MaxDepth = 1.0f;

	ID3D11Device *pDevice = GraphicsDriver::Instance()->GetDevice();// mGraphics.GetDevice();
	ID3D11DeviceContext *pDeviceContext = GraphicsDriver::Instance()->GetDeviceContext();// .GetDeviceContext();

	D3D11_TEXTURE2D_DESC texDesc;
	ZeroMemory(&texDesc, sizeof(D3D11_TEXTURE2D_DESC));
	texDesc.Width = w;
	texDesc.Height = h;
	texDesc.MipLevels = 1;
	texDesc.ArraySize = 1;
	texDesc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
	texDesc.SampleDesc.Count = 1;
	texDesc.Usage = D3D11_USAGE_DEFAULT;
	texDesc.BindFlags = D3D11_BIND_RENDER_TARGET |	D3D11_BIND_SHADER_RESOURCE;
	HRESULT hr = pDevice->CreateTexture2D(&texDesc, 0, &mTexture);
	BZ_ASSERT(SUCCEEDED(hr), "");

	// Generate render target view
	D3D11_RENDER_TARGET_VIEW_DESC RTVDesc;
	ZeroMemory(&RTVDesc, sizeof(D3D11_RENDER_TARGET_VIEW_DESC));
	RTVDesc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
	RTVDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
	hr = pDevice->CreateRenderTargetView(mTexture, &RTVDesc, &mRenderTargetView);
	BZ_ASSERT(SUCCEEDED(hr), "");

	// Generate shader resource view
	D3D11_SHADER_RESOURCE_VIEW_DESC SRVDesc;
	ZeroMemory(&SRVDesc, sizeof(D3D11_SHADER_RESOURCE_VIEW_DESC));
	SRVDesc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
	SRVDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	SRVDesc.Texture2D.MipLevels = 1;
	hr = pDevice->CreateShaderResourceView(mTexture, &SRVDesc, &mShaderResourceView);
	BZ_ASSERT(SUCCEEDED(hr), "");

	// Create depth stencil texture
	//ID3D11Texture2D* depthStencilTexture = NULL;
	D3D11_TEXTURE2D_DESC depthStencilDesc;
	ZeroMemory(&depthStencilDesc, sizeof(D3D11_TEXTURE2D_DESC));
	depthStencilDesc.Width = w;
	depthStencilDesc.Height = h;
	depthStencilDesc.MipLevels = 1;
	depthStencilDesc.ArraySize = 1;
	depthStencilDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	depthStencilDesc.SampleDesc.Count = 1;
	depthStencilDesc.Usage = D3D11_USAGE_DEFAULT;
	depthStencilDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	hr = pDevice->CreateTexture2D(&depthStencilDesc, 0, &mDepthTexture);
	BZ_ASSERT(SUCCEEDED(hr), "");

	// Create depth stencil view
	D3D11_DEPTH_STENCIL_VIEW_DESC depthStencilViewDesc;
	ZeroMemory(&depthStencilViewDesc, sizeof(D3D11_DEPTH_STENCIL_VIEW_DESC));
	depthStencilViewDesc.Format = depthStencilDesc.Format;
	depthStencilViewDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
	hr = pDevice->CreateDepthStencilView(mDepthTexture, &depthStencilViewDesc, &mDepthStencilView);
	BZ_ASSERT(SUCCEEDED(hr), "");

	mSamplerState = SamplerState_NoMips_NoMinTexelLerp_NoMagTexelLerp_Clamp;
}

void TextureGPU::createVoxelizationDummyRenderTexture(UINT32 w, UINT32 h)
{
	mViewport.TopLeftX = 0;
	mViewport.TopLeftY = 0;
	mViewport.Width    = static_cast<float>(w);
	mViewport.Height   = static_cast<float>(h);
	mViewport.MinDepth = 0.0f;
	mViewport.MaxDepth = 1.0f;

	ID3D11Device *pDevice = GraphicsDriver::Instance()->GetDevice();
	ID3D11DeviceContext *pDeviceContext = GraphicsDriver::Instance()->GetDeviceContext();

	D3D11_TEXTURE2D_DESC texDesc;
	ZeroMemory(&texDesc, sizeof(D3D11_TEXTURE2D_DESC));
	texDesc.Width = w;
	texDesc.Height = h;
	texDesc.MipLevels = 1;
	texDesc.ArraySize = 1;
	texDesc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
	texDesc.SampleDesc.Count = 1;
	texDesc.Usage = D3D11_USAGE_DEFAULT;
	texDesc.BindFlags = D3D11_BIND_RENDER_TARGET;
	HRESULT hr = pDevice->CreateTexture2D(&texDesc, 0, &mTexture);
	BZ_ASSERT(SUCCEEDED(hr), "");

	// Generate render target view
	D3D11_RENDER_TARGET_VIEW_DESC RTVDesc;
	ZeroMemory(&RTVDesc, sizeof(D3D11_RENDER_TARGET_VIEW_DESC));
	RTVDesc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
	RTVDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
	hr = pDevice->CreateRenderTargetView(mTexture, &RTVDesc, &mRenderTargetView);
	BZ_ASSERT(SUCCEEDED(hr), "");

	// Create depth stencil texture
	//ID3D11Texture2D* depthStencilTexture = NULL;
	D3D11_TEXTURE2D_DESC depthStencilDesc;
	ZeroMemory(&depthStencilDesc, sizeof(D3D11_TEXTURE2D_DESC));
	depthStencilDesc.Width = w;
	depthStencilDesc.Height = h;
	depthStencilDesc.MipLevels = 1;
	depthStencilDesc.ArraySize = 1;
	depthStencilDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	depthStencilDesc.SampleDesc.Count = 1;
	depthStencilDesc.Usage = D3D11_USAGE_DEFAULT;
	depthStencilDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	hr = pDevice->CreateTexture2D(&depthStencilDesc, 0, &mDepthTexture);
	BZ_ASSERT(SUCCEEDED(hr), "");

	// Create depth stencil view
	D3D11_DEPTH_STENCIL_VIEW_DESC depthStencilViewDesc;
	ZeroMemory(&depthStencilViewDesc, sizeof(D3D11_DEPTH_STENCIL_VIEW_DESC));
	depthStencilViewDesc.Format = depthStencilDesc.Format;
	depthStencilViewDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
	hr = pDevice->CreateDepthStencilView(mDepthTexture, &depthStencilViewDesc, &mDepthStencilView);
	BZ_ASSERT(SUCCEEDED(hr), "");
}

void TextureGPU::createVPLColors2DTextureArrays(UINT32 w, UINT32 h, UINT32 d)
{
	mViewport.TopLeftX = 0;
	mViewport.TopLeftY = 0;
	mViewport.Width = S_CAST(FLOAT, w);
	mViewport.Height = S_CAST(FLOAT, h);
	mViewport.MinDepth = 0.0f;
	mViewport.MaxDepth = 1.0f;

	ID3D11Device *pDevice = GraphicsDriver::Instance()->GetDevice();
	ID3D11DeviceContext *pDeviceContext = GraphicsDriver::Instance()->GetDeviceContext();

	D3D11_TEXTURE2D_DESC texDesc;
	ZeroMemory(&texDesc, sizeof(D3D11_TEXTURE2D_DESC));
	texDesc.Width = w;
	texDesc.Height = h;
	texDesc.MipLevels = 1;
	texDesc.ArraySize = d;
	texDesc.Format = DXGI_FORMAT_R16G16B16A16_FLOAT;
	texDesc.SampleDesc.Count = 1;
	texDesc.Usage = D3D11_USAGE_DEFAULT;
	texDesc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_UNORDERED_ACCESS;
	HRESULT hr = pDevice->CreateTexture2D(&texDesc, 0, &mTexture);
	BZ_ASSERT(SUCCEEDED(hr), "");

	// Generate render target view
	D3D11_RENDER_TARGET_VIEW_DESC RTVDesc;
	ZeroMemory(&RTVDesc, sizeof(D3D11_RENDER_TARGET_VIEW_DESC));
	RTVDesc.Format = texDesc.Format;
	RTVDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2DARRAY;
	RTVDesc.Texture2DArray.ArraySize = d;
	hr = pDevice->CreateRenderTargetView(mTexture, &RTVDesc, &mRenderTargetView);
	BZ_ASSERT(SUCCEEDED(hr), "");

	// Generate shader resource view
	D3D11_SHADER_RESOURCE_VIEW_DESC SRVDesc;
	ZeroMemory(&SRVDesc, sizeof(D3D11_SHADER_RESOURCE_VIEW_DESC));
	SRVDesc.Format = texDesc.Format;
	SRVDesc.ViewDimension = D3D_SRV_DIMENSION_TEXTURE2DARRAY;
	SRVDesc.Texture2DArray.MipLevels = 1;
	SRVDesc.Texture2DArray.ArraySize = d;
	SRVDesc.Texture2DArray.FirstArraySlice = 0;
	SRVDesc.Texture2DArray.MostDetailedMip = 0;
	hr = pDevice->CreateShaderResourceView(mTexture, &SRVDesc, &mShaderResourceView);
	BZ_ASSERT(SUCCEEDED(hr), "");

	// Generate unordered access view to write into from comute shader in VPL propogation
	D3D11_UNORDERED_ACCESS_VIEW_DESC UAVDesc;
	ZeroMemory(&UAVDesc, sizeof(D3D11_UNORDERED_ACCESS_VIEW_DESC));
	UAVDesc.Format = texDesc.Format;
	UAVDesc.ViewDimension = D3D11_UAV_DIMENSION_TEXTURE2DARRAY;
	UAVDesc.Texture2DArray.ArraySize = d;
	UAVDesc.Texture2DArray.MipSlice = 0;
	//UAVDesc.Texture2DArray.FirstArraySlice = 0;
	hr = pDevice->CreateUnorderedAccessView(mTexture, &UAVDesc, &mUnorderedAccessView);
	BZ_ASSERT(SUCCEEDED(hr), "");

	mSamplerState = SamplerState_NoMips_MinLerp_MaxLerp_Clamp;
}


void TextureGPU::createTextureNoFamily(const char *textureFilename, const char *package)
{
	//mTexture = gfxLoadDDSTexture(PEString::s_buf);

	ID3D11Device *pDevice = GraphicsDriver::Instance()->GetDevice();
	ID3D11DeviceContext *pDeviceContext = GraphicsDriver::Instance()->GetDeviceContext();

	// since we are accessing the whole resource in the format that it was created, no need to fill in the D3D11_SHADER_RESOURCE_VIEW_DESC struct
	//HRESULT hr = pDevice->CreateShaderResourceView(mTexture, NULL, &mShaderResourceView);
	//B_ASSERT(SUCCEEDED(hr), "Creating shader resource view");


	// Temp for now
	mShaderResourceView = GraphicsDriver::Instance()->CreateSRVTextureFromFile(textureFilename);

	mSamplerState = SamplerState_MipLerp_MinTexelLerp_MagTexelLerp_Wrap;
}

void TextureGPU::createColorTextureGPU(const char* textureFilename, const char *package)// , ESamlerState samlerState /*= = SamlerState_Count*/)
{
	//default
	//mSamlerState = SamlerState_MipLerp_MinTexelLerp_MagTexelLerp_Wrap;

	// in case custom was provided
	//if (samlerState != SamlerState_Count)
	//	mSamlerState = samlerState;

    createTextureNoFamily(textureFilename, package);
    mFamily = TextureFamily::COLOR_MAP;

	BZ_ASSERT(mShaderResourceView, "texture shader resource not set");
}

void TextureGPU::createBumpTextureGPU(const char* textureFilename, const char *package)
{
	//mSamlerState = SamlerState_MipLerp_MinTexelLerp_MagTexelLerp_Wrap;

	createTextureNoFamily(textureFilename, package);

	mFamily = TextureFamily::NORMAL_MAP;
}

void TextureGPU::createSpecularTextureGPU(const char* textureFilename, const char *package)
{
	//mSamlerState = SamlerState_MipLerp_MinTexelLerp_MagTexelLerp_Wrap;

	createTextureNoFamily(textureFilename, package);

	mFamily = TextureFamily::SPECULAR_MAP;
}

void TextureGPU::createGlowTextureGPU(const char* textureFilename, const char *package)
{
	//mSamlerState = SamlerState_MipLerp_MinTexelLerp_MagTexelLerp_Wrap;

	createTextureNoFamily(textureFilename, package);

	mFamily = TextureFamily::GLOW_MAP;
}


void TextureGPU::createRandom2DTexture()
{
	createColorTextureGPU("Imrod_Diffuse.dds", "Default");
}

void TextureGPU::createColorCubeTextureGPU(const char * textureFilename, const char *package)
{

	// Path is now a full path to the file with the filename itself

	BZ_ASSERT(false, "Not implemented!");
#if 0
	// retrieve device from IRenderer
	ID3D11Device *pD3DDevice = IRenderer::Instance()->mdevice;

	ID3D11Texture2D *tex = 0;
	D3DX11_IMAGE_LOAD_INFO loadInfo;
	loadInfo.MiscFlags = D3D11_RESOURCE_MISC_TEXTURECUBE;
	HRESULT hr = D3DX11CreateTextureFromFileA(pD3DDevice, path, &loadInfo, 0, (ID3D11Resource **)(&tex), 0);
	assert(SUCCEEDED(hr));

	D3D11_TEXTURE2D_DESC texDesc;
	tex->GetDesc(&texDesc);

	D3D11_SHADER_RESOURCE_VIEW_DESC viewDesc;
	viewDesc.Format = texDesc.Format;
	viewDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURECUBE;
	viewDesc.TextureCube.MipLevels = texDesc.MipLevels;
	viewDesc.TextureCube.MostDetailedMip = 0;

	hr = pD3DDevice->CreateShaderResourceView(tex, &viewDesc, &mShaderResourceView);
	assert(SUCCEEDED(hr));
	tex->Release();
#endif

	mFamily = TextureFamily::COLOR_CUBE;
}

void TextureGPU::createDrawableIntoColorTextureWithDepth(UINT32 w, UINT32 h, ESamplerState sampler, bool enableMSAA, bool use32BitRedForDepth /* = false*/)
{
	mSamplerState = sampler;

	mViewport.TopLeftX = 0;
	mViewport.TopLeftY = 0;
	mViewport.Width	 = static_cast<FLOAT>(w);
	mViewport.Height = static_cast<FLOAT>(h);
	mViewport.MinDepth = 0.0f;
	mViewport.MaxDepth = 1.0f;

	ID3D11Device *pDevice = GraphicsDriver::Instance()->GetDevice();
	ID3D11DeviceContext *pDeviceContext = GraphicsDriver::Instance()->GetDeviceContext();

	D3D11_TEXTURE2D_DESC texDesc;
	texDesc.Width = w;
	texDesc.Height = h;
	texDesc.MipLevels = 1;
	texDesc.ArraySize = 1;
	texDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	texDesc.SampleDesc.Count	= enableMSAA ? 4 : 1;
	texDesc.SampleDesc.Quality	= enableMSAA ? 1 : 0;
	texDesc.Usage = D3D11_USAGE_DEFAULT;
	texDesc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
	texDesc.CPUAccessFlags = 0;
	texDesc.MiscFlags = D3D11_RESOURCE_MISC_GENERATE_MIPS;

	HRESULT hr = pDevice->CreateTexture2D(&texDesc, 0, &mTexture);
	BZ_ASSERT(SUCCEEDED(hr), "");

	// Null description means to create a view to all mipmap levels
	// using the format the texture was created with
	hr = pDevice->CreateRenderTargetView(mTexture, 0, &mRenderTargetView);
	BZ_ASSERT(SUCCEEDED(hr), "");
	hr = pDevice->CreateShaderResourceView(mTexture, 0, &mShaderResourceView);
	BZ_ASSERT(SUCCEEDED(hr), "");

	// Now create depth part fo the texture
	//ID3D11Texture2D *pDepthMap = 0;

	texDesc.Format = DXGI_FORMAT_R32_TYPELESS;
	texDesc.Usage = D3D11_USAGE_DEFAULT;
	texDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL | D3D11_BIND_SHADER_RESOURCE;
	texDesc.CPUAccessFlags = 0;
	texDesc.MiscFlags = 0;

	hr = pDevice->CreateTexture2D(&texDesc, 0, &mDepthTexture);
	BZ_ASSERT(SUCCEEDED(hr), "");

	// setting up view for rendering into depth buffer/and reading (stenciling) from it (z-buffer algorithm red/write)
	D3D11_DEPTH_STENCIL_VIEW_DESC dsvDesc;
	dsvDesc.Format = DXGI_FORMAT_D32_FLOAT;
	dsvDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
	dsvDesc.Texture2D.MipSlice = 0;
	dsvDesc.Flags = 0;//D3D11_DSV_READ_ONLY_DEPTH;

	hr = pDevice->CreateDepthStencilView(mDepthTexture, &dsvDesc, &mDepthStencilView);
	BZ_ASSERT(SUCCEEDED(hr), "");

	D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;
	srvDesc.Format = DXGI_FORMAT_R32_FLOAT;
	srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Texture2D.MipLevels = texDesc.MipLevels;
	srvDesc.Texture2D.MostDetailedMip = 0;

	hr = pDevice->CreateShaderResourceView(mDepthTexture, &srvDesc, &mDepthShaderResourceView);
	BZ_ASSERT(SUCCEEDED(hr), "");

	//pDepthMap->Release();
}

void TextureGPU::createDrawableIntoDepthTexture(UINT32 w, UINT32 h, ESamplerState sampler)
{
	mSamplerState = sampler;

	mViewport.TopLeftX = 0;
	mViewport.TopLeftY = 0;
	mViewport.Width = static_cast<float>(w);
	mViewport.Height = static_cast<float>(h);
	mViewport.MinDepth = 0.0f;
	mViewport.MaxDepth = 1.0f;

	ID3D11Device *pDevice = GraphicsDriver::Instance()->GetDevice();
	ID3D11DeviceContext *pDeviceContext = GraphicsDriver::Instance()->GetDeviceContext();


	//ID3D11Texture2D *pDepthMap = 0;

	D3D11_TEXTURE2D_DESC texDesc;
	texDesc.Width = w;
	texDesc.Height = h;
	texDesc.MipLevels = 1;
	texDesc.ArraySize = 1;
	texDesc.Format = DXGI_FORMAT_R32_TYPELESS; // DXGI_FORMAT_R24G8_TYPELESS;
	texDesc.SampleDesc.Count = 1;
	texDesc.SampleDesc.Quality = 0;
	texDesc.Usage = D3D11_USAGE_DEFAULT;
	texDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL | D3D11_BIND_SHADER_RESOURCE;
	texDesc.CPUAccessFlags = 0;
	texDesc.MiscFlags = 0;

	HRESULT hr = pDevice->CreateTexture2D(&texDesc, 0, &mDepthTexture);
	BZ_ASSERT(SUCCEEDED(hr), "");

	// setting up view for rendering into depth buffer/and reading (stenciling) from it (z-buffer algorithm read/write)
	D3D11_DEPTH_STENCIL_VIEW_DESC dsvDesc;
	dsvDesc.Format = DXGI_FORMAT_D32_FLOAT; // DXGI_FORMAT_D24_UNORM_S8_UINT;
	dsvDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
	dsvDesc.Texture2D.MipSlice = 0;
	dsvDesc.Flags = 0;//D3D11_DSV_READ_ONLY_DEPTH;

	hr = pDevice->CreateDepthStencilView(mDepthTexture, &dsvDesc, &mDepthStencilView);
	BZ_ASSERT(SUCCEEDED(hr), "");

	D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;
	srvDesc.Format = DXGI_FORMAT_R32_FLOAT; // DXGI_FORMAT_R24_UNORM_X8_TYPELESS;
	srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Texture2D.MipLevels = texDesc.MipLevels;
	srvDesc.Texture2D.MostDetailedMip = 0;

	hr = pDevice->CreateShaderResourceView(mDepthTexture, &srvDesc, &mDepthShaderResourceView);
	BZ_ASSERT(SUCCEEDED(hr), "");

	//pDepthMap->Release();
}

//void TextureGPU::createVPLColors2DTextureArrays(UINT32 w, UINT32 h, UINT32 d)
//{
//	mViewport.TopLeftX = 0;
//	mViewport.TopLeftY = 0;
//	mViewport.Width = static_cast<float>(w);
//	mViewport.Height = static_cast<float>(h);
//	mViewport.MinDepth = 0.0f;
//	mViewport.MaxDepth = 1.0f;
//
//	D3D11Renderer *pD3D11Renderer = static_cast<D3D11Renderer *>(m_pContext->getGPUScreen());
//	ID3D11Device *pDevice = pD3D11Renderer->m_pD3DDevice;
//	ID3D11DeviceContext *pDeviceContext = pD3D11Renderer->m_pD3DContext;
//
//	D3D11_TEXTURE2D_DESC texDesc;
//	ZeroMemory(&texDesc, sizeof(D3D11_TEXTURE2D_DESC));
//	texDesc.Width = w;
//	texDesc.Height = h;
//	texDesc.MipLevels = 1;
//	texDesc.ArraySize = d;
//	texDesc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
//	texDesc.SampleDesc.Count = 1;
//	texDesc.Usage = D3D11_USAGE_DEFAULT;
//	texDesc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_UNORDERED_ACCESS;
//	HRESULT hr = pDevice->CreateTexture2D(&texDesc, 0, &m_pTexture);
//	assert(SUCCEEDED(hr));
//
//	// Generate render target view
//	D3D11_RENDER_TARGET_VIEW_DESC RTVDesc;
//	ZeroMemory(&RTVDesc, sizeof(D3D11_RENDER_TARGET_VIEW_DESC));
//	RTVDesc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
//	RTVDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2DARRAY;
//	RTVDesc.Texture2DArray.ArraySize = d;
//	hr = pDevice->CreateRenderTargetView(m_pTexture, &RTVDesc, &m_pRenderTargetView);
//	assert(SUCCEEDED(hr));
//
//	// Generate shader resource view
//	D3D11_SHADER_RESOURCE_VIEW_DESC SRVDesc;
//	ZeroMemory(&SRVDesc, sizeof(D3D11_SHADER_RESOURCE_VIEW_DESC));
//	SRVDesc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
//	SRVDesc.ViewDimension = D3D_SRV_DIMENSION_TEXTURE2DARRAY;
//	SRVDesc.Texture2DArray.MipLevels = 1;
//	SRVDesc.Texture2DArray.ArraySize = d;
//	SRVDesc.Texture2DArray.FirstArraySlice = 0;
//	SRVDesc.Texture2DArray.MostDetailedMip = 0;
//	hr = pDevice->CreateShaderResourceView(m_pTexture, &SRVDesc, &m_pShaderResourceView);
//	assert(SUCCEEDED(hr));
//
//	// Generate unordered access view to write into from compute shader in VPL propogation
//	D3D11_UNORDERED_ACCESS_VIEW_DESC UAVDesc;
//	ZeroMemory(&UAVDesc, sizeof(D3D11_UNORDERED_ACCESS_VIEW_DESC));
//	UAVDesc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
//	UAVDesc.ViewDimension = D3D11_UAV_DIMENSION_TEXTURE2DARRAY;
//	UAVDesc.Texture2DArray.ArraySize = d;
//	UAVDesc.Texture2DArray.MipSlice = 0;
//	//UAVDesc.Texture2DArray.FirstArraySlice = 0;
//	hr = pDevice->CreateUnorderedAccessView(m_pTexture, &UAVDesc, &m_pUnorderedAccessView);
//	assert(SUCCEEDED(hr));
//
//	m_samplerState = SamplerState_NoMips_MinLerp_MaxLerp_Clamp;
//}
