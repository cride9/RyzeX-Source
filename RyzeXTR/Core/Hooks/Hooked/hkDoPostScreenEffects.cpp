#include "../hooks.h"
#include "../../Features/Visuals/ESP.h"

int __fastcall h::hkDoPostScreenEffect(void* thisptr, int edx, CViewSetup* pSetup) {

	static auto original = detour::doPostScreenEffects.GetOriginal<decltype(&h::hkDoPostScreenEffect)>();

	if (i::ClientState->iSignonState != SIGNONSTATE_FULL)
		return original(thisptr, edx, pSetup);

	if (g::pLocal) {
		visual::Glow(g::pLocal);
		visual::WorldLightning(cfg::misc::flLampColors);
	}

	return original(thisptr, edx, pSetup);
}