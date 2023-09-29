#include "../hooks.h"
#include "../../Features/Misc/misc.h"
#include "../../SDK/Menu/config.h"
#include "../../SDK/InputSystem.h"

void __fastcall h::hkOverrideView(void* ecx, void* edx, CViewSetup* pSetup) {

	static auto original = detour::overrideView.GetOriginal<decltype(&h::hkOverrideView)>();

	misc::ThirdPerson();
	misc::MotionBlur(pSetup);

	if (!g::pLocal || !g::pLocal->IsAlive())
		return original(ecx, edx, pSetup);

	if (cfg::misc::bRemovals[3])
		pSetup->flFOV = cfg::misc::iDebugFov;
	else if (!g::pLocal->IsScoped())
		pSetup->flFOV = cfg::misc::iDebugFov;

	if (cfg::misc::bRemovals[2]) {

		Vector vecViewPunch = g::pLocal->GetViewPunch();
		Vector vecAimPunch = g::pLocal->GetAimPunch();

		pSetup->angView[0] -= (vecViewPunch[0] + (vecAimPunch[0] * 2 * 0.4499999f));
		pSetup->angView[1] -= (vecViewPunch[1] + (vecAimPunch[1] * 2 * 0.4499999f));
		pSetup->angView[2] -= (vecViewPunch[2] + (vecAimPunch[2] * 2 * 0.4499999f));
	}

	if (IPT::HandleInput(cfg::antiaim::iFakeDuckKey) && cfg::antiaim::bFakeDuck) {
		pSetup->vecOrigin = g::pLocal->GetAbsOrigin() + i::GameMovement->GetPlayerViewOffset(false);
	}

	original(ecx, edx, pSetup);
}