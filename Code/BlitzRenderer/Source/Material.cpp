#include "GraphicsDriver.h"
#include "Material.h"
#include "Texture.h"

Material::Material()
{

}

Material::~Material()
{

}

// Set the basic textures of the material
void Material::SetTextures(TextureGPU* diffuseMap, TextureGPU* normalMap, TextureGPU* specularMap, TextureGPU* glowMap)
{
	// Add diffuse testure
	mTextures.push_back(diffuseMap);

	// Only add additional textures if they exist or if one further down exists (keep indices matching across materials)
	//if (normalMap != nullptr || specularMap != nullptr || glowMap != nullptr)
		mTextures.push_back(normalMap);
	//if (specularMap != nullptr || glowMap != nullptr)
		mTextures.push_back(specularMap);
	//if(glowMap != nullptr)
		mTextures.push_back(glowMap);
}

void Material::SetActive()
{
	// Bind textures to pipeline
	if(mTextures[0])
		GraphicsDriver::Instance()->BindTextureResource(mTextures[0], TextureSlot_ColorMap);
	if (mTextures[1])
		GraphicsDriver::Instance()->BindTextureResource(mTextures[1], TextureSlot_NormalMap);
	if (mTextures[2])
		GraphicsDriver::Instance()->BindTextureResource(mTextures[2], TextureSlot_SpecularMap);
	if (mTextures[3])
		GraphicsDriver::Instance()->BindTextureResource(mTextures[3], TextureSlot_GlowMap);

	// ToDo: bind individual samplers to pipeline as well?
}

