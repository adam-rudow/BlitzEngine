#pragma once
#include <vector>
#include "RendererMacros.h"

struct TextureGPU;

class RENDERER_EXPORT Material
{
public:
	Material();
	~Material();

	// Set the basic textures of the material
	void SetTextures(TextureGPU* diffuseMap, TextureGPU* normalMap = nullptr, TextureGPU* specularMap = nullptr, TextureGPU* glowMap = nullptr);

	// Bind textures and material properties to the pipeline
	void SetActive();

private:
	// List of textures
	std::vector<TextureGPU*> mTextures;

	// List of material properties (?)

};
