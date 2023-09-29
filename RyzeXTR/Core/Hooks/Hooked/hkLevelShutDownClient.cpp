#include "../hooks.h"
#include "../../Features/Rage/Animations/Lagcompensation.h"
#include "../../Features/Misc/enginepred.h"
#include "../../Features/Networking/networking.h"
#include "../../Features/Rage/Animations/EnemyAnimations.h"
#include "../../Features/Visuals/chams.h"
#include "../../Features/Visuals/ESP.h"
#include "../../Features/Misc/misc.h"
#include "../../Features/Misc/Playerlist.h"
#include "../../Features/Rage/aimbot.h"

void __fastcall h::hkLevelShutDownClient(void* ecx, void* edx) {

	static auto original = detour::levelShutDown.GetOriginal<decltype(&h::hkLevelShutDownClient)>();

	original(ecx, edx);

	auto pLocal = CBaseEntity::GetLocalPlayer();
	if (pLocal && pLocal->IsAlive())
		return;

	/* Clear lagcomp & global player data */
	for (size_t i = 0; i < i::GlobalVars->nMaxClients; i++) {

		auto pLog = &lagcomp.GetLog(i);
		pLog->ClearData();

		g::bAllowAnimations[i] = true;
		g::bSettingUpBones[i] = true;
		playerList::arrPlayers[i] = playerSettings_t();
	}
	/* Clear global pointers */
	g::pCmd = nullptr;
	g::pLocal = nullptr;

	/* Reset networking */
	networking.ResetNetwork();
	packetManager.pCommandList.clear();

	/* Reset prediction and ragebot data */
	prediction.pLastCmd = nullptr;
	aimbot.GetAimbotData().pAimbotTarget = nullptr;
	aimbot.GetAimbotData().pTargetMatrix = nullptr;
	anims.bResolverHandler = std::array<bool, Animations::EResolverHandler::HANDLERCOUNT>();
	misc::exploitInitialized = false;

	/* Clear visuals */
	chams::materials.fill(nullptr);
	visual::vecDormatPosition = std::array<Vector, 65>();
	visual::vecLights.clear();
	
	/* Set last sendpacket pointer to true */
	if (g::bSendPacket)
		*g::bSendPacket = true;

	lagcomp.ClearIncomingSequences();
	/* New server = new client these needs to be rehooked */
	//UnHookClientState();
}