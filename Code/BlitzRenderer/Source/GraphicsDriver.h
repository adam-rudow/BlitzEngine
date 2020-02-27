#pragma once
#include "RenderUtility.h"
#include "RendererMacros.h"
#include "Utils/Utility.h"
#include "FW1FontWrapper/FW1Precompiled.h"

#include <vector>
#include <map>


#define HDR 0

class ResourceBufferGPU;
struct Effect;
struct TextureGPU;
struct RasterizerState;

class GraphicsDriver
{
	DeclSingleton(GraphicsDriver)

public:
	GraphicsDriver();
	~GraphicsDriver();

	void InitGraphics(HWND hWnd);
	void InitD3D(HWND hWnd);

	PixelShader*		CreatePixelShader(ID3D10Blob** psBlob, const char* shaderFileName, const char* shaderEntryName, D3D_SHADER_MACRO* macros);
	VertexShader*		CreateVertexShader(ID3D10Blob** vsBlob, const char* shaderFileName, const char* shaderEntryName, D3D_SHADER_MACRO* macros);
	GeometryShader*		CreateGeometryShader(ID3D10Blob** vsBlob, const char* shaderFileName, const char* shaderEntryName, D3D_SHADER_MACRO* macros);
	ComputeShader*		CreateComputeShader(ID3D10Blob** csBlob, const char* shaderFileName, const char* shaderEntryName, D3D_SHADER_MACRO* macros);
	//ID3DX11Effect*		CreateShaderEffect(const char* shaderFileName);
	ShaderResourceView* CreateSRVTextureFromFile(std::string fileName);
	TextureGPU*			CreateTexture();
	ID3D11SamplerState*		CreateSamplerState();

	//void CreateInputLayout(const D3D11_INPUT_ELEMENT_DESC* desc, int numParams, ID3D10Blob* compiledVSCode, ID3D11InputLayout* layoutRef);
	VertexInputLayout*	CreateInputLayout(const D3D11_INPUT_ELEMENT_DESC* desc, int numParams, ID3D10Blob** compiledVSCode);
	BufferGPU*			CreateVertexBuffer(std::vector<float>& vertData, bool allowIndirecDraw = false);
	BufferGPU*			CreateVertexBufferFromSrc_Minimal(std::vector<float>& verts, std::vector<float>& texCoords);
	BufferGPU*			CreateVertexBufferFromSrc_Std(std::vector<float>& verts, std::vector<float>& texCoords, std::vector<float>& normals);
	BufferGPU*			CreateIndexBuffer(std::vector<uint32_t>& idxData);
	BufferGPU*			CreateConstantBuffer(unsigned size);
	BufferGPU*			CreateResourceBuffer(ResourceBufferGPU* rbgpu, void *pData, UINT32 structSize, UINT32 numStructs, bool needsAppendFlag = false);
	BufferGPU*			CreateIndirectArgumentsBuffer(void* pData);

	ID3D11DepthStencilView*	 CreateDepthStencil();
	ID3D11DepthStencilState* CreateDepthStencilState(bool depthTestEnabled);
	
	void				CreateRasterizerState(RasterizerState* rasterizerState);
	 
	void UploadBuffer(ID3D11Buffer* buffer, void* dataRet, int size);

	void BindEffect(Effect* pEffect);
	void BindVertexAndPixelShaders(ShaderType shader);	// old version, now mostly using Effect class above
	void BindInputLayout(ID3D11InputLayout* inputLayout);

	void BindVertexBuffer(ID3D11Buffer* vertData, int size);
	void BindIndexBuffer(ID3D11Buffer* vertData, int size);
	
	// TODO: Rework these along with general shader resoures to be more abstracted
	void BindTextureResource(TextureGPU* mResource, UINT32 pSlot, bool isDepthTexture = false);
	void UnbindResourceFromSlot(UINT32 pSlot);
	void BindResourceBuffer(ResourceBufferGPU* pBuffer, UINT32 pSlot, bool bindSamplerState = false, bool resetCount = false);
	void UnbindResourceBuffer(UINT32 pSlot);

	void BindPSTexture(ID3D11ShaderResourceView* texture, unsigned slot);
	void BindPSSamplerState(ID3D11SamplerState* sampler, unsigned slot);
	void BindSamplerState(ID3D11SamplerState* sampler, unsigned slot);

	void SetConstantBuffer(ID3D11Buffer* inBuffer, int inStartSlot);
	void SetVSConstantBuffer(ID3D11Buffer* inBuffer, int inStartSlot);
	void SetPSConstantBuffer(ID3D11Buffer* inBuffer, int inStartSlot);
	void SetDepthStencil(ID3D11DepthStencilView* depthStencil);
	void SetDepthStencilState(ID3D11DepthStencilState* depthStencilState);
	void SetInputTopology(D3D11_PRIMITIVE_TOPOLOGY topology);

	void ClearRenderTargets();
	void ClearDepthStencils(/*ID3D11DepthStencilView* depthStencil, */float depth);
	void DrawVertexArray(class VertexData* vertData);
	void DrawInstanced(class VertexData* vertData, UINT32 instanceCount);
	void Present();

	void RegisterShaders(ShaderType type, ID3D11VertexShader* vs, ID3D11PixelShader* ps, ID3D11GeometryShader* gs = NULL, ID3D11ComputeShader* cs = NULL);

	// Effect helper functions
	void EndRenderTargets();
	void SetMultiRenderTargetsWithDepths(TextureGPU* pPositionTex, TextureGPU* pNormalTex, TextureGPU* pDiffuseTex);
	void SetRenderTargetAndViewportWithNoDepth(TextureGPU *pDestColorTex, bool clear);
	void SetDepthStencilOnlyRenderTargetAndViewport(TextureGPU *pDestDepthTex, bool clear);
	void SetRenderTargetsAndUAVNoDepth(TextureGPU* pDestColorTex, ResourceBufferGPU* pUAV, unsigned bindSlot, bool clear = false);
	void SetMultiRenderTargetsAndUAVNoDepth(TextureGPU* pRenderTarget0, TextureGPU* pRenderTarget1, TextureGPU* pRenderTarget2, ResourceBufferGPU* pUAV, bool clear = false);

	ID3D11Device* GetDevice() { return g_Device; }
	ID3D11DeviceContext* GetDeviceContext() { return g_DeviceContext; }

private:
	// Driver Objects
	IDXGISwapChain* g_SwapChain;
	ID3D11Device* g_Device;
	ID3D11DeviceContext* g_DeviceContext;

	// Buffers
	ID3D11RenderTargetView* m_BackBuffer;
	ID3D11Buffer* pVBuffer;

	// Settings Vars
	ID3D11DepthStencilView* mCurrentDepthStencil;
	ID3D11DepthStencilView* mMeshDepthStencil;
	ID3D11DepthStencilState* mMeshDepthState;

	//Input
	
	// Shaders
	std::map<ShaderType, ShaderPair> mShaderMap;
	std::map<const char*, Effect*> mEffectMap;

	// Formats
	DXGI_FORMAT mRTV_Format;
};

