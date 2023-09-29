#include "../../hooks.h"
#include "../../../SDK/Entity.h"
#include "../../../Features/Rage/Animations/LocalAnimation.h"
#include "../../../Features/Rage/Animations/Lagcompensation.h"

void __vectorcall h::hkUpdateAnimationState(CAnimState* pAnimstate, void* edx, float z, float y, float x, void* unknown1) {

	static auto original = detour::animationState.GetOriginal<decltype(&h::hkUpdateAnimationState)>();

	const auto pEnt = pAnimstate->pEntity;

	if (!pEnt || !pEnt->IsAlive() || !pEnt->IsPlayer() || pEnt == g::pLocal)
		return original(pAnimstate, edx, z, y, x, unknown1);

	auto pLog = &lagcomp.GetLog(pEnt->EntIndex());
	if (!pLog || pLog->pRecord.empty())
		return original(pAnimstate, edx, z, y, x, unknown1);

	return original(pAnimstate, edx, z, y, x, unknown1);
}