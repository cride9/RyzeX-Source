#include "EnemyAnimations.h"
#include "../../../SDK/Menu/config.h"
#include "../autowall.h"
#include "../../Misc/Playerlist.h"
#include "../../Networking/networking.h"
#include "../aimbot.h"
#include "../../Visuals/drawlist.h"

float flOldLowerbodyYaw[65];
float flOldPlaybackrateYaw[65];

bool Animations::NewDataRecievedFromServer(CBaseEntity* pPlayer)
{
	return pPlayer->GetSimulationTime() != pPlayer->GetOldSimulationTime();
}

void Animations::UpdateClientSideAnimations(CBaseEntity* pEntity, Lagcompensation::LagRecord_t* pRecord)
{
	CAnimState* pState = pEntity->AnimState();

	pState->iLastUpdateFrame = 0;
	if (pState->flLastUpdateTime == i::GlobalVars->flCurrentTime)
		pState->flLastUpdateTime = i::GlobalVars->flCurrentTime + i::GlobalVars->flIntervalPerTick;

	pState->pEntity = pEntity;
	pState->pLastActiveWeapon = pEntity->GetWeapon();

	for (int iLayer = 0; iLayer < ANIMATION_LAYER_COUNT; iLayer++) {
		CAnimationLayer* pLayer = &pEntity->GetAnimationOverlays()[iLayer];
		if (!pLayer)
			continue;

		pLayer->pOwner = pEntity;
	}

	bool bClientSideAnimation = pEntity->IsClientSideAnimation();
	pEntity->IsClientSideAnimation() = true;

	g::bAllowAnimations[pEntity->EntIndex()] = true;
	pEntity->UpdateClientSideAnimations();
	g::bAllowAnimations[pEntity->EntIndex()] = false;

	pEntity->IsClientSideAnimation() = bClientSideAnimation;
	pEntity->InvalidatePhysicsRecursive(ANIMATION_CHANGED | ANGLES_CHANGED | POSITION_CHANGED);
}

float Animations::GetYawRotation(Lagcompensation::LagRecord_t* pRecord, int nRotationSide) {

	float flOldEyeYaw = pRecord->pEntity->AnimState()->flEyeYaw;

	// set eye yaw
	float flEyeRotation = pRecord->vecEyeAngles.y;
	switch (nRotationSide)
	{
	case LEFT: pRecord->pEntity->AnimState()->flEyeYaw = M::NormalizeAngle(flEyeRotation - pRecord->pEntity->AnimState()->GetMaxDesync()); break;
	case CENTER: pRecord->pEntity->AnimState()->flEyeYaw = M::NormalizeAngle(flEyeRotation); break;
	case RIGHT: pRecord->pEntity->AnimState()->flEyeYaw = M::NormalizeAngle(flEyeRotation + pRecord->pEntity->AnimState()->GetMaxDesync()); break;
	}

	// generate foot yaw
	float flFootYaw = 0.0f;
	flFootYaw = BuildFootYaw(pRecord->pEntity, pRecord);

	//switch (nRotationSide)
	//{
	//	case LEFT: flFootYaw = M::NormalizeAngle(flEyeRotation - 58.f); break;
	//	case CENTER: flFootYaw = M::NormalizeAngle(flEyeRotation); break;
	//	case RIGHT: flFootYaw = M::NormalizeAngle(flEyeRotation + 58.f); break;
	//}

	// restore eye yaw                                   
	pRecord->pEntity->AnimState()->flEyeYaw = flOldEyeYaw;

	//return result
	return M::NormalizeYaw(flFootYaw);
}

void Animations::GenerateSafePointMatricies(CBaseEntity* pEntity, Lagcompensation::LagRecord_t* pRecord, Lagcompensation::LagRecord_t* pPrevious) {

	// point building func
	auto BuildSafePoint = [&](int nRotationSide)
	{
		// save animation data
		std::array < CAnimationLayer, 13 > arrLayers;
		std::array < float, 24 > arrPoses;
		CAnimState pAnimationState;

		// copy data
		std::memcpy(arrLayers.data(), pEntity->GetAnimationOverlays(), sizeof(CAnimationLayer) * 13);
		std::memcpy(arrPoses.data(), pEntity->GetPoseParameter().data(), sizeof(float) * 24);
		std::memcpy(&pAnimationState, pEntity->AnimState(), sizeof(CAnimState));

		pEntity->AnimState()->flGoalFeetYaw = GetYawRotation(pRecord, nRotationSide);

		UpdateClientSideAnimations(pEntity, pRecord);

		matrix3x4a_t* aMatrix = pRecord->pMatricies[nRotationSide];

		SetupPlayerMatrix(pEntity, pRecord, aMatrix, BoneUsedByHitbox);

		std::memcpy(pEntity->GetAnimationOverlays(), arrLayers.data(), sizeof(CAnimationLayer) * 13);
		std::memcpy(pEntity->GetPoseParameter().data(), arrPoses.data(), sizeof(float) * 24);
		std::memcpy(pEntity->AnimState(), &pAnimationState, sizeof(CAnimState));
	};

	// check conditions
	if (!g::pLocal || !g::pLocal->IsAlive())
		return;

	// build safe points
	BuildSafePoint(LEFT);
	BuildSafePoint(CENTER);
	BuildSafePoint(RIGHT);
}

float Animations::BuildFootYaw(CBaseEntity* pEntity, Lagcompensation::LagRecord_t* pRecord) {

	CAnimState* pAnimationState = pEntity->AnimState();
	if (!pAnimationState)
		return 0.0f;

	float flAimMatrixWidthRange = M::Lerp(std::clamp(pAnimationState->flRunningSpeed, 0.f, 1.f), 1.0f, M::Lerp(pAnimationState->flWalkToRunTransition, 0.8f, 0.5f));
	if (pAnimationState->flDuckAmount > 0)
		flAimMatrixWidthRange = M::Lerp(pAnimationState->flDuckAmount * std::clamp(pAnimationState->flDuckingSpeed, 0.0f, 1.0f), flAimMatrixWidthRange, 0.5f);

	float flTempYawMax = pAnimationState->flMaxBodyYaw * flAimMatrixWidthRange;
	float flTempYawMin = pAnimationState->flMinBodyYaw * flAimMatrixWidthRange;

	float flFootYaw = M::NormalizeAngle(pRecord->pEntity->AnimState()->flEyeYaw);

	float flEyeFootDelta = M::AngleDiff(pRecord->vecEyeAngles.y, flFootYaw);
	if (flEyeFootDelta > flTempYawMax)
		flFootYaw = pRecord->vecEyeAngles.y - fabs(flTempYawMax);
	else if (flEyeFootDelta < flTempYawMin)
		flFootYaw = pRecord->vecEyeAngles.y + fabs(flTempYawMin);

	if (pAnimationState->bOnGround) {

		if (pAnimationState->flVelocityLenght2D > 0.1f || pAnimationState->flJumpFallVelocity > 100.0f)
			flFootYaw = M::ApproachAngle(pRecord->pEntity->AnimState()->flEyeYaw, flFootYaw, i::GlobalVars->flIntervalPerTick * (30.0f + (20.0f * pAnimationState->flWalkToRunTransition)));
		else
			flFootYaw = M::ApproachAngle(pRecord->flLowerBodyYawTarget, flFootYaw, i::GlobalVars->flIntervalPerTick * 100.f);
	}

	return M::NormalizeAngle(flFootYaw);
}

bool Animations::CopyCachedMatrix(CBaseEntity* pEnt, matrix3x4a_t* pMatrix, int nBoneCount) {

	if (!g::pLocal || g::bUpdatingSkins)
		return false;

	Lagcompensation::AnimationInfo_t* pLog = &lagcomp.GetLog(pEnt->EntIndex());

	if (!pLog)
		return false;

	std::memcpy(pMatrix, pLog->pCachedMatrix.data(), sizeof(matrix3x4a_t) * nBoneCount);
	return true;
}

void Animations::InterpolateMatricies(CBaseEntity* pEntity) {

	if (!exploits::bIsShiftingTicks)
		localAnim->InterpolateMatricies();
	for (int nPlayerID = 1; nPlayerID <= 64; nPlayerID++)
	{
		CBaseEntity* pPlayer = static_cast<CBaseEntity*>(i::EntityList->GetClientEntity(nPlayerID));
		if (!g::pLocal || !pPlayer || pPlayer == g::pLocal || !pPlayer->IsAlive() || !pPlayer->IsEnemy(g::pLocal))
			continue;

		auto pPlayerData = &lagcomp.GetLog(nPlayerID);
		if (!pPlayerData || pPlayerData->pRecord.empty())
			continue;

		// get bone count
		int nBoneCount = pPlayer->GetCachedBoneData().Count();
		if (nBoneCount > MAXSTUDIOBONES)
			nBoneCount = MAXSTUDIOBONES;

		// re-pos matrix
		TransformateMatrix(pPlayer);

		// copy the entire matrix
		pPlayer->SetBoneCache(pPlayerData->pCachedMatrix.data());

		// build attachments
		std::memcpy(pPlayer->GetBoneAccessor().matBones, pPlayerData->pCachedMatrix.data(), sizeof(matrix3x4a_t) * nBoneCount);
		pPlayer->SetupBones_AttachmentHelper();
		std::memcpy(pPlayer->GetBoneAccessor().matBones, pPlayerData->pCachedMatrix.data(), sizeof(matrix3x4a_t) * nBoneCount);
	}
}

void Animations::TransformateMatrix(CBaseEntity* pEnt) {

	auto pRecord = &lagcomp.GetLog(pEnt->EntIndex());
	if (!pRecord || pRecord->pEntity != pEnt)
		return;	

	static std::array<Vector, 65> vecLastOrigin = {Vector(0, 0, 0)};
	if (vecLastOrigin[pEnt->EntIndex()] == Vector(0, 0, 0))
		vecLastOrigin[pEnt->EntIndex()] = pEnt->GetAbsOrigin();

	Vector vecOriginDelta = pEnt->GetAbsOrigin() - vecLastOrigin[pEnt->EntIndex()];

	for (auto& Matrix : pRecord->pCachedMatrix)
	{
		Matrix[0][3] += vecOriginDelta.x;
		Matrix[1][3] += vecOriginDelta.y;
		Matrix[2][3] += vecOriginDelta.z;
	}

	vecLastOrigin[pEnt->EntIndex()] = pEnt->GetAbsOrigin();
}

void Animations::SetUpMovement(Lagcompensation::LagRecord_t* pRecord, CAnimState* pState, Lagcompensation::LagRecord_t::LayerData_t* pLayer) {

	CBaseEntity* m_pPlayer = pRecord->pEntity;

	if (pState->flWalkToRunTransition > 0 && pState->flWalkToRunTransition < 1) {

		if (pState->bWalkToRunTransitionState == ANIM_TRANSITION_WALK_TO_RUN)
			pState->flWalkToRunTransition += pState->flLastUpdateIncrement * CSGO_ANIM_WALK_TO_RUN_TRANSITION_SPEED;
		
		else
			pState->flWalkToRunTransition -= pState->flLastUpdateIncrement * CSGO_ANIM_WALK_TO_RUN_TRANSITION_SPEED;
		
		pState->flWalkToRunTransition = std::clamp(pState->flWalkToRunTransition, 0.f, 1.f);
	}

	if (pRecord->vecVelocity.Length2D() > (CS_PLAYER_SPEED_RUN * CS_PLAYER_SPEED_WALK_MODIFIER) && pState->flWalkToRunTransition == ANIM_TRANSITION_RUN_TO_WALK) {
		
		//crossed the walk to run threshold
		pState->flWalkToRunTransition = ANIM_TRANSITION_WALK_TO_RUN;
		pState->flWalkToRunTransition = max(0.01f, pState->flWalkToRunTransition);
	}
	else if (pRecord->vecVelocity.Length2D() < (CS_PLAYER_SPEED_RUN * CS_PLAYER_SPEED_WALK_MODIFIER) && pState->flWalkToRunTransition == ANIM_TRANSITION_WALK_TO_RUN) {
		
		//crossed the run to walk threshold
		pState->flWalkToRunTransition = ANIM_TRANSITION_RUN_TO_WALK;
		pState->flWalkToRunTransition = min(0.99f, pState->flWalkToRunTransition);
	}
	char szWeaponMoveSeq[64];
	sprintf_s(szWeaponMoveSeq, XorStr("move_%s"), pState->GetWeaponPrefix());

	int nWeaponMoveSeq = m_pPlayer->LookupSequence(szWeaponMoveSeq);
	if (nWeaponMoveSeq == -1)
		nWeaponMoveSeq = m_pPlayer->LookupSequence(XorStr("move"));

	if (m_pPlayer->GetMoveState() != pState->nPreviousMoveState)
		pState->flStutterStep += 10;
	
	pState->nPreviousMoveState = m_pPlayer->GetMoveState();
	pState->flStutterStep = std::clamp(M::Approach(0.f, pState->flStutterStep, pState->flLastUpdateIncrement * 40), 0.f, 100.f);

	// recompute moveweight
	float flTargetMoveWeight = M::Lerp(pState->flDuckAmount, std::clamp(pState->flRunningSpeed, 0.f, 1.f), std::clamp(pState->flDuckingSpeed, 0.f, 1.f));

	if (pState->flMoveWeight <= flTargetMoveWeight) 
		pState->flMoveWeight = flTargetMoveWeight;
	else
		pState->flMoveWeight = M::Approach(flTargetMoveWeight, pState->flMoveWeight, pState->flLastUpdateIncrement * M::RemapValClamped(pState->flStutterStep, 0.0f, 100.0f, 2, 20));
	
	Vector vecMoveYawDir;
	M::AngleVectors(Vector(0, M::NormalizeAngle(pState->flGoalFeetYaw + pState->flMoveYaw + 180), 0), &vecMoveYawDir);
	float flYawDeltaAbsDot = abs(M::DotProduct(pState->vecVelocityNormalizedNonZero, vecMoveYawDir));
	pState->flMoveWeight *= M::Bias(flYawDeltaAbsDot, 0.2);

	float flMoveWeightWithAirSmooth = pState->flMoveWeight * pState->flInAirSmoothValue;

	// dampen move weight for landings
	flMoveWeightWithAirSmooth *= max((1.0f - pRecord->arrLayers[ANIMATION_LAYER_MOVEMENT_LAND_OR_CLIMB].flWeight), 0.55f);

	float flMoveCycleRate = 0;
	if (pRecord->vecVelocity.Length2D() > 0) {

		flMoveCycleRate = m_pPlayer->GetSequenceCycleRate(m_pPlayer->GetModelPtr(), nWeaponMoveSeq);
		float flSequenceGroundSpeed = max(m_pPlayer->GetSequenceMoveDist(m_pPlayer->GetModelPtr(), nWeaponMoveSeq) / (1.0f / flMoveCycleRate), 0.001f);
		flMoveCycleRate *= pRecord->vecVelocity.Length2D() / flSequenceGroundSpeed;

		flMoveCycleRate *= M::Lerp(pState->flWalkToRunTransition, 1.0f, CSGO_ANIM_RUN_ANIM_PLAYBACK_MULTIPLIER);
	}

	float flLocalCycleIncrement = (flMoveCycleRate * pState->flLastUpdateIncrement);
	pState->flFeetCycle = std::clamp(pState->flFeetCycle + flLocalCycleIncrement, 0.0f, 0.999f);

	flMoveWeightWithAirSmooth = std::clamp(flMoveWeightWithAirSmooth, 0.f, 1.f);

	pLayer->nSequence = nWeaponMoveSeq;
	pLayer->flPlaybackRate = flLocalCycleIncrement;
	pLayer->flWeight = flMoveWeightWithAirSmooth;
	pLayer->flCycle = pState->flFeetCycle;
}

void Animations::RebuiltLayer6(CAnimState* m_pState, Lagcompensation::LagRecord_t* pEntity, Lagcompensation::LagRecord_t::LayerData_t* pLayer) {

	// rebuilt layer 6 calculations from csgo
	// links:
	// https://github.com/perilouswithadollarsign/cstrike15_src/blob/master/game/shared/cstrike15/csgo_playeranimstate.cpp#L1393
	// https://github.com/click4dylan/CSGO_AnimationCode_Reversed/blob/master/CCSGOPlayerAnimState_New.cpp#L2323
	// TODO: make code look good

	// TODO: Find these members in the actual animstate struct
	float m_flLastUpdateIncrement = m_pState->flLastUpdateIncrement;
	float m_flFootYaw = m_pState->flGoalFeetYaw;
	float m_flMoveYaw = m_pState->flMoveYaw;
	Vector m_vecVelocityNormalizedNonZero = m_pState->vecVelocityNormalizedNonZero;
	float m_flInAirSmoothValue = m_pState->flInAirSmoothValue;
	AnimationData_t& m_AnimationData = pAnimationData[pEntity->iEntIndex];

	char m_szDestination[64];
	sprintf_s(m_szDestination, XorStr("move_%s"), m_pState->GetWeaponPrefix());

	int m_nMoveSequence = pEntity->pEntity->LookupSequence(m_szDestination);
	if (m_nMoveSequence == -1)
	{
		m_nMoveSequence = pEntity->pEntity->LookupSequence(XorStr("move"));
	}

	// NOTE: 
	// pEntity->get<int>( 0x3984 ) is m_iMoveState 
	if (pEntity->pEntity->GetMoveState() != m_AnimationData.iMoveState)
		m_AnimationData.flMovePlaybackRate += 10.0f;

	m_AnimationData.iMoveState = pEntity->pEntity->GetMoveState();

	float m_flMovementTimeDelta = m_pState->flLastUpdateIncrement * 40.0f;

	if (-m_AnimationData.flMovePlaybackRate <= m_flMovementTimeDelta)
	{
		if (-m_flMovementTimeDelta <= -m_AnimationData.flMovePlaybackRate)
			m_AnimationData.flMovePlaybackRate = 0.0f;
		else
			m_AnimationData.flMovePlaybackRate = m_AnimationData.flMovePlaybackRate - m_flMovementTimeDelta;
	}
	else
	{
		m_AnimationData.flMovePlaybackRate = m_AnimationData.flMovePlaybackRate + m_flMovementTimeDelta;
	}

	m_AnimationData.flMovePlaybackRate = std::clamp(m_AnimationData.flMovePlaybackRate, 0.0f, 100.0f);

	float m_flDuckSpeedClamped = std::clamp(m_pState->flDuckingSpeed, 0.0f, 1.0f);
	float m_flRunSpeedClamped = std::clamp(m_pState->flRunningSpeed, 0.0f, 1.0f);

	float m_flSpeedWeight = ((m_flDuckSpeedClamped - m_flRunSpeedClamped) * pEntity->flDuck) + m_flRunSpeedClamped;

	if (m_flSpeedWeight < pLayer->flFeetWeight)
	{
		float v34 = std::clamp(m_AnimationData.flMovePlaybackRate * 0.01f, 0.0f, 1.0f);
		float m_flFeetWeightElapsed = ((v34 * 18.0f) + 2.0f) * m_pState->flLastUpdateIncrement;
		if (m_flSpeedWeight - pLayer->flFeetWeight <= m_flFeetWeightElapsed)
			pLayer->flFeetWeight = -m_flFeetWeightElapsed <= (m_flSpeedWeight - pLayer->flFeetWeight) ? m_flSpeedWeight : pLayer->flFeetWeight - m_flFeetWeightElapsed;
		else
			pLayer->flFeetWeight = m_flFeetWeightElapsed + pLayer->flFeetWeight;
	}
	else
	{
		pLayer->flFeetWeight = m_flSpeedWeight;
	}

	float m_flYaw = M::NormalizeAngle((m_pState->flMoveYaw + m_pState->flGoalFeetYaw) + 180.0f);
	Vector m_angAngle = { 0.0f, m_flYaw, 0.0f };
	Vector m_vecDirection;
	M::AngleVectors(m_angAngle, &m_vecDirection);

	float m_flMovementSide = M::DotProduct(m_vecVelocityNormalizedNonZero, m_vecDirection);
	if (m_flMovementSide < 0.0f)
		m_flMovementSide = -m_flMovementSide;

	float m_flNewFeetWeight = M::Bias(m_flMovementSide, 0.2f) * pLayer->flFeetWeight;

	float m_flNewFeetWeightWithAirSmooth = m_flNewFeetWeight * m_flInAirSmoothValue;

	// m_flLayer5Weight looks a bit weird so i decided to name it m_flLayer5_Weight instead.
	float m_flLayer5_Weight = pEntity->pEntity->GetAnimationOverlays()[5].flWeight;

	float m_flNewWeight = 0.55f;
	if (1.0f - m_flLayer5_Weight > 0.55f)
		m_flNewWeight = 1.0f - m_flLayer5_Weight;

	float m_flNewFeetWeightLayerWeight = m_flNewWeight * m_flNewFeetWeightWithAirSmooth;
	float m_flFeetCycleRate = 0.0f;

	float m_flSpeed = std::fmin(GetVelocityLengthXY(pEntity->pEntity), 260.0f);
	if (m_flSpeed > 0.00f)
	{
		float m_flSequenceCycleRate = pEntity->pEntity->GetSequenceCycleRate(pEntity->pEntity->GetModelPtr(), m_nMoveSequence);

		float m_flSequenceMoveDist = pEntity->pEntity->GetSequenceMoveDist(pEntity->pEntity->GetModelPtr(), m_nMoveSequence);
		// float flSequenceGroundSpeed = MAX( m_pPlayer->GetSequenceMoveDist( m_pPlayer->GetModelPtr(), nWeaponMoveSeq ) / ( 1.0f / flMoveCycleRate ), 0.001f );
		m_flSequenceMoveDist *= 1.0f / (1.0f / m_flSequenceCycleRate);
		if (m_flSequenceMoveDist <= 0.001f)
			m_flSequenceMoveDist = 0.001f;

		//float flSequenceGroundSpeed = max(pEntity->GetSequenceMoveDist(pEntity->GetModelPtr(), m_nMoveSequence) / (1.0f / m_flFeetCycleRate), 0.001f);
		//m_flFeetCycleRate *= vecVelocity.Length2D() / flSequenceGroundSpeed;
		//m_flFeetCycleRate *= M::Lerp(pEntity->AnimState()->flWalkToRunTransition, 1.0f, CSGO_ANIM_RUN_ANIM_PLAYBACK_MULTIPLIER);

		float m_flSpeedMultiplier = m_flSpeed / m_flSequenceMoveDist;
		m_flFeetCycleRate = (1.0f - (m_pState->flWalkToRunTransition * 0.15f)) * (m_flSpeedMultiplier * m_flSequenceCycleRate);
	}

	float m_flFeetCyclePlaybackRate = m_pState->flLastUpdateIncrement * m_flFeetCycleRate;
	m_AnimationData.flPrimaryCycle = m_flFeetCyclePlaybackRate + m_AnimationData.flPrimaryCycle;

	// store possible information for resolving.
	pLayer->flMovementSide = m_flMovementSide;
	pLayer->angMoveYaw = m_angAngle;
	pLayer->vecDirection = m_vecDirection;
	pLayer->flFeetWeight = m_flNewFeetWeight;

	// maybe it can be used for something, keeping it just in case.
	pLayer->nSequence = m_nMoveSequence;
	pLayer->flPlaybackRate = m_flFeetCyclePlaybackRate;
	pLayer->flCycle = m_AnimationData.flPrimaryCycle;
	pLayer->flFeetWeight = std::clamp(m_flNewFeetWeightLayerWeight, 0.0f, 1.0f);
}

Vector DeterminePlayerVelocity(CBaseEntity* pEntity, Lagcompensation::LagRecord_t* pRecord, Lagcompensation::LagRecord_t* pPrevious, CAnimState* pState) {

	/* Prepare data once */
	if (!pPrevious)
	{
		const float flVelLength = pRecord->vecVelocity.Length();
		if (flVelLength > pRecord->flMaxSpeed)
			pRecord->vecVelocity *= pRecord->flMaxSpeed / flVelLength;

		return pRecord->vecVelocity;
	}

	/* Define const */
	static CConVar* sv_maxspeed = i::ConVar->FindVar(XorStr("sv_maxspeed"));
	const float flMaxSpeed = sv_maxspeed->GetFloat();

	/* Get animation layers */
	const CAnimationLayer* AliveLoop = &pRecord->arrLayers[ANIMATION_LAYER_ALIVELOOP];
	const CAnimationLayer* PrevAliveLoop = &pPrevious->arrLayers[ANIMATION_LAYER_ALIVELOOP];
	const CAnimationLayer* Movement = &pRecord->arrLayers[ANIMATION_LAYER_MOVEMENT_MOVE];
	const CAnimationLayer* PrevMovement = &pPrevious->arrLayers[ANIMATION_LAYER_MOVEMENT_MOVE];
	const CAnimationLayer* Landing = &pRecord->arrLayers[ANIMATION_LAYER_MOVEMENT_LAND_OR_CLIMB];
	const CAnimationLayer* PrevLanding = &pPrevious->arrLayers[ANIMATION_LAYER_MOVEMENT_LAND_OR_CLIMB];

	/* Recalculate velocity using origin delta */
	pRecord->vecVelocity = (pRecord->vecOrigin - pPrevious->vecOrigin) * (1.0f / TICKS_TO_TIME(pRecord->iChoked));

	/* Check PlaybackRate */
	if (Movement->flPlaybackRate < 0.00001f)
		pRecord->vecVelocity.x = pRecord->vecVelocity.y = 0.0f;
	else
	{
		/* Compute velocity using flSpeedAsPortionOfRunTopSpeed */
		float flWeight = AliveLoop->flWeight;
		if (flWeight < 1.0f)
		{
			/* Check PlaybackRate */
			if (AliveLoop->flPlaybackRate == PrevAliveLoop->flPlaybackRate)
			{
				/* Check Sequence */
				if (AliveLoop->nSequence == PrevAliveLoop->nSequence)
				{
					/* Very important cycle check */
					if (AliveLoop->flCycle > PrevAliveLoop->flCycle)
					{
						/* Check weapon */
						if (pState->pActiveWeapon == pEntity->GetWeapon())
						{
							/* Get flSpeedAsPortionOfRunTopSpeed */
							float flSpeedAsPortionOfRunTopSpeed = ((1.0f - flWeight) / 2.8571432f) + 0.55f;

							/* Check flSpeedAsPortionOfRunTopSpeed bounds ( from 55% to 90% from the speed ) */
							if (flSpeedAsPortionOfRunTopSpeed > 0.55f && flSpeedAsPortionOfRunTopSpeed < 0.9f)
							{
								/* Compute velocity */
								pRecord->flAnimationVelocity = flSpeedAsPortionOfRunTopSpeed * pRecord->flMaxSpeed;
								pRecord->nVelocityMode = EFixedVelocity::AliveLoopLayer;
							}
							else if (flSpeedAsPortionOfRunTopSpeed > 0.9f)
							{
								/* Compute velocity */
								pRecord->flAnimationVelocity = pRecord->vecVelocity.Length2D();
							}
						}
					}
				}
			}
		}

		/* Compute velocity using Movement ( 6 ) weight  */
		if (pRecord->flAnimationVelocity <= 0.0f)
		{
			/* Check Weight bounds from 10% to 90% from the speed */
			float flWeight = Movement->flWeight;
			if (flWeight > 0.1f && flWeight < 0.9f)
			{
				/* Skip on land */
				if (Landing->flWeight <= 0.0f)
				{
					/* Check Accelerate */
					if (flWeight > PrevMovement->flWeight)
					{
						/* Skip on direction switch */
						if (pRecord->arrLayers[ANIMATION_LAYER_MOVEMENT_STRAFECHANGE].nSequence == pPrevious->arrLayers[ANIMATION_LAYER_MOVEMENT_STRAFECHANGE].nSequence)
						{
							/* Check move sequence */
							if (Movement->nSequence == PrevMovement->nSequence)
							{
								/* Check land sequence */
								if (Landing->nSequence == PrevLanding->nSequence)
								{
									/* Check stand sequence */
									if (pRecord->arrLayers[ANIMATION_LAYER_ADJUST].nSequence == pPrevious->arrLayers[ANIMATION_LAYER_ADJUST].nSequence)
									{
										/* Check Flags */
										if (pRecord->iFlags & FL_ONGROUND)
										{
											/* Compute MaxSpeed modifier */
											float flSpeedModifier = 1.0f;
											if (pRecord->iFlags & FL_DUCKING)
												flSpeedModifier = CS_PLAYER_SPEED_DUCK_MODIFIER;
											else if (pRecord->bFakewalking)
												flSpeedModifier = CS_PLAYER_SPEED_WALK_MODIFIER;

											/* Compute Velocity ( THIS CODE ONLY WORKS IN DUCK AND WALK ) */
											if (flSpeedModifier < 1.0f)
											{
												pRecord->flAnimationVelocity = (flWeight * (pRecord->flMaxSpeed * flSpeedModifier));
												pRecord->nVelocityMode = EFixedVelocity::MovementLayer;
											}
										}
									}
								}
							}
						}
					}
				}
			}
		}
	}

	/* Compute velocity from Record->flAnimationVelocity floating point */
	if (pRecord->flAnimationVelocity > 0.0f)
	{
		const float flModifier = pRecord->flAnimationVelocity / pRecord->vecVelocity.Length2D();
		pRecord->vecVelocity.x *= flModifier;
		pRecord->vecVelocity.y *= flModifier;
	}

	/* Prepare data once */
	const float flVelLength = pRecord->vecVelocity.Length();

	/* Clamp velocity if its out bounds */
	if (flVelLength > pRecord->flMaxSpeed)
		pRecord->vecVelocity *= pRecord->flMaxSpeed / flVelLength;

	return pRecord->vecVelocity;
}

int DetermineAnimationCycle(CBaseEntity* pEntity, Lagcompensation::LagRecord_t* pRecord, Lagcompensation::LagRecord_t* pPrevious) {

	const CAnimationLayer* pAliveLoop = &pRecord->arrLayers[ANIMATION_LAYER_ALIVELOOP];
	const CAnimationLayer* pPrevAliveLoop = &pPrevious->arrLayers[ANIMATION_LAYER_ALIVELOOP];

	/* Get ticks animated on the server ( by default it's simtime delta ) */
	int nTicksAnimated = pRecord->iChoked;
	if (pAliveLoop->flPlaybackRate == pPrevAliveLoop->flPlaybackRate)
		nTicksAnimated = (pAliveLoop->flCycle - pPrevAliveLoop->flCycle) / (pAliveLoop->flPlaybackRate * i::GlobalVars->flIntervalPerTick);
	else
		nTicksAnimated = ((((pAliveLoop->flCycle / pAliveLoop->flPlaybackRate) + ((1.0f - pPrevAliveLoop->flCycle) / pPrevAliveLoop->flPlaybackRate)) / i::GlobalVars->flIntervalPerTick));

	return min(max(nTicksAnimated, pRecord->iChoked), 17);
}

Lagcompensation::LagRecord_t* SetupData(CBaseEntity* pEntity, Lagcompensation::LagRecord_t* pRecord, Lagcompensation::AnimationInfo_t* pLog) {

	if (pRecord->bFirstAfterDormant)
		return nullptr;

	if (pLog->pRecord.size() < 2)
		return nullptr;

	auto* pPrevious = &pLog->pRecord.at(1);
	pPrevious->iChoked = DetermineAnimationCycle(pEntity, pRecord, pPrevious);
	if (TIME_TO_TICKS(pRecord->flSimulationTime - pPrevious->flSimulationTime) > 17)
		return nullptr;

	return pPrevious;
}

void CopyRecordData(CBaseEntity* pEntity, Lagcompensation::LagRecord_t* pRecord, Lagcompensation::LagRecord_t* pPrevious, CAnimState* pState) {

	Lagcompensation::LagRecord_t* pForceRecord = pRecord;
	if (pPrevious)
		pForceRecord = pPrevious;

	CAnimationLayer* pStrafeLayer = &pForceRecord->arrLayers[7];
	pState->flStrafeChangeCycle = pStrafeLayer->flCycle;
	pState->flStrafeChangeWeight = pStrafeLayer->flWeight;
	pState->nStrafeSequence = pStrafeLayer->nSequence;
	pState->flFeetCycle = pForceRecord->arrLayers[6].flCycle;
	pState->flMoveWeight = pForceRecord->arrLayers[6].flWeight;
	pState->flAccelerationWeight = pForceRecord->arrLayers[12].flWeight;
	pEntity->SetAnimationLayers(pForceRecord->arrLayers);
}

void UpdateCollision(CBaseEntity* pEntity, Lagcompensation::LagRecord_t* pRecord) {

	ICollideable* pCollideable = pEntity->GetCollideable();
	if (!pCollideable)
		return;

	pEntity->UpdateCollisionBounds();
	pRecord->vecMins = pCollideable->OBBMins();
	pRecord->vecMaxs = pCollideable->OBBMaxs();
}

void SimulatePlayerActivity(CBaseEntity* pEntity, Lagcompensation::LagRecord_t* pRecord, Lagcompensation::LagRecord_t* pPrevious) {

	const CAnimationLayer* pJumpingLayer = &pRecord->arrLayers[ANIMATION_LAYER_MOVEMENT_JUMP_OR_FALL];
	const CAnimationLayer* pLandingLayer = &pRecord->arrLayers[ANIMATION_LAYER_MOVEMENT_LAND_OR_CLIMB];
	const CAnimationLayer* pPrevJumpingLayer = &pPrevious->arrLayers[ANIMATION_LAYER_MOVEMENT_JUMP_OR_FALL];
	const CAnimationLayer* pPrevLandingLayer = &pPrevious->arrLayers[ANIMATION_LAYER_MOVEMENT_LAND_OR_CLIMB];

	const int nJumpingActivity = pEntity->GetSequenceActivity(pJumpingLayer->nSequence);
	const int nLandingActivity = pEntity->GetSequenceActivity(pLandingLayer->nSequence);

	if (nJumpingActivity == ACT_CSGO_JUMP) {
		if (pJumpingLayer->flWeight > 0.0f && pJumpingLayer->flPlaybackRate > 0.0f) {
			if (pJumpingLayer->flCycle < pPrevJumpingLayer->flCycle) {
				pRecord->flDurationInAir = pJumpingLayer->flCycle / pJumpingLayer->flPlaybackRate;
				if (pRecord->flDurationInAir > 0.0f) {
					pRecord->iActivityTick = TIME_TO_TICKS(pRecord->flSimulationTime - pRecord->flDurationInAir) + 1;
					pRecord->iActivityType = Jump;
				}
			}
		}
	}

	if (nLandingActivity == ACT_CSGO_LAND_LIGHT || nLandingActivity == ACT_CSGO_LAND_HEAVY) {
		if (pLandingLayer->flWeight > 0.0f && pPrevLandingLayer->flWeight <= 0.0f) {
			if (pLandingLayer->flCycle > pPrevLandingLayer->flCycle) {
				float flLandDuration = pLandingLayer->flCycle / pLandingLayer->flPlaybackRate;
				if (flLandDuration > 0.0f) {

					pRecord->iActivityTick = TIME_TO_TICKS(pRecord->flSimulationTime - flLandDuration) + 1;
					pRecord->iActivityType = Land;

					float flDurationInAir = (pJumpingLayer->flCycle - pLandingLayer->flCycle);
					if (flDurationInAir < 0.0f)
						flDurationInAir += 1.0f;

					pRecord->flDurationInAir = flDurationInAir / pJumpingLayer->flPlaybackRate;
				}
			}
		}
	}
}

float ComputeActivityPlayback(CBaseEntity* pEntity, Lagcompensation::LagRecord_t* pRecord) {

	CAnimationLayer* pJumpingLayer = &pRecord->arrLayers[ANIMATION_LAYER_MOVEMENT_JUMP_OR_FALL];
	CAnimationLayer* pLandingLayer = &pRecord->arrLayers[ANIMATION_LAYER_MOVEMENT_LAND_OR_CLIMB];

	float flActivityPlayback = 0.0f;
	if (pJumpingLayer && pLandingLayer) {

		switch (pRecord->iActivityType) {

		case 1:
			flActivityPlayback = pEntity->GetLayerSequenceCycleRate(pJumpingLayer, pJumpingLayer->nSequence);
			break;

		case 2:
			flActivityPlayback = pEntity->GetLayerSequenceCycleRate(pLandingLayer, pLandingLayer->nSequence);
			break;
		}
	}
	return flActivityPlayback;
}

void HandleDormancyLeaving(CBaseEntity* pEntity, Lagcompensation::LagRecord_t* pRecord, CAnimState* pState) {

	/* Get animation layers */
	const CAnimationLayer* JumpingLayer = &pRecord->arrLayers[ANIMATION_LAYER_MOVEMENT_JUMP_OR_FALL];
	const CAnimationLayer* LandingLayer = &pRecord->arrLayers[ANIMATION_LAYER_MOVEMENT_LAND_OR_CLIMB];

	/* Final flLastUpdateTime */
	float flLastUpdateTime = pRecord->flSimulationTime - i::GlobalVars->flIntervalPerTick;

	/* Fix animation state timing */
	if (pRecord->iFlags & FL_ONGROUND) /* On ground */
	{
		/* Use information from landing */
		int nActivity = pEntity->GetSequenceActivity(LandingLayer->nSequence);
		if (nActivity == ACT_CSGO_LAND_HEAVY || nActivity == ACT_CSGO_LAND_LIGHT)
		{
			/* Compute land duration */
			float flLandDuration = LandingLayer->flCycle / LandingLayer->flPlaybackRate;

			/* Check landing time */
			float flLandingTime = pRecord->flSimulationTime - flLandDuration;
			if (flLandingTime == flLastUpdateTime)
			{
				pState->bOnGround = true;
				pState->bHitGroundAnimation = true;
				pState->flHitGroundCycle = 0.0f;
			}
			else if (flLandingTime - i::GlobalVars->flIntervalPerTick == flLastUpdateTime)
			{
				pState->bOnGround = false;
				pState->bHitGroundAnimation = false;
				pState->flHitGroundCycle = 0.0f;
			}

			/* Determine duration in air */
			float flDurationInAir = (JumpingLayer->flCycle - LandingLayer->flCycle);
			if (flDurationInAir < 0.0f)
				flDurationInAir += 1.0f;

			/* Set time in air */
			pState->flDurationInAir = flDurationInAir / JumpingLayer->flPlaybackRate;

			/* Check bounds.*/
			/* There's two conditions to let this data be useful: */
			/* It's useful if player has landed after the latest client animation update */
			/* It's useful if player has landed before the previous tick */
			if (flLandingTime < flLastUpdateTime && flLandingTime > pState->flLastUpdateTime)
				flLastUpdateTime = flLandingTime;
		}
	}
	else /* In air */
	{
		/* Use information from jumping */
		int nActivity = pEntity->GetSequenceActivity(JumpingLayer->nSequence);
		if (nActivity == ACT_CSGO_JUMP)
		{
			/* Compute duration in air */
			float flDurationInAir = JumpingLayer->flCycle / JumpingLayer->flPlaybackRate;

			/* Check landing time */
			float flJumpingTime = pRecord->flSimulationTime - flDurationInAir;
			if (flJumpingTime <= flLastUpdateTime)
				pState->bOnGround = false;
			else if (flJumpingTime - i::GlobalVars->flIntervalPerTick)
				pState->bOnGround = true;

			/* Check bounds.*/
			/* There's two conditions to let this data be useful: */
			/* It's useful if player has jumped after the latest client animation update */
			/* It's useful if player has jumped before the previous tick */
			if (flJumpingTime < flLastUpdateTime && flJumpingTime > pState->flLastUpdateTime)
				flLastUpdateTime = flJumpingTime;

			/* Set time in air */
			pState->flDurationInAir = flDurationInAir - i::GlobalVars->flIntervalPerTick;

			/* Disable landing */
			pState->bHitGroundAnimation = false;
		}
	}

	/* Set flLastUpdateTime */
	pState->flLastUpdateTime = flLastUpdateTime;
}

void Animations::RebuildEnemyAnimations(CBaseEntity* pEntity, Lagcompensation::LagRecord_t* pRecord, Lagcompensation::AnimationInfo_t* pLog) {

	CAnimState* pState = pEntity->AnimState();
	Lagcompensation::LagRecord_t* pPrevious = SetupData(pEntity, pRecord, pLog);
	
	pRecord->vecVelocity = DeterminePlayerVelocity(pEntity, pRecord, pPrevious, pState);
	//pRecord->bBreakingLagcompensation = lagcomp.IsBreakingLagcompensation(pRecord);
	if (pRecord->bFirstAfterDormant)
		HandleDormancyLeaving(pEntity, pRecord, pState);

	CopyRecordData(pEntity, pRecord, pPrevious, pState);

	const float flCurrentTime = i::GlobalVars->flCurrentTime;
	const float flRealTime = i::GlobalVars->flRealTime;
	const float flFrameTime = i::GlobalVars->flFrameTime;
	const float flAbsFrameTime = i::GlobalVars->flAbsFrameTime;
	const float flInterpolationAmount = i::GlobalVars->flInterpolationAmount;
	const float iTickCount = i::GlobalVars->iTickCount;
	const float iFrameCount = i::GlobalVars->iFrameCount;

	const Vector vecBackupVelocity = pEntity->GetVelocity();
	const Vector vecbackupAbsVelocity = pEntity->GetVecAbsVelocity();
	const Vector vecBackupAbsOrigin = pEntity->GetAbsOrigin();
	const int iBackupFlags = pEntity->GetFlags();
	const int iBackupEFlags = pEntity->GetEFlags();
	const float flBackupDuckAmount = pEntity->GetDuckAmount();
	const float flBackupLowerBodyYaw = pEntity->GetLowerBodyYaw();
	const float flBackupThirdpersonRecoil = pEntity->GetThirdpersonRecoil();

	pEntity->GetEFlags() &= ~(EFL_DIRTY_ABSVELOCITY | EFL_DIRTY_ABSTRANSFORM);

	UpdateCollision(pEntity, pRecord);

	if (pRecord->iChoked <= 1 || !pPrevious) {

		int iSimulatedCount = TIME_TO_TICKS(pRecord->flSimulationTime);

		i::GlobalVars->flCurrentTime = pRecord->flSimulationTime;
		i::GlobalVars->flRealTime = pRecord->flSimulationTime;
		i::GlobalVars->flFrameTime = i::GlobalVars->flIntervalPerTick;
		i::GlobalVars->flAbsFrameTime = i::GlobalVars->flIntervalPerTick;
		i::GlobalVars->iFrameCount = iSimulatedCount;
		i::GlobalVars->iTickCount = iSimulatedCount;
		i::GlobalVars->flInterpolationAmount = 0.f;

		pEntity->SetAbsOrigin(pRecord->vecOrigin);
		pEntity->GetVelocity() = pRecord->vecVelocity;
		pEntity->GetVecAbsVelocity() = pRecord->vecVelocity;

		UpdateClientSideAnimations(pEntity, pRecord);
	}
	else {

		SimulatePlayerActivity(pEntity, pRecord, pPrevious);

		pRecord->flActivityPlayback = ComputeActivityPlayback(pEntity, pRecord);

		for (int iSimulationTick = 1; iSimulationTick <= pRecord->iChoked; iSimulationTick++) {

			float flSimulationTime = pPrevious->flSimulationTime + TICKS_TO_TIME(iSimulationTick);
			int iCurrentSimulationTick = TIME_TO_TICKS(flSimulationTime);

			i::GlobalVars->flCurrentTime = flSimulationTime;
			i::GlobalVars->flRealTime = flSimulationTime;
			i::GlobalVars->flFrameTime = i::GlobalVars->flIntervalPerTick;
			i::GlobalVars->flAbsFrameTime = i::GlobalVars->flIntervalPerTick;
			i::GlobalVars->iFrameCount = iCurrentSimulationTick;
			i::GlobalVars->iTickCount = iCurrentSimulationTick;
			i::GlobalVars->flInterpolationAmount = 0.f;

			pEntity->GetDuckAmount() = M::AnimationLerp(pPrevious->flDuck, pRecord->flDuck, iSimulationTick, pRecord->iChoked);
			pEntity->GetLowerBodyYaw() = pPrevious->flLowerBodyYawTarget;
			pEntity->GetEyeAngles() = pPrevious->vecEyeAngles;

			pEntity->GetVecOrigin() = M::AnimationLerp(pPrevious->vecOrigin, pRecord->vecOrigin, iSimulationTick, pRecord->iChoked);
			pEntity->SetAbsOrigin(pEntity->GetVecOrigin());

			if (flSimulationTime < pRecord->flSimulationTime) {

				if (pRecord->bDidShot) {

					if (iCurrentSimulationTick < TIME_TO_TICKS(pRecord->flLastShotTime))
						pEntity->GetThirdpersonRecoil() = pPrevious->flThirdPersonRecoil;
					else {
						pEntity->GetEyeAngles() = pRecord->vecEyeAngles;
						pEntity->GetLowerBodyYaw() = pRecord->flLowerBodyYawTarget;
						pEntity->GetThirdpersonRecoil() = pRecord->flThirdPersonRecoil;
					}
				}

				if (pRecord->iActivityType != NoActivity) {
					if (iCurrentSimulationTick == pRecord->iActivityTick) {

						int nLayer = ANIMATION_LAYER_MOVEMENT_JUMP_OR_FALL;
						if (pRecord->iActivityType == Land)
							nLayer = ANIMATION_LAYER_MOVEMENT_LAND_OR_CLIMB;

						pEntity->GetAnimationOverlays()[nLayer].flCycle = 0.0f;
						pEntity->GetAnimationOverlays()[nLayer].flWeight = 0.0f;
						pEntity->GetAnimationOverlays()[nLayer].flPlaybackRate = pRecord->flActivityPlayback;

						if (pRecord->iActivityType == Jump)
							pEntity->GetFlags() &= ~FL_ONGROUND;
						else if (pRecord->iActivityType == Land)
							pEntity->GetFlags() |= FL_ONGROUND;
					}
					else if (iCurrentSimulationTick < pRecord->iActivityTick) {

						if (pRecord->iActivityType == Jump)
							pEntity->GetFlags() |= FL_ONGROUND;
						else if (pRecord->iActivityType == Land)
							pEntity->GetFlags() &= ~FL_ONGROUND;
					}
				}
			}
			else {
				pEntity->GetFlags() = pRecord->iFlags;
				pEntity->GetDuckAmount() = pRecord->flDuck;
				pEntity->GetLowerBodyYaw() = pRecord->flLowerBodyYawTarget;
				pEntity->GetEyeAngles() = pRecord->vecEyeAngles;
			}

			Vector& vecVelocity = pEntity->GetVelocity();
			vecVelocity.x = M::AnimationLerp(pPrevious->vecVelocity.x, pRecord->vecVelocity.x, iSimulationTick, pRecord->iChoked);
			vecVelocity.y = M::AnimationLerp(pPrevious->vecVelocity.y, pRecord->vecVelocity.y, iSimulationTick, pRecord->iChoked);
			pEntity->m_vecAbsVelocity() = vecVelocity;

			UpdateClientSideAnimations(pEntity, pRecord);

		}
	}

	pEntity->SetAnimationLayers(pRecord->arrLayers);

	pEntity->SetAbsOrigin(pRecord->vecAbsOrigin);

	pRecord->vecAbsAngles = Vector(0.0f, pState->flGoalFeetYaw, 0.0f);

	float flAimMatrixWidthRange = M::Lerp(std::clamp(pEntity->AnimState()->flRunningSpeed, 0.0f, 1.0f), 1.0f, M::Lerp(pEntity->AnimState()->flWalkToRunTransition, 0.8f, 0.5f)); //-V807
	if (pEntity->AnimState()->flDuckAmount > 0)
		flAimMatrixWidthRange = M::Lerp(pEntity->AnimState()->flDuckAmount * std::clamp(pEntity->AnimState()->flDuckingSpeed, 0.0f, 1.0f), flAimMatrixWidthRange, 0.5f);

	pRecord->flDesyncDelta = flAimMatrixWidthRange * pEntity->AnimState()->flMaxBodyYaw;

	pEntity->GetPoseParameters(pRecord->flPoses);
	GetSideLayersForResolver(pEntity, pRecord);

	if (playerList::arrPlayers[pEntity->EntIndex()].bOverrideResolver)
		pState->flGoalFeetYaw = M::NormalizeYaw(pRecord->vecEyeAngles.y + playerList::arrPlayers[pEntity->EntIndex()].flOverrideYaw);
	else
		Resolver(pEntity, pRecord, pPrevious);

	SetupPlayerMatrix(pEntity, pRecord, pRecord->pMatricies[VISUAL], Interpolated | VisualAdjustment);
	std::memcpy(pLog->pCachedMatrix.data(), pRecord->pMatricies[VISUAL], sizeof(matrix3x4a_t)* MAXSTUDIOBONES);

	if (cfg::rage::bEnable) {
		SetupPlayerMatrix(pEntity, pRecord, pRecord->pMatricies[RESOLVE], BoneUsedByHitbox);
		GenerateSafePointMatricies(pRecord->pEntity, pRecord);
	}

	pEntity->GetVelocity() = vecBackupVelocity;
	pEntity->GetVecAbsVelocity() = vecbackupAbsVelocity;
	pEntity->SetAbsOrigin(vecBackupAbsOrigin);
	pEntity->GetFlags() = iBackupFlags;
	pEntity->GetEFlags() = iBackupEFlags;
	pEntity->GetDuckAmount() = flBackupDuckAmount;
	pEntity->GetLowerBodyYaw() = flBackupLowerBodyYaw;
	pEntity->GetThirdpersonRecoil() = flBackupThirdpersonRecoil;

	i::GlobalVars->flCurrentTime = flCurrentTime;
	i::GlobalVars->flRealTime = flRealTime;
	i::GlobalVars->flFrameTime = flFrameTime;
	i::GlobalVars->flAbsFrameTime = flAbsFrameTime;
	i::GlobalVars->flInterpolationAmount = flInterpolationAmount;
	i::GlobalVars->iTickCount = iTickCount;
	i::GlobalVars->iFrameCount = iFrameCount;
}

void Animations::SetupPlayerMatrix(CBaseEntity* pEntity, Lagcompensation::LagRecord_t* pRecord, matrix3x4a_t* pMatrix, int nFlags) {

	pEntity->SetAnimationLayers(pRecord->arrLayers);

	const float flCurrentTime = i::GlobalVars->flCurrentTime;
	const float flRealTime = i::GlobalVars->flRealTime;
	const float flFrameTime = i::GlobalVars->flFrameTime;
	const float flAbsFrameTime = i::GlobalVars->flAbsFrameTime;
	const float flInterpolationAmount = i::GlobalVars->flInterpolationAmount;
	const float iTickCount = i::GlobalVars->iTickCount;
	const float iFrameCount = i::GlobalVars->iFrameCount;

	const int iLastSkipFrameCount = pEntity->GetLastSkipFrameCount();
	const int iEffect = pEntity->GetEffects();
	const int iClientEffects = pEntity->GetClientEffects();
	const int iOcclusionFlags = pEntity->GetOcclusionFlags();
	const Vector vecAbsOrigin = pEntity->GetAbsOrigin();

	int iSimulationTicks = TIME_TO_TICKS(pRecord->flSimulationTime);
	i::GlobalVars->flCurrentTime = pRecord->flSimulationTime;
	i::GlobalVars->flRealTime = pRecord->flSimulationTime;
	i::GlobalVars->flFrameTime = i::GlobalVars->flIntervalPerTick;
	i::GlobalVars->flAbsFrameTime = i::GlobalVars->flIntervalPerTick;
	i::GlobalVars->iTickCount = iSimulationTicks;
	i::GlobalVars->iFrameCount = INT_MAX; /* ShouldSkipAnimationFrame fix */
	i::GlobalVars->flInterpolationAmount = 0.0f;

	pEntity->InvalidateBoneCache();

	for (int iLayer = 0; iLayer < ANIMATION_LAYER_COUNT; iLayer++) {

		CAnimationLayer* pLayer = &pEntity->GetAnimationOverlays()[iLayer];
		if (!pLayer)
			continue;

		pLayer->pOwner = pEntity;
	}

	/* Disable ACT_CSGO_IDLE_TURN_BALANCEADJUST animation */
	if (nFlags & EMatrixFlags::VisualAdjustment) {

		pEntity->GetAnimationOverlays()[ANIMATION_LAYER_LEAN].flWeight = 0.0f;

		if (pEntity->GetSequenceActivity(pEntity->GetAnimationOverlays()[ANIMATION_LAYER_ADJUST].nSequence) == ACT_CSGO_IDLE_TURN_BALANCEADJUST) {
			pEntity->GetAnimationOverlays()[ANIMATION_LAYER_ADJUST].flCycle = 0.0f;
			pEntity->GetAnimationOverlays()[ANIMATION_LAYER_ADJUST].flWeight = 0.0f;
		}
	}

	/* Remove interpolation if required */
	if (!(nFlags & EMatrixFlags::Interpolated) && pRecord->vecOrigin.IsValid())
		pEntity->SetAbsOrigin(pRecord->vecOrigin);

	/* Compute bone mask */
	int nBoneMask = BONE_USED_BY_ANYTHING;
	if (nFlags & EMatrixFlags::BoneUsedByHitbox)
		nBoneMask = BONE_USED_BY_HITBOX;

	pEntity->GetClientEffects() |= 2;
	pEntity->GetEffects() |= EF_NOINTERP;
	pEntity->GetOcclusionFrameCount() = -1;
	pEntity->GetOcclusionFlags() &= ~2;
	pEntity->GetLastSkipFrameCount() = 0;

	g::bSettingUpBones[pEntity->EntIndex()] = true;
	//setupbones.SetupBones(pEntity, pMatrix, MAXSTUDIOBONES, nBoneMask );
	pEntity->SetupBones(pMatrix, MAXSTUDIOBONES, nBoneMask, 0.0f);
	g::bSettingUpBones[pEntity->EntIndex()] = false;

	pEntity->GetLastSkipFrameCount() = iLastSkipFrameCount;
	pEntity->GetEffects() = iEffect;
	pEntity->GetClientEffects() = iClientEffects;
	pEntity->GetOcclusionFlags() = iOcclusionFlags;
	pEntity->SetAbsOrigin(vecAbsOrigin);

	pEntity->SetAnimationLayers(pRecord->arrLayers);

	i::GlobalVars->flCurrentTime = flCurrentTime;
	i::GlobalVars->flRealTime = flRealTime;
	i::GlobalVars->flFrameTime = flFrameTime;
	i::GlobalVars->flAbsFrameTime = flAbsFrameTime;
	i::GlobalVars->flInterpolationAmount = flInterpolationAmount;
	i::GlobalVars->iTickCount = iTickCount;
	i::GlobalVars->iFrameCount = iFrameCount;
}

void UpdateLmao(CBaseEntity* pEntity, Lagcompensation::LagRecord_t* pRecord) {

	// fix animating in same frame.
	if (pEntity->AnimState()->iLastUpdateFrame == i::GlobalVars->iFrameCount)
		pEntity->AnimState()->iLastUpdateFrame -= 1;

	// negative values possible when clocks on client and server go out of sync.
	pEntity->AnimState()->flLastUpdateIncrement = max(0.0f, i::GlobalVars->flCurrentTime - pEntity->AnimState()->flLastUpdateTime);

	// fix animating in same time.
	if (pEntity->AnimState()->flLastUpdateTime == i::GlobalVars->flCurrentTime)
		pEntity->AnimState()->flLastUpdateTime -= 1;

	// update player animation.
	pEntity->IsClientSideAnimation() = g::bAllowAnimations[pEntity->EntIndex()] = true;
	pEntity->UpdateClientSideAnimations();
	pEntity->IsClientSideAnimation() = g::bAllowAnimations[pEntity->EntIndex()] = false;

	// invalidate physics.
	pEntity->InvalidatePhysicsRecursive(ANIMATION_CHANGED);
}

float Animations::FindMatchingPlaybackRate(float flTarget, std::array<float, 120>& arrPlaybackRates) {

	int closestIndex = 0; // Initialize with the index of the first element
	float minDifference = std::fabsf(arrPlaybackRates[0] - flTarget); // Initialize with the difference to the first element

	for (int i = 1; i < sizeof(arrPlaybackRates) / sizeof(arrPlaybackRates[0]); ++i) {
		float difference = std::fabsf(arrPlaybackRates[i] - flTarget);
		if (difference < minDifference) {
			minDifference = difference;
			closestIndex = i;
		}
	}
	if (closestIndex < 1 || closestIndex > 119)
		return 0.f;

	return closestIndex - 60;
}

void Animations::FindDesyncSide(Lagcompensation::LagRecord_t* pRecord) {

	// backup variables
	CAnimState pBackupState;
	std::array<float, 24> arrBackupPoses;
	std::array<CAnimationLayer, 13> arrBackupLayers;

	// backup crucial information
	pRecord->pEntity->GetPoseParameters(arrBackupPoses.data());
	pRecord->pEntity->GetAnimationLayers(arrBackupLayers.data());
	memcpy(&pBackupState, pRecord->pEntity->AnimState(), sizeof(CAnimState));

	std::array<float, 120> arrPlaybackRates{};
	for (float i = -60; i < 60; i += 10)
	{
		pRecord->pEntity->AnimState()->flGoalFeetYaw = M::NormalizeYaw(pRecord->vecEyeAngles.y + i);
		UpdateClientSideAnimations(pRecord->pEntity, pRecord);
		memcpy(pRecord->pEntity->AnimState(), &pBackupState, sizeof(CAnimState));
		pRecord->pEntity->SetUpMovement();

		arrPlaybackRates[i + 60] = pRecord->pEntity->GetAnimationOverlays()[6].flPlaybackRate;
	}

	// restore animations
	pRecord->pEntity->SetAnimationLayers(arrBackupLayers.data());
	pRecord->pEntity->SetPoseParameters(arrBackupPoses.data());
	memcpy(pRecord->pEntity->AnimState(), &pBackupState, sizeof(CAnimState));

	// update restored anims
	UpdateClientSideAnimations(pRecord->pEntity, pRecord);

	// restore animstate to its original phase so it will update everything normally
	memcpy(pRecord->pEntity->AnimState(), &pBackupState, sizeof(CAnimState));

	pRecord->flGuessedYaw = FindMatchingPlaybackRate(pRecord->arrLayers[6].flPlaybackRate, arrPlaybackRates);
}

void Animations::GetSideLayersForResolver(CBaseEntity* pEntity, Lagcompensation::LagRecord_t* pRecord) {

	// rebuilding server function for layer 6 is not really important
	// feeding the ingame functions with the right data can achieve the same result
	// since it's getting called on each client and then getting networked, you can't really rebuild it properly
	// if someone like me can't really reverse game code from IDA, just call the function that their client calls

	// save animation data
	CAnimState pBackupState;
	float arrBackupPoses[24];
	CAnimationLayer arrBackupLayers[13];

	pEntity->GetAnimationLayers(arrBackupLayers);
	pEntity->GetPoseParameters(arrBackupPoses);
	memcpy(&pBackupState, pEntity->AnimState(), sizeof(CAnimState));
	{
		// Set the yaw that we wan't to get
		pEntity->AnimState()->flGoalFeetYaw = M::NormalizeYaw(pRecord->vecEyeAngles.y);

		// Update with the applied data
		UpdateClientSideAnimations(pEntity, pRecord);

		// Restore animstate (we need to get almost everything, except the goalfeetyaw)
		// layer6 calculation uses flLastUpdateIncrement while updating anims changes that to 0
		// that is why we need to restore the animstate
		memcpy(pEntity->AnimState(), &pBackupState, sizeof(CAnimState));

		// call the client function to rebuild the layer with fixed values
		pEntity->SetUpMovement();

		// Save the layerdata
		pRecord->LayerData[CENTER] = Lagcompensation::LagRecord_t::LayerData_t(pEntity->GetAnimationOverlays()[ANIMATION_LAYER_MOVEMENT_MOVE]);
	}

	{
		pEntity->AnimState()->flGoalFeetYaw = M::NormalizeYaw(pRecord->vecEyeAngles.y + pRecord->flDesyncDelta);

		UpdateClientSideAnimations(pEntity, pRecord);

		memcpy(pEntity->AnimState(), &pBackupState, sizeof(CAnimState));

		pEntity->SetUpMovement();
		pRecord->LayerData[RIGHT] = Lagcompensation::LagRecord_t::LayerData_t(pEntity->GetAnimationOverlays()[ANIMATION_LAYER_MOVEMENT_MOVE]);
	}

	{
		pEntity->AnimState()->flGoalFeetYaw = M::NormalizeYaw(pRecord->vecEyeAngles.y - pRecord->flDesyncDelta);

		UpdateClientSideAnimations(pEntity, pRecord);

		memcpy(pEntity->AnimState(), &pBackupState, sizeof(CAnimState));

		pEntity->SetUpMovement();
		pRecord->LayerData[LEFT] = Lagcompensation::LagRecord_t::LayerData_t(pEntity->GetAnimationOverlays()[ANIMATION_LAYER_MOVEMENT_MOVE]);
	}
	memcpy(pEntity->AnimState(), &pBackupState, sizeof(CAnimState));
	pEntity->SetAnimationLayers(arrBackupLayers);
	pEntity->SetPoseParameters(arrBackupPoses);

	UpdateClientSideAnimations(pEntity, pRecord);
	memcpy(pEntity->AnimState(), &pBackupState, sizeof(CAnimState));
}

void Animations::GenerateFreestandMatricies(CBaseEntity* pEntity, Lagcompensation::LagRecord_t* pRecord) {

	if (!pEntity || !pRecord)
		return;

	auto BuildFreestandMatrix = [&](int iSideAngle)
	{
		// save animation data
		std::array < CAnimationLayer, 13 > arrLayers;
		std::array < float, 24 > arrPoses;
		CAnimState pAnimationState;
		Vector vecEyeAngles;

		// copy data
		std::memcpy(arrLayers.data(), pEntity->GetAnimationOverlays(), sizeof(CAnimationLayer) * 13);
		std::memcpy(arrPoses.data(), pEntity->GetPoseParameter().data(), sizeof(float) * 24);
		std::memcpy(&pAnimationState, pEntity->AnimState(), sizeof(CAnimState));
		vecEyeAngles.y = pRecord->vecEyeAngles.y;

		// set the matrix angle
		pEntity->GetEyeAngles().y += iSideAngle == 0 ? -90 : 90;

		// update animations
		UpdateClientSideAnimations(pEntity, pRecord);

		// build matrix for that animation
		SetupPlayerMatrix(pEntity, pRecord, pRecord->pSideMatrixes[iSideAngle], BoneUsedByHitbox);

		// restore data to not mess with game animations
		std::memcpy(pEntity->GetAnimationOverlays(), arrLayers.data(), sizeof(CAnimationLayer) * 13);
		std::memcpy(pEntity->GetPoseParameter().data(), arrPoses.data(), sizeof(float) * 24);
		std::memcpy(pEntity->AnimState(), &pAnimationState, sizeof(CAnimState));
		pEntity->GetEyeAngles().y = pRecord->vecEyeAngles.y;

		// update our data again to apply back the actual animation we had before
		UpdateClientSideAnimations(pEntity, pRecord);

		// apply animstate again so the updating wont update the last update time or else it wont allow us to update again this frame
		std::memcpy(pEntity->AnimState(), &pAnimationState, sizeof(CAnimState));
	};

	BuildFreestandMatrix(0);
	BuildFreestandMatrix(1);
}