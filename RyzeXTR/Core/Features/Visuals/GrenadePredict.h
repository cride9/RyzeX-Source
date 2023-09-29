#pragma once
#include "../../Interface/interfaces.h"
#include "../../SDK/Menu/config.h"
#include "../../SDK/Entity.h"
#include "../../globals.h"

namespace grenadePrediction {

	enum ENadeFlags : size_t {

		NONE = 0,
		DETONATE = 2 << 0,
		BOUNCE = 2 << 1
	};

	struct Bounce_t {

		Vector vecPoint;
		Color colColor;
	};

	using Path_t = std::vector<Vector>;
	using Bounces_t = std::vector<Bounce_t>;

	inline int iID;
	inline Path_t vecPath;
	inline Bounces_t vecBounces;
	inline float flVelocity;
	inline float flPower;
	inline Vector vecStart;
	inline Vector vecVelocity;
	inline Vector vecMove;

	void Reset();
	void AutoThrow(CUserCmd* pCmd);
	void Draw();
	void Run();
	void Simulate(CBaseEntity* pLocal);
	void Setup(CBaseEntity* pLocal);
	void ResolveFlyCollisionBounce(Trace_t& refTrace, CBaseEntity* pLocal);
	void PhysicsPushEntity(Vector& vecStart, const Vector& vecMove, Trace_t& refTrace, CBaseEntity* pEntity);
	void TraceHull(const Vector& vecStart, const Vector& vecEnd, Trace_t& refTrace, CBaseEntity* pEntity);
	void PhysicsAddGravityMove(Vector& vecMove);
	void PhysicsClipVelocity(const Vector& vecIn, const Vector& vecNormal, Vector& vecOut, float flOverBounce);

	size_t Advance(size_t iTick, CBaseEntity* pLocal);

	bool Detonate(size_t iTick, Trace_t& refTrace);
}