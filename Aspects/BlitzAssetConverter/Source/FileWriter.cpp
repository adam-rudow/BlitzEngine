#include <iostream>
#include <fstream>
#include <sys/stat.h>

#include "FileWriter.h"
#include "DebugUtility.h"
#include "RenderUtility.h"

#include <cerrno>
using std::strerror;
using std::cerr;

#pragma optimize("", off)

#define DEBUG_TEXT_FILE 0

#define MAT_MASK_ALBEDO 1 << 0
#define MAT_MASK_NORMAL 1 << 1
#define MAT_MASK_GLOW	1 << 3
#define MAT_MASK_SPEC	1 << 4

#if _ASSET_PROCESSOR
std::string MeshWriter::sAssetMeshPath		= "../../DirectXBasic/Assets/Meshes/";
#else
std::string MeshWriter::sAssetMeshPath		= "Assets/Meshes/";
#endif

std::string MeshWriter::sMeshAssetPath		= "Meshes/";
std::string MeshWriter::sVertexBufferPath	= "VertexBuffers/";
std::string MeshWriter::sIndexBufferPath	= "IndexBuffers/";
std::string MeshWriter::sMaterialSetPath	= "MaterialSets/";
std::string MeshWriter::sMaterialsPath		= "Materials/";

std::string MeshWriter::sMeshAssetExtension		= ".mesh";
std::string MeshWriter::sVertexBufferExtension	= ".vbuf";
std::string MeshWriter::sIndexBufferExtension	= ".ibuf";
std::string MeshWriter::sMaterialSetExtension	= ".matset";
std::string MeshWriter::sMaterialsExtension		= ".material";

MeshWriter* MeshWriter::sInstance = nullptr;

MeshWriter::MeshWriter()
{
	mOutFile = new std::ofstream();
	mInFile  = new std::ifstream();
	/*std::string name = "vertex buffer #0";
	uint8_t nameLen = S_CAST(uint8_t, name.length());
	mOutFile << nameLen;
	mOutFile->write(name.c_str(), name.length());
	mOutFile->write("\0", sizeof(char));*/

	//std::string meshFileName = "TestOutFile_inner.mesh";
	//std::ofstream OutFile;// meshFileName.c_str(), std::ios::out | std::ios::binary);
	//					  //mOutFile->close();
	//OutFile.open(meshFileName.c_str(), std::ios::out);// | std::ios::binary);
	//if (OutFile.fail())
	//{
	//	return;
	//}
	//OutFile.write(meshFileName.c_str(), meshFileName.length());
	////char terminator = '\0';
	////OutFile.write(&terminator, sizeof(char));
	//OutFile.flush();
	//OutFile.close();

	MeshWriter::sInstance = this;
}

MeshWriter::~MeshWriter()
{

}

bool MeshWriter::CreateNewOutFile(const std::string& fullFileName)
{
	if (mOutFile != nullptr)
	{
		if (mOutFile->is_open())
		{
			mOutFile->flush();
			mOutFile->close();
		}
	}
		

	//mOutFile->open(fullFileName.c_str());
	if (!mOutFile->is_open()) // ...else, create new file...
	{
		int OpenMask = std::ios::out | std::ios::trunc;
		static bool bBinaryFile = true;
		if (bBinaryFile)
		{
			OpenMask |= std::ios::binary;
		}
		mOutFile->open(fullFileName.c_str(), OpenMask);
	}

	if (mOutFile->fail())
	{
		std::cout << "File '" << fullFileName << "' failed to open. Failure reason: '" << strerror(errno) << "' \n";
		CloseOutFile();
		return false;
	}

#if DEBUG_TEXT_FILE
	static std::string DebugFileExt = "_DebugWrite";
	std::string DebugFileName = fullFileName;
	DebugFileName = DebugFileName.append(DebugFileExt);
	mOutDebugTextFile.open(DebugFileName.c_str(), std::ios::out | std::ios::trunc);
#endif

	return true;
}

void MeshWriter::CloseOutFile()
{
	if (mOutFile != nullptr)
	{
		mOutFile->flush();
		mOutFile->close();
	}

#if DEBUG_TEXT_FILE
	mOutDebugTextFile.flush();
	mOutDebugTextFile.close();
#endif
}

bool MeshWriter::OpenFileForReading(const std::string& fullFileName)
{
	if (mInFile != nullptr)
	{
		if (mInFile->is_open())
		{
			mInFile->close();
		}
	}

	//mInFile->open(fullFileName.c_str());
	//if (!mInFile->is_open())
	{
		int OpenMask = std::ios::in;
		static bool bBinaryFile = true;
		if (bBinaryFile)
		{
			OpenMask |= std::ios::binary;
		}
		mInFile->open(fullFileName.c_str(), OpenMask);
	}

	if (mInFile->fail())
	{
		std::cout << "File '" << fullFileName << "' failed to open. Failure reason: '" << strerror(errno) << "' \n";
		mInFile->close();
		return false;
	}

#if DEBUG_TEXT_FILE
	if (mOutDebugTextFile.is_open())
	{
		mOutDebugTextFile.close();
	}
	static std::string DebugFileExt = "_DebugRead";
	std::string DebugFileName = fullFileName;
	DebugFileName = DebugFileName.append(DebugFileExt);
	mOutDebugTextFile.open(DebugFileName.c_str(), std::ios::out | std::ios::trunc);
#endif

	return true;
}

void MeshWriter::CloseReadFile()
{
	mInFile->close();

#if DEBUG_TEXT_FILE
	mOutDebugTextFile.close();
#endif
}


void MeshWriter::WriteMeshFile(std::string fileName)
{
	std::string assetName = fileName.substr(0, fileName.size() - 4);
	std::string meshFileName = sAssetMeshPath + sMeshAssetPath + assetName + sMeshAssetExtension;
	//mOutFile = new std::ofstream(meshFileName.c_str(), std::ios::out | std::ios::binary);
	//mOutFile = new std::ofstream(meshFileName.c_str(), std::ios::out);
	//mOutFile->close();
	if (CreateNewOutFile(meshFileName))
	{

		// Write original asset name
		WriteString(fileName);

		// Write Format Version #
		WriteUnsigned(0);

		// Write VertexBuffer file name
		std::string vBufferFileName = MeshWriter::sVertexBufferPath + assetName + MeshWriter::sVertexBufferExtension;
		WriteString(vBufferFileName);

		// Write IndexBuffer file name
		std::string iBufferFileName = MeshWriter::sIndexBufferPath + assetName + MeshWriter::sIndexBufferExtension;
		WriteString(iBufferFileName);

		// Write material set file name
		std::string MaterialSetFileName = MeshWriter::sMaterialSetPath + assetName + MeshWriter::sMaterialSetExtension;
		WriteString(MaterialSetFileName);

		CloseOutFile();
	}

	//OutFile.flush();
	//OutFile.close();
	//delete(mOutFile);
}

void MeshWriter::ReadMeshFile(std::string assetName, MeshComponent* outMesh, std::vector< std::vector<float>>& outVerts, 
		std::vector<uint8_t>& vertLayouts, std::vector< std::vector<uint32_t>>& outIndices, std::vector<unsigned>& outMatIndices, 
		std::vector< std::vector<std::string> >& outMaterials)
{
	// Open mesh file
	std::string meshFileName = MeshWriter::sAssetMeshPath + MeshWriter::sMeshAssetPath + assetName;
	if (!DoesFileExist(meshFileName))
	{
		outMesh = nullptr;
		Print("MeshWriter::ReadMeshFile: Error: file " + meshFileName + " does not exist!");
		return;
	}
	
	//mInFile = new std::ifstream(meshFileName.c_str(), std::ios::in | std::ios::binary);
	OpenFileForReading(meshFileName);

	// Read the original asset file name
	std::string origAsset = ReadString();

	// Read the asset format version number
	unsigned formatVersion = ReadUnsigned();

	// Read VertexBuffer file name
	std::string vBufferFileName = ReadString();// MeshWriter::sVertexBufferPath + assetName + MeshWriter::sVertexBufferExtension;

	// Read IndexBuffer file name
	std::string iBufferFileName = ReadString();// MeshWriter::sIndexBufferPath + assetName + MeshWriter::sIndexBufferExtension;

	// Read material set file name
	std::string MaterialSetFileName = ReadString(); //MeshWriter::sMaterialSetPath + assetName + MeshWriter::sMaterialSetExtension;

	// Close mesh file
	CloseReadFile();

	// Parse each component file of the mesh
	ReadVertexBuffer(vBufferFileName, outVerts, vertLayouts);

	ReadIndexBuffer(iBufferFileName, outIndices, outMatIndices);

	std::vector<std::string> matNames;
	ReadMaterialSet(MaterialSetFileName, matNames, outMaterials);
}

//void MeshWriter::WriteVertexBuffer(	std::string originalAssetName, uint8_t vertLayout,
//									std::vector<float> poss, std::vector<float> tcs, std::vector<float> norms,
//									std::vector<float> tangents, std::vector<float> binormals)
void MeshWriter::WriteVertexBuffers(std::string originalAssetName, std::vector<uint8_t>& vertLayouts, std::vector< std::vector<float> >& vertBuffers, const std::vector< std::pair<Vector3, Vector3> >& minMaxPoints)
{
	// Check for invalid position buffer
	/*if (poss.size() % 9 != 0)
	{
		Print("Error: Number of vertices is not a multiple of 3. Mesh may not be triangulated.\n");
		return;
	}*/

	//std::ofstream OutFile;
	std::string outPath = sAssetMeshPath + sVertexBufferPath + originalAssetName + sVertexBufferExtension;
	//mOutFile = new std::ofstream(outPath.c_str(), std::ios::out | std::ios::binary);
	if (CreateNewOutFile(outPath))
	{

		// Write Format Version #
		WriteUnsigned(0);

		// Write number of vertex buffers
		WriteUnsigned(vertBuffers.size());

		// Write all vertex buffers / submeshes into one file
		for (unsigned k = 0; k < vertBuffers.size(); ++k)
		{
			// Int mask for vertex layout
			WriteUnsigned8(vertLayouts[k]);

			// Total Number of vertices -- might not be needed if already in format
			unsigned numFloats = vertBuffers[k].size();
			unsigned numVerts = 0;
			if (vertLayouts[k] == VertexFormat_MinimalMesh)
				numVerts = vertBuffers[k].size() / 5; // 3 float pos + 2 float TC
			else if (vertLayouts[k] == VertexFormat_StdMesh)
				numVerts = vertBuffers[k].size() / 8; // 3 pos + 2 TC + 3 Norm
			else if (vertLayouts[k] == VertexFormat_DetailedMesh)
				numVerts = vertBuffers[k].size() / 11; // 3 pos + 2 TC + 3 Norm + 3 Tangent

			// Write number of floats to go through
			WriteUnsigned(numFloats);

			// Write each vertex (actually each float) to file
			for (unsigned i = 0; i < numFloats; ++i)
			{
				WriteFloat(vertBuffers[k][i]);
			}

			// Mark the end of the vertex buffer (also ensure that we are on the correct value)
			WriteString("EndBuffer");

			// Write the min/max points (for frustum culling use)
			WriteVector3(minMaxPoints[k].first);
			WriteVector3(minMaxPoints[k].second);
		}

		// Mark end of file
		WriteString("End");

		CloseOutFile();
	}

	//mOutFile->close();
	//delete(mOutFile);
}

void MeshWriter::ReadVertexBuffer(std::string fileName, std::vector< std::vector<float>>& outBuffer, std::vector<uint8_t>& vertLayouts)
{
	std::string vertBufferFile = sAssetMeshPath + fileName;
	//mInFile = new std::ifstream(vertBufferFile.c_str(), std::ios::in | std::ios::binary);
	OpenFileForReading(vertBufferFile);

	// Read Format Version #
	unsigned formatVersion = ReadUnsigned();

	// Read number of vertex buffers
	unsigned numVertBuffers  = ReadUnsigned();
	outBuffer.resize(numVertBuffers);
	vertLayouts.resize(numVertBuffers);

	// Write all vertex buffers / submeshes into one file
	for (unsigned k = 0; k < numVertBuffers; ++k)
	{
		// Int mask for vertex layout
		vertLayouts[k] = ReadUnsigned8();

		// Total Number of vertices -- might not be needed if already in format
		unsigned numfloats = ReadUnsigned();
		outBuffer[k].resize(numfloats);

		// Write each vertex (actually each float) to file
		for (unsigned i = 0; i < numfloats; ++i)
		{
			outBuffer[k][i] = ReadFloat();
		}

		// Mark the end of the vertex buffer (also ensure that we are on the correct value)
		std::string verifyEnd = ReadString();
		if (verifyEnd != "EndBuffer")
		{
			// SOMETHING BROKE!
			return;
		}

		// Get the min/max points of the vertex buffer
		Vector3 minPoint = ReadVector3();
		Vector3 maxPoint = ReadVector3();
	}

	// Mark end of file
	std::string verifyEnd = ReadString();
	if (verifyEnd != "End")
	{
		// SOMETHING BROKE!
		return;
	}

	CloseReadFile();
}

void MeshWriter::WriteIndexBuffers(std::string originalAssetName, std::vector< std::vector<uint32_t> >& indexBuffers, std::vector<unsigned>& materialIndices)
{

	//std::ofstream OutFile;
	std::string outPath = sAssetMeshPath + sIndexBufferPath + originalAssetName + sIndexBufferExtension;
	//mOutFile = new std::ofstream(outPath.c_str(), std::ios::out | std::ios::binary);
	if (CreateNewOutFile(outPath))
	{

		// Write Format Version #
		WriteUnsigned(0);

		// Write number of index buffers
		unsigned numIndexBuffers = indexBuffers.size();
		WriteUnsigned(numIndexBuffers);

		// Write all index buffers
		for (unsigned k = 0; k < indexBuffers.size(); ++k)
		{
			// Total Number of vertices
			unsigned numIndices = indexBuffers[k].size();
			WriteUnsigned(numIndices);

			// Write the material index to use from the material set
			WriteUnsigned(materialIndices[k]);

			// Write each index to file
			for (unsigned i = 0; i < numIndices; ++i)
			{
				// Indices
				WriteUnsigned(indexBuffers[k][i]);
			}
			//mOutFile->write(reinterpret_cast<const char*>(&indexBuffers[k][0]), numIndices*sizeof(unsigned int));

			// Mark the end of the index buffer (also ensure that we are on the correct value)
			WriteString("EndBuffer");
		}

		// Mark end of file
		WriteString("End");

		CloseOutFile();
	}

	//mOutFile->close();
	//delete(mOutFile);
}

void MeshWriter::ReadIndexBuffer(std::string fileName, std::vector<std::vector<uint32_t> >& outBuffer, std::vector<unsigned>& materialIndices)
{
	//std::ofstream OutFile;
	std::string outPath = sAssetMeshPath + fileName;
	//mInFile = new std::ifstream(outPath.c_str(), std::ios::out | std::ios::binary);
	OpenFileForReading(outPath);

	// Write Format Version #
	unsigned formatVersionNum = ReadUnsigned();

	// Write number of index buffers
	unsigned numBuffers = ReadUnsigned();
	outBuffer.resize(numBuffers);
	materialIndices.resize(numBuffers);

	// Write all index buffers
	for (unsigned k = 0; k < numBuffers; ++k)
	{
		// Total Number of vertices
		unsigned numIndices = ReadUnsigned();
		outBuffer[k].resize(numIndices);

		// Read the material index to use from the material set
		unsigned matIdx = ReadUnsigned();
		materialIndices[k] = matIdx;

		// Write each index to file
		for (unsigned i = 0; i < numIndices; ++i)
		{
			// Indices
			outBuffer[k][i] = ReadUnsigned();
		}
		//mInFile->read(reinterpret_cast<char*>(&outBuffer[k][0]), numIndices*sizeof(unsigned int));

		// Check the end of the index buffer (also ensure that we are on the correct value)
		std::string verifyEnd = ReadString();
		if (verifyEnd != "EndBuffer")
		{
			// SOMETHING BROKE!
			return;
		}
	}

	// Mark end of file
	std::string verifyEnd = ReadString();
	if (verifyEnd != "End")
	{
		// SOMETHING BROKE!
		return;
	}

	CloseReadFile();
}

void MeshWriter::WriteMaterialSet(std::string originalAssetName, std::vector< std::vector<std::string> >& materialList)
{
	std::string outPath = sAssetMeshPath + sMaterialSetPath + originalAssetName + sMaterialSetExtension;
	//mOutFile = new std::ofstream(outPath.c_str(), std::ios::out | std::ios::binary);
	if (CreateNewOutFile(outPath))
	{

		// Write Format Version #
		WriteUnsigned(0);

		// Write number of materials used
		unsigned int numMaterials = materialList.size();
		WriteUnsigned(numMaterials);

		for (unsigned int i = 0; i < numMaterials; ++i)
		{
			// Write each material file name
			std::string matName = ExtractFileNameWithoutFileType(materialList[i][0]);
			WriteString(sMaterialsPath + matName + sMaterialsExtension);
		}

		// Mark end of file
		WriteString("End");
		CloseOutFile();

		// Write each material file
		for (unsigned int i = 0; i < numMaterials; ++i)
		{
			//std::vector<std::string> textures;
			//for (int k = 0; k < materialList[i].size(); ++k)
			//	textures.push_back(materialList[i][k]);

			WriteMaterial(originalAssetName, materialList[i]/*textures*/);
		}
	}

	//mOutFile->close();
	//delete(mOutFile);
}

void MeshWriter::ReadMaterialSet(std::string fileName, std::vector<std::string>& materialFileNames, std::vector< std::vector<std::string> >& outMaterials)
{
	std::string outPath = sAssetMeshPath + fileName;
	//mInFile = new std::ifstream(outPath.c_str(), std::ios::out | std::ios::binary);
	OpenFileForReading(outPath);

	// Read Format Version #
	unsigned int formatVersionNum = ReadUnsigned();

	// Read number of materials used
	unsigned int numMaterials = ReadUnsigned();
	materialFileNames.resize(numMaterials);

	for (unsigned int i = 0; i < numMaterials; ++i)
	{
		// Read each material file name
		materialFileNames[i] = ReadString();
	}

	// Check end of file
	std::string verifyEnd = ReadString();
	if (verifyEnd != "End")
	{
		// SOMETHING BROKE!
		return;
	}
	CloseReadFile();

	// Read each material file
	outMaterials.resize(numMaterials);
	for (unsigned int i = 0; i < numMaterials; ++i)
	{
		ReadMaterial(materialFileNames[i], outMaterials[i]);
	}
}

void MeshWriter::WriteMaterial(std::string originalAssetName, std::vector<std::string>& textureNames)
{
	std::string textureName = ExtractFileNameWithoutFileType(textureNames[0]);
	std::string outPath = sAssetMeshPath + sMaterialsPath + textureName + sMaterialsExtension;
 	//mOutFile = new std::ofstream(outPath.c_str(), std::ios::out | std::ios::binary);
	if (CreateNewOutFile(outPath))
	{

		// Write Format Version #
		WriteUnsigned(0);

		// Write if PBR mat: (0 - no, 1 - yes)
		WriteUnsigned(0);

		// Write number of textures
		unsigned int numTextures = textureNames.size();
		WriteUnsigned(numTextures);

		for (unsigned int i = 0; i < numTextures; ++i)
		{
			WriteString(textureNames[i]);
		}

		// Mark end of file
		WriteString("End");
		CloseOutFile();
	}

	//mOutFile->close();
	//delete(mOutFile);
}

void MeshWriter::ReadMaterial(std::string fileName, std::vector<std::string>& textureNames)
{
	std::string outPath = sAssetMeshPath + fileName;
	//mInFile = new std::ifstream(outPath.c_str(), std::ios::out | std::ios::binary);
	OpenFileForReading(outPath);

	// Read Format Version #
	unsigned formatVersionNum = ReadUnsigned();

	// Write if PBR mat: (0 - no, 1 - yes)
	unsigned isPBR = ReadUnsigned();

	// Write number of textures
	unsigned numTextures = ReadUnsigned();
	textureNames.resize(numTextures);

	// ToDo: add support for more than albedo texture (normal, glow, specular)
	for (unsigned i = 0; i < numTextures; ++i)
	{
		textureNames[i] = ReadString();
	}

	// Check end of file
	std::string verifyEnd = ReadString();
	if (verifyEnd != "End")
	{
		// SOMETHING BROKE!
		return;
	}
	CloseReadFile();
}

void MeshWriter::WriteInt(int data)
{
	mOutFile->write((char*)&data, sizeof(int));

#if DEBUG_TEXT_FILE
	std::string dataStr = std::to_string(data);
	mOutDebugTextFile.write(dataStr.c_str(), dataStr.length());
	char terminator = '\0';
	mOutDebugTextFile.write(&terminator, sizeof(char));
#endif
}

void MeshWriter::WriteFloat(float data)
{
	mOutFile->write((char*)&data, sizeof(float));

#if DEBUG_TEXT_FILE
	std::string dataStr = std::to_string(data);
	mOutDebugTextFile.write(dataStr.c_str(), dataStr.length());
	char terminator = '\0';
	mOutDebugTextFile.write(&terminator, sizeof(char));
#endif
}

void MeshWriter::WriteDouble(double data)
{
	mOutFile->write((char*)&data, sizeof(double));

#if DEBUG_TEXT_FILE
	std::string dataStr = std::to_string(data);
	mOutDebugTextFile.write(dataStr.c_str(), dataStr.length());
	char terminator = '\0';
	mOutDebugTextFile.write(&terminator, sizeof(char));
#endif
}

void MeshWriter::WriteString(const std::string& data)
{
	mOutFile->write(data.c_str(), data.length());
	char terminator = '\0';
	mOutFile->write(&terminator, sizeof(char));

#if DEBUG_TEXT_FILE
	mOutDebugTextFile.write(data.c_str(), data.length());
	mOutDebugTextFile.write(&terminator, sizeof(char));
#endif
}

void MeshWriter::WriteChar(char data)
{
	mOutFile->write(&data, sizeof(char));

#if DEBUG_TEXT_FILE
	std::string dataStr(1, data);
	mOutDebugTextFile.write(dataStr.c_str(), dataStr.length());
	char terminator = '\0';
	mOutDebugTextFile.write(&terminator, sizeof(char));
#endif
}

void MeshWriter::WriteUnsigned(unsigned int data)
{
	mOutFile->write(reinterpret_cast<const char*>(&data), sizeof(unsigned int));

#if DEBUG_TEXT_FILE
	std::string dataStr = std::to_string(data);
	mOutDebugTextFile.write(dataStr.c_str(), dataStr.length());
	char terminator = '\0';
	mOutDebugTextFile.write(&terminator, sizeof(char));
#endif
}

void MeshWriter::WriteUnsigned8(uint8_t data)
{
	mOutFile->write((char*)&data, sizeof(uint8_t));

#if DEBUG_TEXT_FILE
	std::string dataStr = std::to_string(data);
	mOutDebugTextFile.write(dataStr.c_str(), dataStr.length());
	char terminator = '\0';
	mOutDebugTextFile.write(&terminator, sizeof(char));
#endif
}

void MeshWriter::WriteVector3(const Vector3& data)
{
	WriteFloat(data.x);
	WriteFloat(data.y);
	WriteFloat(data.z);
}

int MeshWriter::ReadInt()
{
	int val;
	mInFile->read((char*)&val, sizeof(int));

#if DEBUG_TEXT_FILE
	std::string dataStr = std::to_string(val);
	mOutDebugTextFile.write(dataStr.c_str(), dataStr.length());
	char terminator = '\0';
	mOutDebugTextFile.write(&terminator, sizeof(char));
#endif

	return val;
}

float MeshWriter::ReadFloat()
{
	float val;
	mInFile->read((char*)&val, sizeof(float));

#if DEBUG_TEXT_FILE
	std::string dataStr = std::to_string(val);
	mOutDebugTextFile.write(dataStr.c_str(), dataStr.length());
	char terminator = '\0';
	mOutDebugTextFile.write(&terminator, sizeof(char));
#endif

	return val;
}

double MeshWriter::ReadDouble()
{
	double val;
	mInFile->read((char*)&val, sizeof(double));

#if DEBUG_TEXT_FILE
	std::string dataStr = std::to_string(val);
	mOutDebugTextFile.write(dataStr.c_str(), dataStr.length());
	char terminator = '\0';
	mOutDebugTextFile.write(&terminator, sizeof(char));
#endif

	return val;
}

std::string MeshWriter::ReadString()
{
	std::string string;
	std::getline(*mInFile, string, '\0');

#if DEBUG_TEXT_FILE
	mOutDebugTextFile.write(string.c_str(), string.length());
	char terminator = '\0';
	mOutDebugTextFile.write(&terminator, sizeof(char));
#endif

	return string;
}

char MeshWriter::ReadChar()
{
	char val;
	mInFile->read(&val, sizeof(char));

#if DEBUG_TEXT_FILE
	std::string dataStr(1, val);
	mOutDebugTextFile.write(dataStr.c_str(), dataStr.length());
	char terminator = '\0';
	mOutDebugTextFile.write(&terminator, sizeof(char));

#endif

	return val;
}

unsigned int MeshWriter::ReadUnsigned()
{
	unsigned int val;
	mInFile->read(reinterpret_cast<char*>(&val), sizeof(unsigned int));

#if DEBUG_TEXT_FILE
	std::string dataStr = std::to_string(val);
	mOutDebugTextFile.write(dataStr.c_str(), dataStr.length());
	char terminator = '\0';
	mOutDebugTextFile.write(&terminator, sizeof(char));
#endif

	return val;
}

uint8_t MeshWriter::ReadUnsigned8()
{
	uint8_t val;
	mInFile->read((char*)&val, sizeof(uint8_t));

#if DEBUG_TEXT_FILE
	std::string dataStr = std::to_string(val);
	mOutDebugTextFile.write(dataStr.c_str(), dataStr.length());
	char terminator = '\0';
	mOutDebugTextFile.write(&terminator, sizeof(char));
#endif

	return val;
}
Vector3 MeshWriter::ReadVector3()
{
	Vector3 val;
	val.x = ReadFloat();
	val.y = ReadFloat();
	val.z = ReadFloat();
	return val;
}

std::string MeshWriter::ExtractFileNameWithoutFileType(std::string fileName)
{
	int i = fileName.length() - 1;
	for (; i > 0; --i)
	{
		if (fileName[i] == '/' || fileName[i] == '\\')
			break;
	}

	std::string localFile = fileName.substr(i);
	for (unsigned int k = 0; k < localFile.size(); ++k)
	{
		if (localFile[k] == '.')
		{
			localFile = localFile.substr(0, k);
			break;
		}
	}

	return localFile;
}

bool MeshWriter::DoesFileExist(const std::string& filePath) const
{
	struct stat buf;
	if (stat(filePath.c_str(), &buf) != -1)
	{
		return true;
	}
	return false;
}

#pragma optimize("", on)
