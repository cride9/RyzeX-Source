#include "../../hooks.h"
#include "../../../SDK/Entity.h"
#include "../../../globals.h"

void __fastcall h::hkCalculateView(void* entityPointer, void* edx, Vector& eyeOrigin, Vector& eyeAngles, float& zNear, float& zFar, float& iDebugFov) {

	static auto original = detour::calculateView.GetOriginal<decltype(&h::hkCalculateView)>();

	const auto pEnt = reinterpret_cast<CBaseEntity*>(entityPointer);

	if (pEnt != g::pLocal)
		return original( entityPointer, edx, eyeOrigin, eyeAngles, zNear, zFar, iDebugFov);

	if (!pEnt->IsAlive())
		return original(entityPointer, edx, eyeOrigin, eyeAngles, zNear, zFar, iDebugFov);

	const auto backup = pEnt->UseNewAnimationState();

	pEnt->UseNewAnimationState() = false;

	original(entityPointer, edx, eyeOrigin, eyeAngles, zNear, zFar, iDebugFov);

	pEnt->UseNewAnimationState() = backup;
}