#include "../hooks.h"
#include "../../Features/Misc/misc.h"

void __fastcall h::hkGetColorModulation(void* ecx, void* edx, float* r, float* g, float* b) {

	static auto original = detour::getColorModulation.GetOriginal<decltype(&h::hkGetColorModulation)>();
	
	original(ecx, edx, r, g, b);

	static CConVar* r_DrawSpecificStaticProp = i::ConVar->FindVar("r_DrawSpecificStaticProp");
	if (r_DrawSpecificStaticProp->GetInt() != 0)
		r_DrawSpecificStaticProp->SetValue(0);

	IMaterial* pMaterial = reinterpret_cast<IMaterial*>(ecx);
	const char* szName = pMaterial->GetName();
	const char* szGroup = pMaterial->GetTextureGroupName();

	if (strstr(szGroup, XorStr("SkyBox textures"))) {

		*r *= cfg::misc::flSkyboxColor[0];
		*g *= cfg::misc::flSkyboxColor[1];
		*b *= cfg::misc::flSkyboxColor[2];

		misc::SkyboxChanger();
		return;
	}

	if (cfg::misc::bNightmode) {

		if (strstr(szGroup, XorStr("World textures"))) {

			*r *= cfg::misc::flNightmodeColor[0];
			*g *= cfg::misc::flNightmodeColor[1];
			*b *= cfg::misc::flNightmodeColor[2];
		}
		else if (strstr(szGroup, XorStr("StaticProp textures"))) {

			*r *= cfg::misc::flPropColor[0] * 1.2f;
			*g *= cfg::misc::flPropColor[1] * 1.2f;
			*b *= cfg::misc::flPropColor[2] * 1.2f;
		}
	}
}