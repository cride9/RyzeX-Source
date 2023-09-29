#include "../../hooks.h"
#include "../../../SDK/Entity.h"

void __fastcall h::hkModifyEyePosition(void* animstatePointer, void* edx, Vector& unused1) 
{
	return;

	static auto original = detour::modifyEyePosition.GetOriginal<decltype(&h::hkModifyEyePosition)>();

	const auto pAnimstate = reinterpret_cast<CAnimState*>(animstatePointer);
	if (!pAnimstate || !pAnimstate->pEntity || !pAnimstate->pEntity->IsAlive())
		return original(animstatePointer, edx, unused1);

	pAnimstate->bSmoothHeightValid = false;

	return original(animstatePointer, edx, unused1);
}