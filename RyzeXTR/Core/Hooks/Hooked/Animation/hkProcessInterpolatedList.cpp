#include "../../hooks.h"
#include "../../../SDK/Entity.h"

void __fastcall h::hkProcessInterpolatedList(CBaseEntity* pEntity, uintptr_t edx) {

	if (pEntity == g::pLocal)
		return detour::processInterpolatedList.GetOriginal<decltype(&hkProcessInterpolatedList)>()(pEntity, edx);

	static auto retToInterpolation = MEM::FindPattern(CLIENT_DLL, "0F B6 0D ? ? ? ? 84 C0 0F 44") + 0x3;
	**(bool**)(retToInterpolation) = false;

	return detour::processInterpolatedList.GetOriginal<decltype(&hkProcessInterpolatedList)>()(pEntity, edx);
}