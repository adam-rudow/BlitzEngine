#include <fstream>
#include <direct.h>
#include <iostream>
#include <time.h>
#include <set>
#include <array>
#include <Windows.h>
#include <cassert>

#include "AssetLoader.h"
#include "FileWriter.h"
#include "DebugUtility.h"
#include "Utils/Utility.h"
#include "RenderUtility.h"
#include "BlitzSystem/Rendering/IRenderer.h"
#include "EffectManager.h"
//#include "Enum.h"
//#include "EffectManager.h"
#include "MeshComponent.h"
//#include <Game.h>

//#include "PreCompiledHeader.h"
#include "Texture.h"

ImplSingleton(AssetLoader)

const static unsigned ReadLineLength = 128;


AssetLoader::AssetLoader()
{
	InitAssetMap();
}

AssetLoader::~AssetLoader()
{

	// Shutdown FBXSDK stuff
	if(mFbxImporter != nullptr)		mFbxImporter->Destroy(true);
	if(g_pFbxSdkManager != nullptr)	g_pFbxSdkManager->Destroy();

}

void AssetLoader::InitAssetMap() 
{
	std::string meshLoc = mRootFolder + mMeshFolder;
	std::string texLoc = "";// mRootFolder + mTextureFolder;

}

MeshComponent* AssetLoader::LoadMesh(std::string fileName)
{
	MeshComponent* meshComp = NULL;

#if _DEBUG
	std::string loadMsg = "Loading Mesh: " + fileName + "\n";
	BEPrint(loadMsg.c_str());
#endif

	// Load a mesh based on the file type (until we have a custom asset type)
	if (fileName.find(".mesh") != std::string::npos)
	{
		meshComp = LoadBlitzEngineMesh(fileName);
	}

	// Increment count of triangles for debug info
	if (meshComp)
	{
		IRenderer::Instance()->AdjustNumTrianglesInScene(meshComp->GetVertexData()->GetNumTriangles());
		for (size_t i = 0; i < meshComp->GetNumAdditionalVertexBuffers(); ++i)
		{
			IRenderer::Instance()->AdjustNumTrianglesInScene(meshComp->GetAdditionalVertexData(i)->GetNumTriangles());
		}
	}

	return meshComp;
}

TextureGPU* AssetLoader::LoadTexture(std::string fileName)
{
	fileName = mRootFolder + mTextureFolder + fileName;
	std::map<std::string, TextureGPU*>::iterator it = mTextureNameMap.find(fileName);
	if (it == mTextureNameMap.end())
	{
		// Load the texture
		//[S1]
		TextureGPU* tex = new TextureGPU();
		tex->createTextureNoFamily(fileName.c_str());
		mTextureNameMap.insert(std::pair <std::string, TextureGPU*>(fileName, tex));
		return tex;
		//return mGame.GetRenderer().GetGraphicsDriver().CreateSRVTextureFromFile(fileName);
	}
	else
	{
		// Returned saved texture
		return it->second;
	}
}

TextureGPU* AssetLoader::LoadTextureOfMesh(std::string meshFileName, AssetType type)
{
	AssetFileLink link = mAssetMap.find(mRootFolder + mMeshFolder + meshFileName)->second;
	std::string texFile;
	if (type == AssetType::AT_Diffuse)
		texFile = link.diffuse;
	else if (type == AssetType::AT_Normal)
		texFile = link.normal;

	return LoadTexture(texFile);
}

bool AssetLoader::ConvertMesh(const std::string& fileName)
{
	std::string loadMsg = "Loading Mesh: " + fileName + "\n";
	Print(loadMsg);

	// Load a mesh based on the file type (until we have a custom asset type)
	if (fileName.find(".mesh") != std::string::npos)
	{
		//if (LoadBlitzEngineMesh(fileName))
		Print("<AssetLoader>: Asset already in engine's native format; no need to convert.");
		return true;
	}
	else if (fileName.find(".obj") != std::string::npos)
	{
		// Load an OBJ file
		if (ConvertObjMesh(fileName))
			return true;
	}
	else if(fileName.find(".fbx") != std::string::npos || fileName.find(".FBX") != std::string::npos)
	{
		// Load an FBX file
		if (ConvertFbxMesh(fileName))
			return true;
	}

	return false;
}

// Texture load
//TextureGPU* AssetLoader::LoadTexture(std::string fileName)
//{
//	fileName = mRootFolder + mTextureFolder + fileName;
//	std::map<std::string, TextureGPU*>::iterator it = mTextureNameMap.find(fileName);
//	if (it == mTextureNameMap.end())
//	{
//		// Load the texture
//		TextureGPU* tex = new TextureGPU(*GraphicsDriver::Instance());
//		tex->createTextureNoFamily(fileName.c_str());
//		mTextureNameMap.insert(std::pair <std::string, TextureGPU*>(fileName, tex));
//		return tex;
//		//return mGame.GetRenderer().GetGraphicsDriver().CreateSRVTextureFromFile(fileName);
//	}
//	else
//	{
//		// Returned saved texture
//		return it->second;
//	}
//}
//
//TextureGPU* AssetLoader::LoadTextureOfMesh(std::string meshFileName, AssetType type)
//{
//	AssetFileLink link = mAssetMap.find(mRootFolder + mMeshFolder + meshFileName)->second;
//	std::string texFile;
//	if(type == AssetType::AT_Diffuse)
//		texFile = link.diffuse;
//	else if (type == AssetType::AT_Normal)
//		texFile = link.normal;
//
//	return LoadTexture(texFile);
//}

bool AssetLoader::ConvertObjMesh(const std::string& fileName)
{
	// Add full path
	std::string fullname = mRootFolder + mMeshFolder + fileName;


	// Open file for reading
	FILE* file;
	fopen_s(&file, fullname.c_str(), "r");
	if (file == NULL)
		return NULL;

	// Storage variables
	//std::vector<unsigned> vertexIndices, uvIndices, normalIndices;
	std::vector<Vector3> temp_vertices;
	std::vector<Vector3> temp_vertNorms;
	std::vector<Vector2> temp_texCoords;

	std::vector<float> realVerts;
	std::vector<uint32_t> realIdxs;

	// attempt 2
	using Triangle	= std::array<VertexTriplet, 3>;
	using Faces		= std::vector<Triangle>;

	using Vertex	= std::tuple<Vector3, Vector2, Vector3>;
	using Index		= unsigned;

	using VertexBuffer	= std::vector<Vertex>;
	using IndexBuffer	= std::vector<Index>;
	using GPUObject		= std::pair<VertexBuffer, IndexBuffer>;
	Faces faces;

	// Parse file
	while(true)
	{
		char line[ReadLineLength] = { 0 };
		ZeroMemory(line, ReadLineLength);
		int res = fscanf_s(file, "%s", line, ReadLineLength);
		if (res == EOF)
			break;

		// Parse vertex
		if (strcmp(line, "v") == 0)
		{
			Vector3 vec;
			// Adjust read-in vertices to match right-hand system, (y,z,x)
			//fscanf_s(file, "%f %f %f\n", &vec.y, &vec.z, &vec.x/*, ReadLineLength*/);
			fscanf_s(file, "%f %f %f\n", &vec.x, &vec.y, &vec.z/*, ReadLineLength*/);
			vec.z *= -1;
			temp_vertices.push_back(vec);
		}
		// Parse texture coord
		else if (strcmp(line, "vn") == 0)
		{
			Vector3 vec;
			//fscanf_s(file, "%f %f %f\n", &vec.y, &vec.z, &vec.x/*, ReadLineLength*/);
			fscanf_s(file, "%f %f %f\n", &vec.x, &vec.y, &vec.z/*, ReadLineLength*/);
			vec.z *= -1;
			temp_vertNorms.push_back(vec);
		}
		// Parse vertex normal
		else if (strcmp(line, "vt") == 0)
		{
			Vector2 vec;
			fscanf_s(file, "%f %f \n", &vec.x, &vec.y/*, ReadLineLength*/);
			temp_texCoords.push_back(vec);
		}
		// Parse face
		else if (strcmp(line, "f") == 0)
		{
			Triangle t;
			int matches = fscanf_s(file, "%u/%u/%u %u/%u/%u %u/%u/%u\n", 
				&t[0].pos, &t[0].tc, &t[0].norm,
				&t[1].pos, &t[1].tc, &t[1].norm, 
				&t[2].pos, &t[2].tc, &t[2].norm);

			faces.push_back(t);
		}
	}

	// Close the file
	fclose(file);

	CreateVertexAndIndexListsFromRawData(temp_vertices, temp_vertNorms, temp_texCoords, NULL, faces, realVerts, realIdxs);

	
	return true;
}

void AssetLoader::AddVertexBufferToMeshFromRawData(MeshComponent* meshComp, std::vector<float>& combinedVerts, std::vector<UINT32> indices, std::string meshFileName)
{
#ifndef ASSET_PROCESSOR
	// Create the actual mesh
	//int pathOnlyLength = (mRootFolder + mMeshFolder).length();
	std::string localFileName = meshFileName;
	if(meshFileName.find('/') != std::string::npos || meshFileName .find('\\') != std::string::npos)
	{
		localFileName = ExtractLocalFileNameFromPath(meshFileName);
	}

	ShaderType type = ST_BasicMesh;// mAssetMap.find(meshFileName)->second.shader;
	Effect* effect = type == ST_NoDefault ? EffectManager::Instance()->GetEffect(ShaderType::ST_BasicMesh) : EffectManager::Instance()->GetEffect(type);
	
	meshComp->InitVertexData(effect, combinedVerts, indices, 8); // 8 floats (3 pos, 2 texCoord, 3 normal)
	meshComp->SetMesh(type, localFileName.c_str());

	// Store for future use and release
	mVertexArrayMap.emplace(meshFileName, meshComp->GetVertexData());
#endif
}

bool AssetLoader::ConvertFbxMesh(const std::string& fileName)
{
	if (g_pFbxSdkManager == nullptr)
	{
		g_pFbxSdkManager = FbxManager::Create();

		FbxIOSettings* pIOsettings = FbxIOSettings::Create(g_pFbxSdkManager, IOSROOT);
		g_pFbxSdkManager->SetIOSettings(pIOsettings);
	}

	if (mFbxImporter == nullptr)
	{
		mFbxImporter = FbxImporter::Create(g_pFbxSdkManager, "");
	}

	FbxScene* pFbxScene = FbxScene::Create(g_pFbxSdkManager, "");
	std::string workingDir = GetWorkingDirectory();
	std::string assetFullPath = workingDir.append("../").append(mRootFolder).append(mMeshFolder).append(fileName);
	// "C:/Users/Adam/Documents/Visual Studio 2015/Projects/DirectXBasic/DirectXBasic/Assets/Meshes/" + fileName;
	MeshWriter mw;
	bool exists = mw.DoesFileExist(assetFullPath);
	bool bSuccess = mFbxImporter->Initialize((assetFullPath).c_str(), -1, g_pFbxSdkManager->GetIOSettings());
	//bool bSuccess = mFbxImporter->Initialize((mRootFolder + mMeshFolder + fileName).c_str(), -1, g_pFbxSdkManager->GetIOSettings());
	if (!bSuccess)
	{
		Print("Failed to initialize FBX Importer /n\n");
		return false;
	}

	bSuccess = mFbxImporter->Import(pFbxScene);
	if (!bSuccess)
	{
		Print("Failed to Import FBX scene\n\n");
		return false;
	}

	FbxNode* pFbxRootNode = pFbxScene->GetRootNode();

	// Open Files
	// Extract name without ".fbx"
	std::string assetName = fileName.substr(0, fileName.size() - 4);
	/*std::string meshFileName = MeshWriter::sMeshAssetPath + assetName + ".mesh";
	std::ofstream meshFileOut;
	meshFileOut.open(meshFileName.c_str(), std::ios::out | std::ios::binary);

	std::string vBufferFileName = MeshWriter::sVertexBufferPath + assetName + MeshWriter::sVertexBufferExtension;
	std::string BufferFileName	= MeshWriter::sIndexBufferPath + assetName + MeshWriter::sIndexBufferExtension;
	std::string MaterialSetFileName = MeshWriter::sMaterialSetPath + assetName + MeshWriter::sMaterialSetExtension;*/

	MeshWriter::sInstance->WriteMeshFile(fileName);

	std::vector< std::vector<float> > allVertexBuffers;
	std::vector< std::vector<unsigned> > allIndexBuffers;
	std::vector<std::vector<std::string>> materialNames;
	std::vector<unsigned> matIndices;
	std::vector<uint8_t> vertFormats;
	std::vector< std::pair<Vector3, Vector3> > allMinMaxPoints;


	if (pFbxRootNode)
	{
		FbxNodeAttribute::EType rootAttributeType = pFbxRootNode->GetChild(0)->GetNodeAttribute()->GetAttributeType();

		if (rootAttributeType != FbxNodeAttribute::eMesh)
			pFbxRootNode = pFbxRootNode->GetChild(0);

		int childCount = pFbxRootNode->GetChildCount();
		for (int i = 0; i < childCount; i++)
		{
			FbxNode* pFbxChildNode = pFbxRootNode->GetChild(i);

			if (pFbxChildNode->GetNodeAttribute() == NULL)
				continue;


			std::string loadMsg = "\tloading submesh " + std::to_string(i + 1) + "/" + std::to_string(childCount);
			if (i % 4 == 0)
				loadMsg += "\n";
			Print(loadMsg);


			FbxNodeAttribute::EType AttributeType = pFbxChildNode->GetNodeAttribute()->GetAttributeType();

			if (AttributeType != FbxNodeAttribute::eMesh)
				continue;

			// Data structures to do work in
			std::vector<float> realvertices;
			std::vector<unsigned> realIdxs;

			std::vector<Vector3> temp_vertices;
			std::vector<Vector2> temp_texCoords;
			std::vector<Vector3> temp_normals;
			std::vector<Vector3> temp_tangents;
			UINT32 indexCounter = 1;

			FbxMesh* pMesh = (FbxMesh*)pFbxChildNode->GetNodeAttribute();

			// Get FBX submesh transforms
			//FbxAMatrix& lGlobalTransform = pFbxChildNode->EvaluateGlobalTransform();
			//Vector3 trans(Vector3(S_CAST(float, lGlobalTransform.GetT().mData[0]),
			//					S_CAST(float, lGlobalTransform.GetT().mData[1]),
			//					S_CAST(float, lGlobalTransform.GetT().mData[2])));
			//Matrix4 transMatrix = Matrix4::CreateTranslation(trans);
			//Vector3 scale(Vector3(S_CAST(float, lGlobalTransform.GetS().mData[0]),
			//					S_CAST(float, lGlobalTransform.GetS().mData[1]),
			//					S_CAST(float, lGlobalTransform.GetS().mData[2])));
			Vector3 scale(100, 100, 100);
			Matrix4 scaleMatrix = Matrix4::CreateScale(scale);
			//Quaternion rot = Quaternion(S_CAST(float, lGlobalTransform.GetQ().mData[0]),
			//							S_CAST(float, lGlobalTransform.GetQ().mData[1]),
			//							S_CAST(float, lGlobalTransform.GetQ().mData[2]),
			//							S_CAST(float, lGlobalTransform.GetQ().mData[3]));
			//// Rotate by 180 to match Maya
			////rot = Concatenate(rot, Quaternion(Vector3::UnitY, 180.0f));
			//Matrix4 rotMatrix = Matrix4::CreateFromQuat(rot);

			Matrix4 meshTransform = scaleMatrix;// scaleMatrix * rotMatrix * transMatrix;
			//FbxDouble3 fbxTrans = pFbxChildNode->LclTranslation.Get();
			//Vector3 trans(S_CAST(float, fbxTrans.mData[0]), S_CAST(float, fbxTrans.mData[1]), S_CAST(float, fbxTrans.mData[2]));
			//trans = trans * 100;

			bool hasNormals = false;
			bool useTangents = false;

			//****************//
			//*** Textures ***//
			//****************//
			// Get textures before parsing buffers to see if we should store tangents or other optional data

			// Get texture's local file name
			std::string dir = GetWorkingDirectory();
			std::vector<std::string> textureNames = GetFBXTextureNames(pFbxChildNode, pMesh);
			//materialName = materialName.substr((dir + mRootFolder + mTextureFolder).length());
			bool containsMat = false;
			for (uint32_t m = 0; m < materialNames.size(); ++m)
			{
				if (materialNames[m][0] == textureNames[0])
				{
					containsMat = true;
					matIndices.push_back(m);
					break;
				}
			}
			if (!containsMat)
			{
				materialNames.push_back(textureNames);
				matIndices.push_back(materialNames.size() - 1);
			}

			if (textureNames.size() > 1 && textureNames[1] != "")
				useTangents = true;

			//****************//
			//*** Vertices ***//
			//****************//
			indexCounter = 1;
			std::map<Vector3, UINT32, Vector3Comparator_Less> vertMap;
			FbxVector4* pVertices = pMesh->GetControlPoints();

			Vector3 maxPoint(FBXSDK_FLOAT_MIN, FBXSDK_FLOAT_MIN, FBXSDK_FLOAT_MIN);
			Vector3 minPoint(FBXSDK_FLOAT_MAX, FBXSDK_FLOAT_MAX, FBXSDK_FLOAT_MAX);

			int vertexCount = pMesh->GetControlPointsCount();
			for (int j = 0; j < vertexCount; j++)
			{
				Vector3 vertex;
				vertex.x = S_CAST(float, pVertices[j].mData[0]);
				vertex.y = S_CAST(float, pVertices[j].mData[1]);
				vertex.z = S_CAST(float, pVertices[j].mData[2]);
				Vector4 transformedPos = Vector4(vertex, 1) * meshTransform;
				vertex = Vector3(transformedPos.x, transformedPos.y, transformedPos.z);
				temp_vertices.push_back(vertex);
				vertMap[vertex] = indexCounter++;
				if (vertMap[vertex] == 0)
				{
					Print("Error in mapping vertices to faces\n\n");
					return false;
				}

				// Get Bounding Points for frustum culling purposes
				ExtractBoundingPoints(vertex, minPoint, maxPoint);
			}

			// Store min/max points
			allMinMaxPoints.push_back(std::pair<Vector3, Vector3>(minPoint, maxPoint));

			//**************************//
			//*** Normals + Tangents ***//
			//**************************//
			indexCounter = 1;
			std::map<Vector3, UINT32, Vector3Comparator_Less> normalMap;
			FbxLayerElement::EMappingMode mapMode = pMesh->GetElementNormal()->GetMappingMode();
			/*if (mapMode != FbxLayerElement::EMappingMode::eByPolygonVertex)
				return NULL;
			FbxLayerElement::EReferenceMode refMode = pMesh->GetElementNormal()->GetReferenceMode();
			if (refMode != FbxLayerElement::EReferenceMode::eDirect)
				return NULL;*/

			// Make a list of tangents, the index of the tangent corresponds to the normal's index
			
			FbxLayerElementArrayTemplate<FbxVector4>* pTangents;
			pMesh->GetTangents(&pTangents);

			unsigned tangentCount = 0;
			if (useTangents && pTangents != NULL && pTangents->GetCount() > 0)
			{
				tangentCount = pTangents->GetCount();
				temp_tangents.resize(tangentCount + 1); // " + 1 " to stay in sync with normals list (index starting at 1)
			}

			FbxLayerElementArrayTemplate<FbxVector4>* pNormals;// = pMesh->GetElementNormal()->GetDirectArray();
			pMesh->GetNormals(&pNormals);
			int normalCount = pNormals->GetCount();
			for (int j = 0; j < normalCount; j++)
			{
				// Extract Normal
				Vector3 normal;
				normal.x = S_CAST(float, pNormals->GetAt(j).mData[0]);
				normal.y = S_CAST(float, pNormals->GetAt(j).mData[1]);
				normal.z = S_CAST(float, pNormals->GetAt(j).mData[2]);
				Vector4 transformedNorm = Vector4(normal, 1) /** meshTransform*/;
				normal = Vector3(transformedNorm.x, transformedNorm.y, transformedNorm.z);
				temp_normals.push_back(normal);
				normalMap[normal] = indexCounter++;

				// Extract Tangent (if valid)
				if (useTangents && tangentCount > 0)
				{
					Vector3 tangent;
					tangent.x = S_CAST(float, pTangents->GetAt(j).mData[0]);
					tangent.y = S_CAST(float, pTangents->GetAt(j).mData[1]);
					tangent.z = S_CAST(float, pTangents->GetAt(j).mData[2]);
					//Vector4 transformedTangent = Vector4(tangent, 1) /** meshTransform*/;
					temp_tangents[j + 1] = tangent;
				}

			}

			//****************//
			//*** Tangents ***//
			//****************//
			/*std::map<Vector3, Vector3, Vector3Comparator_Less> normalToTangentMap;
			FbxLayerElementArrayTemplate<FbxVector4>* pTangents;
			pMesh->GetTangents(&pTangents);
			if (pTangents != NULL && pTangents->GetCount() > 0)
			{
				int tangentCount = pTangents->GetCount();

			}*/

			//*****************//
			//*** TexCoords ***//
			//*****************//
			indexCounter = 1;
			std::map<Vector2, UINT32, Vector2Comparator_Less> tcMap;
			FbxStringList lUVSetNameList;
			pMesh->GetUVSetNames(lUVSetNameList);
			if (lUVSetNameList.GetCount() > 1)
			{
				Print("Error: handling more than 1 UV set is not yet implemented\n\n");
				return false;
			}


			const char* lUVSetName = lUVSetNameList.GetStringAt(0);
			const FbxGeometryElementUV* lUVElement = pMesh->GetElementUV(lUVSetName);

			assert(lUVElement != NULL);

			// only support mapping mode eByPolygonVertex and eByControlPoint
			if (lUVElement->GetMappingMode() != FbxGeometryElement::eByPolygonVertex &&
				lUVElement->GetMappingMode() != FbxGeometryElement::eByControlPoint)
			{
				Print("Error: element mapping only supported for eByPolygonVertex and eByControlPoint,\n\n");
				return false;
			}

			FbxLayerElementArrayTemplate<FbxVector2>& pUVs = lUVElement->GetDirectArray();
			int tcCount = pUVs.GetCount();
			for (int j = 0; j < tcCount; j++)
			{
				Vector2 texCoord;
				texCoord.x = S_CAST(float, pUVs[j].mData[0]);
				texCoord.y = S_CAST(float, pUVs[j].mData[1]);
				temp_texCoords.push_back(texCoord);
				tcMap[texCoord] = indexCounter++;
			}

			//***************//
			//*** Indices ***//
			//***************//

			// typedefs for easy use
			using Triangle = std::array<VertexTriplet, 3>;
			using Faces = std::vector<Triangle>;

			Faces faces;

			// Create index/face list for all polygons
			int polyCount = pMesh->GetPolygonCount();
			for (int j = 0; j < min(polyCount, 9000); j++)
			{
				int iNumVertices = pMesh->GetPolygonSize(j);
				assert(iNumVertices == 3);// , "File contains an incorrect format!");

				// Store which index is used for each vertex in a polygon
				Triangle t;
				for (int k = 0; k < iNumVertices; k++)
				{
					// Get vertex position
					Vector3 vertex;
					int iControlPointIndex = pMesh->GetPolygonVertex(j, k);

					vertex.x = S_CAST(float, pVertices[iControlPointIndex].mData[0]);
					vertex.y = S_CAST(float, pVertices[iControlPointIndex].mData[1]);
					vertex.z = S_CAST(float, pVertices[iControlPointIndex].mData[2]);
					Vector4 transformedPos = Vector4(vertex, 1) * meshTransform;
					vertex = Vector3(transformedPos.x, transformedPos.y, transformedPos.z);
					t[k].pos = vertMap[vertex];

					// Get vertex normal
					FbxVector4 vertNorm;
					pMesh->GetPolygonVertexNormal(j, k, vertNorm);
					Vector3 normal;
					normal.x = S_CAST(float, vertNorm.mData[0]);
					normal.y = S_CAST(float, vertNorm.mData[1]);
					normal.z = S_CAST(float, vertNorm.mData[2]);
					Vector4 transformedNorm = Vector4(normal, 1) /** meshTransform*/;
					normal = Vector3(transformedNorm.x, transformedNorm.y, transformedNorm.z);
					t[k].norm = normalMap[normal];

					// Get vertex uv coord
					FbxVector2 vertTexCoord;
					bool isUvMapped = true;
					pMesh->GetPolygonVertexUV(j, k, lUVSetName, vertTexCoord, isUvMapped);
					//if (!isUvMapped)
					//	vertTexCoord = FbxVector2(0, 0);	// TODO: figure out what to do in this case
					Vector2 uv;
					uv.x = S_CAST(float, vertTexCoord.mData[0]);
					uv.y = S_CAST(float, vertTexCoord.mData[1]);
					t[k].tc = tcMap[uv];
				}

				faces.push_back(t);
			}

			

			// Create the combined vertex and index buffers to be used for the vertex arrays for GPU
			CreateVertexAndIndexListsFromRawData(temp_vertices, temp_normals, temp_texCoords, &temp_tangents, faces, realvertices, realIdxs);

			// ToDo: optimize this so that we aren't copying all of the data over an additional time
			allVertexBuffers.push_back(realvertices);
			allIndexBuffers.push_back(realIdxs);
			if (tangentCount > 0 && useTangents)
				vertFormats.push_back(S_CAST(uint8_t, VertexFormat_DetailedMesh));
			else if(normalCount > 0)
				vertFormats.push_back(S_CAST(uint8_t, VertexFormat_StdMesh));
			else
				vertFormats.push_back(S_CAST(uint8_t, VertexFormat_MinimalMesh));
		}
		

		MeshWriter::sInstance->WriteVertexBuffers(assetName, vertFormats, allVertexBuffers, allMinMaxPoints);
		MeshWriter::sInstance->WriteIndexBuffers(assetName, allIndexBuffers, matIndices);
		MeshWriter::sInstance->WriteMaterialSet(assetName, materialNames);
	}
	
	return true;
}

MeshComponent* AssetLoader::LoadBlitzEngineMesh(std::string fileName)
{
	MeshWriter meshWriter;
	MeshComponent* meshComp = new MeshComponent(); // [S1] Fix this dependency

	std::vector< std::vector<float> > vertBuffers;
	std::vector<uint8_t> vertLayouts;
	std::vector< std::vector<uint32_t> > indexBuffers;
	std::vector<unsigned> perBufferMatIndices;
	std::vector< std::vector<std::string> > materialList;

	meshWriter.ReadMeshFile(fileName, meshComp, vertBuffers, vertLayouts, indexBuffers, perBufferMatIndices, materialList);

	for (unsigned i = 0; i < vertBuffers.size(); ++i)
	{
		AddVertexBufferToMeshFromRawData(meshComp, vertBuffers[i], indexBuffers[i], fileName);
		const size_t matIndex = perBufferMatIndices[i];
		for (unsigned k = 0; k < materialList[matIndex].size(); ++k)
		{
			if(materialList[matIndex][k] != "")
			{
				// TODO: do material sets better
				TextureGPU* tex = LoadTexture(materialList[matIndex][k]);
				meshComp->SetTexture(tex);
			}
		}
	}

	// Rotate mesh properly to match Maya view
	//meshComp->SetRotation(Quaternion(Vector3::UnitY, 180.0f));

	return meshComp;
}

AssetFileLink AssetLoader::GetAssetData(std::string fileName)
{
	if (fileName.find(mRootFolder) == std::string::npos)
		return mAssetMap.find(mRootFolder + mMeshFolder + fileName)->second;
	else
		return mAssetMap.find(fileName)->second;
}


void AssetLoader::StoreVertexArray(std::string fileName, VertexData* vertexArray)
{
	//mVertexArrayMap.emplace(fileName, vertexArray);
	mVertexBuffersList.push_back(vertexArray);
}

void AssetLoader::CreateVertexAndIndexListsFromRawData(
	std::vector<Vector3>& temp_vertices, std::vector<Vector3>& temp_vertNorms, std::vector<Vector2>& temp_texCoords, std::vector<Vector3>* temp_tangents,
	std::vector< std::array<VertexTriplet, 3> > faces,
	std::vector<float>& realVerts, std::vector<uint32_t>& realIdxs
	)
{
	// typedefs for easy use
	using Triangle = std::array<VertexTriplet, 3>;
	using Faces = std::vector<Triangle>;

	using Vertex = std::tuple<Vector3, Vector2, Vector3>;
	using Index = unsigned;

	using VertexBuffer = std::vector<Vertex>;
	using IndexBuffer = std::vector<Index>;
	using GPUObject = std::pair<VertexBuffer, IndexBuffer>;
	//Faces faces;

	GPUObject result;
	// first, we create unique index for each existing triplet
	auto & indexBuffer = result.second;
	indexBuffer.reserve(faces.size() * 3);
	std::map<VertexTriplet, Index> remap;
	Index freeIndex = 0;
	for (auto & triangle : faces)
	{
		for (auto & vertex : triangle)
		{
			auto it = remap.find(vertex);
			if (it != remap.end()) { // index already exists
				indexBuffer.push_back(it->second);
				realIdxs.push_back(it->second);
			}
			else { // create new index
				indexBuffer.push_back(freeIndex);
				realIdxs.push_back(freeIndex);
				remap[vertex] = freeIndex++;
			}
		}
	}

	// we now have the index buffer, we can fill the vertex buffer
	// we start by reversing the mapping from triplet to index
	// so wee can fill the memory with monotonic increasing address
	std::map< Index, VertexTriplet > reverseRemap;
	for (auto & pair : remap)
	{
		reverseRemap[pair.second] = pair.first;
	}

	auto & vertexBuffer = result.first;
	vertexBuffer.reserve(reverseRemap.size());
	for (auto & pair : reverseRemap)
	{
		auto & triplet = pair.second;

		// Positions
		realVerts.push_back(temp_vertices[triplet.pos - 1].x);
		realVerts.push_back(temp_vertices[triplet.pos - 1].y);
		realVerts.push_back(temp_vertices[triplet.pos - 1].z * -1);

		// TexCoords
		// TODO: clean up this hack lol. What do we do if a triangle isnt UV mapped?
		if (triplet.tc > 0)
		{
			realVerts.push_back(temp_texCoords[triplet.tc - 1].x);
			realVerts.push_back(1 - temp_texCoords[triplet.tc - 1].y);
		}
		else
		{
			realVerts.push_back(0);
			realVerts.push_back(0);
		}

		// Normals
		realVerts.push_back(temp_vertNorms[triplet.norm - 1].x);
		realVerts.push_back(temp_vertNorms[triplet.norm - 1].y);
		realVerts.push_back(temp_vertNorms[triplet.norm - 1].z * -1);

		// Tangents (optional)
		if (temp_tangents != NULL && temp_tangents->size() > 0)
		{
			// Use normal indices since we do not have the tangent indices from fbx (should match up hopefully?)
			realVerts.push_back((*temp_tangents)[triplet.norm - 1].x);
			realVerts.push_back((*temp_tangents)[triplet.norm - 1].y);
			realVerts.push_back((*temp_tangents)[triplet.norm - 1].z * -1);
		}
	}
}

void AssetLoader::ExtractBoundingPoints(const Vector3& newPoint, Vector3& curMin, Vector3& curMax) const
{
	curMax.x = newPoint.x > curMax.x ? newPoint.x : curMax.x;
	curMin.x = newPoint.x < curMin.x ? newPoint.x : curMin.x;

	curMax.y = newPoint.y > curMax.y ? newPoint.y : curMax.y;
	curMin.y = newPoint.y < curMin.y ? newPoint.y : curMin.y;

	curMax.z = newPoint.z > curMax.z ? newPoint.z : curMax.z;
	curMin.z = newPoint.z < curMin.z ? newPoint.z : curMin.z;
}

void AssetLoader::LoadFbxUVInformation(FbxMesh* pMesh, std::vector<float>* texCoords)
{
	//get all UV set names
	FbxStringList lUVSetNameList;
	pMesh->GetUVSetNames(lUVSetNameList);

	if (lUVSetNameList.GetCount() > 1)
		assert(false);

	//iterating over all uv sets
	for (int lUVSetIndex = 0; lUVSetIndex < lUVSetNameList.GetCount(); lUVSetIndex++)
	{
		//get lUVSetIndex-th uv set
		const char* lUVSetName = lUVSetNameList.GetStringAt(lUVSetIndex);
		const FbxGeometryElementUV* lUVElement = pMesh->GetElementUV(lUVSetName);

		if (!lUVElement)
			continue;

		// only support mapping mode eByPolygonVertex and eByControlPoint
		if (lUVElement->GetMappingMode() != FbxGeometryElement::eByPolygonVertex &&
			lUVElement->GetMappingMode() != FbxGeometryElement::eByControlPoint)
			return;

		//index array, where holds the index referenced to the uv data
		const bool lUseIndex = lUVElement->GetReferenceMode() != FbxGeometryElement::eDirect;
		const int lIndexCount = (lUseIndex) ? lUVElement->GetIndexArray().GetCount() : 0;

		FbxLayerElementArrayTemplate<FbxVector2> pUVs = lUVElement->GetDirectArray();
		for (int i = 0; i < pUVs.GetCount(); ++i)
		{
			texCoords->push_back(S_CAST(float, pUVs[i].mData[0]));
			texCoords->push_back(S_CAST(float, pUVs[i].mData[1]));
		}

		return;

		//*** For extra, more complicated meshes or formats ***//
		/*
		//iterating through the data by polygon
		const int lPolyCount = pMesh->GetPolygonCount();

		if (lUVElement->GetMappingMode() == FbxGeometryElement::eByControlPoint)
		{
			for (int lPolyIndex = 0; lPolyIndex < lPolyCount; ++lPolyIndex)
			{
				// build the max index array that we need to pass into MakePoly
				const int lPolySize = pMesh->GetPolygonSize(lPolyIndex);
				for (int lVertIndex = 0; lVertIndex < lPolySize; ++lVertIndex)
				{
					FbxVector2 lUVValue;

					//get the index of the current vertex in control points array
					int lPolyVertIndex = pMesh->GetPolygonVertex(lPolyIndex, lVertIndex);

					//the UV index depends on the reference mode
					int lUVIndex = lUseIndex ? lUVElement->GetIndexArray().GetAt(lPolyVertIndex) : lPolyVertIndex;

					lUVValue = lUVElement->GetDirectArray().GetAt(lUVIndex);

					//User TODO:
					//Print out the value of UV(lUVValue) or log it to a file
					texCoords->push_back(static_cast<float>(lUVValue.mData[0]));
					texCoords->push_back(static_cast<float>(lUVValue.mData[1]));
				}
			}
		}
		else if (lUVElement->GetMappingMode() == FbxGeometryElement::eByPolygonVertex)
		{
			int lPolyIndexCounter = 0;
			for (int lPolyIndex = 0; lPolyIndex < lPolyCount; ++lPolyIndex)
			{
				// build the max index array that we need to pass into MakePoly
				const int lPolySize = pMesh->GetPolygonSize(lPolyIndex);
				for (int lVertIndex = 0; lVertIndex < lPolySize; ++lVertIndex)
				{
					if (lPolyIndexCounter < lIndexCount)
					{
						FbxVector2 lUVValue;

						//the UV index depends on the reference mode
						int lUVIndex = lUseIndex ? lUVElement->GetIndexArray().GetAt(lPolyIndexCounter) : lPolyIndexCounter;

						lUVValue = lUVElement->GetDirectArray().GetAt(lUVIndex);

						//User TODO:
						//Print out the value of UV(lUVValue) or log it to a file
						texCoords->push_back(static_cast<float>(lUVValue.mData[0]));
						texCoords->push_back(static_cast<float>(lUVValue.mData[1]));

						lPolyIndexCounter++;
					}
				}
			}
		}*/
	}
}

std::vector<std::string> AssetLoader::GetFBXTextureNames(FbxNode* node, FbxMesh* pMesh)
{
	std::vector<std::string> textureList;
	textureList.resize(2); // ToDo: allow use for all texture possibilites (normal, specular, glow, PBR?)

	//FbxGeometryElementMaterial* mat = pMesh->get GetElementMaterial(0);

	// ToDo: Use multiple/layered materials
	//int matCount = node->GetSrcObjectCount<FbxSurfaceMaterial>();
	int i = 0;
	//for (int i = 0; i < matCount; ++i)
	{
		FbxSurfaceMaterial* material = (FbxSurfaceMaterial*)node->GetSrcObject<FbxSurfaceMaterial>(i);

		if (material != NULL)
		{
			// Albedo texture
			// This only gets the material of type sDiffuse, you probably need to traverse all Standard Material Property by its name to get all possible textures.
			FbxProperty prop = material->FindProperty(FbxSurfaceMaterial::sDiffuse);
			// Check if it's layeredtextures
			//int layeredTextureCount = prop.GetSrcObjectCount<FbxLayeredTexture>();
			//if (layeredTextureCount > 0)
			//{
			//	for (int j = 0; j < layeredTextureCount; j++)
			//	{
			//		FbxLayeredTexture* layered_texture = FbxCast<FbxLayeredTexture>(prop.GetSrcObject<FbxLayeredTexture>(0));
			//		int lcount = layered_texture->GetSrcObjectCount<FbxTexture>();
			//
			//		for (int k = 0; k < lcount; k++)
			//		{
			//			FbxFileTexture* texture = FbxCast<FbxFileTexture>(layered_texture->GetSrcObject<FbxTexture>(k));
			//			// Then, you can get all the properties of the texture, include its name
			//			const char* textureName = texture->GetFileName();
			//			
			//			return ExtractLocalFileNameFromPath(textureName);
			//			//return textureName;
			//			//LOG(INFO) << textureName;
			//		}
			//	}
			//}
			//else
			{
				// Directly get textures
				int textureCount = prop.GetSrcObjectCount<FbxTexture>();
				for (int j = 0; j < textureCount; j++)
				{
					FbxFileTexture* texture = FbxCast<FbxFileTexture>(prop.GetSrcObject<FbxTexture>(j));
					// Then, you can get all the properties of the texture, include its name
					const char* textureName = texture->GetFileName();
					std::string texloadMsg = "\t\tDiffuse Map: ";
					texloadMsg += textureName;
					texloadMsg += "\n";
			
					textureList[0] = ExtractLocalFileNameFromPath(textureName);

					/*
					//FbxProperty p = texture->RootProperty.Find("Filename");
					//LOG(INFO) << p.Get<FbxString>() << std::endl;
					HRESULT hr;
					D3DX11CreateTextureFromFile(Game::GetInstance()->GetRenderer()->GetDevice(), textureName, 0, 0, &m_texture, &hr);
					if (FAILED(hr))
					{
						std::string message;
						message.append("Load Texture: ");
						message.append(texture->GetName());
						message.append(" failed");
						SHOWMESSAGEBOX(hr, message.c_str());
					}*/
				}
			}

			// Normal map
			FbxProperty normalMapProp = material->FindProperty(FbxSurfaceMaterial::sNormalMap);
			if(normalMapProp.IsValid())
			{
				int textureCount = normalMapProp.GetSrcObjectCount<FbxTexture>();
				for (int j = 0; j < textureCount; j++)
				{
					FbxFileTexture* texture = FbxCast<FbxFileTexture>(normalMapProp.GetSrcObject<FbxTexture>(j));
					const char* textureName = texture->GetFileName();
					std::string texloadMsg = "\t\tNormal Map: ";
					texloadMsg += textureName;
					texloadMsg += "\n";

					textureList[1] = ExtractLocalFileNameFromPath(textureName);
				}
			}
			else
			{
				textureList[1] = "";
			}
		}
	}

	return textureList;
}

std::string AssetLoader::ExtractLocalFileNameFromPath(std::string filePath)
{
	int i = filePath.length() - 1;
	for (; i > 0; --i)
	{
		if (filePath[i] == '/' || filePath[i] == '\\')
			break;
	}

	return filePath.substr(i + 1);
}

std::string AssetLoader::CreateAbsolutePathFromLocalFile(const std::string& fileName, const std::string& subFolderPath)
{
	//*** Untested ***//
	/*bool containsRoot		= fileName.find(mRootFolder) != std::string::npos;
	bool containsSubFolder = fileName.find(subFolderPath) != std::string::npos;
	if (!containsRoot)
	{
		if (!containsSubFolder)
		{
			return mRootFolder + subFolderPath + fileName;
		}

		return subFolderPath + fileName;
	}*/
	return fileName;
}

std::string AssetLoader::GetWorkingDirectory(/*std::string& dir*/)
{
	unsigned int length = 0;
	char* buffer;
	length = GetCurrentDirectory(0, NULL);
	buffer = new char[length];
	GetCurrentDirectory((DWORD)length, (LPTSTR)buffer);
	//dir = buffer;
	std::string fullPath(buffer);
	delete[] buffer;

	for (unsigned i = 0; i < fullPath.length(); ++i)
	{
		if (fullPath.at(i) == '\\' )
		{
			fullPath.replace(i, 1, "/", 0, 1);
		}
	}

	return fullPath + "/";
}