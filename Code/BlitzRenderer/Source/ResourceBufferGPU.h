#pragma once

//#include "Utils/RenderUtility.h"
#include "GraphicsDriver.h"

struct SamplerState;

class ResourceBufferGPU
{
public:
	ResourceBufferGPU(class GraphicsDriver& gDriver, UINT32 pStructSize, UINT32 pStructCount, /*UINT32 pBindSlot,*/ bool SRV, bool UAV);
	~ResourceBufferGPU();

	void SetActive();
	void SetUnactive();

	void CreateBuffer(void* pData, bool appendFlag = false);

	inline size_t GetStructCount() const { return mStructCount; }
	inline size_t GetStructSize() const { return mStructSize; }
	inline BufferGPU* GetBuffer() const { return mBuffer; }

	inline UnorderedAccessView** GetUAV() { return &mUnorderedAccessView; }
	inline ShaderResourceView** GetSRV() { return &mShaderResourceView; }
	inline UINT32 GetBindSlot() const { return mBindSlot; }
	inline SamplerState* GetSamplerState() const { return mSamplerState; }

	inline void SetBindSlot(UINT32 slot) { mBindSlot = slot; }
	inline void SetSamplerState(SamplerState* samplerState) { mSamplerState = samplerState; }

private:
	class GraphicsDriver& mGraphics;
	BufferGPU* mBuffer;

	size_t mStructCount;
	size_t mStructSize;
	UINT32 mBindSlot;

	UnorderedAccessView* mUnorderedAccessView;
	ShaderResourceView* mShaderResourceView;

	SamplerState* mSamplerState;

};