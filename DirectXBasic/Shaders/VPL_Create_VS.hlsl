#ifndef VPL_Create_VS
#define VPL_Create_VS

#include "Constants.hlsl"
#include "Std_Structs.hlsl"
#include "VoxelGI_Structs.hlsl"


VPLCreate_GS_IN main(BasicQuad_VS_In vIn, uint instanceID : SV_InstanceID)
{
	VPLCreate_GS_IN vOut;
	vOut.iPosH = float4(vIn.mPosition, 1.0);
	vOut.iColor = vIn.mTexCoord;
	vOut.iInstanceID = instanceID;
	return vOut;
}

#endif
