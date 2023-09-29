#include "hooks.h"
#include <stdexcept>
#include "../globals.h"
#include "../SDK/Menu/gui.h"
#include "../SDK/DataTyes/Color.h"
#include "../SDK/Menu/gui.h"
#include "../../Dependecies/ImGui/imgui_impl_win32.h"
#include "../Features/Misc/misc.h"
#include "../Features/Visuals/ESP.h"
#include "../Features/Misc/Playerlist.h"
#include "../Features/Rage/Animations/Lagcompensation.h"

void h::SetupHooks() {

	if (MH_init() != MH_OK)
		throw std::runtime_error(XorStr("Failed to initialize minhook"));

	gameevent.Init();

	// ImGui Hooks
	HookTable(detour::D3DReset, menu::device, table::D3DReset, &Reset);
	HookTable(detour::D3DEndScene, menu::device, table::D3DEndScene, &EndScene);

	// CSGO vtable Hooks
	HookTable(detour::allocKeyValues, i::KeyValuesSystem, table::allocKeyValues, &hkAllocKeyValuesMemory);
	HookTable(detour::createMove, i::ClientDll, table::createMove, &hkCreateMoveProxy);
	HookTable(detour::frameStageNotify, i::ClientDll, table::frameStageNotify, &hkFrameStageNotify);
	HookTable(detour::overrideView, i::ClientMode, table::overrideView, &hkOverrideView);
	HookTable(detour::listLeaves, i::EngineClient->GetBSPTreeQuery(), table::listLeaves, &hkListLeavesInBox);
	HookTable(detour::paintTraverse, i::Panel, table::paintTraverse, &hkPaintTraverse);
	HookTable(detour::drawModel, i::StudioRender, table::drawModel, &hkDrawModel);
	HookTable(detour::drawModelMdl, i::ModelRender, table::drawModelMdl, &hkDrawModelMDL);
	HookTable(detour::lockCursor, i::Surface, table::lockCursor, &hkLockCursor);
	HookTable(detour::runCommand, i::Prediction, table::runCommand, &hkRunCommand);
	HookTable(detour::getViewmodelFov, i::ClientMode, table::getViewmodelFov, &hkGetViewModelFov);
	HookTable(detour::writeUserCmd, i::ClientDll, table::writeUserCmd, &hkWriteUserCmdDeltaToBuffer);
	HookTable(detour::doPostScreenEffects, i::ClientMode, table::doPostScreenEffects, &hkDoPostScreenEffect);
	HookTable(detour::inPrediction, i::Prediction, table::inPrediction, &hkInPrediction);
	//HookTable(detour::renderView, (**reinterpret_cast<void***>(MEM::FindPattern(CLIENT_DLL, XorStr("8B 0D ? ? ? ? FF 75 0C 8B 45 08")) + 0x2)), 6, &hkRenderView);
	//HookTable(detour::fireEvent, i::GameEvent, table::fireEvent, &hkFireEvent);

	HookTable( detour::traceRay, i::EngineTrace, table::traceRay, &hkTraceRay );

	// Signature hooks
	HookSignature(detour::clMove, ENGINE_DLL, "55 8B EC 81 EC ? ? ? ? 53 56 8A F9", &hkClMove);
	HookSignature(detour::buildTransform, CLIENT_DLL, "55 8B EC 83 E4 F0 81 ? ? ? ? ? 56 57 8B F9 8B ? ? ? ? ? 89 7C 24 28", &hkBuildTransformation);
	HookSignature(detour::calculateView, CLIENT_DLL, "55 8B EC 83 EC 14 53 56 57 FF 75 18", &hkCalculateView);
	HookSignature(detour::sequenceChange, CLIENT_DLL, "55 8B EC 51 53 8B 5D 08 56 8B F1 57 85", &hkCheckForSequenceChange);
	HookSignature(detour::procedrualFoot, CLIENT_DLL, "55 8B EC 83 E4 F0 83 EC 78 56 8B F1 57 8B", &hkDoProceduralFootPlant);
	HookSignature(detour::isHltv, ENGINE_DLL, "A1 ? ? ? ? 80 ? ? ? ? ? ? 75 0C", &hkIsHltv);
	HookSignature(detour::modifyEyePosition, CLIENT_DLL, "55 8B EC 83 E4 F8 83 EC 70 56 57 8B F9 89 7C 24 14", &hkModifyEyePosition);
	HookSignature(detour::skipAnimation, CLIENT_DLL, "57 8B F9 8B 07 8B 80 ? ? ? ? FF D0 84 C0 75 02", &hkShouldSkipAnimationFrame);
	HookSignature(detour::blendingRules, CLIENT_DLL, "55 8B EC 83 E4 F0 B8 ? ? ? ? E8 ? ? ? ? 56 8B 75 08 57 8B F9 85 F6", &hkStandardBlendingRules);
	HookSignature(detour::clientAnimation, CLIENT_DLL, "55 8B EC 51 56 8B F1 80 BE ? ? ? ? ? 74", &hkUpdateClientSideAnimations);
	HookSignature(detour::setupBones, CLIENT_DLL, "55 8B EC 83 E4 F0 B8 D8", &hkSetupBones);
	HookSignature(detour::physicsSimulate, CLIENT_DLL, "56 8B F1 8B 8E ? ? ? ? 83 F9 FF 74 23", &hkPhysicsSimulate);
	HookSignature(detour::interpolateEntites, CLIENT_DLL, "55 8B EC 83 EC 1C 8B 0D ? ? ? ? 53 56 57", &hkInterpolateServerEntites);
	//HookSignature(detour::isFollowingEntity, CLIENT_DLL, "F6 ? ? ? ? ? ? 74 31 80", &hkIsFollowingEntity);
	//HookSignature(detour::clampBonesInBBox, CLIENT_DLL, "55 8B EC 83 E4 F8 83 EC 70 56 57 8B F9 89 7C 24 38", &hkClampBonesInBBox);
	HookSignature(detour::getColorModulation, MATERIALSYSTEM_DLL, "55 8B EC 83 EC ? 56 8B F1 8A 46", &hkGetColorModulation);
	HookSignature(detour::getAlphaModulation, MATERIALSYSTEM_DLL, "56 8B F1 8A 46 20 C0 E8 02 A8 01 75 0B 6A 00 6A 00 6A 00 E8 ? ? ? ? 80 7E 22 05 76 0E", &hkGetAlphaModulation);
	HookSignature(detour::setText, CLIENT_DLL, "55 8B EC 8B 45 08 85 C0 53", &hkSetText);
	HookSignature(detour::levelShutDown, CLIENT_DLL, "55 8B EC 83 E4 F8 83 EC 30 C6", &hkLevelShutDownClient);
	HookSignature(detour::setupAliveLoop, CLIENT_DLL, "55 8B EC 51 56 8B 71 60 83 BE 9C 29 ? ? ? 0F 84 93 ? ? ? 8B", &hkSetupAliveLoop);
	HookSignature(detour::verifyReturnAddress, CLIENT_DLL, "55 8B EC 56 8B F1 33 C0 57 8B 7D 08 8B 8E", &hkVerifyReturnAddress);
	HookSignature(detour::animationState, CLIENT_DLL, "55 8B EC 83 E4 F8 83 EC 18 56 57 8B F9 F3", &hkUpdateAnimationState);
	//HookSignature(detour::baseInterpolatePart1, CLIENT_DLL, "55 8B EC 51 8B 45 14 56", &hkBaseInterpolatePart1);
	//HookSignature(detour::setupMovement, CLIENT_DLL, "55 8B EC 83 E4 F8 81 ? ? ? ? ? 56 57 8B ? ? ? ? ? 8B F1", &hkSetUpMovement);
	HookSignature(detour::extraBoneProcessing, CLIENT_DLL, "55 8B EC 83 E4 F8 81 EC ? ? ? ? 53 56 8B F1 57 89 74 24 1C", &hkDoExtraBoneProcessing);
	HookSignature(detour::processInterpolatedList, CLIENT_DLL, "0F B7 05 ? ? ? ? 3D FF FF 00 00 74 3F 56 57", &hkProcessInterpolatedList);
	HookSignature(detour::drawViewmodel, CLIENT_DLL, "55 8B EC 51 57 E8", &hkShouldDrawViewmodel);
	HookSignature(detour::drawModelAnimating, CLIENT_DLL, "55 8B EC 56 8B F1 80 ? ? ? ? ? ? 75 07 33", &hkDrawModelAnimating);
	HookSignature(detour::isDepthOfFieldEnabled, CLIENT_DLL, "8B ? ? ? ? ? 56 8B 01 FF 50 34 8B F0 85 F6 75", &hkIsDepthOfFieldEnabled);

	menu::DestroyDirectX();

	util::Print(XorStr("Hooks initialized"));
}

void h::HookNetChannel(INetChannel* pNetChannel) {

	// netchannel pointer
	if (!pNetChannel)
		return;

	// @note: doesnt need rehook cuz detours here
	if (pNetChannel != nullptr)
	{
		if (!detour::processPacket.IsHooked())
			h::HookTable(detour::processPacket, pNetChannel, table::processPacket, &h::hkProcessPacket);

		if (!detour::sendNetMsg.IsHooked())
			h::HookTable(detour::sendNetMsg, pNetChannel, table::sendNetMsg, &h::hkSendNetMsg);

		//if ( !detour::voiceData.IsHooked( ) )
		//	HookSignature( detour::voiceData, "engine.dll", "55 8B EC 83 E4 F8 A1 ? ? ? ? 81 EC 84 01 00", &hkSVCMsg_VoiceData );

		//if ( !detour::setChoked.IsHooked( ) )
		//	h::HookTable(detour::setChoked, pNetChannel, table::setChoked, &h::hkSetChoked);

		if (!detour::sendDatagram.IsHooked())
			h::HookTable(detour::sendDatagram, pNetChannel, table::sendDatagram, &h::hkSendDatagram);
	}
}

void h::HookClientState() {

	static const auto clientStateHookable = reinterpret_cast<void*>(reinterpret_cast<uintptr_t>(i::ClientState) + 0x8);

	if (clientStateHookable != nullptr)
	{
		oldpacketStart = util::GetVFunc(clientStateHookable, table::packetStart);
		oldpacketEnd = util::GetVFunc(clientStateHookable, table::packetEnd);
		oldtemptEntities = util::GetVFunc(clientStateHookable, table::temptEntities);

		// PacketStart Detour
		if (!detour::packetStart.IsHooked())
			h::HookTable(detour::packetStart, clientStateHookable, table::packetStart, &h::hkPacketStart);

		// PacketEnd Detour
		if (!detour::packetEnd.IsHooked())
			h::HookTable(detour::packetEnd, clientStateHookable, table::packetEnd, &h::hkPacketEnd);

		if (!detour::temptEntities.IsHooked())
			h::HookTable(detour::temptEntities, clientStateHookable, table::temptEntities, &h::hkTemptEntities);
	}
}

void h::UnHookClientState() {

	static const auto clientStateHookable = reinterpret_cast<void*>(reinterpret_cast<uintptr_t>(i::ClientState) + 0x8);

	if (clientStateHookable != nullptr)
	{
		// PacketStart Detour
		if (detour::packetStart.IsHooked()) {
			MH_DisableHook(oldpacketStart);
			MH_RemoveHook(oldpacketStart);
			//detour::packetStart = CDetourHook();
		}
			
		// PacketEnd Detour
		if (detour::packetEnd.IsHooked()) {
			MH_DisableHook(oldpacketEnd);
			MH_RemoveHook(oldpacketEnd);
			//detour::packetEnd = CDetourHook();
		}

		if (detour::temptEntities.IsHooked()) {
			MH_DisableHook(oldtemptEntities);
			MH_RemoveHook(oldtemptEntities);
			//detour::temptEntities = CDetourHook();
		}
	}
}

void h::HookEntites() {

	for (int i = 0; i < i::GlobalVars->nMaxClients; i++) {

		CBaseEntity* pEntity = playerList::arrPlayers[i].pEntity;

		if (!pEntity) 
			continue;

		if (!detour::estimateAbsVelocity.IsHooked()) {

			vecEstimateAbsVelocityHooks[i] = util::GetVFunc(pEntity, 145);
			h::HookTable(detour::estimateAbsVelocity, pEntity, 145, &h::hkEstimateAbsVelocity);
		}
	}
}

void h::DestroyHooks() {

	gameevent.Destroy();

	MH_DisableHook(MH_ALL_HOOKS);
	MH_RemoveHook(MH_ALL_HOOKS);
	MH_Uninit();
}