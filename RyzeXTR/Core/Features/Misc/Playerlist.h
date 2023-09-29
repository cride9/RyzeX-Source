#pragma once
#include "../../SDK/Entity.h"
#include "../../SDK/Menu/gui.h"
#include "../../Features/Misc/misc.h"

enum EPriority : short {

	// default hitscan
	NEUTRAL,
	// scan this player at the beginning
	RAGE,
	// do not scan this player
	FRIEND
};

struct playerSettings_t {

	playerSettings_t() {
		pEntity = nullptr;
		iIndex = -1;
	}
	playerSettings_t(CBaseEntity* _pEntity) {

		pEntity = _pEntity;
		iIndex = pEntity->EntIndex();
		vecOrigin = pEntity->GetVecOrigin();
		vecAngles = pEntity->GetEyeAngles();
		iTeamID = (ETeamID)pEntity->GetTeam();

		bOverrideResolver = false;
		flOverrideYaw = 0;

		bLocalPlayer = pEntity == g::pLocal;
		i::EngineClient->GetPlayerInfo(iIndex, &playerInfo);
	}

	void UpdateData(CBaseEntity* _pEntity) {

		pEntity = _pEntity;
		bLocalPlayer = iIndex == g::pLocal->EntIndex();
		vecOrigin = pEntity->GetVecOrigin();
		vecAngles = pEntity->GetEyeAngles();
		iTeamID = (ETeamID)pEntity->GetTeam();
		i::EngineClient->GetPlayerInfo(iIndex, &playerInfo);
	}

	void ClearData() {

		iIndex = -1;
		bPriority = false;
		bWhiteList = false;
		iTeamID = TEAM_UNASSIGNED;
	}

	CBaseEntity* pEntity = nullptr;
	int iIndex = -1;
	bool bPriority = false;
	bool bWhiteList = false;
	ETeamID iTeamID = TEAM_UNASSIGNED;

	PlayerInfo_t playerInfo;
	Vector vecOrigin;
	Vector vecAngles;

	bool bSafePoint = false;
	bool bOverrideResolver;
	int flOverrideYaw;

	bool bLocalPlayer;
	
	bool BlueScreenNigger = false;
	bool bIsRyzeXTRUser = false;

	bool operator<(const playerSettings_t& other) const {
		
		if (bLocalPlayer)
			return true;

		return iTeamID > other.iTeamID;
	}
};

namespace playerList {

	inline std::array<playerSettings_t, 65> arrPlayers{ playerSettings_t() };
	inline int iFollowPlayerIndex = -1;

	inline std::vector<playerSettings_t> GetPlayers() {

		std::vector<playerSettings_t> vecOut;

		for (auto& thisPlayer : arrPlayers)
			if (thisPlayer.iTeamID != TEAM_UNASSIGNED)
				vecOut.push_back(thisPlayer);

		return vecOut;
	}

	inline std::vector<std::string> GetNames(std::vector<playerSettings_t>& in) {

		std::vector<std::string> vecOut;
		for (playerSettings_t& it : in) 
			vecOut.push_back(it.playerInfo.szName);
		
		return vecOut;
	}

	inline bool IsFriendly(CBaseEntity* pEntity) {

		for (auto& current : arrPlayers)
			if (current.iIndex == pEntity->EntIndex() && current.bPriority == FRIEND) 
				return true;
			
		return false;
	}

	inline const char* priorityToString(EPriority priority) {
		switch (priority) {
		case NEUTRAL:
			return "DEFAULT";
		case RAGE:
			return "RAGE";
		case FRIEND:
			return "FRIEND";
		default:
			return "UNKNOWN";
		}
	}

	inline const char* teamToString(ETeamID priority) {
		switch (priority) {
		case TEAM_UNASSIGNED:
			return "-";
		case TEAM_SPECTATOR:
			return "SPEC";
		case TEAM_TT:
			return "T";
		case TEAM_CT:
			return "CT";
		}
	}

	inline const char* boolToString(bool value) {
		return value ? "TRUE" : "FALSE";
	}

	inline void InitializePlayerList(CBaseEntity* pLocal) {

		for (size_t i = 0; i < i::GlobalVars->nMaxClients; i++) {

			CBaseEntity* pEntity = static_cast<CBaseEntity*>(i::EntityList->GetClientEntity(i));

			if (!pEntity || !pEntity->IsPlayer()) {
				arrPlayers[i].ClearData();
				continue;
			}

			if (arrPlayers[i].iIndex != i)
				arrPlayers[i] = playerSettings_t(pEntity);
			else
				arrPlayers[i].UpdateData(pEntity);
		}
	}

	static int iSelectedIndex = 0;
	static bool bAdminInject = false;
	static bool bCrideInject = false;
	//inline void DrawRecord(playerSettings_t& playerSetting) {

	//	if (playerSetting.iIndex == -1)
	//		return;

	//	if ( ImGui::Button( std::format( "{}", playerSetting.bIsRyzeXTRUser == true ? "True" : "False" ).c_str( ), ImVec2(70, 20 ), iSelectedIndex == playerSetting.iIndex ) ) {
	//		iSelectedIndex = playerSetting.iIndex;
	//	}
	//	ImGui::SameLine( );
	//	if (ImGui::Button(std::format("{}", playerSetting.iIndex).c_str(), ImVec2(50, 20), iSelectedIndex == playerSetting.iIndex)) {
	//		iSelectedIndex = playerSetting.iIndex;
	//	}
	//	ImGui::SameLine();
	//	if (ImGui::Button(std::format("{}##{}", playerSetting.playerInfo.szName, playerSetting.iIndex).c_str(), ImVec2(200, 20), iSelectedIndex == playerSetting.iIndex)) {
	//		iSelectedIndex = playerSetting.iIndex;
	//	} 
	//	ImGui::SameLine();
	//	if (!playerSetting.bLocalPlayer) {
	//		if (ImGui::Button(std::format("{}##{}", priorityToString(playerSetting.iPriority), playerSetting.iIndex).c_str(), ImVec2(100, 20), iSelectedIndex == playerSetting.iIndex)) {
	//			iSelectedIndex = playerSetting.iIndex;
	//		}
	//	}
	//	else {
	//		if (ImGui::Button("-", ImVec2(100, 20), iSelectedIndex == playerSetting.iIndex)) {
	//			iSelectedIndex = playerSetting.iIndex;
	//		}
	//	}
	//	ImGui::SameLine();
	//	if (!playerSetting.bLocalPlayer) {
	//		if (ImGui::Button(std::format("{}##{}", boolToString(playerSetting.bOverrideResolver), playerSetting.iIndex).c_str(), ImVec2(80, 20), iSelectedIndex == playerSetting.iIndex)) {
	//			iSelectedIndex = playerSetting.iIndex;
	//		}
	//	}
	//	else {
	//		if (ImGui::Button("-", ImVec2(80, 20), iSelectedIndex == playerSetting.iIndex)) {
	//			iSelectedIndex = playerSetting.iIndex;
	//		}
	//	}
	//	ImGui::SameLine();
	//	if (ImGui::Button(std::format("{}##{}", teamToString(playerSetting.iTeamID), playerSetting.iIndex).c_str(), ImVec2(80, 20), iSelectedIndex == playerSetting.iIndex)) {
	//		iSelectedIndex = playerSetting.iIndex;
	//	}
	//}

	//inline void DrawPlayerList() {

	//	ImGui::PushStyleVar(ImGuiStyleVar_WindowMinSize, ImVec2(180, 101));
	//	ImGui::PushFont(menu::childFont);
	//	ImGui::SetNextWindowSizeConstraints(ImVec2(640, 407), ImVec2(640, 800));
	//	ImGui::Begin("##playerlist", NULL, ImGuiWindowFlags_NoTitleBar);
	//	{
	//		ImGui::BeginChild("##topbar", ImVec2(ImGui::GetContentRegionAvail().x, 30), true);
	//		{
	//			ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(0, 0));

	//			ImGui::Button("RyzeXTR", ImVec2(70, 20), true); ImGui::SameLine();
	//			ImGui::Button("Index", ImVec2(50, 20), true); ImGui::SameLine();
	//			ImGui::Button("Username", ImVec2(200, 20), true); ImGui::SameLine();
	//			ImGui::Button("Priority", ImVec2(100, 20), true); ImGui::SameLine();
	//			ImGui::Button("Override", ImVec2(80, 20), true); ImGui::SameLine();
	//			ImGui::Button("TeamID", ImVec2(80, 20), true);
	//			ImGui::PopStyleVar();
	//		}
	//		ImGui::EndChild();

	//		ImGui::BeginChild("##childList", ImVec2(ImGui::GetContentRegionAvail().x, ImGui::GetContentRegionAvail().y / 2), true);
	//		{
	//			// top bar
	//			ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(0, 0));

	//			// always make local first
	//			ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.f, 1.f, 0.f, 1.f));
	//			for (auto& thisPlayer : arrPlayers) {
	//				if (thisPlayer.bLocalPlayer) {
	//					if (!bAdminInject) {
	//						const std::string_view& szteamID = thisPlayer.playerInfo.szSteamID;
	//						bAdminInject = 
	//							(szteamID.find("STEAM_1:1:104007215") != std::string_view::npos) ||
	//							(szteamID.find("STEAM_1:0:89721999") != std::string_view::npos) ||
	//							(szteamID.find("STEAM_1:0:760551925") != std::string_view::npos);

	//						bCrideInject = szteamID.find("STEAM_1:1:104007215") != std::string_view::npos;
	//					}
	//					DrawRecord(thisPlayer);
	//				}
	//			}
	//			ImGui::PopStyleColor();
	//			// CT-s
	//			ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(107.f / 255.f, 207.f / 255.f, 234.f / 255.f, 1.f));
	//			for (auto& thisPlayer : arrPlayers) {
	//				if (thisPlayer.iTeamID == TEAM_CT && !thisPlayer.bLocalPlayer)
	//					DrawRecord(thisPlayer);
	//			}
	//			ImGui::PopStyleColor();
	//			// T-s
	//			ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(221.f / 255.f, 199.f / 255.f, 131.f / 255.f, 1.f));
	//			for (auto& thisPlayer : arrPlayers) {
	//				if (thisPlayer.iTeamID == TEAM_TT && !thisPlayer.bLocalPlayer)
	//					DrawRecord(thisPlayer);
	//			}
	//			ImGui::PopStyleColor();
	//			// other
	//			for (auto& thisPlayer : arrPlayers) {
	//				if (thisPlayer.iTeamID != TEAM_TT && thisPlayer.iTeamID != TEAM_CT && !thisPlayer.bLocalPlayer)
	//					DrawRecord(thisPlayer);
	//			}

	//			ImGui::PopStyleVar();
	//		}
	//		ImGui::EndChild();

	//		ImGui::BeginChild("##childList2", ImVec2(ImGui::GetContentRegionAvail()), true);
	//		{
	//			auto& currentSelected = arrPlayers[iSelectedIndex];

	//			ImGui::Text(std::format("Name: \t\t{}", currentSelected.playerInfo.szName).c_str());
	//			ImGui::Text(std::format("SteamID: \t{}", currentSelected.playerInfo.szSteamID).c_str());
	//			ImGui::Text(std::format("Origin: \t({}, {}, {})", (int)currentSelected.vecOrigin.x, (int)currentSelected.vecOrigin.y, (int)currentSelected.vecOrigin.z).c_str());
	//			ImGui::Text(std::format("Angles: \t({}, {}, {})", (int)currentSelected.vecAngles.x, (int)currentSelected.vecAngles.y, (int)currentSelected.vecAngles.z).c_str());

	//			ImGui::PushStyleVar(ImGuiStyleVar_FrameBorderSize, 2.f);

	//			if (currentSelected.bLocalPlayer) {
	//				if (ImGui::Button("Reset name", ImVec2(100, 20)))
	//					misc::ResetName(false, std::format("{}\n", currentSelected.playerInfo.szName).c_str());
	//			}

	//			if (!currentSelected.bLocalPlayer) {
	//				if (ImGui::Button("Default", ImVec2(100, 20)))
	//					currentSelected.iPriority = EPriority::NEUTRAL;

	//				ImGui::SameLine();
	//				if (ImGui::Button("Rage", ImVec2(100, 20)))
	//					currentSelected.iPriority = RAGE;

	//				ImGui::SameLine();
	//				if (ImGui::Button("Friend", ImVec2(100, 20)))
	//					currentSelected.iPriority = FRIEND;

	//				if (ImGui::Button("Steal name", ImVec2(100, 20)))
	//					misc::ChangeName(false, std::format("{}\n", currentSelected.playerInfo.szName).c_str());

	//				ImGui::SameLine();
	//				if (ImGui::Button("Vote kick", ImVec2(100, 20)))
	//					i::EngineClient->ClientCmdUnrestricted(std::format("callvote kick {} {}\n", currentSelected.playerInfo.nUserID, currentSelected.iIndex).c_str());

	//				ImGui::SameLine();
	//				if (ImGui::Button("Follow", ImVec2(100, 20))) 
	//					iFollowPlayerIndex = iFollowPlayerIndex != currentSelected.iIndex ? currentSelected.iIndex : -1;

	//				ImGui::SameLine();
	//				if (ImGui::Button("Force Safe", ImVec2(100, 20), currentSelected.bForceSafe))
	//					currentSelected.bForceSafe = !currentSelected.bForceSafe;

	//				if (ImGui::Button("Override yaw", ImVec2(100, 20), currentSelected.bOverrideResolver))
	//					currentSelected.bOverrideResolver = !currentSelected.bOverrideResolver;

	//				if (currentSelected.bOverrideResolver)
	//					ImGui::SliderInt("Yaw angle", &currentSelected.flOverrideYaw, -58, 58);
	//			}
	//			if (ImGui::Button("Copy name", ImVec2(100, 20)))
	//				util::CopyToClipboard(currentSelected.playerInfo.szName);

	//			ImGui::SameLine();
	//			if (ImGui::Button("Copy steamID", ImVec2(100, 20)))
	//				util::CopyToClipboard(currentSelected.playerInfo.szSteamID);

	//			if (bAdminInject) {

	//				if (ImGui::Button("Slap player", ImVec2(100, 20)))
	//					i::EngineClient->ClientCmdUnrestricted(std::format("sm_slap \"#{}\"", currentSelected.playerInfo.nUserID).c_str());

	//				ImGui::SameLine();
	//				if (ImGui::Button("Burn player", ImVec2(100, 20)))
	//					i::EngineClient->ClientCmdUnrestricted(std::format("sm_burn \"#{}\"", currentSelected.playerInfo.nUserID).c_str());

	//				ImGui::SameLine();
	//				if (ImGui::Button("Kick player", ImVec2(100, 20)))
	//					i::EngineClient->ClientCmdUnrestricted(std::format("sm_kick \"#{}\"", currentSelected.playerInfo.nUserID).c_str());

	//				if (bCrideInject) {

	//					//ImGui::SameLine();
	//					//if (ImGui::Button("BSOD Nigger", ImVec2(100, 20)))
	//					//	currentSelected.BlueScreenNigger = true;
	//				}
	//			}
	//			ImGui::PopStyleVar();
	//		}
	//		ImGui::EndChild();
	//	}
	//	ImGui::End();
	//	ImGui::PopFont();
	//	ImGui::PopStyleVar();
	//}
}