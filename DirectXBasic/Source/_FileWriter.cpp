//#include <iostream>
//#include <fstream>
//
//#include "FileWriter.h"
//#include "Utils/RenderUtility.h"
////#include "Enum.h"
//
//#define MAT_MASK_ALBEDO 0x1
//#define MAT_MASK_NORMAL 0x2
//#define MAT_MASK_GLOW	0x4
//#define MAT_MASK_SPEC	0x8
//
//std::string MeshWriter::sAssetMeshPath		= "../DirectXBasic/Assets/Meshes/";
//
//std::string MeshWriter::sMeshAssetPath		= "Meshes/";
//std::string MeshWriter::sVertexBufferPath	= "VertexBuffers/";
//std::string MeshWriter::sIndexBufferPath	= "IndexBuffers/";
//std::string MeshWriter::sMaterialSetPath	= "MaterialSets/";
//std::string MeshWriter::sMaterialsPath		= "Materials/";
//
//std::string MeshWriter::sMeshAssetExtension		= ".mesh";
//std::string MeshWriter::sVertexBufferExtension	= ".vbuf";
//std::string MeshWriter::sIndexBufferExtension	= ".ibuf";
//std::string MeshWriter::sMaterialSetExtension	= ".matset";
//std::string MeshWriter::sMaterialsExtension		= ".material";
//
//MeshWriter* MeshWriter::sInstance = nullptr;
//
//MeshWriter::MeshWriter()
//{
//	/*std::string name = "vertex buffer #0";
//	uint8_t nameLen = S_CAST(uint8_t, name.length());
//	mOutFile << nameLen;
//	mOutFile->write(name.c_str(), name.length());
//	mOutFile->write("\0", sizeof(char));*/
//
//	MeshWriter::sInstance = this;
//}
//
//MeshWriter::~MeshWriter()
//{
//
//}
//
//void MeshWriter::WriteMeshFile(std::string fileName)
//{
//	std::string assetName = fileName.substr(0, fileName.size() - 4);
//	std::string meshFileName = sAssetMeshPath + sMeshAssetPath + assetName + sMeshAssetExtension;
//	mOutFile = new std::ofstream(meshFileName.c_str(), std::ios::out | std::ios::binary);
//
//	// Write original asset name
//	WriteString(fileName);
//
//	// Write Format Version #
//	WriteUnsigned(0);
//
//	// Write VertexBuffer file name
//	std::string vBufferFileName = MeshWriter::sVertexBufferPath + assetName + MeshWriter::sVertexBufferExtension;
//	WriteString(vBufferFileName);
//
//	// Write IndexBuffer file name
//	std::string iBufferFileName = MeshWriter::sIndexBufferPath + assetName + MeshWriter::sIndexBufferExtension;
//	WriteString(iBufferFileName);
//
//	// Write material set file name
//	std::string MaterialSetFileName = MeshWriter::sMaterialSetPath + assetName + MeshWriter::sMaterialSetExtension;
//	WriteString(MaterialSetFileName);
//
//	mOutFile->close();
//	delete(mOutFile);
//}
//
//void MeshWriter::ReadMeshFile(std::string assetName, MeshComponent* outMesh, std::vector< std::vector<float>>& outVerts, 
//		std::vector<uint8_t>& vertLayouts, std::vector< std::vector<uint32_t>>& outIndices, std::vector<unsigned>& outMatIndices, 
//		std::vector< std::vector<std::string> >& outMaterials)
//{
//	// Open mesh file
//	std::string meshFileName = MeshWriter::sAssetMeshPath + MeshWriter::sMeshAssetPath + assetName;
//	mInFile = new std::ifstream(meshFileName.c_str(), std::ios::in | std::ios::binary);
//
//	// Read the original asset file name
//	std::string origAsset = ReadString();
//
//	// Read the asset format version number
//	unsigned formatVersion = ReadUnsigned();
//
//	// Read VertexBuffer file name
//	std::string vBufferFileName = ReadString();// MeshWriter::sVertexBufferPath + assetName + MeshWriter::sVertexBufferExtension;
//
//	// Read IndexBuffer file name
//	std::string iBufferFileName = ReadString();// MeshWriter::sIndexBufferPath + assetName + MeshWriter::sIndexBufferExtension;
//
//	// Read material set file name
//	std::string MaterialSetFileName = ReadString(); //MeshWriter::sMaterialSetPath + assetName + MeshWriter::sMaterialSetExtension;
//
//	// Close mesh file
//	mInFile->close();
//	delete(mInFile);
//
//	// Parse each component file of the mesh
//	ReadVertexBuffer(vBufferFileName, outVerts, vertLayouts);
//
//	ReadIndexBuffer(iBufferFileName, outIndices, outMatIndices);
//
//	std::vector<std::string> matNames;
//	ReadMaterialSet(MaterialSetFileName, matNames, outMaterials);
//}
//
////void MeshWriter::WriteVertexBuffer(	std::string originalAssetName, uint8_t vertLayout,
////									std::vector<float> poss, std::vector<float> tcs, std::vector<float> norms,
////									std::vector<float> tangents, std::vector<float> binormals)
//void MeshWriter::WriteVertexBuffers(std::string originalAssetName, std::vector<uint8_t>& vertLayouts, std::vector< std::vector<float> >& vertBuffers)
//{
//	// Check for invalid position buffer
//	/*if (poss.size() % 9 != 0)
//	{
//		Print("Error: Number of vertices is not a multiple of 3. Mesh may not be triangulated.\n");
//		return;
//	}*/
//
//	//std::ofstream OutFile;
//	std::string outPath = sAssetMeshPath + sVertexBufferPath + originalAssetName + sVertexBufferExtension;
//	mOutFile = new std::ofstream(outPath.c_str(), std::ios::out | std::ios::binary);
//
//	// Write Format Version #
//	WriteUnsigned(0);
//
//	// Write number of vertex buffers
//	WriteUnsigned(vertBuffers.size());
//
//	// Write all vertex buffers / submeshes into one file
//	for (unsigned k = 0; k < vertBuffers.size(); ++k)
//	{
//		// Int mask for vertex layout
//		WriteUnsigned8(vertLayouts[k]);
//
//		// Total Number of vertices -- might not be needed if already in format
//		unsigned numFloats = vertBuffers[k].size();
//		unsigned numVerts = 0;
//		if (vertLayouts[k] == VertexFormat_MinimalMesh)
//			numVerts = vertBuffers[k].size() / 5; // 3 float pos + 2 float TC
//		else if (vertLayouts[k] == VertexFormat_StdMesh)
//			numVerts = vertBuffers[k].size() / 8; // 3 pos + 2 TC + 3 Norm
//		else if (vertLayouts[k] == VertexFormat_DetailedMesh)
//			numVerts = vertBuffers[k].size() / 11; // 3 pos + 2 TC + 3 Norm + 3 Tangent
//
//		// Write number of floats to go through
//		WriteUnsigned(numFloats);
//
//		// Write each vertex (actually each float) to file
//		for (unsigned i = 0; i < numFloats; ++i)
//		{
//			WriteFloat(vertBuffers[k][i]);
//		}
//
//		// Mark the end of the vertex buffer (also ensure that we are on the correct value)
//		WriteString("EndBuffer");
//	}
//	
//	// Mark end of file
//	WriteString("End");
//
//	mOutFile->close();
//	delete(mOutFile);
//}
//
//void MeshWriter::ReadVertexBuffer(std::string fileName, std::vector< std::vector<float>>& outBuffer, std::vector<uint8_t>& vertLayouts)
//{
//	std::string vertBufferFile = sAssetMeshPath + fileName;
//	mInFile = new std::ifstream(vertBufferFile.c_str(), std::ios::in | std::ios::binary);
//
//	// Read Format Version #
//	unsigned formatVersion = ReadUnsigned();
//
//	// Read number of vertex buffers
//	unsigned numVertBuffers  = ReadUnsigned();
//	outBuffer.resize(numVertBuffers);
//	vertLayouts.resize(numVertBuffers);
//
//	// Write all vertex buffers / submeshes into one file
//	for (unsigned k = 0; k < numVertBuffers; ++k)
//	{
//		// Int mask for vertex layout
//		vertLayouts[k] = ReadUnsigned8();
//
//		// Total Number of vertices -- might not be needed if already in format
//		unsigned numfloats = ReadUnsigned();
//		outBuffer[k].resize(numfloats);
//
//		// Write each vertex (actually each float) to file
//		for (unsigned i = 0; i < numfloats; ++i)
//		{
//			outBuffer[k][i] = ReadFloat();
//		}
//
//		// Mark the end of the vertex buffer (also ensure that we are on the correct value)
//		std::string verifyEnd = ReadString();
//		if (verifyEnd != "EndBuffer")
//		{
//			// SOMETHING BROKE!
//			return;
//		}
//	}
//
//	// Mark end of file
//	std::string verifyEnd = ReadString();
//	if (verifyEnd != "End")
//	{
//		// SOMETHING BROKE!
//		return;
//	}
//
//	mInFile->close();
//	delete(mInFile);
//}
//
//void MeshWriter::WriteIndexBuffers(std::string originalAssetName, std::vector< std::vector<uint32_t> >& indexBuffers, std::vector<unsigned>& materialIndices)
//{
//
//	//std::ofstream OutFile;
//	std::string outPath = sAssetMeshPath + sIndexBufferPath + originalAssetName + sIndexBufferExtension;
//	mOutFile = new std::ofstream(outPath.c_str(), std::ios::out | std::ios::binary);
//
//	// Write Format Version #
//	WriteUnsigned(0);
//
//	// Write number of index buffers
//	unsigned numIndexBuffers = indexBuffers.size();
//	WriteUnsigned(numIndexBuffers);
//
//	// Write all index buffers
//	for (unsigned k = 0; k < indexBuffers.size(); ++k)
//	{
//		// Total Number of vertices
//		unsigned numIndices = indexBuffers[k].size();
//		WriteUnsigned(numIndices);
//
//		// Write the material index to use from the material set
//		WriteUnsigned(materialIndices[k]);
//
//		// Write each index to file
//		for (unsigned i = 0; i < numIndices; ++i)
//		{
//			// Indices
//			WriteUnsigned(indexBuffers[k][i]);
//		}
//
//		// Mark the end of the index buffer (also ensure that we are on the correct value)
//		WriteString("EndBuffer");
//	}
//
//	// Mark end of file
//	WriteString("End");
//
//	mOutFile->close();
//	delete(mOutFile);
//}
//
//void MeshWriter::ReadIndexBuffer(std::string fileName, std::vector<std::vector<uint32_t> >& outBuffer, std::vector<unsigned>& materialIndices)
//{
//	//std::ofstream OutFile;
//	std::string outPath = sAssetMeshPath + fileName;
//	mInFile = new std::ifstream(outPath.c_str(), std::ios::out | std::ios::binary);
//
//	// Write Format Version #
//	unsigned formatVersionNum = ReadUnsigned();
//
//	// Write number of index buffers
//	unsigned numBuffers = ReadUnsigned();
//	outBuffer.resize(numBuffers);
//	materialIndices.resize(numBuffers);
//
//	// Write all index buffers
//	for (unsigned k = 0; k < numBuffers; ++k)
//	{
//		// Total Number of vertices
//		unsigned numIndices = ReadUnsigned();
//		outBuffer[k].resize(numIndices);
//
//		// Read the material index to use from the material set
//		unsigned matIdx = ReadUnsigned();
//		materialIndices[k] = matIdx;
//
//		// Write each index to file
//		for (unsigned i = 0; i < numIndices; ++i)
//		{
//			// Indices
//			outBuffer[k][i] = ReadUnsigned();
//		}
//
//		// Check the end of the index buffer (also ensure that we are on the correct value)
//		std::string verifyEnd = ReadString();
//		if (verifyEnd != "EndBuffer")
//		{
//			// SOMETHING BROKE!
//			return;
//		}
//	}
//
//	// Mark end of file
//	std::string verifyEnd = ReadString();
//	if (verifyEnd != "End")
//	{
//		// SOMETHING BROKE!
//		return;
//	}
//
//	mInFile->close();
//	delete(mInFile);
//}
//
//void MeshWriter::WriteMaterialSet(std::string originalAssetName, std::vector<std::string>& materialList)
//{
//	std::string outPath = sAssetMeshPath + sMaterialSetPath + originalAssetName + sMaterialSetExtension;
//	mOutFile = new std::ofstream(outPath.c_str(), std::ios::out | std::ios::binary);
//
//	// Write Format Version #
//	WriteUnsigned(0);
//
//	// Write number of materials used
//	unsigned numMaterials = materialList.size();
//	WriteUnsigned(numMaterials);
//
//	for (int i = 0; i < numMaterials; ++i)
//	{
//		// Write each material file name
//		std::string matName = ExtractFileNameWithoutFileType(materialList[i]);
//		WriteString(sMaterialsPath + matName + sMaterialsExtension);
//	}
//
//	// Mark end of file
//	WriteString("End");
//	mOutFile->close();
//	delete(mOutFile);
//
//	// Write each material file
//	for (int i = 0; i < numMaterials; ++i)
//	{
//		std::vector<std::string> textures;
//		textures.push_back(materialList[i]);
//		WriteMaterial(originalAssetName, textures);
//	}
//}
//
//void MeshWriter::ReadMaterialSet(std::string fileName, std::vector<std::string>& materialFileNames, std::vector< std::vector<std::string> >& outMaterials)
//{
//	std::string outPath = sAssetMeshPath + fileName;
//	mInFile = new std::ifstream(outPath.c_str(), std::ios::out | std::ios::binary);
//
//	// Read Format Version #
//	unsigned formatVersionNum = ReadUnsigned();
//
//	// Read number of materials used
//	unsigned numMaterials = ReadUnsigned();
//	materialFileNames.resize(numMaterials);
//
//	for (int i = 0; i < numMaterials; ++i)
//	{
//		// Read each material file name
//		materialFileNames[i] = ReadString();
//	}
//
//	// Check end of file
//	std::string verifyEnd = ReadString();
//	if (verifyEnd != "End")
//	{
//		// SOMETHING BROKE!
//		return;
//	}
//	mInFile->close();
//	delete(mInFile);
//
//	// Read each material file
//	outMaterials.resize(numMaterials);
//	for (int i = 0; i < numMaterials; ++i)
//	{
//		ReadMaterial(materialFileNames[i], outMaterials[i]);
//	}
//}
//
//void MeshWriter::WriteMaterial(std::string originalAssetName, std::vector<std::string>& textureNames)
//{
//	std::string textureName = ExtractFileNameWithoutFileType(textureNames[0]);
//	std::string outPath = sAssetMeshPath + sMaterialsPath + textureName + sMaterialsExtension;
// 	mOutFile = new std::ofstream(outPath.c_str(), std::ios::out | std::ios::binary);
//
//	// Write Format Version #
//	WriteUnsigned(0);
//
//	// Write if PBR mat: (0 - no, 1 - yes)
//	WriteUnsigned(0);
//
//	// Write number of textures
//	unsigned numTextures = textureNames.size();
//	WriteUnsigned(numTextures);
//
//	for (int i = 0; i < numTextures; ++i)
//	{
//		WriteString(textureNames[i]);
//	}
//
//	// Mark end of file
//	WriteString("End");
//	mOutFile->close();
//	delete(mOutFile);
//}
//
//void MeshWriter::ReadMaterial(std::string fileName, std::vector<std::string>& textureNames)
//{
//	std::string outPath = sAssetMeshPath + fileName;
//	mInFile = new std::ifstream(outPath.c_str(), std::ios::out | std::ios::binary);
//
//	// Read Format Version #
//	unsigned formatVersionNum = ReadUnsigned();
//
//	// Write if PBR mat: (0 - no, 1 - yes)
//	unsigned isPBR = ReadUnsigned();
//
//	// Write number of textures
//	unsigned numTextures = ReadUnsigned();
//	textureNames.resize(numTextures);
//
//	// ToDo: add support for more than albedo texture (normal, glow, specular)
//	for (int i = 0; i < numTextures; ++i)
//	{
//		textureNames[i] = ReadString();
//	}
//
//	// Check end of file
//	std::string verifyEnd = ReadString();
//	if (verifyEnd != "End")
//	{
//		// SOMETHING BROKE!
//		return;
//	}
//	mInFile->close();
//	delete(mInFile);
//}
//
//void MeshWriter::WriteInt(int data)
//{
//	mOutFile->write((char*)&data, sizeof(int));
//}
//
//void MeshWriter::WriteFloat(float data)
//{
//	mOutFile->write((char*)&data, sizeof(float));
//}
//
//void MeshWriter::WriteDouble(double data)
//{
//	mOutFile->write((char*)&data, sizeof(double));
//}
//
//void MeshWriter::WriteString(const std::string& data)
//{
//	mOutFile->write(data.c_str(), data.length());
//	char terminator = '\0';
//	mOutFile->write(&terminator, sizeof(char));
//}
//
//void MeshWriter::WriteChar(char data)
//{
//	mOutFile->write(&data, sizeof(char));
//}
//
//void MeshWriter::WriteUnsigned(unsigned int data)
//{
//	mOutFile->write((char*)&data, sizeof(unsigned int));
//}
//
//void MeshWriter::WriteUnsigned8(uint8_t data)
//{
//	mOutFile->write((char*)&data, sizeof(uint8_t));
//}
//
//int MeshWriter::ReadInt()
//{
//	int val;
//	mInFile->read((char*)&val, sizeof(int));
//	return val;
//}
//
//float MeshWriter::ReadFloat()
//{
//	float val;
//	mInFile->read((char*)&val, sizeof(float));
//	return val;
//}
//
//double MeshWriter::ReadDouble()
//{
//	double val;
//	mInFile->read((char*)&val, sizeof(double));
//	return val;
//}
//
//std::string MeshWriter::ReadString()
//{
//	std::string string;
//	std::getline(*mInFile, string, '\0');
//	return string;
//}
//
//char MeshWriter::ReadChar()
//{
//	char val;
//	mInFile->read(&val, sizeof(char));
//	return val;
//}
//
//unsigned int MeshWriter::ReadUnsigned()
//{
//	unsigned int val;
//	mInFile->read((char*)&val, sizeof(unsigned int));
//	return val;
//}
//
//uint8_t MeshWriter::ReadUnsigned8()
//{
//	uint8_t val;
//	mInFile->read((char*)&val, sizeof(uint8_t));
//	return val;
//}
//
//std::string MeshWriter::ExtractFileNameWithoutFileType(std::string fileName)
//{
//	int i = fileName.length() - 1;
//	for (; i > 0; --i)
//	{
//		if (fileName[i] == '/' || fileName[i] == '\\')
//			break;
//	}
//
//	std::string localFile = fileName.substr(i);
//	for (int k = 0; k < localFile.size(); ++k)
//	{
//		if (localFile[k] == '.')
//		{
//			localFile = localFile.substr(0, k);
//			break;
//		}
//	}
//
//	return localFile;
//}
