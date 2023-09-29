#include "GrenadePredict.h"
#include "../Rage/autowall.h"
#include "../../SDK/InputSystem.h"

void grenadePrediction::Reset() {

	vecStart = Vector{};
	vecMove = Vector{};
	vecVelocity = Vector{};
	flVelocity = float{};
	flPower = float{};

	vecPath.clear();
	vecBounces.clear();
}

void grenadePrediction::AutoThrow( CUserCmd* pCmd ) 
{
	/* Config check, bind */
	if (!cfg::rage::bAutoNade || !IPT::HandleInput(cfg::rage::iAutoNadeBind))
		return;

	if (!g::pLocal->IsAlive( ) || vecBounces.empty( ))
		return;

	// get refference to last entry in bounces
	Bounce_t& lastBounce = vecBounces.back();
	// Should we release?
	static bool bRemoveAttack = false;

	// loop through all entities
	for (int i = 1; i < i::GlobalVars->nMaxClients; i++) 
	{
		CBaseEntity* pEntity = static_cast< CBaseEntity* >( i::EntityList->GetClientEntity( i ) );
		if (!pEntity || !pEntity->IsAlive( ) || !pEntity->IsEnemy( g::pLocal ) )
			continue;

		Vector vecCenter = pEntity->GetWorldSpaceCenter( );
		Vector vecDelta = vecCenter - lastBounce.vecPoint;

		if (iID == WEAPON_HEGRENADE && !vecDelta.IsZero( )) {

			if (vecDelta.Length( ) > 350.f)
				continue;

			static CTraceFilter pFilter( nullptr );
			Trace_t pTrace;
			i::EngineTrace->TraceRay( Ray_t( lastBounce.vecPoint, vecCenter ), MASK_SHOT, &pFilter, &pTrace );

			if (!pTrace.pHitEntity || pTrace.pHitEntity != pEntity)
				continue;

			float flDamage = 105.f * std::exp( ( -( vecDelta.Length( ) - 25.f ) / 140.f ) * ( ( vecDelta.Length( ) - 25.f ) / 140.f ) );
			autowall.ScaleDamage( HITGROUP_CHEST, pEntity, 1.f, 4.f, flDamage );

			flDamage = min( flDamage, ( pEntity->GetArmor( ) > 0 ) ? 57.f : 98.f );

			if (bRemoveAttack)
			{
				bRemoveAttack = false;
				// remove attack
				pCmd->iButtons &= ~IN_ATTACK;
				return;
			}

			//autonade
			if (flDamage >= cfg::rage::iAutoNadeMinDmg)
			{
				// set to true so we can remove attack next tick
				bRemoveAttack = true;
				pCmd->iButtons |= IN_ATTACK;
			}
		}
	}
}

void grenadePrediction::Draw() {
	
	static CTraceFilter pFilter(nullptr);
	Trace_t pTrace;
	std::pair<float, CBaseEntity*> pTarget{ 0.f, nullptr };

	CBaseEntity* pLocal = CBaseEntity::GetLocalPlayer();
	if (!pLocal || !pLocal->IsAlive())
		return;

	if (vecPath.size() < 2)
		return;

	pFilter = CTraceFilter(pLocal);

	Vector vecPrevious = vecPath.front();

	for (const Vector& vecCurrent : vecPath) {

		Vector vecScreen[2];

		if(cfg::misc::bNadePrediction)
		{
			if (i::DebugOverlay->ScreenPosition(vecPrevious, vecScreen[0]) || i::DebugOverlay->ScreenPosition(vecCurrent, vecScreen[1]))
				continue;

			i::Surface->DrawSetColor(Color(255, 255, 255, 255));
			i::Surface->DrawLine(vecScreen[0].x, vecScreen[0].y, vecScreen[1].x, vecScreen[1].y);
		}

		vecPrevious = vecCurrent;
	}

	static Color colTextColor{};
	for (size_t i = 1; i < i::GlobalVars->nMaxClients; i++) {

		CBaseEntity* pEntity = static_cast<CBaseEntity*>(i::EntityList->GetClientEntity(i));
		if (!pEntity || !pEntity->IsAlive() || !pEntity->IsEnemy(pLocal))
			continue;

		Vector vecCenter = pEntity->GetWorldSpaceCenter();

		Vector vecDelta = vecCenter - vecPrevious;

		if (iID == WEAPON_HEGRENADE) {

			if (vecDelta.Length() > 350.f)
				continue;

			i::EngineTrace->TraceRay(Ray_t(vecPrevious, vecCenter), MASK_SHOT, &pFilter, &pTrace);

			if (!pTrace.pHitEntity || pTrace.pHitEntity != pEntity)
				continue;

			float flDamage = 105.f * std::exp((-(vecDelta.Length() - 25.f) / 140.f) * ((vecDelta.Length() - 25.f) / 140.f));
			autowall.ScaleDamage(HITGROUP_CHEST, pEntity, 1.f, 4.f, flDamage);

			flDamage = min(flDamage, (pEntity->GetArmor() > 0) ? 57.f : 98.f);

			if (pEntity->GetHealth() < flDamage)
				colTextColor = Color(50, 255, 50, 180);
			else
				colTextColor = Color(255, 255, 255, 180);

			if (flDamage > pTarget.first)
				pTarget = { flDamage, pEntity };
		}
	}

	if (cfg::misc::bNadePrediction) {
		if (pTarget.second) {

			Vector vecScreen{};

			if (!vecBounces.empty())
				vecBounces.back().colColor = { 0, 255, 0, 255 };

			if (!i::DebugOverlay->ScreenPosition(vecPrevious, vecScreen))
				i::Surface->DrawT(vecScreen.x, vecScreen.y, colTextColor, g::fonts::FlagESP, true, std::format("{} dmg", pTarget.first).c_str());
		}

		for (const Bounce_t& vecBounce : vecBounces) {

			Vector vecScreen{};

			if (!i::DebugOverlay->ScreenPosition(vecBounce.vecPoint, vecScreen)) {
				i::Surface->DrawSetColor(vecBounce.colColor);
				i::Surface->DrawOutlinedCircle(vecScreen.x, vecScreen.y, 2, 85);
			}
		}
	}
}

void grenadePrediction::Run() {

	bool bAttack1, bAttack2;

	Reset();

	/* Config check */
	if (!cfg::misc::bNadePrediction && !cfg::rage::bAutoNade)
		return;

	CBaseEntity* pLocal = CBaseEntity::GetLocalPlayer();

	if (!pLocal || !pLocal->IsAlive())
		return;

	CBaseCombatWeapon* pWeapon = pLocal->GetWeapon();

	if (!pWeapon)
		return;

	const short iIndex = pWeapon->GetItemDefinitionIndex();
	const CCSWeaponInfo* pWeaponInfo = pWeapon->GetCSWpnData();

	if (!pWeaponInfo)
		return;

	if (!pWeapon->IsGrenade())
		return;

	bAttack1 = g::pCmd->iButtons & IN_ATTACK;
	bAttack2 = g::pCmd->iButtons & IN_SECOND_ATTACK;

	if (!bAttack1 && !bAttack2 /* && always on predict */ && !IPT::HandleInput(cfg::rage::iAutoNadeBind))
		return;

	iID = iIndex;
	flPower = static_cast<CBaseCSGrenade*>(pWeapon)->GetThrowStrength();
	flVelocity = pWeaponInfo->flThrowVelocity;

	Simulate(pLocal);
}

void grenadePrediction::Simulate(CBaseEntity* pLocal) {

	Setup(pLocal);

	size_t iStep = static_cast<size_t>(TIME_TO_TICKS(0.05f)), iTimer{ 0u };

	for (size_t i {0u}; i < 2 << 11; ++i) {

		if (!iTimer)
			vecPath.push_back(vecStart);

		size_t iFlags = Advance(i, pLocal);

		if (iFlags & DETONATE)
			break;

		if (iFlags & BOUNCE || iTimer >= iStep)
			iTimer = 0;
		else
			++iTimer;

		if (vecVelocity == Vector{})
			break;
	}

	if (iID == WEAPON_MOLOTOV || iID == WEAPON_FIREBOMB || iID == WEAPON_INCGRENADE) {

		Trace_t pTrace;
		PhysicsPushEntity(vecStart, {0.f, 0.f, -131.f}, pTrace, pLocal);

		if (pTrace.flFraction < 0.9f)
			vecStart = pTrace.vecEnd;
	}

	vecPath.push_back(vecStart);
	vecBounces.push_back({ vecStart, {255, 0, 0, 255} });
}

void grenadePrediction::Setup(CBaseEntity* pLocal) {

	Vector vecAngle = g::pCmd->angViewPoint;

	float flPitch = vecAngle.x;

	/* Normalize?!?!?! */
	if (flPitch < -90.f)
		flPitch += 360.f;
	else if (flPitch > 90.f)
		flPitch -= 360.f;

	vecAngle.x = flPitch - (90.f - abs(flPitch)) * 10.f / 90.f;

	float flTempVelocity = flVelocity * 0.9f;
	flTempVelocity = std::clamp(flTempVelocity, 15.f, 750.f);

	flTempVelocity *= ((flPower * 0.7f) + 0.3f);

	Vector vecForward;
	M::AngleVectors(vecAngle, &vecForward);

	vecStart = localAnim->GetShootPosition();
	vecStart.z += (flPower * 12.f) - 12.f;

	Vector vecEnd = vecStart + (vecForward * 22.f);

	Trace_t pTrace;
	TraceHull(vecStart, vecEnd, pTrace, pLocal);

	vecStart = pTrace.vecEnd - (vecForward * 6.f);

	vecVelocity = pLocal->GetVelocity();
	vecVelocity *= 1.25f;
	vecVelocity += (vecForward * flTempVelocity);
}

void grenadePrediction::ResolveFlyCollisionBounce(Trace_t& refTrace, CBaseEntity* pLocal) {

	float flSurface = 1.f;

	if (refTrace.pHitEntity) {
		if (refTrace.pHitEntity->IsBreakable()) {
			
			EClassIndex iClass = refTrace.pHitEntity->GetClientClass()->nClassID;
			if (iClass != EClassIndex::CFuncBrush &&
				iClass != EClassIndex::CBaseDoor &&
				iClass != EClassIndex::CCSPlayer &&
				iClass != EClassIndex::CBaseEntity) {

				PhysicsPushEntity(vecStart, vecMove, refTrace, refTrace.pHitEntity);

				vecVelocity *= 0.4f;
				return;
			}
		}
	}

	float flElasticity = 0.45f * flSurface;
	flElasticity = std::clamp(flElasticity, 0.f, 0.9f);

	Vector vecTempVelocity{};
	PhysicsClipVelocity(vecVelocity, refTrace.plane.vecNormal, vecTempVelocity, 2.f);
	vecTempVelocity *= flElasticity;

	if (refTrace.plane.vecNormal.z > 0.7f) {

		float flSpeed = vecTempVelocity.LengthSqr();

		if (flSpeed > 96000.f) {

			float flLength = vecTempVelocity.Normalized().DotProduct(refTrace.plane.vecNormal);
			if (flLength > 0.5f)
				vecTempVelocity *= 1.5f - flLength;
		}

		if (flSpeed < 400.f)
			vecVelocity = Vector{};
		else {

			vecVelocity = vecTempVelocity;

			float flLeft = 1.f - refTrace.flFraction;

			PhysicsPushEntity(refTrace.vecEnd, vecTempVelocity * (flLeft * i::GlobalVars->flIntervalPerTick), refTrace, pLocal);
		}
	}

	else {

		vecVelocity = vecTempVelocity;

		float flLeft = 1.f - refTrace.flFraction;

		PhysicsPushEntity(refTrace.vecEnd, vecTempVelocity * (flLeft * i::GlobalVars->flIntervalPerTick), refTrace, pLocal);
	}

	vecBounces.push_back({refTrace.vecEnd, { 255, 255, 255, 210 } });
}

void grenadePrediction::PhysicsPushEntity(Vector& vecStart, const Vector& vecMove, Trace_t& refTrace, CBaseEntity* pEntity) {

	Vector vecEnd = vecStart + vecMove;

	TraceHull(vecStart, vecEnd, refTrace, pEntity);
}

void grenadePrediction::TraceHull(const Vector& vecStart, const Vector& vecEnd, Trace_t& refTrace, CBaseEntity* pEntity) {

	static CTraceFilter pFilter(nullptr);
	pFilter = CTraceFilter(pEntity);

	i::EngineTrace->TraceRay(Ray_t(vecStart, vecEnd, { -2.f, -2.f, -2.f }, { 2.f, 2.f, 2.f }), MASK_SOLID, &pFilter, &refTrace);
}

void grenadePrediction::PhysicsAddGravityMove(Vector& vecMove) {

	float flGravity = 800.f * 0.4f;

	vecMove.x = vecVelocity.x * i::GlobalVars->flIntervalPerTick;
	vecMove.y = vecVelocity.y * i::GlobalVars->flIntervalPerTick;

	float z = vecVelocity.z - (flGravity * i::GlobalVars->flIntervalPerTick);

	vecMove.z = ((vecVelocity.z + z) / 2.f) * i::GlobalVars->flIntervalPerTick;

	vecVelocity.z = z;
}

void grenadePrediction::PhysicsClipVelocity(const Vector& vecIn, const Vector& vecNormal, Vector& vecOut, float flOverBounce){

	constexpr float STOP_EPSILON = 0.1f;

	float flBackoff = vecIn.DotProduct(vecNormal) * flOverBounce;

	for (int i{ }; i < 3; ++i) {
		vecOut[i] = vecIn[i] - (vecNormal[i] * flBackoff);

		if (vecOut[i] > -STOP_EPSILON && vecOut[i] < STOP_EPSILON)
			vecOut[i] = 0.f;
	}
}

size_t grenadePrediction::Advance(size_t iTick, CBaseEntity* pLocal) {

	size_t iFlags{ NONE };
	Trace_t pTrace{};

	PhysicsAddGravityMove(vecMove);
	PhysicsPushEntity(vecStart, vecMove, pTrace, pLocal);

	if (Detonate(iTick, pTrace))
		iFlags |= DETONATE;

	if (pTrace.flFraction != 1.f) {

		iFlags |= BOUNCE;
		ResolveFlyCollisionBounce(pTrace, pLocal);
	}

	vecStart = pTrace.vecEnd;

	if (auto size = vecBounces.size() - 1; !vecBounces.empty() && size > 9)
		vecBounces.erase(vecBounces.begin() + size);

	return iFlags;
}

bool grenadePrediction::Detonate(size_t iTick, Trace_t& refTrace) {

	float flTime = TICKS_TO_TIME(iTick);
	static CConVar* weapon_molotov_maxdetonateslope = i::ConVar->FindVar(XorStr("weapon_molotov_maxdetonateslope"));
	if (!weapon_molotov_maxdetonateslope)
		return false;

	switch (iID) {

	case WEAPON_FLASHBANG:
		break;

	case WEAPON_HEGRENADE:
		return flTime >= 1.5f && !(iTick % TIME_TO_TICKS(0.2f));

	case WEAPON_SMOKEGRENADE:
		return vecVelocity.Length() <= 0.1f && !(iTick % TIME_TO_TICKS(0.2f));

	case WEAPON_DECOY:
		return vecVelocity.Length() <= 0.2f && !(iTick % TIME_TO_TICKS(0.2f));

	case WEAPON_MOLOTOV:
		break;

	case WEAPON_FIREBOMB:
		if (refTrace.flFraction != 1.f && (std::cos(M_DEG2RAD(weapon_molotov_maxdetonateslope->GetFloat())) <= refTrace.plane.vecNormal.z))
			return true;

		return flTime >= weapon_molotov_maxdetonateslope->GetFloat() && !(iTick % TIME_TO_TICKS(0.1f));

	default:
		return false;
	}

	return false;
}