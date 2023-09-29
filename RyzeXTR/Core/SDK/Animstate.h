#pragma once
#include <cstddef>
class CBaseCombatWeapon;

/* max animation layers */
#define MAXOVERLAYS 15

enum EPoseParam {

	STRAFE_YAW,
	STAND,
	LEAN_YAW,
	SPEED,
	LADDER_YAW,
	LADDER_SPEED,
	JUMP_FALL,
	MOVE_YAW,
	MOVE_BLEND_CROUCH,
	MOVE_BLEND_WALK,
	MOVE_BLEND_RUN,
	BODY_YAW,
	BODY_PITCH,
	AIM_BLEND_STAND_IDLE,
	AIM_BLEND_STAND_WALK,
	AIM_BLEND_STAND_RUN,
	AIM_BLEND_COURCH_IDLE,
	AIM_BLEND_CROUCH_WALK,
	DEATH_YAW
};

enum EServerPoseParams
{
	PLAYER_POSE_PARAM_FIRST = 0,
	PLAYER_POSE_PARAM_LEAN_YAW = PLAYER_POSE_PARAM_FIRST,
	PLAYER_POSE_PARAM_SPEED,
	PLAYER_POSE_PARAM_LADDER_SPEED,
	PLAYER_POSE_PARAM_LADDER_YAW,
	PLAYER_POSE_PARAM_MOVE_YAW,
	PLAYER_POSE_PARAM_RUN,
	PLAYER_POSE_PARAM_BODY_YAW,
	PLAYER_POSE_PARAM_BODY_PITCH,
	PLAYER_POSE_PARAM_DEATH_YAW,
	PLAYER_POSE_PARAM_STAND,
	PLAYER_POSE_PARAM_JUMP_FALL,
	PLAYER_POSE_PARAM_AIM_BLEND_STAND_IDLE,
	PLAYER_POSE_PARAM_AIM_BLEND_CROUCH_IDLE,
	PLAYER_POSE_PARAM_STRAFE_DIR,
	PLAYER_POSE_PARAM_AIM_BLEND_STAND_WALK,
	PLAYER_POSE_PARAM_AIM_BLEND_STAND_RUN,
	PLAYER_POSE_PARAM_AIM_BLEND_CROUCH_WALK,
	PLAYER_POSE_PARAM_MOVE_BLEND_WALK,
	PLAYER_POSE_PARAM_MOVE_BLEND_RUN,
	PLAYER_POSE_PARAM_MOVE_BLEND_CROUCH_WALK,
	//PLAYER_POSE_PARAM_STRAFE_CROSS,
	PLAYER_POSE_PARAM_COUNT,
};


enum EAlayers
{
	ANIMATION_LAYER_AIMMATRIX = 0,
	ANIMATION_LAYER_WEAPON_ACTION,
	ANIMATION_LAYER_WEAPON_ACTION_RECROUCH,
	ANIMATION_LAYER_ADJUST,
	ANIMATION_LAYER_MOVEMENT_JUMP_OR_FALL,
	ANIMATION_LAYER_MOVEMENT_LAND_OR_CLIMB,
	ANIMATION_LAYER_MOVEMENT_MOVE,
	ANIMATION_LAYER_MOVEMENT_STRAFECHANGE,
	ANIMATION_LAYER_WHOLE_BODY,
	ANIMATION_LAYER_FLASHED,
	ANIMATION_LAYER_FLINCH,
	ANIMATION_LAYER_ALIVELOOP,
	ANIMATION_LAYER_LEAN,
	ANIMATION_LAYER_COUNT,
};

enum ESequenceActivity : int
{
	ACT_CSGO_NULL = 957,
	ACT_CSGO_DEFUSE,
	ACT_CSGO_DEFUSE_WITH_KIT,
	ACT_CSGO_FLASHBANG_REACTION,
	ACT_CSGO_FIRE_PRIMARY,
	ACT_CSGO_FIRE_PRIMARY_OPT_1,
	ACT_CSGO_FIRE_PRIMARY_OPT_2,
	ACT_CSGO_FIRE_SECONDARY,
	ACT_CSGO_FIRE_SECONDARY_OPT_1,
	ACT_CSGO_FIRE_SECONDARY_OPT_2,
	ACT_CSGO_RELOAD,
	ACT_CSGO_RELOAD_START,
	ACT_CSGO_RELOAD_LOOP,
	ACT_CSGO_RELOAD_END,
	ACT_CSGO_OPERATE,
	ACT_CSGO_DEPLOY,
	ACT_CSGO_CATCH,
	ACT_CSGO_SILENCER_DETACH,
	ACT_CSGO_SILENCER_ATTACH,
	ACT_CSGO_TWITCH,
	ACT_CSGO_TWITCH_BUYZONE,
	ACT_CSGO_PLANT_BOMB,
	ACT_CSGO_IDLE_TURN_BALANCEADJUST,
	ACT_CSGO_IDLE_ADJUST_STOPPEDMOVING,
	ACT_CSGO_ALIVE_LOOP,
	ACT_CSGO_FLINCH,
	ACT_CSGO_FLINCH_HEAD,
	ACT_CSGO_FLINCH_MOLOTOV,
	ACT_CSGO_JUMP,
	ACT_CSGO_FALL,
	ACT_CSGO_CLIMB_LADDER,
	ACT_CSGO_LAND_LIGHT,
	ACT_CSGO_LAND_HEAVY,
	ACT_CSGO_EXIT_LADDER_TOP,
	ACT_CSGO_EXIT_LADDER_BOTTOM
};

class matrix3x4a_t;
class CBaseAnimating;
class CBoneAccessor
{
public:
	const CBaseAnimating*	pAnimating;		//0x00
	matrix3x4a_t*			matBones;		//0x04
	int						nReadableBones;	//0x08
	int						nWritableBones;	//0x0C
}; // Size: 0x10

class CAnimationLayer
{
public:
	float			flAnimationTime;		//0x00
	float			flFadeOut;				//0x04
	void* pStudioHdr;				//0x08
	int				nDispatchedSrc;			//0x0C
	int				nDispatchedDst;			//0x10
	int				iOrder;					//0x14
	std::uintptr_t  nSequence;				//0x18
	float			flPrevCycle;			//0x1C
	float			flWeight;				//0x20
	float			flWeightDeltaRate;		//0x24
	float			flPlaybackRate;			//0x28
	float			flCycle;				//0x2C
	CBaseEntity* pOwner;					//0x30
	int				nInvalidatePhysicsBits;	//0x34


}; // Size: 0x38

#pragma pack(push, 1)
struct CAimLayer
{
	float flUnknown0;
	float flTotalTime;
	float flUnknown1;
	float flUnknown2;
	float flWeight;
};
#pragma pack(pop)

#pragma pack(push, 1)
struct CAimLayers
{
	CAimLayer layers[ 3 ];
};
#pragma pack(pop)

struct AnimstatePose
{
	bool		bInitialized;
	int			nIndex;
	const char* szName;

	AnimstatePose( )
	{
		bInitialized = false;
		nIndex = -1;
		szName = "";
	}
};

struct ProceduralFoot
{
	Vector vecPosAnim;
	Vector vecPosAnimLast;
	Vector vecPosPlant;
	Vector vecPlantVel;
	float flLockAmount;
	float flLastPlantTime;
};

class CAnimState {

public:
	void*					pThis;
	bool					bIsReset;
	bool					bUnknownClientBoolean;
	char					aSomePad[ 2 ];
	int						nTick;
	float					flFlashedStartTime;
	float					flFlashedEndTime;
	CAimLayers				AimLayers;
	int						iModelIndex;
	int						iUnknownArray[ 3 ];
	CBaseEntity*			pEntity;
	CBaseCombatWeapon*		pActiveWeapon;
	CBaseCombatWeapon*		pLastActiveWeapon;
	float					flLastUpdateTime;
	int						iLastUpdateFrame;
	float					flLastUpdateIncrement;
	float					flEyeYaw;
	float					flEyePitch;
	float					flGoalFeetYaw;
	float					flGoalFeetYawLast;
	float					flMoveYaw;
	float					flMoveYawIdeal;
	float					flMoveYawCurrentToIdeal;
	float					flTimeToAlignLowerBody;
	float					flFeetCycle;
	float					flMoveWeight;
	float					flMoveWeightSmoothed;
	float					flDuckAmount;
	float					flHitGroundCycle;
	float					flRecrouchWeight;
	Vector					vecPositionCurrent;
	Vector					vecPositionLast;
	Vector					vecVelocity;
	Vector					vecVelocityNormalized;
	Vector					vecVelocityNormalizedNonZero;
	float					flVelocityLenght2D;
	float					flJumpFallVelocity;
	float					flSpeedNormalized;
	float					flRunningSpeed;
	float					flDuckingSpeed;
	float					flDurationMoving;
	float					flDurationStill;
	bool					bOnGround;
	bool					bHitGroundAnimation;
	std::byte				pad2[ 0x2 ];
	float					flJumpToFall;
	float					flDurationInAir;
	float					flLeftGroundHeight;
	float					flLandAnimMultiplier;
	float					flWalkToRunTransition;
	bool					bLandedOnGroundThisFrame;
	bool					bLeftTheGroundThisFrame;
	float					flInAirSmoothValue;
	bool					bOnLadder;
	float					flLadderWeight;
	float					flLadderSpeed;
	bool					bWalkToRunTransitionState;
	bool					bDefuseStarted;
	bool					bPlantAnimStarted;
	bool					bTwitchAnimStarted;
	bool					bAdjustStarted;
	char					ActivityModifiers[ 20 ];
	float					flNextTwitchTime;
	float					flTimeOfLastKnownInjury;
	float					flLastVelocityTestTime;
	Vector					vecVelocityLast;
	Vector					vecTargetAcceleration;
	Vector					vecAcceleration;
	float					flAccelerationWeight;
	float					flAimMatrixTransition;
	float					flAimMatrixTransitionDelay;
	bool					bFlashed;
	float					flStrafeChangeWeight;
	float					flStrafeChangeTargetWeight;
	float					flStrafeChangeCycle;
	int						nStrafeSequence;
	bool					bStrafeChanging;
	float					flDurationStrafing;
	float					flFootLerp;
	bool					bFeetCrossed;
	bool					bPlayerIsAccelerating;
	AnimstatePose			tPoseParamMappings[ 20 ];
	float					flDurationMoveWeightIsTooHigh;
	float					flStaticApproachSpeed;
	int						nPreviousMoveState;
	float					flStutterStep;
	float					flActionWeightBiasRemainder;
	ProceduralFoot			tFootLeft;
	ProceduralFoot			tFootRight;
	float					flCameraSmoothHeight;
	bool					bSmoothHeightValid;
	float					flLastTimeVelocityOverTen;
	float					flUnknownFL0;
	float					flMinBodyYaw;
	float					flMaxBodyYaw;
	float					flMinPitch;
	float					flMaxPitch;
	int						iAnimsetVersion;

	// m_client_dll, ( "53 56 57 8b f9 33 f6 8b 4f ? 8b 01 ff 90 ? ? ? ? 89 47" )
	const char* GetWeaponPrefix() {
		static uintptr_t sig = uintptr_t(MEM::FindPattern(CLIENT_DLL, XorStr("53 56 57 8b f9 33 f6 8b 4f ? 8b 01 ff 90 ? ? ? ? 89 47")));
		using GetWeaponPrefixFn = const char* (__thiscall*)(void*);
		return ((GetWeaponPrefixFn)sig)(this);
	}

	float GetMaxDesync() {

		auto speedfactor = std::clamp(this->flRunningSpeed, 0.0f, 1.0f);
		auto avg_speedfactor = (this->flWalkToRunTransition * -0.30000001f - 0.19999999f) * speedfactor + 1.f;

		if (this->flDuckAmount > 0.0f)

			avg_speedfactor += ((this->flDuckAmount * speedfactor) * (0.5f - avg_speedfactor));

		return this->flMaxBodyYaw * avg_speedfactor;
	}

	float flYawModifier()
	{
		auto animstate = this;

		float v38 = animstate->flRunningSpeed;
		float v39, v40, v41, v42, v43, v44;

		if (v38 >= 0.0)
			v39 = fminf(v38, 1.0);
		else
			v39 = 0.0;

		v40 = ((animstate->flWalkToRunTransition * -0.30000001) - 0.19999999) * v39;
		v41 = animstate->flDuckAmount;
		v42 = v40 + 1.0;

		if (v41 > 0.0) {

			v43 = animstate->flDuckingSpeed;
			if (v43 >= 0.0)
				v44 = fminf(v43, 1.0);
			else
				v44 = 0.0;
			v42 = v42 + ((v41 * v44) * (0.5 - v42));
		}

		return v42;
	}

	void Create(CBaseEntity* pEntity) {

		using CreateAnimationStateFn = void(__thiscall*)(void*, CBaseEntity*);
		static auto oCreateAnimationState = reinterpret_cast<CreateAnimationStateFn>(MEM::FindPattern(CLIENT_DLL, XorStr("55 8B EC 56 8B F1 B9 ? ? ? ? C7 46"))); // @xref: "ggprogressive_player_levelup"

		if (oCreateAnimationState == nullptr)
			return;

		oCreateAnimationState(this, pEntity);
	}

	void Update(Vector angView) {

		using UpdateAnimationStateFn = void(__vectorcall*)(void*, void*, float, float, float, void*);
		static auto oUpdateAnimationState = reinterpret_cast<UpdateAnimationStateFn>(MEM::FindPattern(CLIENT_DLL, XorStr("55 8B EC 83 E4 F8 83 EC 18 56 57 8B F9 F3 0F 11 54 24"))); // @xref: "%s_aim"

		if (oUpdateAnimationState == nullptr)
			return;

		oUpdateAnimationState(this, nullptr, angView.z, angView.y, angView.x, nullptr);
	}

	void Reset() {

		using ResetAnimationStateFn = void(__thiscall*)(void*);
		static auto oResetAnimationState = reinterpret_cast<ResetAnimationStateFn>(MEM::FindPattern(CLIENT_DLL, XorStr("56 6A 01 68 ? ? ? ? 8B F1"))); // @xref: "player_spawn"

		if (oResetAnimationState == nullptr)
			return;

		oResetAnimationState(this);
	}

	void IncrementLayerCycle(CAnimationLayer* Layer, bool bIsLoop)
	{
		float_t flNewCycle = (Layer->flPlaybackRate * this->flLastUpdateIncrement) + Layer->flCycle;
		if (!bIsLoop && flNewCycle >= 1.0f)
			flNewCycle = 0.999f;

		flNewCycle -= (int32_t)(flNewCycle);
		if (flNewCycle < 0.0f)
			flNewCycle += 1.0f;

		if (flNewCycle > 1.0f)
			flNewCycle -= 1.0f;

		Layer->flCycle = flNewCycle;
	}
	bool IsLayerSequenceFinished(CAnimationLayer* Layer, float_t flTime)
	{
		return (Layer->flPlaybackRate * flTime) + Layer->flCycle >= 1.0f;
	}
	void SetLayerCycle(CAnimationLayer* pAnimationLayer, float_t flCycle)
	{
		if (pAnimationLayer)
			pAnimationLayer->flCycle = flCycle;
	}
	void SetLayerRate(CAnimationLayer* pAnimationLayer, float_t flRate)
	{
		if (pAnimationLayer)
			pAnimationLayer->flPlaybackRate = flRate;
	}
	void SetLayerWeight(CAnimationLayer* pAnimationLayer, float_t flWeight)
	{
		if (pAnimationLayer)
			pAnimationLayer->flWeight = flWeight;
	}
	void SetLayerWeightRate(CAnimationLayer* pAnimationLayer, float_t flPrevious)
	{
		if (pAnimationLayer)
			pAnimationLayer->flWeightDeltaRate = (pAnimationLayer->flWeight - flPrevious) / flLastUpdateIncrement;
	}
	int SelectSequenceFromActivityModifier(int iActivity)
	{
		bool bIsPlayerDucked = flDuckAmount > 0.55f;
		bool bIsPlayerRunning = flRunningSpeed > 0.25f;

		int iLayerSequence = -1;
		switch (iActivity)
		{
		case ACT_CSGO_JUMP:
		{
			iLayerSequence = 15 + static_cast <int>(bIsPlayerRunning);
			if (bIsPlayerDucked)
				iLayerSequence = 17 + static_cast <int>(bIsPlayerRunning);
		}
		break;

		case ACT_CSGO_ALIVE_LOOP:
		{
			iLayerSequence = 9;
			if (pLastActiveWeapon != pActiveWeapon)
				iLayerSequence = 8;
		}
		break;

		case ACT_CSGO_IDLE_ADJUST_STOPPEDMOVING:
		{
			iLayerSequence = 6;
		}
		break;

		case ACT_CSGO_FALL:
		{
			iLayerSequence = 14;
		}
		break;

		case ACT_CSGO_IDLE_TURN_BALANCEADJUST:
		{
			iLayerSequence = 4;
		}
		break;

		case ACT_CSGO_LAND_LIGHT:
		{
			iLayerSequence = 20;
			if (bIsPlayerRunning)
				iLayerSequence = 22;

			if (bIsPlayerDucked)
			{
				iLayerSequence = 21;
				if (bIsPlayerRunning)
					iLayerSequence = 19;
			}
		}
		break;

		case ACT_CSGO_LAND_HEAVY:
		{
			iLayerSequence = 23;
			if (bIsPlayerDucked)
				iLayerSequence = 24;
		}
		break;

		case ACT_CSGO_CLIMB_LADDER:
		{
			iLayerSequence = 13;
		}
		break;
		default: break;
		}

		return iLayerSequence;
	}
	float GetLayerSequenceCycleRate(CBaseEntity* thisptr, CAnimationLayer* pLayer, int iSequence) {

		using GetLayerSequenceCycleRateFn = float(__thiscall*)(void*, CAnimationLayer*, int);
		static auto original = reinterpret_cast<GetLayerSequenceCycleRateFn>(MEM::FindPattern(CLIENT_DLL, XorStr("55 8B EC 56 57 FF 75 0C 8B 7D 08 8B F1 57 E8")));
		return original(thisptr, pLayer, iSequence);

		//using GetLayerSequenceCycleRateFn = float(__thiscall*)(void*, CAnimationLayer*, int);
		//return util::CallVFunc< GetLayerSequenceCycleRateFn >(this, 223)(this, pLayer, iSequence);
	}
	void SetLayerSequence(CAnimationLayer* pAnimationLayer, int iActivity)
	{
		int iSequence = this->SelectSequenceFromActivityModifier(iActivity);
		if (iSequence < 2)
			return;

		pAnimationLayer->flCycle = 0.0f;
		pAnimationLayer->flWeight = 0.0f;
		pAnimationLayer->nSequence = iSequence;
		pAnimationLayer->flPlaybackRate = GetLayerSequenceCycleRate(pEntity, pAnimationLayer, iSequence);
	}
};