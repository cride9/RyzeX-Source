#pragma once
#include "../../SDK/NetVar/Netvar.h"

#define ADD_NETVAR(name, type, netvar) type& name() { \
	static auto offset = n::netvars[fnv::HashConst(netvar)].uOffset; \
	return *reinterpret_cast<type*>(uint32_t(this) + offset); \
}

class CCSGameRulesProxy
{
public:

	//[DT_CSGameRules]
	ADD_NETVAR( m_bFreezePeriod, bool, "CCSGameRulesProxy->m_bFreezePeriod" );
	ADD_NETVAR( m_bMatchWaitingForResume, bool, "CCSGameRulesProxy->m_bMatchWaitingForResume" );
	ADD_NETVAR( m_bWarmupPeriod, bool, "CCSGameRulesProxy->m_bWarmupPeriod" );
	ADD_NETVAR( m_fWarmupPeriodEnd, float, "CCSGameRulesProxy->m_fWarmupPeriodEnd" );
	ADD_NETVAR( m_fWarmupPeriodStart, float, "CCSGameRulesProxy->m_fWarmupPeriodStart" );
	ADD_NETVAR( m_bTerroristTimeOutActive, bool, "CCSGameRulesProxy->m_bTerroristTimeOutActive" );
	ADD_NETVAR( m_bCTTimeOutActive, bool, "CCSGameRulesProxy->m_bCTTimeOutActive" );
	ADD_NETVAR( m_flTerroristTimeOutRemaining, float, "CCSGameRulesProxy->m_flTerroristTimeOutRemaining" );
	ADD_NETVAR( m_flCTTimeOutRemaining, float, "CCSGameRulesProxy->m_flCTTimeOutRemaining" );
	ADD_NETVAR( m_nTerroristTimeOuts, int, "CCSGameRulesProxy->m_nTerroristTimeOuts" );
	ADD_NETVAR( m_nCTTimeOuts, int, "CCSGameRulesProxy->m_nCTTimeOuts" );
	ADD_NETVAR( m_bTechnicalTimeOut, bool, "CCSGameRulesProxy->m_bTechnicalTimeOut" );
	ADD_NETVAR( m_iRoundTime, int, "CCSGameRulesProxy->m_iRoundTime" );
	ADD_NETVAR( m_gamePhase, int, "CCSGameRulesProxy->m_gamePhase" );
	ADD_NETVAR( m_totalRoundsPlayed, int, "CCSGameRulesProxy->m_totalRoundsPlayed" );
	ADD_NETVAR( m_nOvertimePlaying, int, "CCSGameRulesProxy->m_nOvertimePlaying" );
	ADD_NETVAR( m_timeUntilNextPhaseStarts, float, "CCSGameRulesProxy->m_timeUntilNextPhaseStarts" );
	ADD_NETVAR( m_flCMMItemDropRevealStartTime, float, "CCSGameRulesProxy->m_flCMMItemDropRevealStartTime" );
	ADD_NETVAR( m_flCMMItemDropRevealEndTime, float, "CCSGameRulesProxy->m_flCMMItemDropRevealEndTime" );
	ADD_NETVAR( m_fRoundStartTime, float, "CCSGameRulesProxy->m_fRoundStartTime" );
	ADD_NETVAR( m_bGameRestart, bool, "CCSGameRulesProxy->m_bGameRestart" );
	ADD_NETVAR( m_flRestartRoundTime, float, "CCSGameRulesProxy->m_flRestartRoundTime" );
	ADD_NETVAR( m_flGameStartTime, float, "CCSGameRulesProxy->m_flGameStartTime" );
	ADD_NETVAR( m_iHostagesRemaining, int, "CCSGameRulesProxy->m_iHostagesRemaining" );
	ADD_NETVAR( m_bAnyHostageReached, bool, "CCSGameRulesProxy->m_bAnyHostageReached" );
	ADD_NETVAR( m_bMapHasBombTarget, bool, "CCSGameRulesProxy->m_bMapHasBombTarget" );
	ADD_NETVAR( m_bMapHasRescueZone, bool, "CCSGameRulesProxy->m_bMapHasRescueZone" );
	ADD_NETVAR( m_bMapHasBuyZone, bool, "CCSGameRulesProxy->m_bMapHasBuyZone" );
	ADD_NETVAR( m_bIsQueuedMatchmaking, bool, "CCSGameRulesProxy->m_bIsQueuedMatchmaking" );
	ADD_NETVAR( m_nQueuedMatchmakingMode, int, "CCSGameRulesProxy->m_nQueuedMatchmakingMode" );
	ADD_NETVAR( m_bIsValveDS, bool, "CCSGameRulesProxy->m_bIsValveDS" );
	ADD_NETVAR( m_bIsQuestEligible, bool, "CCSGameRulesProxy->m_bIsQuestEligible" );
	ADD_NETVAR( m_bLogoMap, bool, "CCSGameRulesProxy->m_bLogoMap" );
	ADD_NETVAR( m_bPlayAllStepSoundsOnServer, bool, "CCSGameRulesProxy->m_bPlayAllStepSoundsOnServer" );
	ADD_NETVAR( m_iNumGunGameProgressiveWeaponsCT, int, "CCSGameRulesProxy->m_iNumGunGameProgressiveWeaponsCT" );
	ADD_NETVAR( m_iNumGunGameProgressiveWeaponsT, int, "CCSGameRulesProxy->m_iNumGunGameProgressiveWeaponsT" );
	ADD_NETVAR( m_iSpectatorSlotCount, int, "CCSGameRulesProxy->m_iSpectatorSlotCount" );
	ADD_NETVAR( m_bBombDropped, bool, "CCSGameRulesProxy->m_bBombDropped" );
	ADD_NETVAR( m_bBombPlanted, bool, "CCSGameRulesProxy->m_bBombPlanted" );
	ADD_NETVAR( m_iRoundWinStatus, int, "CCSGameRulesProxy->m_iRoundWinStatus" );
	ADD_NETVAR( m_eRoundWinReason, int, "CCSGameRulesProxy->m_eRoundWinReason" );
	ADD_NETVAR( m_flDMBonusStartTime, float, "CCSGameRulesProxy->m_flDMBonusStartTime" );
	ADD_NETVAR( m_flDMBonusTimeLength, float, "CCSGameRulesProxy->m_flDMBonusTimeLength" );
	ADD_NETVAR( m_unDMBonusWeaponLoadoutSlot, short, "CCSGameRulesProxy->m_unDMBonusWeaponLoadoutSlot" );
	ADD_NETVAR( m_bDMBonusActive, bool, "CCSGameRulesProxy->m_bDMBonusActive" );
	ADD_NETVAR( m_bTCantBuy, bool, "CCSGameRulesProxy->m_bTCantBuy" );
	ADD_NETVAR( m_bCTCantBuy, bool, "CCSGameRulesProxy->m_bCTCantBuy" );
	ADD_NETVAR( m_flGuardianBuyUntilTime, float, "CCSGameRulesProxy->m_flGuardianBuyUntilTime" );
	ADD_NETVAR( m_MatchDevice, int, "CCSGameRulesProxy->m_MatchDevice" );
	ADD_NETVAR( m_bHasMatchStarted, bool, "CCSGameRulesProxy->m_bHasMatchStarted" );
	ADD_NETVAR( m_nNextMapInMapgroup, int, "CCSGameRulesProxy->m_nNextMapInMapgroup" );
	ADD_NETVAR( m_nEndMatchMapVoteWinner, int, "CCSGameRulesProxy->m_nEndMatchMapVoteWinner" );
	ADD_NETVAR( m_bIsDroppingItems, bool, "CCSGameRulesProxy->m_bIsDroppingItems" );
	ADD_NETVAR( m_iActiveAssassinationTargetMissionID, int, "CCSGameRulesProxy->m_iActiveAssassinationTargetMissionID" );
	ADD_NETVAR( m_fMatchStartTime, float, "CCSGameRulesProxy->m_fMatchStartTime" );
	/*ADD_NETVAR( m_szTournamentEventName, char[ 260 ], "CCSGameRulesProxy->m_szTournamentEventName" );
	ADD_NETVAR( m_szTournamentEventStage, char[ 260 ], "CCSGameRulesProxy->m_szTournamentEventStage" );
	ADD_NETVAR( m_szTournamentPredictionsTxt, char[ 260 ], "CCSGameRulesProxy->m_szTournamentPredictionsTxt" );*/
	ADD_NETVAR( m_nTournamentPredictionsPct, int, "CCSGameRulesProxy->m_nTournamentPredictionsPct" );
	//ADD_NETVAR( m_szMatchStatTxt, char[ 260 ], "CCSGameRulesProxy->m_szMatchStatTxt" );
	ADD_NETVAR( m_nGuardianModeWaveNumber, int, "CCSGameRulesProxy->m_nGuardianModeWaveNumber" );
	ADD_NETVAR( m_nGuardianModeSpecialKillsRemaining, int, "CCSGameRulesProxy->m_nGuardianModeSpecialKillsRemaining" );
	ADD_NETVAR( m_nGuardianModeSpecialWeaponNeeded, int, "CCSGameRulesProxy->m_nGuardianModeSpecialWeaponNeeded" );
	ADD_NETVAR( m_nHalloweenMaskListSeed, int, "CCSGameRulesProxy->m_nHalloweenMaskListSeed" );
	ADD_NETVAR( m_numGlobalGiftsGiven, int, "CCSGameRulesProxy->m_numGlobalGiftsGiven" );
	ADD_NETVAR( m_numGlobalGifters, int, "CCSGameRulesProxy->m_numGlobalGifters" );
	ADD_NETVAR( m_numGlobalGiftsPeriodSeconds, int, "CCSGameRulesProxy->m_numGlobalGiftsPeriodSeconds" );
	ADD_NETVAR( m_numBestOfMaps, int, "CCSGameRulesProxy->m_numBestOfMaps " );
	ADD_NETVAR( m_iNumConsecutiveCTLoses, int, "CCSGameRulesProxy->m_iNumConsecutiveCTLoses" );
	ADD_NETVAR( m_iNumConsecutiveTerroristLoses, int, "CCSGameRulesProxy->m_iNumConsecutiveTerroristLoses" );
};
inline CCSGameRulesProxy** GameRules;