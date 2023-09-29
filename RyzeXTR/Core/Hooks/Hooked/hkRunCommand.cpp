#include "../hooks.h"
#include "../../SDK/Entity.h"
#include "../../globals.h"
#include "../../Features/Rage/Animations/LocalAnimation.h"
#include "../../Features/Networking/networking.h"
#include "../../Features/Misc/misc.h"
#include "../../Features/Rage/exploits.h"

void __fastcall h::hkRunCommand(void* ecx, void* edx, CBaseEntity* pEnt, CUserCmd* pCmd, IMoveHelper* pMovehelper) {

	static auto original = detour::runCommand.GetOriginal<decltype(&h::hkRunCommand)>();

	if (!g::pCmd || pEnt != g::pLocal)
		return original( ecx, edx, pEnt, pCmd, pMovehelper);

	i::MoveHelper = pMovehelper;

	if (pCmd->iTickCount > i::GlobalVars->iTickCount + (int)(1.0f / i::GlobalVars->flIntervalPerTick)) {

		pCmd->bHasBeenPredicted = true;
		return;
	}

	original(ecx, edx, pEnt, pCmd, pMovehelper);
	misc::RevolverRunCommand(pEnt);
}