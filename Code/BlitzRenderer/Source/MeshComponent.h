#pragma once
#include "DrawComponent.h"
#include "MaterialSet.h"
#include "Mesh.h"
#include "Renderer.h"
#include "Utils/Utility.h"

struct TextureGPU;
enum TextureFamily;
struct SamplerState;

class RENDERER_EXPORT MeshComponent : public DrawComponent
{
public:
	MeshComponent(/*class Game& game*/);
	virtual ~MeshComponent();

	void Draw(class Renderer& render) override;

	//void SetMesh(Mesh* mesh) { mMesh = mesh; }
	void SetMesh(ShaderType shaderType, const char* fileName = "");
	//Mesh* GetMesh() { return mMesh; }

	//void SetTexture(const char* fileName, bool usePresetTexture = false);
	void SetTexture(TextureGPU* texture, bool usePresetTexture = false);

	// Add a material to the material set of this mesh
	inline void AddMaterial(Material material) { mMaterialSet.AddMaterial(material); }

	// Create a vertex buffer from raw data
	void InitVertexData(GraphicsDriver& mGraphics, Effect* pEffect, std::vector<float>& vertices, std::vector<uint32_t>& indices, UINT16 vertNumFloats);
	void InitVertexData(Effect* pEffect, std::vector<float>& vertices, std::vector<uint32_t>& indices, UINT16 vertNumFloats);
	
	// Store reference to an existing vertex buffer
	void InitVertexData(VertexData* existingVertexData);

	// Send data to GraphicsDriver to prepare for drawing the mesh
	void UploadMeshDataToGPU();

	inline size_t GetNumAdditionalVertexBuffers() const { return mAdditionalVertexBuffers.size(); }
	inline VertexData* GetAdditionalVertexData(UINT16 index) const { return mAdditionalVertexBuffers[index]; }

protected:
	// Mesh File name to assist in debugging
	std::string mDbgMeshName;

	// Mesh Data
	PerObjectConstants mPerObjConst;
	ID3D11Buffer* mPerObjectBuffer;

	// Texturing
	TextureGPU* mDiffuseTexture;
	SamplerState* mDiffuseSampler;
	//ID3D11SamplerState* mDiffuseSampler;
	TextureGPU* mNormalTexture;

	MaterialSet mMaterialSet;

	// For meshes with multiple sub-meshes
	std::vector<VertexData*> mAdditionalVertexBuffers;
	std::vector<TextureGPU*> mAdditionalTextures;	// TODO: Add support for material sets which have material properties
													// well as diffuse/normal/specular (etc) maps

	
	// Shaders
	//ShaderType mShaderType;
};
