//#include "PreCompiledHeader.h"
#include "MeshComponent.h"
#include "Texture.h"
#include "SamplerState.h"

//[S1] Fix Dependecy
//#include "AssetLoader.h"

MeshComponent::MeshComponent(/*class Game& game*/) 
	: DrawComponent(/*game*/), mDbgMeshName(""), mDiffuseTexture(NULL)
{
	mVertexBuffer	 = NULL;
	mPerObjectBuffer = reinterpret_cast<Renderer*>(Renderer::Instance())->GetGraphicsDriver().CreateConstantBuffer(sizeof(PerObjectConstants));
	mDrawOrderMask	 = DrawOrder_Default;

	DrawComponent::Register();
}

MeshComponent::~MeshComponent()
{
	if (mPerObjectBuffer)	mPerObjectBuffer->Release();
	//if (mDiffuseSampler)	mDiffuseSampler->Release();
}


void MeshComponent::InitVertexData(GraphicsDriver& mGraphics, Effect* pEffect, std::vector<float>& vertices, std::vector<uint32_t>& indices, UINT16 vertNumFloats)
{
	if (mVertexBuffer == NULL)
		mVertexBuffer = new VertexData(pEffect, vertices, indices, vertNumFloats);
	else
		mAdditionalVertexBuffers.push_back(new VertexData(pEffect, vertices, indices, vertNumFloats));
}

void MeshComponent::InitVertexData(Effect* pEffect, std::vector<float>& vertices, std::vector<uint32_t>& indices, UINT16 vertNumFloats)
{
	if (mVertexBuffer == NULL)
		mVertexBuffer = new VertexData(pEffect, vertices, indices, vertNumFloats);
	else
		mAdditionalVertexBuffers.push_back(new VertexData(pEffect, vertices, indices, vertNumFloats));
}

void MeshComponent::InitVertexData(VertexData* vertexArray)
{
	if (mVertexBuffer == NULL)
		mVertexBuffer = vertexArray;
	else
		mAdditionalVertexBuffers.push_back(vertexArray);
}

void MeshComponent::Draw(class Renderer& render)
{
	if (mVertexBuffer && mIsVisible)
	{

		// Bind buffers for GPU
		render.GetGraphicsDriver().SetConstantBuffer(mPerObjectBuffer, PerObjectSlot);
		mPerObjConst.mWorldTransformMatrix = GetWorldTransform();
		
		render.GetGraphicsDriver().UploadBuffer(mPerObjectBuffer, (void*)&mPerObjConst, sizeof(PerObjectConstants));

		// Bind texture and sampler state to use
		render.GetGraphicsDriver().BindPSTexture(mDiffuseTexture->mShaderResourceView, 2);
		//if (mMaterialSet.GetNumMaterials() > 0)
		//{
		//	mMaterialSet.BindMaterial(0);
		//	//render.GetGraphicsDriver().BindPSSamplerState(mDiffuseSampler->mSamplerStateObject, 2);
		//}

		// Draw the buffer
		render.DrawVertexBuffer(mVertexBuffer/*, mShaderType*/);

		// Draw any additional vertex buffers for multi-mesh objects
		for (size_t i = 0; i < mAdditionalVertexBuffers.size(); ++i)
		{
			if (mAdditionalVertexBuffers[i])
			{
				// Bind texture to use
				render.GetGraphicsDriver().BindPSTexture(mAdditionalTextures[i]->mShaderResourceView, 2);
				//mMaterialSet.BindMaterial(i + 1);

				// Draw the buffer
				render.DrawVertexBuffer(mAdditionalVertexBuffers[i]/*, mShaderType*/);
			}
		}
	}
}

void MeshComponent::UploadMeshDataToGPU()
{

}

void MeshComponent::SetMesh(ShaderType shaderType, const char* fileName)
{
	if (mDbgMeshName == "")
	{
		mDbgMeshName = fileName;
		mShaderType = shaderType;// AssetLoader::Instance()->GetAssetData(fileName).shader;
	}
}

//void MeshComponent::SetTexture(const char* fileName, bool usePresetTexture)
//{
//	if (mDiffuseTexture == NULL)
//	{
//		if (!usePresetTexture)
//			//[S1] Fix Dependecy
//			mDiffuseTexture = AssetLoader::Instance()->LoadTexture(fileName);
//		// [S1] TODO: Uncomment
//		else // TODO: remove this dependency on hard coded values
//			mDiffuseTexture = AssetLoader::Instance()->LoadTextureOfMesh(fileName, AssetType::AT_Diffuse);
//	}
//	else
//	{
//		if(!usePresetTexture)
//			//[S1] Fix Dependecy
//			mAdditionalTextures.push_back(nullptr/*AssetLoader::Instance()->LoadTexture(fileName)*/);
//		// [S1] TODO: Uncomment
//		//else // TODO: remove this dependency on hard coded values
//		//	mAdditionalTextures.push_back(AssetLoader::Instance()->LoadTextureOfMesh(fileName, AssetType::AT_Diffuse));
//	}
//
//	// TODO: use sampler from texture only, not a general one
//	mDiffuseSampler = &SamplerStateManager::Instance()->GetSamplerState(SamplerState_MipLerp_MinTexelLerp_MagTexelLerp_Wrap);// mGame.GetRenderer().GetGraphicsDriver().CreateSamplerState();
//}

void MeshComponent::SetTexture(TextureGPU* texture, bool usePresetTexture)
{
	if (mDiffuseTexture == NULL)
	{
		if (!usePresetTexture)
			//[S1] Fix Dependecy
			mDiffuseTexture = texture;
		// [S1] TODO: Uncomment
		else // TODO: remove this dependency on hard coded values
			mDiffuseTexture = texture;// AssetLoader::Instance()->LoadTextureOfMesh(fileName, AssetType::AT_Diffuse);
	}
	else
	{
		if (!usePresetTexture)
			//[S1] Fix Dependecy
			mAdditionalTextures.push_back(texture);
		// [S1] TODO: Uncomment
		else // TODO: remove this dependency on hard coded values
			mAdditionalTextures.push_back(texture/*AssetLoader::Instance()->LoadTextureOfMesh(fileName, AssetType::AT_Diffuse)*/);
	}

	// TODO: use sampler from texture only, not a general one
	mDiffuseSampler = &SamplerStateManager::Instance()->GetSamplerState(SamplerState_MipLerp_MinTexelLerp_MagTexelLerp_Wrap);
}
