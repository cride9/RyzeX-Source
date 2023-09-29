#pragma once
#include "../../../SDK/Menu/config.h"
#include "../../../SDK/math.h"
#include "Lagcompensation.h"

#define ANIM_TRANSITION_WALK_TO_RUN 0
#define ANIM_TRANSITION_RUN_TO_WALK 1
#define CSGO_ANIM_WALK_TO_RUN_TRANSITION_SPEED 2.0f
#define CSGO_ANIM_ONGROUND_FUZZY_APPROACH 8.0f
#define CSGO_ANIM_ONGROUND_FUZZY_APPROACH_CROUCH 16.0f
#define CSGO_ANIM_LADDER_CLIMB_COVERAGE 100.0f
#define CSGO_ANIM_RUN_ANIM_PLAYBACK_MULTIPLIER 0.85f

#define CS_PLAYER_SPEED_RUN 260.0f
#define CS_PLAYER_SPEED_VIP 227.0f
#define CS_PLAYER_SPEED_SHIELD 160.0f
#define CS_PLAYER_SPEED_HAS_HOSTAGE 200.0f
#define CS_PLAYER_SPEED_STOPPED 1.0f
#define CS_PLAYER_SPEED_OBSERVER 900.0f

#define CS_PLAYER_SPEED_DUCK_MODIFIER 0.34f
#define CS_PLAYER_SPEED_WALK_MODIFIER 0.52f
#define CS_PLAYER_SPEED_CLIMB_MODIFIER 0.34f
#define CS_PLAYER_HEAVYARMOR_FLINCH_MODIFIER 0.5f

#define CS_PLAYER_DUCK_SPEED_IDEAL 8.0f

#define CSGO_ANIM_LOWER_CATCHUP_IDLE	100.0f
#define CSGO_ANIM_AIM_NARROW_WALK	0.8f
#define CSGO_ANIM_AIM_NARROW_RUN	0.5f
#define CSGO_ANIM_AIM_NARROW_CROUCHMOVING	0.5f
#define CSGO_ANIM_LOWER_CATCHUP_WITHIN	3.0f
#define CSGO_ANIM_LOWER_REALIGN_DELAY	1.1f
#define CSGO_ANIM_READJUST_THRESHOLD	120.0f
#define EIGHT_WAY_WIDTH 22.5f

class Animations {

public:

	int iLastGuessedYaw{};
	float flGuessedYaw{};
	int nInvalidateFlags{};
	Lagcompensation::AnimationInfo_t pPlayerLogs[65];

	struct AnimationData_t
	{
		float flPrimaryCycle{};
		float flMovePlaybackRate{};
		float flFeetWeight{};
		float flVelocityLengthXY{};
		int iMoveState{};
	};

	enum EResolverHandler {

		WEAPONFIRE,
		PLAYERHURT,
		BULLETIMPACT,
		PLAYERDEATH,
		HANDLERCOUNT
	};

	AnimationData_t pAnimationData[65];
	std::array<bool, HANDLERCOUNT> bResolverHandler{false};
	float iHitDmg = 0.f;
	int iHitHitbox = 0;

	bool NewDataRecievedFromServer(CBaseEntity* pPlayer);
	void UpdateClientSideAnimations(CBaseEntity* pEntity, Lagcompensation::LagRecord_t* pRecord);
	float GetYawRotation(Lagcompensation::LagRecord_t* pRecord, int nRotationSide);
	std::array<int, 65> arrMissedShots{0};
	bool didHurt = false, didFire = false, didImpact = false, didDie = false;
	Vector bBulletImpact = Vector( 0, 0, 0 );

	void ResolverHandler(IGameEvent*);
	void ResolverLogic();
	float GetVelocityLengthXY(CBaseEntity* pEntity);
	void SetYaw(Lagcompensation::LagRecord_t* pRecord, int flYaw);
    float GetLocalCycleIncrement(CBaseEntity* pEntity, float flPlaybackrate);
    void Resolver(CBaseEntity* pEntity, Lagcompensation::LagRecord_t* pRecord, Lagcompensation::LagRecord_t* pPrevious);

	/* PASTED FUNTIONS */
	void RebuiltLayer6(CAnimState*, Lagcompensation::LagRecord_t* pEntity, Lagcompensation::LagRecord_t::LayerData_t* pLayer);
	void GenerateSafePointMatricies(CBaseEntity* pEntity, Lagcompensation::LagRecord_t* pRecord, Lagcompensation::LagRecord_t* pPrevious = nullptr);
	float BuildFootYaw(CBaseEntity* pEntity, Lagcompensation::LagRecord_t* pRecord);
	void RebuildEnemyAnimations(CBaseEntity* pEntity, Lagcompensation::LagRecord_t* pRecord, Lagcompensation::AnimationInfo_t* pLog);
	void SetupPlayerMatrix(CBaseEntity* pEntity, Lagcompensation::LagRecord_t* pRecord, matrix3x4a_t* pMatrix, int nFlags);
	float FindMatchingPlaybackRate(float flTarget, std::array<float, 120>& arrPlaybackRates);
	void FindDesyncSide(Lagcompensation::LagRecord_t* pRecord);
	void GetSideLayersForResolver(CBaseEntity* pEntity, Lagcompensation::LagRecord_t* pRecord);
	void GenerateFreestandMatricies(CBaseEntity* pEntity, Lagcompensation::LagRecord_t* pRecord);

	/* resolver */
	std::pair<CAnimationLayer*, float*> BuildSideLayerAndPose(CBaseEntity*, float);
	bool CopyCachedMatrix(CBaseEntity* pEnt, matrix3x4a_t* pMatrix, int nBoneCount);
	void TransformateMatrix(CBaseEntity* pPlayer);
	void SetUpMovement(Lagcompensation::LagRecord_t* pRecord, CAnimState* pState, Lagcompensation::LagRecord_t::LayerData_t* pLayer);
	void InterpolateMatricies(CBaseEntity* pEntity = nullptr);

private:
	void FakePitchResolver( CBaseEntity*, Lagcompensation::LagRecord_t* );
};
inline Animations anims;