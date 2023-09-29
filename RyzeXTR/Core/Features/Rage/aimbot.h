#pragma once
#include "Animations/Lagcompensation.h"
#include "../../SDK/Menu/config.h"
#include "../../SDK/InputSystem.h"
#include "../../SDK/Menu/gui.h"
#include "../Misc/enginepred.h"
#include "../../SDK/Entity.h"
#include "exploits.h"
#include "autowall.h"

enum EConditions : int {
	CONDITION_BETWEEN_SHOTS,
	CONDITION_INAIR,
	CONDITION_MAX
};

enum EHitboxType : int {
	NORMAL,
	MULTIPOINT,
	SAFE,
	HITBOXTYPE_MAX
};

struct Hitscan_t {

	Hitscan_t() {}
	Hitscan_t(Lagcompensation::LagRecord_t* _record, Vector& _point, FireBulletData_t& data, bool _baimlethal, bool _safe, int _damage) {

		pRecord = _record;
		vecPoint = _point;

		bBaimLethal = _baimlethal;
		bSafe = _safe;
		bBacktrack = &lagcomp.GetLog(_record->iEntIndex).pRecord.front() != _record;

		flAnimationVelocity = _record->flAnimationVelocity;
		flDesyncDelta = _record->flDesyncDelta;
		flDamage = static_cast<int>(data.flCurrentDamage);

		iHitbox = data.enterTrace.iHitbox;
		iHitgroup = data.enterTrace.iHitGroup;
		iTransformedDamage = _damage;
	}

	Lagcompensation::LagRecord_t* pRecord = nullptr;

	Vector vecPoint{};

	bool bBaimLethal{};
	bool bSafe{};
	bool bBacktrack{};

	float flAnimationVelocity{};
	float flDesyncDelta{};
	float flDamage{};

	int iHitbox{};
	int iHitgroup{};
	int iTransformedDamage{};

	bool operator<(const Hitscan_t& other) const {
		// Sort by baimlethal (true first, false second)
		if (bBaimLethal && !other.bBaimLethal)
			return true;
		if (!bBaimLethal && other.bBaimLethal)
			return false;

		// Sort by bSafe (true first, false second)
		if (bSafe && !other.bSafe)
			return true;
		if (!bSafe && other.bSafe)
			return false;

		// Sort by flDesyncDelta (smaller values first)
		if (flDesyncDelta < other.flDesyncDelta)
			return true;
		if (flDesyncDelta > other.flDesyncDelta)
			return false;

		// Sort by flAnimationVelocity (larger values first)
		return flAnimationVelocity > other.flAnimationVelocity;
	}
};

struct rageBotData_t
{
	rageBotData_t() { pRecord, pAimbotTarget, pTargetMatrix = nullptr; }

	Lagcompensation::LagRecord_t* pRecord{};
	CBaseEntity* pAimbotTarget{};
	matrix3x4a_t* pTargetMatrix{};
	Vector vecLocalShootPosition{};
	Vector vecTargetShootPosition{};

	int	iHealth{};
	int	iTickcount{};
	int	iHitbox{};
	int	iHitGroup{};
	int iEntityIndex{};

	bool bBacktrack{};
	bool bCanShoot{};
	bool bSafe{};

	float flTargetSimulation{};
	float flDamage{};
	float flHitchance{};

	int iBacktrackTicks{};
	int iServerHitbox{};

	void SetTarget(Lagcompensation::LagRecord_t* _pRecord, Vector vecEyePosition, bool _bBacktrack) {

		pRecord = _pRecord;
		pAimbotTarget = _pRecord->pEntity;
		pTargetMatrix = _pRecord->pMatricies[1];
		flTargetSimulation = _pRecord->flSimulationTime;
		iHealth = _pRecord->pEntity->GetHealth();
		vecLocalShootPosition = vecEyePosition;
		bBacktrack = _bBacktrack;
		iEntityIndex = _pRecord->iEntIndex;
	}

	void ClearTarget() {

		pRecord = nullptr;
		pAimbotTarget = nullptr;
		pTargetMatrix = nullptr;
		flTargetSimulation = 0.f;
		iHealth = -1;
		vecLocalShootPosition = Vector(0, 0, 0);
		bBacktrack = false;
		flHitchance = 0.f;
	}

	sol::object _lua_get( sol::stack_object key, sol::this_state L ) {
		// we use stack_object for the arguments because we
		// know the values from Lua will remain on Lua's stack,
		// so long we we don't mess with it
		auto maybe_string_key
			= key.as< sol::optional< std::string > >( );
		if (maybe_string_key) {
			const std::string& k = *maybe_string_key;
			if (k == XorStr( "GetBacktrack" )) { return sol::object( L, sol::in_place, this->iBacktrackTicks ); }
			else if (k == XorStr( "GetDamage" )) { return sol::object( L, sol::in_place, this->flDamage ); }
			else if (k == XorStr( "GetServerHitbox" )) { return sol::object( L, sol::in_place, this->iServerHitbox ); }
			else if (k == XorStr( "GetHitChance" )) { return sol::object( L, sol::in_place, this->flHitchance ); }
			else if (k == XorStr( "GetLocalShootPosition" )) { return sol::object( L, sol::in_place, this->vecLocalShootPosition ); }
			else if (k == XorStr( "GetTarget" )) { return sol::object( L, sol::in_place, this->pAimbotTarget ); }
			else if (k == XorStr( "GetTargetSimulationTime" )) { return sol::object( L, sol::in_place, this->flTargetSimulation ); }
			else if (k == XorStr( "GetClientHitbox" )) { return sol::object( L, sol::in_place, this->iHitbox ); }
			else if (k == XorStr( "GetServerHitbox" )) { return sol::object( L, sol::in_place, this->iServerHitbox ); }
			else if (k == XorStr( "GetSafe" )) { return sol::object( L, sol::in_place, this->bSafe ); }
			else if (k == XorStr( "GetResolveDelta" )) { return sol::object( L, sol::in_place, this->pRecord->flResolveDelta ); }
			else if (k == XorStr( "CanShoot" )) { return sol::object( L, sol::in_place, this->bCanShoot ); }
		}

		// No valid key: push nil
		return sol::object( L, sol::in_place, sol::lua_nil );
	}
};

struct weaponConfig_t {

	CBaseCombatWeapon* pWeapon{};

	int iMinimumDamage{};
	int iTransformedDamage{};
	int iHitchance{};
	int iAccuracyBoost{};
	int iAutostopValue{};
	int iHeadScale{};
	int iBodyScale{};

	bool bOverrideDamage{};
	bool bAutoScope{};
	bool bSafePoint{};

	bool bAutostop{};
	bool bConditions[CONDITION_MAX]{};
	std::vector<int> vecHitboxes[HITBOXTYPE_MAX]{};
};

class CAimBot {

public:
	void CreateMove(CUserCmd* pCmd, CBaseEntity* pLocal);
    void ResetAimbotData();
	void PostPrediction(CUserCmd* pCmd, bool& bSendPacket);

	rageBotData_t& GetHitLogData() { return hitlogData; }
	rageBotData_t& GetAimbotData() { return aimData; }
	weaponConfig_t& GetCurrentConfig() { return curConfig; }

private:

	bool bShouldSendPacket = false;
	int iTickCount = 0;

	rageBotData_t aimData;
	rageBotData_t hitlogData;

	weaponConfig_t curConfig;
	Vector vecEyePosition;



	bool HasEnoughAccuracy(CBaseEntity* pLocal, float flWeaponInAccuracy);
	bool HitChance(CUserCmd* pCmd, CBaseEntity* pLocal, Vector vecWorldPosition, Vector vecPosition, Lagcompensation::LagRecord_t* pRecord);
	
	void GetHitBoxes(int i, std::vector<int>& vecOut, int iWeapon);
	bool AutoStop(std::vector<Lagcompensation::AnimationInfo_t*>& vecIn, CBaseEntity* pLocal, CUserCmd* pCmd, bool bSkipCheck);

	weaponConfig_t GetWeaponConfiguration(short iItemDefinitionIndex);

	Vector ScanHitboxes(std::vector<Lagcompensation::AnimationInfo_t*>& vecIn, CBaseEntity* pLocal);

	std::vector<Lagcompensation::AnimationInfo_t*> GetTargetableEntities(CBaseEntity* pLocal);
	std::vector<Vector> CreatePoints(Vector, CBaseCombatWeapon*, Lagcompensation::LagRecord_t*, int, EMatrixType = RESOLVE, bool = false);
};
inline CAimBot aimbot;