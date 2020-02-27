#ifndef VPL_CREATE_GS
#define VPL_CREATE_GS

#include "VoxelGI_Structs.hlsl"

[maxvertexcount(3)]
void GS(triangle VPLCreate_GS_IN input[3], inout TriangleStream<VPLCreate_PS_IN> triangleStream)
{
	VPLCreate_PS_IN gOut[3];
	[unroll]
	for (int i = 0; i < 3; ++i)
	{
		gOut[i].iPosH = input[i].iPosH;
		gOut[i].iColor = input[i].iColor;
		gOut[i].iInstanceID = input[i].iInstanceID;
	}

	triangleStream.Append(gOut[0]);
	triangleStream.Append(gOut[1]);
	triangleStream.Append(gOut[2]);

	triangleStream.RestartStrip();
}

#endif
