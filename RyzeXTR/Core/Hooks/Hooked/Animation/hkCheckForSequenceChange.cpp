#include "../../hooks.h"
#include "../../../SDK/Entity.h"

void __fastcall	h::hkCheckForSequenceChange(void* entityPointer, void* edx, void* hdr, int nCurSequence, bool bForceNewSequence, bool bInterpolate) {

	static auto original = detour::sequenceChange.GetOriginal<decltype(&h::hkCheckForSequenceChange)>();

	const auto pEnt = reinterpret_cast<CBaseEntity*>(entityPointer);

	if (pEnt && !pEnt->IsAlive())
		return original( entityPointer, edx, hdr, nCurSequence, bForceNewSequence, bInterpolate);

	return original(entityPointer, edx, hdr, nCurSequence, bForceNewSequence, bInterpolate);
}