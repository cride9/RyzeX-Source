#include "../../hooks.h"
#include "../../../Features/Rage/Animations/Lagcompensation.h"
#include "../../../Features/Rage/Animations/EnemyAnimations.h"

void __fastcall h::hkSetUpMovement(CAnimState* ecx, void* edx) {

	static auto original = detour::setupMovement.GetOriginal<decltype(&hkSetUpMovement)>();

	return original(ecx, edx);
}