#pragma once
#include "Material.h"

class RENDERER_EXPORT MaterialSet
{
public:
	MaterialSet();
	~MaterialSet();

	// Add a material to the material list
	inline void AddMaterial(Material material) { mMaterials.push_back(material); }

	// Bind all of the necessary textures and material properties for a specific index
	inline void BindMaterial(unsigned matIndex) { mMaterials[matIndex].SetActive(); }

	// Get the number of materials in this set
	inline unsigned GetNumMaterials() const { return mMaterials.size(); }

private:
	std::vector<Material> mMaterials;

};
