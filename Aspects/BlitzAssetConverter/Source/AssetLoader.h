#pragma once
#include <string>
#include <vector>
#include <map>

#include <fbxsdk.h>
#include "Math/math.h"
#include "Utils/Utility.h"
//#include <Utils/RenderUtility.h>

#include "AssetMacros.h"

//class Game;
struct TextureGPU;
class VertexData;
class MeshComponent;
enum ShaderType;

struct AssetFileLink
{
	std::string mesh;
	std::string diffuse;
	std::string normal;
	ShaderType shader;

	AssetFileLink(std::string m, std::string d, std::string n, ShaderType st)
	{
		mesh = m;
		diffuse = d;
		normal = n;
		shader = st;
	}
};

struct VertexTriplet {
	int pos, tc, norm;
	friend bool operator< (VertexTriplet const & a, VertexTriplet const & b) {
		return    a.pos < b.pos
			|| (a.pos == b.pos && a.norm < b.norm)
			|| (a.pos == b.pos && a.norm == b.norm && a.tc < b.tc);
	}
};


class ASSET_LOADER_EXPORT AssetLoader
{
	DeclSingleton(AssetLoader)

public:
	AssetLoader(/*Game& game*/);
	~AssetLoader();

	// Loads
	MeshComponent* LoadMesh(std::string fileName);
	TextureGPU* LoadTexture(std::string fileName);
	TextureGPU* LoadTextureOfMesh(std::string meshFileName, AssetType type);

	bool ConvertMesh(const std::string& fileName);

	std::string ExtractLocalFileNameFromPath(std::string filePath);
	std::string CreateAbsolutePathFromLocalFile(const std::string& fileName, const std::string& subFolderPath);

	// Getters / Setters
	AssetFileLink GetAssetData(std::string fileName);
	void StoreVertexArray(std::string fileName, VertexData* vertexArray);

private:
	void InitAssetMap();
	bool ConvertObjMesh(const std::string& fileName);
	bool ConvertFbxMesh(const std::string& fileName);
	void LoadFbxUVInformation(FbxMesh* pMesh, std::vector<float>* texCoords);
	MeshComponent* LoadBlitzEngineMesh(std::string fileName);
	std::vector<std::string> GetFBXTextureNames(FbxNode* node, FbxMesh* pMesh);

	void CreateVertexAndIndexListsFromRawData(
		std::vector<Vector3>& temp_verts, std::vector<Vector3>& temp_normals, std::vector<Vector2>& temp_texCoords, std::vector<Vector3>* temp_tangents,
		std::vector< std::array<VertexTriplet, 3> > faces,
		std::vector<float>& realVerts, std::vector<uint32_t>& realIdxs);

	void ExtractBoundingPoints(const Vector3& newPoint, Vector3& curMin, Vector3& curMax) const;

	//	void CreateVertexAndIndexListsFromRawData(
	//		std::vector<Vector3>& temp_verts, std::vector<Vector3>& temp_normals, std::vector<Vector2>& temp_texCoords, std::vector< std::array<VertexTriplet, 3> > faces,
	//		std::vector<float>& realVerts, std::vector<uint32_t>& realIdxs);
	//
		//void AddVertexBufferToMeshFromRawData(std::vector<float>& combinedVerts, std::vector<UINT32> indices, std::string meshFileName);
		void AddVertexBufferToMeshFromRawData(MeshComponent* meshComp, std::vector<float>& combinedVerts, std::vector<UINT32> indices, std::string meshFileName);
		//void AddVertexBufferToMeshFromRawData(MeshComponent* meshComp, std::vector<float>& combinedVerts, std::vector<UINT32> indices, std::string meshFileName, VertexFormatGPU vertexFormat);
	//
	std::string GetWorkingDirectory(/*std::string& dir*/);

	//Game& mGame;
	std::string mRootFolder		= "../DirectXBasic/Assets/";
	std::string mMeshFolder		= "Meshes/";
	std::string mTextureFolder	= "Textures/";

	// map file name to hard-coded info about mesh (diffuse map, normal map, other?) until custom file type made
	std::map<std::string, AssetFileLink> mAssetMap;

	// map file name to an already-created vertex array (save time on parsing multiple objects and saves memory)
	std::map<std::string, VertexData*> mVertexArrayMap;

	// map file name to created TextureGPUs
	std::map<std::string, TextureGPU*> mTextureNameMap;

	// List of all VertexDatas and buffers
	std::vector<VertexData*> mVertexBuffersList;

	FbxManager* g_pFbxSdkManager = nullptr;
	FbxImporter* mFbxImporter = nullptr;
};