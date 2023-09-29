#pragma once
// used: angle/vector calculations
#include "../../SDK/math.h"
// used: convar, cliententitylist, physics, trace, clients, globals interfaces
#include "../../Interface/interfaces.h"
// used: baseentity, cliententity, baseweapon, weapondata classes
#include "../../SDK/Entity.h"
// used: lagcompensation::lagrecord*
#include "../../Features/Rage/Animations/Lagcompensation.h"

struct FireBulletData_t
{
	Vector			vecPosition = { };
	Vector			vecDirection = { };
	Trace_t			enterTrace = { };
	float			flCurrentDamage = 0.0f;
	int				iPenetrateCount = 0;
};

// @credits: outlassn
class CAutoWall
{
public:
	// Get
	/* returns damage at point and simulated bullet data (if given) */
	static float GetDamage( CBaseEntity* pLocal, const Vector& vecEyePosition, const Vector& vecPoint, CBaseCombatWeapon* pWeapon, Lagcompensation::LagRecord_t* pRecord, FireBulletData_t* pDataOut = nullptr);
	/* calculates damage factor */
	static void ScaleDamage( const int iHitGroup, CBaseEntity* pEntity, const float flWeaponArmorRatio, const float flWeaponHeadShotMultiplier, float& flDamage );
	/* simulates fire bullet to penetrate up to 4 walls, return true when hitting player */
	static bool SimulateFireBullet( CBaseEntity* pLocal, CBaseCombatWeapon* pWeapon, FireBulletData_t& data, Lagcompensation::LagRecord_t* pRecord);
	/* I need this exlo trust me this will be fun*/
	static bool CanHitFloatingPoint(const Vector& vecPoint, const Vector& vecSource, Lagcompensation::LagRecord_t* pRecord);
	/* BBOX can be shot or not */
	static bool bCollidePoint(const Vector& vecStart, const Vector& vecEnd, mstudiobbox_t* pHitbox, matrix3x4a_t* aMatrix);
	/* Checks if matrixes collide, aka can we shoot every matrix at once */
	static int SafePoint(Vector& vecEyePosition, Lagcompensation::LagRecord_t* pRecord, Vector vecShootposition, int iHitbox);
	/* Checks if our trace hit the desired hitbox */
	static bool bTraceMeantForHitbox(const Vector& vecEyePosition, const Vector& vecEnd, int iHitbox, Lagcompensation::LagRecord_t* pRecord, EMatrixType iMatrix = RESOLVE);

private:
	// Main
	static void ClipTraceToPlayers( const Vector& vecAbsStart, const Vector& vecAbsEnd, const unsigned int fMask, CTraceFilter* pFilter, Trace_t* pTrace, Lagcompensation::LagRecord_t* pRecord, const float flMinRange = 0.0f );
	static bool TraceToExit( Trace_t& enterTrace, Trace_t& exitTrace, const Vector& vecPosition, const Vector& vecDirection, const CBaseEntity* pClipPlayer, Lagcompensation::LagRecord_t* pRecord);
	static bool HandleBulletPenetration( CBaseEntity* pLocal, const CCSWeaponInfo* pWeaponData, const surfacedata_t* pEnterSurfaceData, FireBulletData_t& data, Lagcompensation::LagRecord_t* pRecord);
};

inline CAutoWall autowall;