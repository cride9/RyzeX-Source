#include "../hooks.h"
#include "../../globals.h"
#include "../../SDK/Menu/config.h"

float __fastcall h::hkGetViewModelFov(void* ecx, void* edx) {

	static auto original = detour::getViewmodelFov.GetOriginal<decltype(&h::hkGetViewModelFov)>();

	return cfg::misc::iViewModelFov;
}