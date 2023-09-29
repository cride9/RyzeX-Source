#include "../../hooks.h"
#include "../../../SDK/Entity.h"
#include "../../../globals.h"
#include "../../../Features/Rage/Animations/LocalAnimation.h"
#include "../../../Features/Rage/antiaim.h"
#include "../../../Features/Misc/enginepred.h"

void __fastcall	h::hkUpdateClientSideAnimations(void* entityPointer, void* edx) {

	static auto original = detour::clientAnimation.GetOriginal<decltype(&h::hkUpdateClientSideAnimations)>();

	const auto pEnt = reinterpret_cast<CBaseEntity*>(entityPointer);
	if (!g::pLocal || !pEnt || !pEnt->IsAlive() || pEnt->IsDormant() || (!pEnt->IsEnemy(g::pLocal) && pEnt != g::pLocal))
		return original( entityPointer, edx);

	// like this the client wont fuck up animations since WE DO NOT ALLOW it to happen LOL
	if (g::bAllowAnimations[pEnt->EntIndex()])
		return original( entityPointer, edx);
	else
		return;
}