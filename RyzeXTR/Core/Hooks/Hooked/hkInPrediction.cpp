#include "../hooks.h"

bool __fastcall h::hkInPrediction(void* ecx, void* edx) {

	static auto retSetupBones = reinterpret_cast<uintptr_t>(MEM::FindPattern(CLIENT_DLL, "84 C0 74 0A F3 0F 10 05 ? ? ? ? EB 05"));

	if (reinterpret_cast<uintptr_t>(_ReturnAddress()) == retSetupBones)
		return false;

	return detour::inPrediction.GetOriginal<decltype(&hkInPrediction)>()(ecx, edx);
}