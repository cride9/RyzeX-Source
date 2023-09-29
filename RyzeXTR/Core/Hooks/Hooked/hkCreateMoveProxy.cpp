#include "../hooks.h"
#include "../../SDK/Entity.h"
#include "../../globals.h"
#include "../../SDK/math.h"
#include "../../SDK/Menu/gui.h"
#include "../../Features/Misc/enginepred.h"
#include "../../Features/Misc/misc.h"
#include "../../Features/Misc/Playerlist.h"
#include "../../Features/Rage/antiaim.h"
#include "../../Features/Rage/Animations/LocalAnimation.h"
#include "../../Features/Rage/exploits.h"
#include "../../Features/Networking/networking.h"
#include "../../Features/Rage/Animations/Lagcompensation.h"
#include "../../Features/Rage/Animations/EnemyAnimations.h"
#include "../../SDK/InputSystem.h"
#include <shlobj.h>
#include "../../Features/Changers/SkinChanger.h"
#include "../../Features/Rage/aimbot.h"
#include "../../Lua/Lua.h"
#include "../../Features/Visuals/GrenadePredict.h"
#include "../../Features/Misc/Vizualizer.h"

static void __stdcall CreateMove(int nSequenceNumber, float flInputSampleFrametime, bool bIsActive, bool& bSendPacket) {

	static auto original = detour::createMove.GetOriginal<decltype(&h::hkCreateMoveProxy)>();

	// call original first so our movement and other stuff will be sent normally
	CUserCmd* pCmd = i::Input->GetUserCmd(nSequenceNumber);
	CVerifiedUserCmd* pVerifiedCmd = i::Input->GetVerifiedCmd(nSequenceNumber);

	original(i::ClientDll, 0, nSequenceNumber, flInputSampleFrametime, bIsActive);

	if (!pCmd || !pVerifiedCmd || !bIsActive)
		return;

	bSendPacket = true;
	g::bSendPacket = &bSendPacket;
	CBaseEntity* pLocal = g::pLocal = static_cast<CBaseEntity*>(i::EntityList->GetClientEntity(i::EngineClient->GetLocalPlayer()));
	g::pCmd = pCmd;
	g::vecEyePosition = pLocal->GetEyePosition(false);
	skinChanger.iItemDefinitionIndex = pLocal->GetWeapon() ? pLocal->GetWeapon()->GetItemDefinitionIndex() : 0;
	skinChanger.bIsKnife = pLocal->GetWeapon() ? pLocal->GetWeapon()->IsKnife() : false;
	lagcomp.StartLagcompensation(pLocal);

	Vector oldViewAngle = g::vecOriginalViewAngle = pCmd->angViewPoint;

	misc::CreateMove(pCmd, oldViewAngle, bSendPacket);
	playerList::InitializePlayerList(pLocal);

	if (i::ClientState->nChokedCommands >= 14 || exploits::bIsCurrentlyCharging)
		bSendPacket = true;

	vizualizer::Draw(pLocal);
	prediction.SaveNetvars(pCmd->iCommandNumber, pLocal);

	bool bShooting = false;
	prediction.Start(pCmd, pLocal, nSequenceNumber);
	{
		if (i::ClientState->iDeltaTick > 0 && !exploits::bIsShiftingTicks)
			i::Prediction->Update(i::ClientState->iDeltaTick, i::ClientState->iDeltaTick > 0, i::ClientState->iLastCommandAck, i::ClientState->iLastOutgoingCommand + i::ClientState->nChokedCommands);
		
		grenadePrediction::Run();
		grenadePrediction::AutoThrow( pCmd );

		misc::BoostMovement(pCmd);
		g::vecEyePosition = pLocal->GetEyePosition(false);
		localAnim->CopyPlayerAnimationData(false, pLocal);
		localAnim->SetupShootPosition(pLocal );

		antiaim::AntiAim(pCmd, bSendPacket);

		aimbot.CreateMove(pCmd, pLocal);

		exploits::HandleDoubleTap( bSendPacket, pCmd );
		exploits::HandleHideShots(bSendPacket, pCmd);
		exploits::HandleBreakLagcomp(pCmd);
		misc::IdealTick(pCmd, pLocal);
		misc::AutoPistol(pCmd, pLocal);
		antiaim::InvertOnShoot(pCmd);

		if (pLocal->CanShoot(pLocal->GetWeapon()) && pCmd->iButtons & IN_ATTACK)
			bShooting = true;
	}
	prediction.End(pCmd, pLocal);
	
	aimbot.PostPrediction(pCmd, bSendPacket);
	prediction.RestoreNetvars( pCmd->iCommandNumber, pLocal);

	misc::MovementFix(pCmd, g::vecOriginalViewAngle);

	INetChannel* pNetChannel = i::ClientState->pNetChannel;

	h::HookNetChannel(pNetChannel);
	h::HookClientState();

	if ( cfg::misc::bFakePing && pNetChannel)
		lagcomp.UpdateIncomingSequences( pNetChannel );
	else
		lagcomp.ClearIncomingSequences( );

	if (bSendPacket)
		packetManager.pCommandList.emplace_back(pCmd->iCommandNumber);

	// re-verify the command after running lua callbacks
	if ( i::ClientState->nChokedCommands >= 14 )
		bSendPacket = true;

	pCmd->angViewPoint.Normalize();
	pCmd->angViewPoint.Clamp();

	pVerifiedCmd->userCmd = *pCmd;
	pVerifiedCmd->uHashCRC = pCmd->GetChecksum();

	if (bSendPacket && !antiaim::bHideFlick)
		localAnim->vecViewAngle = pVerifiedCmd->userCmd.angViewPoint;

	localAnim->OnCreateMove(bSendPacket, pLocal);
	lagcomp.FinishLagcompensation(pLocal);
	anims.ResolverLogic();
}

__declspec(naked) void __fastcall h::hkCreateMoveProxy(IBaseClientDLL* thisptr, int edx, int nSequenceNumber, float flInputSampleFrametime, bool bIsActive)
{
	__asm
	{
		push	ebp
		mov		ebp, esp; // store the stack
		push	ebx; // bSendPacket
		push	esp; // restore the stack
		push	dword ptr[bIsActive]; // ebp + 16
		push	dword ptr[flInputSampleFrametime]; // ebp + 12
		push	dword ptr[nSequenceNumber]; // ebp + 8
		call	CreateMove // call our function
			pop		ebx
			pop		ebp
			retn	0Ch
	}
}