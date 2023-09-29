#include "Lagcompensation.h"
#include "EnemyAnimations.h"
#include "../../Networking/networking.h"
#include "../../../SDK/InputSystem.h"
#include "../exploits.h"
#include "../../Misc/Playerlist.h"
#include "../../../Interface/Classes/CCSGameRulesProxy.h"

Lagcompensation::LagRecord_t::LagRecord_t(CBaseEntity* pEntity)
{
	CBaseCombatWeapon* pWeapon = pEntity->GetWeapon();

	this->pEntity = pEntity;
	iEntIndex = pEntity->EntIndex();
	bDormant = pEntity->IsDormant();
	vecVelocity = pEntity->GetVelocity();
	vecAbsVelocity = pEntity->GetVecAbsVelocity();
	vecOrigin = pEntity->GetVecOrigin();
	vecAbsOrigin = pEntity->GetAbsOrigin();
	vecMins = pEntity->GetCollideable()->OBBMins();
	vecMaxs = pEntity->GetCollideable()->OBBMaxs();
	pEntity->GetAnimationLayers(arrLayers);
	pEntity->GetPoseParameters(flPoses);
	bValid = true;
	bDidShot = false;
	bFakewalking = false;
	bBreakingLagcompensation = false;
	
	flSimulationTime = pEntity->GetSimulationTime();
	flOldSimulationTime = pEntity->GetOldSimulationTime();
	flLastShotTime = pWeapon ? pWeapon->GetLastShotTime() : 0.f;
	flDuck = pEntity->GetDuckAmount();
	flLowerBodyYawTarget = pEntity->GetLowerBodyYaw();
	flInterpTime = 0.f;
	flMaxSpeed = pWeapon ? pEntity->IsScoped() ? pWeapon->GetCSWpnData()->flMaxSpeed[0] : pWeapon->GetCSWpnData()->flMaxSpeed[1] : 260.f;
	flThirdPersonRecoil = pEntity->GetThirdpersonRecoil();
	flWalkToRunTransition = pEntity->AnimState()->flWalkToRunTransition;

	vecEyeAngles = pEntity->GetEyeAngles();
	vecAbsAngles = pEntity->GetAbsAngles();

	iFlags = pEntity->GetFlags();
	iEFlags = pEntity->GetEFlags();
	iEffects = pEntity->GetEffects();
	iChoked = TIME_TO_TICKS(flSimulationTime - flOldSimulationTime);
	bBreakingLagcompensation = iChoked < 1;
	iChoked = std::clamp(iChoked, 1, 16);

}

void Lagcompensation::LagRecord_t::Apply(CBaseEntity* pEntity, bool Backup)
{
	pEntity->GetFlags() = iFlags;
	pEntity->GetSimulationTime() = flSimulationTime;
	pEntity->GetLowerBodyYaw() = flLowerBodyYawTarget;
	pEntity->GetDuckAmount() = flDuck;
	pEntity->GetEyeAngles() = vecEyeAngles;
	pEntity->GetCollideable()->OBBMaxs() = vecMaxs;
	pEntity->GetCollideable()->OBBMins() = vecMins;
	pEntity->GetVelocity() = vecVelocity;
	pEntity->GetVecAbsVelocity() = Backup ? vecAbsVelocity : vecVelocity;
	pEntity->GetVecOrigin() = vecOrigin;
	pEntity->SetAbsOrigin(Backup ? vecAbsOrigin : vecOrigin);
	if (!Backup)
		ApplyMatrix(pEntity, RESOLVE);
}

void Lagcompensation::LagRecord_t::Apply(CBaseEntity* pEntity)
{
	// set poses
	pEntity->SetPoseParameters(flPoses);
	pEntity->GetEyeAngles() = vecEyeAngles;
	pEntity->GetVelocity() = pEntity->GetVecAbsVelocity() = vecVelocity;
	pEntity->GetLowerBodyYaw() = flLowerBodyYawTarget;
	pEntity->GetDuckAmount() = flDuck;
	pEntity->GetFlags() = iFlags;
	pEntity->GetVecOrigin() = vecOrigin;
	pEntity->SetAbsOrigin(vecOrigin);
}

void Lagcompensation::LagRecord_t::Restore(CBaseEntity* pEntity)
{
	pEntity->GetVelocity() = vecVelocity;
	pEntity->GetVecAbsVelocity() = vecAbsVelocity;
	pEntity->GetFlags() = iFlags;
	pEntity->GetEFlags() = iEFlags;
	pEntity->GetDuckAmount() = flDuck;
	// set animlayers
	pEntity->SetAnimationLayers(arrLayers);
	pEntity->GetLowerBodyYaw() = flLowerBodyYawTarget;
	pEntity->GetVecOrigin() = vecOrigin;
	pEntity->SetAbsOrigin(vecAbsOrigin);
}

void Lagcompensation::FrameStageNotify() noexcept {

	if (!g::pLocal || g::bUpdatingSkins)
		return;

	for (int i = 1; i <= i::GlobalVars->nMaxClients; i++) {

		auto pLog = &pPlayerLogs[i];

		CBaseEntity* pEntity = static_cast<CBaseEntity*>(i::EntityList->GetClientEntity(i));
		if (!pEntity || !pEntity->IsPlayer() || !pEntity->IsAlive() || pEntity == g::pLocal) 
		{
			pLog->bLeftDormancy = true;
			continue;
		}
		pLog->pEntity = pEntity;
		if (!pEntity->IsEnemy(g::pLocal)) {
			g::bSettingUpBones[i] = g::bAllowAnimations[i] = true;
			continue;
		}

		if (pEntity->GetSimulationTime() == pEntity->GetOldSimulationTime())
			continue;

		if (pEntity->IsDormant()) 
		{
			pLog->bLeftDormancy = true;
			continue;
		}

		LagRecord_t pPrevious;
		pPrevious.bRestoreData = false;
		if (!pLog->pRecord.empty()) 
		{
			pPrevious = pLog->pRecord.front();
			pPrevious.bRestoreData = true;
		}

		Lagcompensation::LagRecord_t pRecord(pPlayerLogs[i].pEntity);
		if (pPrevious.bRestoreData)
		{
			if (!DataChanged(pEntity, &pPrevious))
				continue;
		}

		if (pPrevious.flSimulationTime > pRecord.flSimulationTime) 
		{
			float flOldSimulationTime = pEntity->GetOldSimulationTime();
			if (&pPrevious)
				flOldSimulationTime = pPrevious.flSimulationTime;

			pLog->flExploitTime = pEntity->GetSimulationTime();
			pLog->InvalidateRecords();
		}

		if (pLog->bLeftDormancy)
		{
			pLog->InvalidateRecords();
		}
		
		if (pRecord.flSimulationTime <= pLog->flExploitTime) 
		{
			pRecord.bValid = false;
			pRecord.bBreakingLagcompensation = true;
		}

		if (pPrevious.bRestoreData) 
		{
			if ((pRecord.vecOrigin - pPrevious.vecOrigin).Length2DSqr() > 4096.f) 
			{
				pRecord.bBreakingLagcompensation = true;
				pLog->InvalidateRecords();
			}
		}

		if (pRecord.flSimulationTime < pPrevious.flSimulationTime) {

			pRecord.bValid = false;
			pRecord.bBreakingLagcompensation = true;

			if (pPrevious.bRestoreData)
				pRecord.iChoked = TIME_TO_TICKS(pRecord.flOldSimulationTime - pPrevious.flOldSimulationTime);
			else
				pRecord.iChoked = pPrevious.iChoked;
		}
		else {
			if (pPrevious.bRestoreData)
				pRecord.iChoked = TIME_TO_TICKS(pRecord.flSimulationTime - pPrevious.flSimulationTime);
			else
				pRecord.iChoked = TIME_TO_TICKS(pRecord.flSimulationTime - pRecord.flOldSimulationTime);
		}

		if (pLog->bLeftDormancy)
			pRecord.bFirstAfterDormant = true;

		pLog->pRecord.push_front(pRecord);
		while (pLog->pRecord.size() > 32)
			pLog->pRecord.pop_back();

		anims.RebuildEnemyAnimations(pEntity, &pLog->pRecord.front(), pLog);

		pLog->bLeftDormancy = false;
	}
}

Lagcompensation::AnimationInfo_t& Lagcompensation::GetLog(const int iEntIndex)
{
	return pPlayerLogs[iEntIndex];
}

void Lagcompensation::FilterRecords()
{
	for (int i = 1; i <= i::GlobalVars->nMaxClients; i++)
	{
		if (pPlayerLogs[i].pRecord.empty())
			continue;

		if (!pPlayerLogs[i].pEntity) {
			pPlayerLogs[i].pRecord.clear();
			continue;
		}

		// max tick amt.
		float flMaxTickAmt = /*C::Get<bool>( Vars.bRageSafeBacktracking ) ? std::ceil( I::ConVar->FindVar( Str( "sv_maxunlag" ) )->GetFloat( ) * ( 1.f / I::Globals->m_flIntervalPerTick ) ) + 3 :*/ 64;

		// no need to store insane amount of data.
		while (pPlayerLogs[i].pRecord.size() > flMaxTickAmt)
			pPlayerLogs[i].pRecord.pop_back();

		for (auto j = 0u; j < pPlayerLogs[i].pRecord.size(); j++)
		{
			auto& pCurrentRecord = pPlayerLogs[i].pRecord[j];
			if (pCurrentRecord.bDormant && !j)
				continue;

			if (pCurrentRecord.bDormant)
			{
				pPlayerLogs[i].pRecord.erase(pPlayerLogs[i].pRecord.begin() + j);
				j--;
				continue;
			}

			if (pCurrentRecord.bValid)
				if (pCurrentRecord.bValid = lagcomp.IsValidRecord(pCurrentRecord.flSimulationTime))
					pPlayerLogs[i].iLastValid = j;
		}
	}
}

void Lagcompensation::SetInterpolationFlags()
{
	if ((*GameRules)->m_bIsValveDS())
		return;

	for (size_t i = 1; i <= i::GlobalVars->nMaxClients; i++) {

		CBaseEntity* pEntity = static_cast<CBaseEntity*>(i::EntityList->GetClientEntity(i));

		if (!pEntity || !pEntity->IsAlive() || pEntity->IsDormant() || pEntity->HasImmunity() || g::pLocal == pEntity)
			continue;

		void* m_VarMap = *(void**)((DWORD)(pEntity)+0x24);
		if (m_VarMap)
		{
			*(float*)(*(DWORD*)((DWORD)(m_VarMap)+0x8) + 0x24) = i::GlobalVars->flIntervalPerTick * 1.5f;
			*(float*)(*(DWORD*)((DWORD)(m_VarMap)+0x44) + 0x24) = i::GlobalVars->flIntervalPerTick * 1.5f;
		}
	}
}

void Lagcompensation::DisableInterpolation() {

	for (size_t i = 1; i <= i::GlobalVars->nMaxClients; i++) {

		CBaseEntity* pEntity = static_cast<CBaseEntity*>(i::EntityList->GetClientEntity(i));

		if (!pEntity || !pEntity->IsAlive() || pEntity->IsDormant() || pEntity->HasImmunity())
			continue;

		VarMapping_t* pVarmap = pEntity->GetVarMap();

		for (int i = 0; i < pVarmap->m_nInterpolatedEntries; i++) {

			VarMapEntry_t* pEntry = &pVarmap->m_Entries[i];
			pEntry->m_bNeedsToInterpolate = false;
		}
	}
}

//bool Lagcompensation::IsBreakingLagcompensation(Lagcompensation::LagRecord_t* pLagRecord)
//{
//	Lagcompensation::AnimationInfo_t& pInfo = lagcomp.GetLog(pLagRecord->iEntIndex);
//
//	// check if we have at least one entry.
//	if (!&pInfo || pInfo.pRecord.size() <= 0)
//		return false;
//
//	Vector previousOrigin = pLagRecord->pEntity->GetAbsOrigin();
//
//	bool m_bFoundRecord = false;
//
//	Lagcompensation::LagRecord_t* pPrevious = nullptr;
//
//	// the previous record.
//	if (pInfo.pRecord.size() >= 2)
//		pPrevious = &pInfo.pRecord[1];
//
//	// walk context looking for any invalidating event.
//	for (Lagcompensation::LagRecord_t& pRecord : pInfo.pRecord)
//	{
//		if (!pRecord.pEntity->IsAlive())
//		{
//			return false;
//		}
//
//		Vector delta = pRecord.vecOrigin - previousOrigin;
//		if (delta.LengthSqr() > LAG_COMPENSATION_TELEPORTED_DISTANCE_SQR)
//		{
//			//ExtrapolatePlayer(pRecord.pEntity, &pRecord, pPrevious);
//
//			// lost track, too much difference.
//			return true;
//		}
//
//		// player is abusing tickbase and breaking lagcompensation
//		if (pRecord.flSimulationTime < pRecord.flOldSimulationTime)
//		{
//			return true;
//		}
//		else if (&pRecord && (pInfo.pEntity->GetSimulationTime() == pRecord.flSimulationTime))
//			return true;
//
//		previousOrigin = pRecord.vecOrigin;
//	}
//	return false;
//}

void Lagcompensation::ExtrapolatePlayer(CBaseEntity* m_pEntity, Lagcompensation::LagRecord_t* pCurrent, Lagcompensation::LagRecord_t* pPrevious) const
{
	if (!pPrevious)
		return;

	static CConVar* sv_gravity = i::ConVar->FindVar(XorStr("sv_gravity"));
	static CConVar* sv_jump_impulse = i::ConVar->FindVar(XorStr("sv_jump_impulse"));

	Vector velocity = pCurrent->vecVelocity;
	int flags = pCurrent->iFlags;

	if (!(flags & FL_ONGROUND))
		velocity.z -= (i::GlobalVars->flFrameTime * sv_gravity->GetFloat());
	else if (pPrevious->iFlags & FL_ONGROUND && !(pCurrent->iFlags & FL_ONGROUND))
		velocity.z = sv_jump_impulse->GetFloat();

	const Vector mins = m_pEntity->vecMins();
	const Vector max = m_pEntity->vecMaxs();

	const Vector src = pCurrent->vecOrigin;
	Vector end = src + (velocity * i::GlobalVars->flFrameTime);

	Ray_t ray(src, end, mins, max);

	CGameTrace trace;
	CTraceFilter filter(m_pEntity);

	i::EngineTrace->TraceRay(ray, MASK_PLAYERSOLID, &filter, &trace);

	if (trace.flFraction != 1.f) {

		for (int i = 0; i < 2; i++) {

			velocity -= trace.plane.vecNormal * velocity.DotProduct(trace.plane.vecNormal);

			const float dot = velocity.DotProduct(trace.plane.vecNormal);
			if (dot < 0.f) {

				velocity.x -= dot * trace.plane.vecNormal.x;
				velocity.y -= dot * trace.plane.vecNormal.y;
				velocity.z -= dot * trace.plane.vecNormal.z;
			}

			end = trace.vecEnd + (velocity * (i::GlobalVars->flIntervalPerTick * (1.f - trace.flFraction)));

			ray = Ray_t(trace.vecEnd, end, mins, max);
			i::EngineTrace->TraceRay(ray, MASK_PLAYERSOLID, &filter, &trace);

			if (trace.flFraction == 1.f)
				break;
		}
	}

	pCurrent->vecOrigin = trace.vecEnd;
	end = trace.vecEnd;
	end.z -= 2.f;

	ray = Ray_t(pCurrent->vecOrigin, end, mins, max);
	i::EngineTrace->TraceRay(ray, MASK_PLAYERSOLID, &filter, &trace);

	flags &= ~(1 << 0);

	if (trace.DidHit() && trace.plane.vecNormal.z > 0.7f)
		flags |= (1 << 0);

	pCurrent->flSimulationTime += i::GlobalVars->flIntervalPerTick;
	m_pEntity->GetVecOrigin() = pCurrent->vecOrigin;
	m_pEntity->SetAbsOrigin(pCurrent->vecOrigin);

	pCurrent->vecVelocity = velocity;
	m_pEntity->GetVelocity() = velocity;

	/*CSimulationData simulationData;

	simulationData.pEntity = m_pEntity;
	simulationData.vecOrigin = m_pCurrentRecord->vecOrigin;
	simulationData.vecVelocity = m_pCurrentRecord->vecVelocity;
	simulationData.bOnGround = m_pCurrentRecord->iFlags & FL_ONGROUND;
	simulationData.bDataFilled = true;

	int iSimulationTickDelta = std::clamp(m_pCurrentRecord->iChoked, 1, 15);
	int iDeltaTicks = (std::clamp(TIME_TO_TICKS(i::EngineClient->GetNetChannelInfo()->GetAvgLatency(FLOW_INCOMING) + i::EngineClient->GetNetChannelInfo()->GetAvgLatency(FLOW_OUTGOING)) + i::GlobalVars->iTickCount -
		TIME_TO_TICKS(m_pCurrentRecord->flSimulationTime + lagcomp.GetClientInterpAmount()), 0, 100)) - iSimulationTickDelta;

	static CConVar* sv_gravity = i::ConVar->FindVar("sv_gravity");

	if (iDeltaTicks > 0 && simulationData.bDataFilled)
	{
		for (; iDeltaTicks >= 0; iDeltaTicks -= iSimulationTickDelta)
		{
			int iTicksLeft = iSimulationTickDelta;
			do
			{
				Vector vecPredictedOrigin = simulationData.vecOrigin;
				float flTimeToExtrapolate = TICKS_TO_TIME(i::GlobalVars->iTickCount) - m_pEntity->GetSimulationTime();
				float flChokeTimeDelta = TICKS_TO_TIME(m_pCurrentRecord->iChoked);
				float flGravity = sv_gravity->GetFloat();

				static auto PredictNextVelocity = [=](Vector vecCurrent, Vector vecPrevious)
				{
					Vector vecOutput = vecPrevious;

					if (vecCurrent == vecPrevious)
						return vecCurrent;

					if (vecOutput.Length2D() < 0.1f)
						return Vector(0, 0, 0);

					auto a = (vecPrevious - vecCurrent) / flChokeTimeDelta;
					vecOutput += a * flTimeToExtrapolate;

					return vecOutput;
				};

				vecPredictedOrigin = PredictNextVelocity(m_pCurrentRecord->vecVelocity, m_pPrevious->vecVelocity);
				vecPredictedOrigin.z += simulationData.vecVelocity.z - flGravity * flTimeToExtrapolate;

				m_pCurrentRecord->flSimulationTime = m_pEntity->GetSimulationTime() + flTimeToExtrapolate;
				--iTicksLeft;

			} while (iTicksLeft);
		}

		m_pCurrentRecord->vecOrigin = simulationData.vecOrigin;
		m_pCurrentRecord->vecAbsOrigin = simulationData.vecOrigin;
	}*/
}

float Lagcompensation::GetClientInterpAmount()
{
	if (!i::ConVar->FindVar(XorStr("cl_interpolate"))->GetInt())
		return 0.0f;

	static CConVar* m_pMinUpdateRate = i::ConVar->FindVar(XorStr("sv_minupdaterate"));
	static CConVar* m_pMaxUpdateRate = i::ConVar->FindVar(XorStr("sv_maxupdaterate"));

	float m_flLerpAmount = i::ConVar->FindVar(XorStr("cl_interp"))->GetFloat();
	float m_nUpdateRate = i::ConVar->FindVar(XorStr("cl_updaterate"))->GetFloat();

	if (m_pMinUpdateRate && m_pMaxUpdateRate)
		m_nUpdateRate = std::clamp(m_nUpdateRate, m_pMinUpdateRate->GetFloat(), m_pMaxUpdateRate->GetFloat());

	float flLerpRatio = i::ConVar->FindVar(XorStr("cl_interp_ratio"))->GetFloat();
	if (flLerpRatio == 0)
		flLerpRatio = 1.0f;

	static CConVar* pMin = i::ConVar->FindVar(XorStr("sv_client_min_interp_ratio"));
	static CConVar* pMax = i::ConVar->FindVar(XorStr("sv_client_max_interp_ratio"));

	if (pMin && pMax && pMin->GetFloat() != -1)
		flLerpRatio = std::clamp(flLerpRatio, pMin->GetFloat(), pMax->GetFloat());

	return fmax(m_flLerpAmount, flLerpRatio / m_nUpdateRate);
}

/*
	//auto NetChannelInfo = i::EngineClient->GetNetChannelInfo();

	//constexpr float flMagicNumber = 0.00075f;

	//const float flLerpTime = GetClientInterpAmount();
	//float flLatency = NetChannelInfo->GetLatency(FLOW_INCOMING) + NetChannelInfo->GetLatency(FLOW_OUTGOING);

	//float flIncoming = i::EngineClient->GetNetChannelInfo()->GetLatency(FLOW_INCOMING) * 1000.f;
	//flRange += (min(flIncoming, 200.f) * flMagicNumber) - (i::EngineClient->GetNetChannelInfo()->GetLatency(FLOW_OUTGOING));

	//int nDeadTime = static_cast<int>(static_cast<float>(TICKS_TO_TIME(i::GlobalVars->iTickCount + TIME_TO_TICKS(flLatency))) - flRange);
	//if (TIME_TO_TICKS(mflSimulationTime + flLerpTime) < nDeadTime)
	//	return false;

	//return (i::GlobalVars->flCurrentTime - mflSimulationTime + flLerpTime) < flRange;
*/

bool Lagcompensation::IsValidRecord(float mflSimulationTime, float flRange)
{
	if (!i::EngineClient->GetNetChannelInfo())
		return false;

	auto NetChannelInfo = i::EngineClient->GetNetChannelInfo();

	static CConVar* sv_maxunlag = i::ConVar->FindVar("sv_maxunlag");

	int iTickBase = g::pLocal->GetTickBase();
	const float flLerpTime = GetClientInterpAmount();
	float flLatency = NetChannelInfo->GetLatency(FLOW_INCOMING) + NetChannelInfo->GetLatency(FLOW_OUTGOING);

	float flDeltaTime = fminf(flLatency + flLerpTime, sv_maxunlag->GetFloat()) - (i::GlobalVars->flCurrentTime - mflSimulationTime);
	if (fabsf(flDeltaTime) >= flRange)
		return false;

	int nDeadTime = static_cast<int>(static_cast<float>(TICKS_TO_TIME(i::GlobalVars->iTickCount + TIME_TO_TICKS(flLatency))) - flRange);
	if (TIME_TO_TICKS(mflSimulationTime + flLerpTime) < nDeadTime)
		return false;

	return true;
}

int Lagcompensation::FixTickCount(float flSimulationTime)
{

	return TIME_TO_TICKS(flSimulationTime + lagcomp.GetClientInterpAmount());
}

void Lagcompensation::UpdateIncomingSequences(INetChannel* pNetChannel)
{
	if (pNetChannel == nullptr)
		return;

	// set to real sequence to update, otherwise needs time to get it work again
	if (nLastIncomingSequence == 0)
		nLastIncomingSequence = pNetChannel->iInSequenceNr;

	// check how much sequences we can spike
	if (pNetChannel->iInSequenceNr > nLastIncomingSequence)
	{
		nLastIncomingSequence = pNetChannel->iInSequenceNr;
		vecSequences.emplace_front(SequenceObject_t(pNetChannel->iInReliableState, pNetChannel->iOutReliableState, pNetChannel->iInSequenceNr, i::GlobalVars->flRealTime));
	}

	// is cached too much sequences
	while (vecSequences.size() > 4096U)
		vecSequences.pop_back();
}

void Lagcompensation::ClearIncomingSequences()
{
	if (!vecSequences.empty())
	{
		nLastIncomingSequence = 0;
		vecSequences.clear();
	}
}

void Lagcompensation::AddLatencyToNetChannel(INetChannel* pNetChannel, float flLatency)
{
	for (const auto& sequence : vecSequences)
	{
		if (i::GlobalVars->flRealTime - sequence.flCurrentTime >= flLatency)
		{
			pNetChannel->iInReliableState = sequence.iInReliableState;
			pNetChannel->iInSequenceNr = sequence.iSequenceNr;
			break;
		}
	}
}

void Lagcompensation::LagRecord_t::ApplyMatrix(CBaseEntity* pEntity, EMatrixType iType) {

	if (!pMatricies || !pMatricies[iType] || pMatricies[iType]->Base() == nullptr)
		return;

	if (!this)
		return;

	//pEntity->SetCollisionBounds(vecMins, vecMaxs);
	pEntity->SetBoneCache(pMatricies[iType]);
	return pEntity->InvalidateBoneCache();
}

void Lagcompensation::LagRecord_t::ApplyMatrix(CBaseEntity* pEntity, matrix3x4a_t* pMatrix) {

	if (!pMatrix)
		return;

	//pEntity->SetCollisionBounds(vecMins, vecMaxs);
	pEntity->SetBoneCache(pMatrix);
	return pEntity->InvalidateBoneCache();
}

bool Lagcompensation::LagRecord_t::IsValid() {

	if (/*!this->bValid ||*/ this->bBreakingLagcompensation || !i::EngineClient->GetNetChannelInfo())
		return false;

	if (this->flSimulationTime < this->flOldSimulationTime)
		return false;

	auto NetChannelInfo = i::EngineClient->GetNetChannelInfo();

	static CConVar* sv_maxunlag = i::ConVar->FindVar("sv_maxunlag");

	const float flLerpTime = GetClientInterpAmount();
	float flLatency = NetChannelInfo->GetLatency(FLOW_INCOMING) + NetChannelInfo->GetLatency(FLOW_OUTGOING);

	float flDeltaTime = fminf(flLatency + flLerpTime, sv_maxunlag->GetFloat()) - (TICKS_TO_TIME(g::pLocal->GetTickBase()) - this->flSimulationTime);
	if (fabsf(flDeltaTime) >= 0.2f)
		return false;

	// idk if lagcomp counts with the servertick or current time
	int nDeadTime = static_cast<int>(static_cast<float>(TICKS_TO_TIME(networking.GetServerTick())) - 0.2f);
	if (TIME_TO_TICKS(this->flSimulationTime + flLerpTime) < nDeadTime)
		return false;

	nDeadTime = static_cast<int>(i::GlobalVars->flCurrentTime - 0.2f);
	if (TIME_TO_TICKS(this->flSimulationTime + flLerpTime) < nDeadTime)
		return false;

	return true;

	//INetChannelInfo* pNetChannel = i::EngineClient->GetNetChannelInfo();
	//if (!pNetChannel)
	//	return false;

	//float flServerTime = i::GlobalVars->flCurrentTime;
	//if (g::pLocal && g::pLocal->IsAlive())
	//	flServerTime = TICKS_TO_TIME(networking.GetCorrectedTickbase());
	//
	//const float flDeltaTime = std::clamp(pNetChannel->GetLatency(MAX_FLOWS) + lagcomp.GetClientInterpAmount(), 0.f, 0.2f) - (flServerTime - this->flSimulationTime);
	//if (fabsf(flDeltaTime) > 0.2f)
	//	return false;

	//int iChokedModifierClient = 0;
	//if (cfg::antiaim::bFakeDuck && IPT::HandleInput(cfg::antiaim::iFakeDuckKey))
	//	iChokedModifierClient = 15 - i::ClientState->nChokedCommands;

	//iChokedModifierClient += i::ClientState->clockDriftMgr.nServerTick + TIME_TO_TICKS(pNetChannel->GetLatency(FLOW_OUTGOING));

	//const auto flDeadTime = (int)(TICKS_TO_TIME(iChokedModifierClient) - 0.2f);
	//if (this->flSimulationTime < flDeadTime)
	//	return false;

	//return true;
}

void Lagcompensation::StartLagcompensation(CBaseEntity* pLocal) {

	if (g::bUpdatingSkins)
		return;

	for (size_t i = 0; i < i::GlobalVars->nMaxClients; i++) {

		CBaseEntity* pEntity = static_cast<CBaseEntity*>(i::EntityList->GetClientEntity(i));

		if (!pEntity || !pEntity->IsAlive() || !pEntity->GetModel() || pEntity->IsDormant() || pEntity == g::pLocal || !pEntity->IsEnemy(g::pLocal))
			continue;

		Lagcompensation::LagRecord_t data = Lagcompensation::LagRecord_t();

		data.vecAbsAngles = pEntity->GetAbsAngles();
		data.vecAbsOrigin = pEntity->GetAbsOrigin();
		data.vecEyeAngles = pEntity->GetEyeAngles();
		data.vecOrigin = pEntity->GetVecOrigin();
		data.flSimulationTime = pEntity->GetSimulationTime();
		data.iFlags = pEntity->GetFlags();
		data.vecMins = pEntity->GetCollideable()->OBBMins();
		data.vecMaxs = pEntity->GetCollideable()->OBBMaxs();
		data.flCollisionChangeTime = pEntity->GetCollisionChangeTime();
		data.flCollisionChangeOrigin = pEntity->GetCollisionChangeOrigin();

		pEntity->GetAnimationLayers(data.arrLayers);
		pEntity->GetPoseParameters(data.flPoses);
		pEntity->GetBoneCache(data.pMatricies[VISUAL]);

		arrBackupData[i].first = data;
		arrBackupData[i].second = true;
	}
}

void Lagcompensation::FinishLagcompensation(CBaseEntity* pLocal) {

	if (g::bUpdatingSkins)
		return;

	for (size_t i = 0; i < i::GlobalVars->nMaxClients; i++) {

		CBaseEntity* pEntity = static_cast<CBaseEntity*>(i::EntityList->GetClientEntity(i));

		if (!arrBackupData[i].second)
			continue;

		auto& data = arrBackupData[i].first;

		pEntity->SetAnimationLayers(data.arrLayers);
		pEntity->SetPoseParameters(data.flPoses);
		pEntity->SetBoneCache(data.pMatricies[VISUAL]);

		pEntity->SetAbsAngles(data.vecAbsAngles);
		pEntity->SetAbsOrigin(data.vecAbsOrigin);
		pEntity->GetEyeAngles() = data.vecEyeAngles;
		pEntity->GetVecOrigin() = data.vecOrigin;
		pEntity->GetSimulationTime() = data.flSimulationTime;
		pEntity->GetFlags() = data.iFlags;
		pEntity->GetCollideable()->OBBMins() = data.vecMins;
		pEntity->GetCollideable()->OBBMaxs() = data.vecMaxs;
		pEntity->GetCollisionChangeTime() = data.flCollisionChangeTime;
		pEntity->GetCollisionChangeOrigin() = data.flCollisionChangeOrigin;

		arrBackupData[i].second = false;
	}
}

bool Lagcompensation::DataChanged(CBaseEntity* pEntity, Lagcompensation::LagRecord_t* pPrevious) {

	if (pPrevious)
		if (pEntity->GetAnimationOverlays()[ANIMATION_LAYER_ALIVELOOP].flCycle == pPrevious->arrLayers[ANIMATION_LAYER_ALIVELOOP].flCycle)
			pEntity->GetSimulationTime() = pEntity->GetOldSimulationTime();

	return pEntity->GetSimulationTime() != pEntity->GetOldSimulationTime();
}