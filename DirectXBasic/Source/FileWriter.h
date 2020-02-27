//#pragma once
//#include <string> 
//#include <fstream>
//#include <vector>
//
//class MeshComponent;
//
//class MeshWriter
//{
//public:
//	MeshWriter();
//	~MeshWriter();
//
//	void WriteMeshFile(std::string assetName);
//	void ReadMeshFile(std::string assetName, MeshComponent* outMesh, 
//		std::vector< std::vector<float>>& outVerts, 
//		std::vector<uint8_t>& vertLayouts, 
//		std::vector< std::vector<uint32_t>>& outIndices,
//		std::vector<unsigned>& outMatIndices,
//		std::vector< std::vector<std::string> >& outMaterials);
//
//	void WriteVertexBuffers(std::string originalAssetName, std::vector<uint8_t>& vertLayouts, std::vector< std::vector<float> >& vertBuffers);
//	void ReadVertexBuffer(std::string fileName, std::vector< std::vector<float>>& outBuffer, std::vector<uint8_t>& vertLayouts);
//
//	void WriteIndexBuffers(std::string originalAssetName, std::vector< std::vector<uint32_t> >& indexBuffers, std::vector<unsigned>& materialIndices);
//	void ReadIndexBuffer(std::string fileName, std::vector<std::vector<uint32_t> >& outBuffer, std::vector<unsigned>& materialIndices);
//
//	void WriteMaterialSet(std::string originalAssetName, std::vector<std::string>& materialList);
//	void ReadMaterialSet(std::string fileName, std::vector<std::string>& materialFileNames, std::vector< std::vector<std::string> >& outMaterials);
//
//	void WriteMaterial(std::string originalAssetName, std::vector<std::string>& textureNames);
//	void ReadMaterial(std::string fileName, std::vector<std::string>& textureNames);
//
//	static std::string sAssetMeshPath;
//
//	static std::string sMeshAssetPath;
//	static std::string sVertexBufferPath;
//	static std::string sIndexBufferPath;
//	static std::string sMaterialSetPath;
//	static std::string sMaterialsPath;
//
//	static std::string sMeshAssetExtension;
//	static std::string sVertexBufferExtension;
//	static std::string sIndexBufferExtension;
//	static std::string sMaterialSetExtension;
//	static std::string sMaterialsExtension;
//
//
//	static MeshWriter* sInstance;
//
//private:
//
//	std::ofstream* mOutFile;
//	std::ifstream* mInFile;
//
//
//	void WriteInt(int data);
//	void WriteFloat(float data);
//	void WriteDouble(double data);
//	void WriteString(const std::string& data);
//	void WriteChar(char data);
//	void WriteUnsigned(unsigned int data);
//	void WriteUnsigned8(uint8_t data);
//
//	int ReadInt();
//	float ReadFloat();
//	double ReadDouble();
//	std::string ReadString();
//	char ReadChar();
//	unsigned int ReadUnsigned();
//	uint8_t ReadUnsigned8();
//
//	void ReadVertexBufferInternal_PosTexCoordNormal(std::vector<float>& outBuffer);
//	void ReadVertexBufferInternal_PosTexCoordNormalTangent(std::vector<float>& outBuffer);
//	std::string ExtractFileNameWithoutFileType(std::string fileName);
//};
//
//