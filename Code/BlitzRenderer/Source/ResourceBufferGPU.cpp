#include "ResourceBufferGPU.h"
#include "SamplerState.h"

ResourceBufferGPU::ResourceBufferGPU(class GraphicsDriver& gDriver, UINT32 pStructSize, UINT32 pStructCount, /*UINT32 pBindSlot,*/ bool SRV, bool UAV)
	: mGraphics(gDriver)
	, mStructSize(pStructSize)
	, mStructCount(pStructCount)
	//, mBindSlot(pBindSlot)
{
	mUnorderedAccessView	= UAV ? (UnorderedAccessView*)1 : NULL;
	mShaderResourceView		= SRV ? (ShaderResourceView*)1	: NULL;
	mBuffer = NULL;
}

ResourceBufferGPU::~ResourceBufferGPU()
{
	if (mBuffer)				mBuffer->Release();
	if (mUnorderedAccessView)	mUnorderedAccessView->Release();
	if (mShaderResourceView)	mShaderResourceView->Release();
}

void ResourceBufferGPU::CreateBuffer(void* pData, bool appendFlag)
{
	mGraphics.CreateResourceBuffer(this, pData, mStructSize, mStructCount, appendFlag);
}

void ResourceBufferGPU::SetActive()
{
	mGraphics.BindResourceBuffer(this, mBindSlot, true);
}

void ResourceBufferGPU::SetUnactive()
{
	mGraphics.UnbindResourceBuffer(mBindSlot);
}
