#include "../hooks.h"
#include "../../SDK/Menu/config.h"

bool __fastcall h::hkShouldDrawViewmodel(void* ecx, void* edx) {

	static auto original = detour::drawViewmodel.GetOriginal<decltype(&h::hkShouldDrawViewmodel)>();

	return cfg::misc::bOnScopeViewmodel ? true : original(ecx, edx);
}