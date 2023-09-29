#include "../hooks.h"

float __fastcall h::hkGetAlphaModulation(void* ecx, void* edx) {

	static auto original = detour::getAlphaModulation.GetOriginal<decltype(&h::hkGetAlphaModulation)>();

	IMaterial* pMaterial = static_cast<IMaterial*>(ecx);
	if (!pMaterial || pMaterial->IsErrorMaterial())
		return original( ecx, edx);

	const char* szGroup = pMaterial->GetTextureGroupName();

	if (strstr(szGroup, XorStr("World textures")))
		return cfg::misc::flNightmodeColor[3];
	else if (strstr(szGroup, XorStr("StaticProp textures")))
		return cfg::misc::flPropColor[3];
	else if (strstr(szGroup, XorStr("SkyBox textures")))
		return cfg::misc::flSkyboxColor[3];

	return original(ecx, edx);
}