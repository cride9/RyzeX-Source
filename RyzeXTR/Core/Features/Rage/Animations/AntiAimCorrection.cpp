#include "EnemyAnimations.h"
#include "../aimbot.h"
#include "../autowall.h"
#include "../../Misc/Playerlist.h"
#include "../../../SDK/Menu/config.h"
#include "../../Networking/networking.h"
#include "../../../xorstr.h"
#include "../../Visuals/ESP.h"
#include "../../Visuals/chams.h"
#include <format>
#include "../../../Lua/Lua.h"

/* LMAO ye adding back again this one thanks exlo again */
float Animations::GetLocalCycleIncrement(CBaseEntity* pEntity, float flPlaybackrate)
{
	float flMoveCycleRate = flPlaybackrate ? flPlaybackrate : pEntity->GetAnimationOverlays()[6].flPlaybackRate;
	float flVelocityLengthXY = pEntity->AnimState()->flVelocityLenght2D <= 1.f ? 1.f : pEntity->AnimState()->flVelocityLenght2D;
	if (flVelocityLengthXY > 0.f)
	{
		float flSequenceCycleRate = pEntity->GetSequenceCycleRate(pEntity->GetModelPtr(), pEntity->GetAnimationOverlays()[6].nSequence);
		float flSequenceGroundSpeed = fmax(pEntity->GetSequenceMoveDist(pEntity->GetModelPtr(), pEntity->GetAnimationOverlays()[6].nSequence) / (1.0f / flSequenceCycleRate), 0.001f);

		float flSpeedMultiplier = flSequenceCycleRate * (flVelocityLengthXY / flSequenceGroundSpeed) * (1.0f - (pEntity->AnimState()->flWalkToRunTransition * 0.15f));
		flMoveCycleRate /= flSpeedMultiplier;
	}

	float flLocalCycleIncrement = (flMoveCycleRate * pEntity->AnimState()->flLastUpdateIncrement);
	return flLocalCycleIncrement * 1000000.0f;
}

void Animations::Resolver(CBaseEntity* pEntity, Lagcompensation::LagRecord_t* pRecord, Lagcompensation::LagRecord_t* pPrevious) {

	CBaseEntity* pLocal = CBaseEntity::GetLocalPlayer();
	if (!cfg::rage::bResolver || !pLocal || !pEntity || !pEntity->IsAlive() || !pRecord || !pPrevious)
		return;

#ifdef NDEBUG
	if (pEntity->GetPlayerInfo().bFakePlayer)
		return;
#endif

	Lagcompensation::AnimationInfo_t* pLog = &lagcomp.GetLog(pRecord->iEntIndex);
	static std::array<int, 65> arrMissedShotsBackup{ 0 };
	static float flFakePitch[65];

	const int iEntityID = pEntity->EntIndex();

	if (fabsf(pEntity->AnimState()->flEyePitch) == 180.f)
		flFakePitch[iEntityID] = pEntity->AnimState()->flEyePitch;
	else if (pRecord->bDidShot) flFakePitch[iEntityID] = NULL;

	if (fabsf(flFakePitch[iEntityID]) == 180.f)
		pEntity->GetEyeAngles() = Vector(89.f, pEntity->AnimState()->flEyeYaw, 0.f);
		
	float flHitPercentage = static_cast<float>(pLog->iHitAmount) / static_cast<float>(pLog->iShotAmount);
	if (arrMissedShots[iEntityID] == 0 && flHitPercentage < 0.2f)
		arrMissedShots[iEntityID] = 1;

	int iCurrentBrute = arrMissedShots[iEntityID] % 2;

	if (pRecord->iFlags & FL_ONGROUND && pPrevious->iFlags & FL_ONGROUND && pRecord->flWalkToRunTransition > 0.f && pRecord->flWalkToRunTransition < 1.f) {

		float flFromServerPlaybackrate = GetLocalCycleIncrement(pEntity, pRecord->arrLayers[ANIMATION_LAYER_MOVEMENT_MOVE].flPlaybackRate);

		const float fCenterPlaybackrate = GetLocalCycleIncrement(pEntity, pRecord->LayerData[CENTER].flPlaybackRate);
		const float fRightPlaybackrate = GetLocalCycleIncrement(pEntity, pRecord->LayerData[RIGHT].flPlaybackRate);
		const float fLeftPlaybackrate = GetLocalCycleIncrement(pEntity, pRecord->LayerData[LEFT].flPlaybackRate);

		// differences.
		const float fDifferenceCenterPlaybackrate = fabsf(flFromServerPlaybackrate - fCenterPlaybackrate);
		const float fDifferenceRightPlaybackrate = fabsf(flFromServerPlaybackrate - fRightPlaybackrate);
		const float fDifferenceLeftPlaybackrate = fabsf(flFromServerPlaybackrate - fLeftPlaybackrate);

		if (GetVelocityLengthXY(pEntity) > 0.f)
		{
			if (fDifferenceCenterPlaybackrate <= fDifferenceRightPlaybackrate && fDifferenceCenterPlaybackrate <= fDifferenceLeftPlaybackrate)
				SetYaw(pRecord, CENTER);
			else if (fDifferenceRightPlaybackrate <= fDifferenceCenterPlaybackrate && fDifferenceRightPlaybackrate <= fDifferenceLeftPlaybackrate)
				SetYaw(pRecord, RIGHT);
			else if (fDifferenceLeftPlaybackrate <= fDifferenceCenterPlaybackrate && fDifferenceLeftPlaybackrate <= fDifferenceRightPlaybackrate)
				SetYaw(pRecord, LEFT);
		}
	}
	/* Don't resolve onshot */
	if (pRecord->bDidShot)
		return;

	/* breaking to the left */
	else if (pRecord->flPoses[BODY_YAW] > 0.85f && pRecord->arrLayers[6].flPlaybackRate == 0.f)
		SetYaw(pRecord, RIGHT);

	/* breaking to the right */
	else if (pRecord->flPoses[BODY_YAW] < 0.15f && pRecord->arrLayers[6].flPlaybackRate == 0.f)
		SetYaw(pRecord, LEFT);

	/* Apply previous data if no new data */
	else if (pPrevious->iResolveSide != VISUAL)
		SetYaw(pRecord, pPrevious->iResolveSide);

	/* If we didn't get any data apply right side & no previous data */
	else if (pRecord->iResolveSide == VISUAL)
		SetYaw(pRecord, RIGHT);

	if (arrMissedShots[iEntityID] != arrMissedShotsBackup[iEntityID]) {
		SetYaw(pRecord, pPrevious->iResolveSide == LEFT ? pPrevious->iResolveSide + iCurrentBrute : pPrevious->iResolveSide == RIGHT ? pPrevious->iResolveSide - iCurrentBrute : RIGHT);
		arrMissedShotsBackup[iEntityID] = arrMissedShots[iEntityID];
	}
}

void Animations::ResolverLogic() {

	if (!aimbot.GetHitLogData().pAimbotTarget || !g::pLocal || bBulletImpact == Vector(0, 0, 0) || !aimbot.GetHitLogData().pTargetMatrix)
		return;

	if (aimbot.GetHitLogData().iTickcount + 64 < g::pCmd->iTickCount)
		return aimbot.GetHitLogData().ClearTarget();

	// make pointers and references for easier handling
	auto& refCurrentData = aimbot.GetHitLogData();
	CBaseEntity* pTarget = refCurrentData.pAimbotTarget;

	if (i::EntityList->GetClientEntity(refCurrentData.iEntityIndex) == nullptr) {
		refCurrentData.ClearTarget();
		return;
	}

	auto* pLog = &lagcomp.GetLog(refCurrentData.pAimbotTarget->EntIndex());
	if (!pLog) {
		refCurrentData.ClearTarget();
		return;
	}

	// get player name and info
	PlayerInfo_t info;
	i::EngineClient->GetPlayerInfo(pTarget->EntIndex(), &info);

	// check if we did shot & had an impact
	if (!bResolverHandler[WEAPONFIRE] || !bResolverHandler[BULLETIMPACT]) 
		return;
	
	// apply shot matrix
	refCurrentData.pRecord->ApplyMatrix(pTarget, RESOLVE);
	refCurrentData.iServerHitbox = iHitHitbox;
	refCurrentData.iBacktrackTicks = (refCurrentData.iTickcount - TIME_TO_TICKS(refCurrentData.flTargetSimulation + lagcomp.GetClientInterpAmount()));

	// Simulate a bullet shot
	FireBulletData_t data;
	autowall.GetDamage(g::pLocal, refCurrentData.vecLocalShootPosition, bBulletImpact, g::pLocal->GetWeapon(), refCurrentData.pRecord , &data);

	// Check if we killed, or hurt the player
	if (bResolverHandler[PLAYERHURT] || bResolverHandler[PLAYERDEATH]) {

		if (data.enterTrace.iHitGroup != iHitHitbox)
			anims.arrMissedShots[refCurrentData.pAimbotTarget->EntIndex()]++;

		bResolverHandler = std::array<bool, HANDLERCOUNT>();
		misc::Print(std::vformat(
			XorStr("Hit {}'s {} for {} hp. | [hc] {} | [bt] {} | [wanted hg: {}] | [wanted dmg: {}] | [yaw] {}" ), std::make_format_args(
			info.szName,
			misc::GetHitgroupName(iHitHitbox),
			iHitDmg,
			refCurrentData.flHitchance,
			(refCurrentData.iTickcount - TIME_TO_TICKS(refCurrentData.flTargetSimulation + lagcomp.GetClientInterpAmount())),
			misc::GetHitgroupName(refCurrentData.iHitGroup),
			refCurrentData.flDamage,
			refCurrentData.pRecord->flResolveDelta
		) ) );
		pLog->iHitAmount++;
		visual::vecDamageIndicator.push_back(std::make_pair(refCurrentData.vecTargetShootPosition, static_cast<int>(iHitDmg)));
		refCurrentData.ClearTarget();
		return;
	}

	// Check again just to be safe
	if (pTarget->GetHealth() < refCurrentData.iHealth || !pTarget->IsAlive()) {

		if (data.enterTrace.iHitGroup != iHitHitbox)
			anims.arrMissedShots[refCurrentData.pAimbotTarget->EntIndex()]++; 

		bResolverHandler = std::array<bool, HANDLERCOUNT>();
		misc::Print(std::vformat(
			XorStr("Hit {}'s {} for {} hp. | [hc] {} | [bt] {} | [wanted hg: {}] | [wanted dmg: {}] | [yaw] {}"), std::make_format_args(
			info.szName,
			misc::GetHitgroupName(iHitHitbox),
			iHitDmg,
			refCurrentData.flHitchance,
			(refCurrentData.iTickcount - TIME_TO_TICKS(refCurrentData.flTargetSimulation + lagcomp.GetClientInterpAmount())),
			misc::GetHitgroupName(refCurrentData.iHitGroup),
			refCurrentData.flDamage,
			refCurrentData.pRecord->flResolveDelta
		)));
		pLog->iHitAmount++;
		visual::vecDamageIndicator.push_back(std::make_pair(bBulletImpact, static_cast<int>(iHitDmg)));
		refCurrentData.ClearTarget();
		return;
	}

	// If we hit an entity but didn't deal any dmg its a resolver miss
	if (data.enterTrace.pHitEntity != nullptr && data.enterTrace.pHitEntity == pTarget) {

		anims.arrMissedShots[refCurrentData.pAimbotTarget->EntIndex()]++;
		pLog->iLastResolve = refCurrentData.pRecord->iResolveSide;

		if (refCurrentData.bBacktrack) {

			bResolverHandler = std::array<bool, HANDLERCOUNT>();
			misc::Print(std::vformat(
				XorStr("Missed {}'s {} due to invalid record | [hc] {} | [bt] {} | [dmg] {} | [yaw] {}"), std::make_format_args( 
				info.szName,
				misc::GetHitgroupName(refCurrentData.iHitGroup),
				refCurrentData.flHitchance,
				(refCurrentData.iTickcount - TIME_TO_TICKS(refCurrentData.flTargetSimulation + lagcomp.GetClientInterpAmount())),
				refCurrentData.flDamage,
				refCurrentData.pRecord->flResolveDelta
			)));
			visual::vecDamageIndicator.push_back(std::make_pair(refCurrentData.vecTargetShootPosition, 0));
			refCurrentData.ClearTarget();
			return;
		}

		bResolverHandler = std::array<bool, HANDLERCOUNT>();
		misc::Print(std::vformat(
			XorStr("Missed {}'s {} due to resolver. | [hc] {} | [bt] {} | [dmg] {} | [yaw] {}"), std::make_format_args( 
			info.szName,
			misc::GetHitgroupName(refCurrentData.iHitGroup),
			refCurrentData.flHitchance,
			(refCurrentData.iTickcount - TIME_TO_TICKS(refCurrentData.flTargetSimulation + lagcomp.GetClientInterpAmount())),
			refCurrentData.flDamage,
			refCurrentData.pRecord->flResolveDelta
		)));
		visual::vecDamageIndicator.push_back(std::make_pair(refCurrentData.vecTargetShootPosition, 0));
		refCurrentData.ClearTarget();
	}
	else {

		if (refCurrentData.flHitchance >= 99) {

			bResolverHandler = std::array<bool, HANDLERCOUNT>();
			misc::Print(std::vformat(
				XorStr("Missed {}'s {} due to correction. | [hc] {} | [bt] {} | [dmg] {} | [yaw] {}"), std::make_format_args(
				info.szName,
				misc::GetHitgroupName(refCurrentData.iHitGroup),
				refCurrentData.flHitchance,
				(refCurrentData.iTickcount - TIME_TO_TICKS(refCurrentData.flTargetSimulation + lagcomp.GetClientInterpAmount())),
				refCurrentData.flDamage,
				refCurrentData.pRecord->flResolveDelta
			)));
			visual::vecDamageIndicator.push_back(std::make_pair(refCurrentData.vecTargetShootPosition, 0));
			refCurrentData.ClearTarget();
			return;
		}

		// check for occlusion
		Trace_t traceData;
		Ray_t rayData(refCurrentData.vecLocalShootPosition, bBulletImpact);
		CTraceFilter filterData(g::pLocal, TRACE_ENTITIES_ONLY);
		i::EngineTrace->TraceRay(rayData, MASK_SHOT | CONTENTS_GRATE, &filterData, &traceData);

		bool bOccluded = (bBulletImpact - refCurrentData.vecLocalShootPosition).LengthSqr() < (refCurrentData.vecTargetShootPosition - refCurrentData.vecLocalShootPosition).LengthSqr();
		if (bOccluded) { // traceData.pHitEntity != nullptr && traceData.pHitEntity == refCurrentData.pAimbotTarget || data.flCurrentDamage == 0.f

			bResolverHandler = std::array<bool, HANDLERCOUNT>();
			misc::Print(std::vformat(
				XorStr("Missed {}'s {} due to occlusion. | [hc] {} | [bt] {} | [dmg] {} | [yaw] {}"), std::make_format_args(
				info.szName,
				misc::GetHitgroupName(refCurrentData.iHitGroup),
				refCurrentData.flHitchance,
				(refCurrentData.iTickcount - TIME_TO_TICKS(refCurrentData.flTargetSimulation + lagcomp.GetClientInterpAmount())),
				refCurrentData.flDamage,
				refCurrentData.pRecord->flResolveDelta
			)));
			visual::vecDamageIndicator.push_back(std::make_pair(refCurrentData.vecTargetShootPosition, 0));
			refCurrentData.ClearTarget();
			return;
		}

		bResolverHandler = std::array<bool, HANDLERCOUNT>();
		misc::Print(std::vformat(
			XorStr("Missed {}'s {} due to spread. | [hc] {} | [bt] {} | [dmg] {} | [yaw] {}"), std::make_format_args(
			info.szName,
			misc::GetHitgroupName(refCurrentData.iHitGroup),
			refCurrentData.flHitchance,
			(refCurrentData.iTickcount - TIME_TO_TICKS(refCurrentData.flTargetSimulation + lagcomp.GetClientInterpAmount())),
			refCurrentData.flDamage,
			refCurrentData.pRecord->flResolveDelta
		)));
		visual::vecDamageIndicator.push_back(std::make_pair(refCurrentData.vecTargetShootPosition, 0));
		refCurrentData.ClearTarget();
	}
}

void Animations::ResolverHandler(IGameEvent* pEvent) {

	if (!aimbot.GetHitLogData().pAimbotTarget || !g::pLocal)
		return;

	std::string_view szEventName = pEvent->GetName();

	if (szEventName.find(cachedEvents::weaponFire) != std::string_view::npos) {

		auto iUser = i::EngineClient->GetPlayerForUserID(pEvent->GetInt(XorStr("userid")));

		if (iUser == i::EngineClient->GetLocalPlayer()) {
			bResolverHandler[WEAPONFIRE] = true;
		}
	}
	if (szEventName.find(cachedEvents::playerHurt) != std::string_view::npos) {

		auto iUser = i::EngineClient->GetPlayerForUserID(pEvent->GetInt(XorStr("userid")));
		auto iAttacker = i::EngineClient->GetPlayerForUserID(pEvent->GetInt(XorStr("attacker")));
		auto targetIndex = aimbot.GetHitLogData().pAimbotTarget->EntIndex();

		if (iAttacker == i::EngineClient->GetLocalPlayer() && iUser == targetIndex) {
			bResolverHandler[PLAYERHURT] = true;
			iHitDmg = pEvent->GetInt(XorStr("dmg_health"));
			iHitHitbox = pEvent->GetInt(XorStr("hitgroup"));
		}
	}
	if (szEventName.find(cachedEvents::bBulletImpact) != std::string_view::npos) {

		auto iUser = i::EngineClient->GetPlayerForUserID(pEvent->GetInt(XorStr("userid")));

		if (iUser != i::EngineClient->GetLocalPlayer())
			return;

		bBulletImpact = Vector(pEvent->GetFloat(XorStr("x")), pEvent->GetFloat(XorStr("y")), pEvent->GetFloat(XorStr("z")));
		bResolverHandler[BULLETIMPACT] = true;
	}
	if (szEventName.find(cachedEvents::playerDeath) != std::string_view::npos) {

		auto iUser = i::EngineClient->GetPlayerForUserID(pEvent->GetInt(XorStr("userid")));
		auto iAttacker = i::EngineClient->GetPlayerForUserID(pEvent->GetInt(XorStr("attacker")));
		auto targetIndex = aimbot.GetHitLogData().pAimbotTarget->EntIndex();

		if (iAttacker == i::EngineClient->GetLocalPlayer() && iUser == targetIndex) {
			bResolverHandler[PLAYERDEATH] = true;
		}
	}
	if (szEventName.find(cachedEvents::roundStart) != std::string_view::npos) {

		for (size_t i = 0; i < 65; i++)
			lagcomp.GetLog(i).ClearData();
	}
}

void SetResolveMatrix(Lagcompensation::LagRecord_t* pRecord, int iType) {

	//memcpy(pRecord->pMatricies[RESOLVE], pRecord->pMatricies[iType], sizeof(matrix3x4_t) * MAXSTUDIOBONES);
	pRecord->iResolveSide = iType;
}

float Animations::GetVelocityLengthXY(CBaseEntity* pEntity)
{
	Lagcompensation::AnimationInfo_t* pRecord = &lagcomp.GetLog(pEntity->EntIndex());
	if (!pRecord)
		return 0.f;

	float flSequenceCycleRate = pEntity->GetSequenceCycleRate(pEntity->GetModelPtr(), pEntity->GetAnimationOverlays()[6].nSequence);
	float flSequenceGroundSpeed = fmax(pEntity->GetSequenceMoveDist(pEntity->GetModelPtr(), pEntity->GetAnimationOverlays()[6].nSequence) / (1.0f / flSequenceCycleRate), 0.001f);

	return (pEntity->GetAnimationOverlays()[6].flPlaybackRate / flSequenceCycleRate) / (1.0f - (pEntity->AnimState()->flWalkToRunTransition * 0.15f)) * (flSequenceGroundSpeed) / pEntity->AnimState()->flLastUpdateIncrement;
}

void Animations::SetYaw(Lagcompensation::LagRecord_t* pRecord, int flYaw) {

	CAnimState* pState = pRecord->pEntity->AnimState();

	if (flYaw == VISUAL)
		flYaw = LEFT;
	pRecord->iResolveSide = flYaw;
	switch (flYaw) {

	case LEFT:
		pState->flGoalFeetYaw = GetYawRotation(pRecord, LEFT);
		pRecord->flResolveDelta = M::NormalizeYaw(pRecord->vecEyeAngles.y - pState->flGoalFeetYaw);
		break;

	case CENTER:
		pState->flGoalFeetYaw = GetYawRotation(pRecord, CENTER);
		pRecord->flResolveDelta = M::NormalizeYaw(pRecord->vecEyeAngles.y - pState->flGoalFeetYaw);
		break;

	case RIGHT:
		pState->flGoalFeetYaw = GetYawRotation(pRecord, RIGHT);
		pRecord->flResolveDelta = M::NormalizeYaw(pRecord->vecEyeAngles.y - pState->flGoalFeetYaw);
		break;
	}
}
