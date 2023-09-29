#include "aimbot.h"
#include "Animations/EnemyAnimations.h"
#include "../../Lua/Lua.h"
#include "../../Interface/Classes/CCSGameRulesProxy.h"

void CAimBot::CreateMove(CUserCmd* pCmd, CBaseEntity* pLocal) {

	static CConVar* recoilScale = i::ConVar->FindVar(XorStr("weapon_recoil_scale"));

	if (!cfg::rage::bEnable || (!IPT::HandleInput(cfg::rage::iAimbotKey) && cfg::rage::iAimbotKey != 0))
		return ResetAimbotData();

	if ((*GameRules)->m_bFreezePeriod())
		return ResetAimbotData();

	/* Run R8 Revolver cock */
	misc::RevolverCreateMove();

	/* Create new data for aimbot */
	aimData = rageBotData_t();
	CBaseCombatWeapon* pWeapon = pLocal->GetWeapon();

	if (pWeapon == nullptr)
		return ResetAimbotData();

	if (pWeapon->IsGrenade() || pWeapon->IsKnife() || !pWeapon->IsWeapon())
		return ResetAimbotData();

	if (!pWeapon->GetAmmo())
		return ResetAimbotData();

	/* Get currently equipped weapon config */
	curConfig = GetWeaponConfiguration(pWeapon->GetItemDefinitionIndex());
	curConfig.pWeapon = pWeapon;

	/* Make a vector out of every valid & tragetable entity */
	std::vector<Lagcompensation::AnimationInfo_t*> vecTargets = GetTargetableEntities(pLocal);

	if (vecTargets.empty())
		return ResetAimbotData();

	/* Get our extrapolated shooting position */
	vecEyePosition = localAnim->GetShootPosition();

	/* Scan stored entities & safepoint */
	Vector vecShootPosition = ScanHitboxes(vecTargets, pLocal);
	
	/* Run predicted autostop */
	bool bSkipPostStop = AutoStop(vecTargets, pLocal, pCmd, false);

	if (aimData.pRecord == nullptr || vecShootPosition == Vector(0, 0, 0))
		return ResetAimbotData();

	/* Apply the selected record to the desired entity */
	aimData.pRecord->ApplyMatrix(aimData.pAimbotTarget, RESOLVE);

	/* Tell exploits that dont recharge */
	exploits::bCanCharge = false;

	/* Auto zoom & m_bResumeZoom check */
	if (curConfig.bAutoScope && !pLocal->IsScoped() && !pLocal->IsResumingScope())
		pCmd->iButtons |= IN_ZOOM;

	/* Calculate the aim angle with vectorAngles */
	Vector vecAimAngle = (M::VectorAngles(vecShootPosition - vecEyePosition) -= (pLocal->GetAimPunch() * recoilScale->GetFloat()));
	
	/* Run normal autostop */
	if (!bSkipPostStop)
		AutoStop(vecTargets, pLocal, pCmd, true);

	/* Calculate current hitchance & accuracy boost */
	aimData.bCanShoot = HitChance(pCmd, pLocal, vecShootPosition, vecAimAngle, aimData.pRecord);

	/* Wait til we can shoot */
	if (!aimData.bCanShoot || !pLocal->CanShoot(pWeapon))
		return ResetAimbotData();
	
	/* Shoot entity when we can & have enough hitchance */
	pCmd->angViewPoint = vecAimAngle;
	if (!cfg::rage::bSilentAim) i::EngineClient->SetViewAngles(vecAimAngle);

	lagcomp.GetLog(aimData.pRecord->iEntIndex).iShotAmount++;
	pCmd->iButtons |= IN_ATTACK;
	aimData.iTickcount = pCmd->iTickCount;
	pCmd->iTickCount = TIME_TO_TICKS(aimData.flTargetSimulation + lagcomp.GetClientInterpAmount());

	/* Sending packet in prediction will cause hit registration delay ( only in CHLClient::CreateMove() ) */
	bShouldSendPacket = true;
	hitlogData = aimData;
}

void CAimBot::ResetAimbotData() {

	aimData.ClearTarget();
	exploits::bCanCharge = true;
}

void CAimBot::PostPrediction(CUserCmd* pCmd, bool& bSendPacket) {

	if (cfg::antiaim::bFakeDuck && IPT::HandleInput(cfg::antiaim::iFakeDuckKey))
		bShouldSendPacket = false;
		
	if (bShouldSendPacket) 
		bSendPacket = true;

	if (cfg::rage::bDoubletap && IPT::HandleInput(cfg::rage::iDoubletapKey) && exploits::bIsShiftingTicks)
		bSendPacket = false;

	if (exploits::bForcePacket)
		bSendPacket = true;

	bShouldSendPacket = false;
	exploits::bForcePacket = false;
}

Vector CAimBot::ScanHitboxes(std::vector<Lagcompensation::AnimationInfo_t*>& vecIn, CBaseEntity* pLocal) {

	static CConVar* ax = i::ConVar->FindVar("cl_lagcompensation");

	std::unordered_map<int, std::vector<float>> mapHitboxDamages{};
	std::vector<Hitscan_t> vecHitscan{};
	/* Loop through our valid entity logs */
	for (Lagcompensation::AnimationInfo_t* it : vecIn) {

		/* Invalid data check */
		if (it->iLastValid >= it->pRecord.size())
			continue;

		if (ax->GetInt() == 0)
			it->iLastValid = 0;

		float flTransformedDamage = curConfig.iMinimumDamage;
		if (curConfig.iMinimumDamage > 100)
			flTransformedDamage = max(it->pEntity->GetHealth(), 8) + (curConfig.iMinimumDamage - 100);

		/* Retarded fix for retarded people who uses 100 mindmg and trying to baim */
		if (cfg::rage::bForceBaim && IPT::HandleInput(cfg::rage::iForceBaimKey) && 
			curConfig.pWeapon->GetItemDefinitionIndex() != WEAPON_AWP && 
			curConfig.pWeapon->GetItemDefinitionIndex() != WEAPON_TASER && 
			flTransformedDamage > curConfig.pWeapon->GetCSWpnData()->iDamage)
			flTransformedDamage = curConfig.pWeapon->GetCSWpnData()->iDamage;

		/* Go through every valid data */
		for (int i = 0; i <= it->iLastValid; i++) {

			if (cfg::rage::bLimitScan && i != 0)
				i = it->iLastValid;

			Lagcompensation::LagRecord_t* pRecord = &it->pRecord.at(i);
			if (pRecord->bBreakingLagcompensation || !pRecord->bValid && pRecord != &it->pRecord.front() || !pRecord->IsValid())
				continue;

			/* Set current matrix for accurate tracing */
			pRecord->ApplyMatrix(pRecord->pEntity, RESOLVE);
			for (int& iHitbox : curConfig.vecHitboxes[NORMAL]) {

				if (flTransformedDamage >= 98 && (curConfig.pWeapon->GetItemDefinitionIndex() != WEAPON_AWP && curConfig.pWeapon->GetItemDefinitionIndex() != WEAPON_TASER) && iHitbox != HITBOX_HEAD)
					continue;

				bool bShouldMultiPoint = std::find(curConfig.vecHitboxes[MULTIPOINT].begin(), curConfig.vecHitboxes[MULTIPOINT].end(), iHitbox) != curConfig.vecHitboxes[MULTIPOINT].end();
				bool bShouldForceSafePoint = std::find(curConfig.vecHitboxes[SAFE].begin(), curConfig.vecHitboxes[SAFE].end(), iHitbox) != curConfig.vecHitboxes[SAFE].end();
				bool bShouldSafe = curConfig.bSafePoint;
				mstudiobbox_t* pStudioHitbox = it->pEntity->StudioHitbox(iHitbox);

				if (cfg::rage::iAimbotFov < 180) {
					float flRadius = 0.f;
					Vector vecCenter = pRecord->pEntity->GetHitboxPosition(iHitbox, pRecord->pMatricies[RESOLVE], flRadius);

					Vector output;
					M::VectorAngles(vecCenter - vecEyePosition, output);
					Vector vecDistanceBetween = (g::vecOriginalViewAngle - output.Normalize());

					if (fabsf((vecDistanceBetween).Normalize().Length2D()) > cfg::rage::iAimbotFov)
						continue;
				}

				std::vector<float> vecDamages = std::vector<float>();
				std::vector<Vector> vecWorldPoints = CreatePoints(vecEyePosition, curConfig.pWeapon, pRecord, iHitbox, RESOLVE, bShouldMultiPoint);
				// OPTIMIZATION: first element in the vector is always the hitbox center
				bool bFirstElement = true;
				for (Vector& vecHitboxPoint : vecWorldPoints) {

					/* Only backtrack safe entities */
					int iCollidePoints = (iHitbox != HITBOX_HEAD && bFirstElement) ? 3 : autowall.SafePoint(vecEyePosition, pRecord, vecHitboxPoint, iHitbox);

					// OPTIMIZATION: calculate safe points once and save it later in a variable. It will return if not safe, but when safe we already scanned safety
					if (!iCollidePoints || (iCollidePoints < 3 && i))
						continue;

					if (playerList::arrPlayers[pRecord->iEntIndex].bSafePoint && iCollidePoints < 3)
						continue;

					/* If we resolved with 70% accuracy skip safepoint */
					if (static_cast<float>(it->iHitAmount) / static_cast<float>(it->iShotAmount) < 0.7f || it->iShotAmount < 3) {

						if (bShouldSafe) {
							if (iCollidePoints < 2) {
								bFirstElement = false;
								continue;
							}

							if (bShouldForceSafePoint && iCollidePoints < 3) {
								bFirstElement = false;
								continue;
							}
						}
					}
					bFirstElement = false;

					/* Prepare bullet data variable for later usage */
					FireBulletData_t pData;
					float flDamage = autowall.GetDamage(pLocal, vecEyePosition, vecHitboxPoint, curConfig.pWeapon, pRecord, &pData);
					if (flDamage >= 1) {

						/* Push back this shit */
						vecHitscan.push_back(Hitscan_t(pRecord, vecHitboxPoint, pData, iHitbox != HITBOX_HEAD && flDamage > pRecord->pEntity->GetHealth(), (bShouldSafe || bShouldForceSafePoint), flTransformedDamage));
						
						/* ATTEMPT: fix underdamage by getting avarage damage that can be dealt to that hitbox */
						vecDamages.push_back(flDamage);

						/* If that's a baim hitbox and we can hit it break out of the loop ( Fixes shooting hitbox edge while the middle is out ) */
						if (iHitbox != HITBOX_HEAD)
							break;
					}
				}
				mapHitboxDamages.emplace(iHitbox, vecDamages);

			}
		}
	}

	if (vecHitscan.empty())
		return Vector(0, 0, 0);

	/* Sort hitboxes with a bit of logic ( REF: Hitscan_t::operator< ) */
	std::sort(vecHitscan.begin(), vecHitscan.end());

	for (Hitscan_t& refRecord : vecHitscan)
	{
		if (refRecord.flDamage == -1.f || refRecord.iTransformedDamage > refRecord.flDamage)
			continue;

		if (mapHitboxDamages.contains(refRecord.iHitbox)) {
			std::vector<float>& map = mapHitboxDamages.at(refRecord.iHitbox);
			if (!map.empty()) {

				float flSummary = 0.f;
				for (float& it : map)
					flSummary += it;

				if (flSummary / map.size() < refRecord.iTransformedDamage)
					continue;
			}
		}
		
		aimData.SetTarget(refRecord.pRecord, vecEyePosition, refRecord.bBacktrack);
		aimData.bSafe = refRecord.bSafe;
		aimData.flDamage = static_cast<int>(refRecord.flDamage);
		aimData.iHitbox = refRecord.iHitbox;
		aimData.iHitGroup = refRecord.iHitgroup;
		aimData.vecTargetShootPosition = refRecord.vecPoint;

		return refRecord.vecPoint;
	}
	return Vector(0, 0, 0);
}

bool CAimBot::AutoStop(std::vector<Lagcompensation::AnimationInfo_t*>& vecIn, CBaseEntity* pLocal, CUserCmd* pCmd, bool bSkipCheck) {

	static CConVar* weapon_accuracy_nospread = i::ConVar->FindVar(XorStr("weapon_accuracy_nospread"));
	if (!curConfig.bAutostop)
		return false;

	if (aimData.bCanShoot && !curConfig.bConditions[CONDITION_BETWEEN_SHOTS])
		return false;

	if (!pLocal->CanShoot(curConfig.pWeapon) && !curConfig.bConditions[CONDITION_BETWEEN_SHOTS])
		return false;

	if ((pCmd->iButtons & IN_JUMP || !(pLocal->GetFlags() & FL_ONGROUND)) && !curConfig.bConditions[CONDITION_INAIR])
		return false;

	if (weapon_accuracy_nospread->GetInt() == 1)
		return false;

	if (exploits::bIsShiftingTicks) {
		pCmd->flForwardMove = pCmd->flSideMove =0.f;
		return false;
	}

	if (!bSkipCheck) {

		float flDamage = -1.f;
		for (Lagcompensation::AnimationInfo_t* it : vecIn) {

			Vector vecHitboxPosition = vecIn.front()->pEntity->GetHitboxPosition(HITBOX_STOMACH, vecIn.front()->pRecord.front().pMatricies[RESOLVE]);
			flDamage = autowall.GetDamage(pLocal, vecEyePosition + (pLocal->GetVelocity() * (i::GlobalVars->flIntervalPerTick * 1)), vecHitboxPosition, curConfig.pWeapon, &vecIn.front()->pRecord.front());

			if (flDamage >= 1.f)
				break;
		}
		if (flDamage < 1.f)
			return false;
	}

	float flMultiplier = 0.28f;
	switch (curConfig.iAutostopValue) {
	case 1: flMultiplier = 0.26f;
		break;
	case 2: flMultiplier = 0.24f;
		break;
	case 3: flMultiplier = 0.20f;
		break;
	}

	float flIdealSpeed = (flMultiplier) * (pLocal->IsScoped() ? curConfig.pWeapon->GetCSWpnData()->flMaxSpeed[1] : curConfig.pWeapon->GetCSWpnData()->flMaxSpeed[0]);

	pCmd->iButtons &= ~IN_FORWARD | IN_BACK | IN_MOVELEFT | IN_MOVERIGHT;

	// Get the ideal speed for shooting (playstyle)
	Vector vecVelocity = pLocal->GetVelocity();
	Vector vecDirection;
	Vector vecRealView;

	if (flIdealSpeed > vecVelocity.Length2D())
		return false;

	M::VectorAngles(vecVelocity, vecDirection);
	i::EngineClient->GetViewAngles(vecRealView);

	vecDirection.y = vecRealView.y - vecDirection.y;

	Vector vecForward;
	M::AngleVectors(vecDirection, &vecForward);

	static CConVar* cl_forwardspeed = i::ConVar->FindVar(XorStr("cl_forwardspeed"));
	static CConVar* cl_sidespeed = i::ConVar->FindVar(XorStr("cl_sidespeed"));

	auto flNegativeForwardSpeed = -cl_forwardspeed->GetFloat();
	auto flNegativeSideSpeed = -cl_sidespeed->GetFloat();

	auto flNegativeForwardDirection = vecForward * flNegativeForwardSpeed;
	auto flNegativeSideDirection = vecForward * flNegativeSideSpeed;

	pCmd->flForwardMove = flNegativeForwardDirection.x;
	pCmd->flSideMove = flNegativeSideDirection.y;

	return true;
}

bool CAimBot::HitChance(CUserCmd* pCmd, CBaseEntity* pLocal, Vector vecWorldPosition, Vector vecAimPosition, Lagcompensation::LagRecord_t* pRecord) {

	matrix3x4a_t* pMatrix = pRecord->pMatricies[RESOLVE];

	if (curConfig.iHitchance <= 1)
		return true;

	CBaseCombatWeapon* pWeapon = curConfig.pWeapon;
	const CCSWeaponInfo* pWeaponData = curConfig.pWeapon->GetCSWpnData();
	const Vector vecShootPosition = vecEyePosition;
	const float flGetSpread = prediction.flSpread;
	const float flGetInaccuracy = prediction.flInaccuracy;

	if (vecShootPosition.DistTo(vecWorldPosition) > pWeaponData->flRange)
		return false;

	//if (HasEnoughAccuracy(pLocal, flGetInaccuracy))
	//	return true;

	Vector vecForward = Vector(0, 0, 0);
	Vector vecRight = Vector(0, 0, 0);
	Vector vecUp = Vector(0, 0, 0);

	M::AngleVectors(vecAimPosition, &vecForward, &vecRight, &vecUp);

	vecForward.Normalize();
	vecRight.Normalize();
	vecUp.Normalize();

	bool bSpecialWeapon = pWeapon->GetItemDefinitionIndex() == EItemDefinitionIndex::WEAPON_AWP || pWeapon->GetItemDefinitionIndex() == EItemDefinitionIndex::WEAPON_SSG08;
	int iAccuracry = bSpecialWeapon ? 256 : 128;

	static bool bSetupSpreadValues = true;
	static float flSpreadValues[256][6];

	if (bSetupSpreadValues)
	{
		bSetupSpreadValues = false;

		for (auto i = 0; i < iAccuracry; ++i)
		{
			M::RandomSeed(i + 1);

			float a = M::RandomFloat(0.0f, 1.0f);
			float b = M::RandomFloat(0.0f, 6.283185307f);
			float c = M::RandomFloat(0.0f, 1.0f);
			float d = M::RandomFloat(0.0f, 6.283185307f);

			flSpreadValues[i][0] = a;
			flSpreadValues[i][1] = c;

			auto flSinB = 0.0f, flCosB = 0.0f;
			M::SinCos(b, &flSinB, &flCosB);

			auto flSinD = 0.0f, flCosD = 0.0f;
			M::SinCos(b, &flSinD, &flCosD);

			flSpreadValues[i][2] = flSinB;
			flSpreadValues[i][3] = flCosB;
			flSpreadValues[i][4] = flSinD;
			flSpreadValues[i][5] = flCosD;
		}
	}

	int iHits = 0;
	int iAccuracyHits = 0;
	for (auto i = 0; i < iAccuracry; ++i)
	{
		float flInacc = flSpreadValues[i][0] * flGetInaccuracy;
		float flSpread = flSpreadValues[i][1] * flGetSpread;

		float flSpreadX = flSpreadValues[i][3] * flInacc + flSpreadValues[i][5] * flSpread;
		float flSpreadY = flSpreadValues[i][2] * flInacc + flSpreadValues[i][4] * flSpread;

		Vector vecDirection = Vector(0, 0, 0);
		vecDirection.x = vecForward.x + vecRight.x * flSpreadX + vecUp.x * flSpreadY;
		vecDirection.y = vecForward.y + vecRight.y * flSpreadX + vecUp.y * flSpreadY;
		vecDirection.z = vecForward.z + vecRight.z * flSpreadX + vecUp.z * flSpreadY;

		Vector vecEnd = vecEyePosition + vecDirection * pWeaponData->flRange;

		if (autowall.bCollidePoint(vecShootPosition, vecEnd, aimData.pRecord->pEntity->StudioHitbox(aimData.iHitbox), pMatrix))
			iHits++;

		//if (autowall.bTraceMeantForHitbox(vecShootPosition, vecEnd, aimData.iHitbox, pRecord))
		//	iAccuracyHits++;
	}

	int flFinalHitchance = static_cast<int>(iHits / (static_cast<float>(iAccuracry) / 100.f));
	//int flFinalAccuracyBoost = static_cast<int>(static_cast<float>(iAccuracyHits) / (iAccuracry / 100.f));

	/* Hitchance * accuracyboost% = how much shot MUST hit head */
	//int flHitchanceAccuracy = static_cast<int>(curConfig.iHitchance * (static_cast<float>(curConfig.iAccuracyBoost) / 100.f));
	//if (flFinalAccuracyBoost < flHitchanceAccuracy)
	//	return false;

	if (flFinalHitchance >= curConfig.iHitchance) {
		aimData.flHitchance = flFinalHitchance;
		return true;
	}

	return false;
}

bool CAimBot::HasEnoughAccuracy(CBaseEntity* pLocal, float flWeaponInAccuracy) {

	static CConVar* weapon_accuracy_nospread = i::ConVar->FindVar(XorStr("weapon_accuracy_nospread"));
	if (weapon_accuracy_nospread->GetInt() == 1)
		return true;

	CBaseCombatWeapon* pWeapon = curConfig.pWeapon;

	// dont spamshot please
	if (pWeapon->GetNextPrimaryAttack() == i::GlobalVars->flCurrentTime)
		return false;

	// jumpscout
	if (pWeapon->GetItemDefinitionIndex() == WEAPON_SSG08 || pWeapon->GetItemDefinitionIndex() == WEAPON_REVOLVER)
		if (!(pLocal->GetFlags() & FL_ONGROUND))
			if (flWeaponInAccuracy <= 0.009f)
				return true;

	return false;
}

weaponConfig_t CAimBot::GetWeaponConfiguration(short iItemDefinitionIndex) {

	using namespace cfg::rage;

	int iWeapon = OTHER;
	
	if (iItemDefinitionIndex == WEAPON_USP_SILENCER || iItemDefinitionIndex == WEAPON_HKP2000 || iItemDefinitionIndex == WEAPON_ELITE || iItemDefinitionIndex == WEAPON_P250 || iItemDefinitionIndex == WEAPON_FIVESEVEN || iItemDefinitionIndex == WEAPON_CZ75A || iItemDefinitionIndex == WEAPON_GLOCK || iItemDefinitionIndex == WEAPON_TEC9)
		iWeapon = PISTOL;
	else if (iItemDefinitionIndex == WEAPON_REVOLVER || iItemDefinitionIndex == WEAPON_DEAGLE)
		iWeapon = HEAVY_PISTOL;
	else if (iItemDefinitionIndex == WEAPON_SSG08)
		iWeapon = SCOUT;
	else if (iItemDefinitionIndex == WEAPON_AWP)
		iWeapon = AWP;
	else if (iItemDefinitionIndex == WEAPON_SCAR20 || iItemDefinitionIndex == WEAPON_G3SG1)
		iWeapon = AUTO;
	else if (iItemDefinitionIndex == WEAPON_TASER)
		iWeapon = ZEUS;

	weaponConfig_t ret{};

	ret.iMinimumDamage = (bOverride && IPT::HandleInput(iOverrideBind)) ? iOverride[iWeapon] : iMinDamages[iWeapon];
	ret.iHitchance = iHitchances[iWeapon];
	ret.iAccuracyBoost = iAccuracyBoost[iWeapon];
	ret.iHeadScale = iHeadPoints[iWeapon];
	ret.iBodyScale = iBodyPoints[iWeapon];
	ret.bSafePoint = bSafePoint[iWeapon];
	ret.bAutostop = bAutostop[iWeapon];
	ret.iAutostopValue = bAutostopAggressiveness[iWeapon];
	ret.bConditions[CONDITION_BETWEEN_SHOTS] = bConditions[iWeapon][CONDITION_BETWEEN_SHOTS];
	ret.bConditions[CONDITION_INAIR] = bConditions[iWeapon][CONDITION_INAIR];
	ret.bAutoScope = (iWeapon == SCOUT || iWeapon == AWP || iWeapon == AUTO) ? bAutoScope[iWeapon] : false;
	GetHitBoxes(NORMAL, ret.vecHitboxes[NORMAL], iWeapon);
	GetHitBoxes(MULTIPOINT, ret.vecHitboxes[MULTIPOINT], iWeapon);
	GetHitBoxes(SAFE, ret.vecHitboxes[SAFE], iWeapon);

	return ret;
}

void CAimBot::GetHitBoxes(int i, std::vector<int>& vecOut, int iWeapon) {

	if (i::ConVar->FindVar(XorStr("mp_damage_headshot_only"))->GetBool()) {
		vecOut.push_back(HITBOX_HEAD);
		return;
	}

	if (cfg::rage::bForceBaim && IPT::HandleInput(cfg::rage::iForceBaimKey)) {

		vecOut.push_back(HITBOX_STOMACH);
		vecOut.push_back(HITBOX_CHEST);
		return;
	}

	for (size_t iIndex = 0; iIndex < 6; iIndex++)
	{
		if (!cfg::rage::bHitboxes[i][iWeapon][iIndex])
			continue;

		switch (iIndex)
		{
		case 0:
			vecOut.push_back(HITBOX_HEAD);
			break;

		case 1:
			vecOut.push_back(HITBOX_UPPER_CHEST);
			break;

		case 2:
			vecOut.push_back(HITBOX_CHEST);
			break;

		case 3:
			vecOut.push_back(HITBOX_STOMACH);
			break;

		case 4:
			vecOut.push_back(HITBOX_RIGHT_FOREARM);
			vecOut.push_back(HITBOX_LEFT_FOREARM);
			break;

		case 5:
			vecOut.push_back(HITBOX_RIGHT_FOOT);
			vecOut.push_back(HITBOX_LEFT_FOOT);
			break;
		}
	}
}

std::vector<Lagcompensation::AnimationInfo_t*> CAimBot::GetTargetableEntities(CBaseEntity* pLocal) {

	std::vector<Lagcompensation::AnimationInfo_t*> ret{};

	CCSWeaponInfo* pWeaponData = curConfig.pWeapon->GetCSWpnData();
	if (!pWeaponData)
		return ret;

	for (size_t i = 1; i < i::GlobalVars->nMaxClients; i++) {

		CBaseEntity* pEntity = static_cast<CBaseEntity*>(i::EntityList->GetClientEntity(i));

		if (!pEntity || !pEntity->IsEnemy(pLocal) || !pEntity->IsAlive() || pEntity->IsDormant() || pEntity->HasImmunity())
			continue;

		Lagcompensation::AnimationInfo_t* pLog = &lagcomp.GetLog(i);
		if (!pLog || pLog->pRecord.empty() || pLog->pEntity != pEntity)
			continue;

		// OPTIMIZATION: skip entities that are further than our weapons range
		if (pLog->pEntity->GetVecOrigin().DistTo(pLocal->GetVecOrigin()) > pWeaponData->flRange)
			continue;

		if (playerList::arrPlayers[i].bWhiteList)
			continue;

		for (auto j = 0u; j < pLog->pRecord.size(); j++) {

			Lagcompensation::LagRecord_t* pCurrentRecord = &pLog->pRecord.at(j);

			if (pCurrentRecord->bValid = pCurrentRecord->IsValid(); pCurrentRecord->bValid)
				pLog->iLastValid = j;
		}

		ret.push_back(pLog);
	}

	if (!ret.empty())
		std::sort(ret.begin(), ret.end());

	return ret;
}

std::vector<Vector> CAimBot::CreatePoints(Vector vecEyePosition, CBaseCombatWeapon* pWeapon, Lagcompensation::LagRecord_t* pRecord, int iHitbox, EMatrixType iType, bool bShouldMultipoint) {

	std::vector<Vector> refVecPoints{};

	float flRadius = 0.f;
	Vector vecCenter = pRecord->pEntity->GetHitboxPosition(iHitbox, pRecord->pMatricies[iType], flRadius);

	if (flRadius < 0 || !bShouldMultipoint) 
		return { vecCenter };

	float flHitboxDistance = flRadius * ((iHitbox == HITBOX_HEAD ? curConfig.iHeadScale : curConfig.iBodyScale) * 0.01f);

	Vector vecCurrentAngles = M::VectorAngles(vecCenter - vecEyePosition);
	Vector vecForward; M::AngleVectors(vecCurrentAngles, &vecForward);

	const Vector vecRight = vecForward.CrossProduct(Vector(0, 0, 1));
	const Vector vecLeft = Vector(-vecRight.x, -vecRight.y, vecRight.z);
	const Vector vecTop = Vector(0, 0, 1);

	if (iHitbox == HITBOX_HEAD) {
		refVecPoints.push_back(vecCenter);
		refVecPoints.push_back(vecCenter + vecTop * flHitboxDistance);

		refVecPoints.push_back(vecCenter + (vecTop * flHitboxDistance) + (vecLeft * (flHitboxDistance * 0.65f)));
		refVecPoints.push_back(vecCenter + (vecTop * flHitboxDistance) + (vecRight * (flHitboxDistance * 0.65f)));

		refVecPoints.push_back(vecCenter - (vecTop * flHitboxDistance) + (vecLeft * (flHitboxDistance * 0.65f)));
		refVecPoints.push_back(vecCenter - (vecTop * flHitboxDistance) + (vecRight * (flHitboxDistance * 0.65f)));
	}
	refVecPoints.push_back(vecCenter + vecLeft * flHitboxDistance);
	refVecPoints.push_back(vecCenter + vecRight * flHitboxDistance);
	refVecPoints.push_back(vecCenter);

	return refVecPoints;
}
