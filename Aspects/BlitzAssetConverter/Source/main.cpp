#include <iostream>
#include <fstream>

#include "AssetLoader.h"
#include "FileWriter.h"
#include "DebugUtility.h"

//enum DrawPassType
//{
//	DrawPass_Default,
//	DrawPass_DepthOnly,
//	DrawPass_Voxelization
//};
//
//enum DrawCallType
//{
//	VBUF_INDBUF,
//	VBUF,
//	COMPUTE,
//	VBUF_INDIRECT,
//};
//
//enum DrawOrder
//{
//	DrawOrder_First = 1,
//	DrawOrder_Default = 2,
//	DrawOrder_Last = 4
//};

int main(int argc, char* argv[])
{
	std::cout << "Enter the file to be converted to BEAsset v0.1:\n\n";

	char fileName[64];
	std::cin.getline(fileName, 256);
	std::string file(fileName);

	MeshWriter mw;
	AssetLoader loader;

	if (loader.ConvertMesh(file))
		Print("\nAsset finished loading, you may safely exit the program.\n\n");
	else
		Print("\nAsset failed to load.\n\n");

	system("Pause");
	return 0;
}