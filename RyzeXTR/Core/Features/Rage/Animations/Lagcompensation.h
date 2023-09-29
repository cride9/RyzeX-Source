#pragma once
#include <deque>
#include "../../../SDK/Entity.h"
#include "../../../globals.h"
#include "../../../SDK/math.h"
#include "../../Misc/Playerlist.h"

#pragma region lagcompensation_definitions
#define LAG_COMPENSATION_TELEPORTED_DISTANCE_SQR ( 64.0f * 64.0f )
#define LAG_COMPENSATION_EPS_SQR ( 0.1f * 0.1f )
#define LAG_COMPENSATION_ERROR_EPS_SQR ( 4.0f * 4.0f )
#pragma endregion

enum EFixedVelocity
{
	Unresolved = 0,
	MovementLayer,
	AliveLoopLayer,
	LeanLayer
};

enum EPlayerActivity
{
	NoActivity = 0,
	Jump = 1,
	Land = 2
};

struct SequenceObject_t
{
	SequenceObject_t(int iInReliableState, int iOutReliableState, int iSequenceNr, float flCurrentTime)
		: iInReliableState(iInReliableState), iOutReliableState(iOutReliableState), iSequenceNr(iSequenceNr), flCurrentTime(flCurrentTime) { }

	int iInReliableState;
	int iOutReliableState;
	int iSequenceNr;
	float flCurrentTime;
};

class CSimulationData
{
public:
	CSimulationData() : pEntity(nullptr), bOnGround(false)
	{
	}

	~CSimulationData()
	{
	}

	CBaseEntity* pEntity;

	Vector vecOrigin;
	Vector vecVelocity;

	bool bOnGround;

	bool bDataFilled = false;
};

enum EMatrixType : int {

	VISUAL,
	RESOLVE,
	LEFT,
	RIGHT,
	CENTER,
	MAX
};

class Lagcompensation {

public:

	struct LagRecord_t {

		struct LayerData_t
		{
			int nSequence{};
			float flPlaybackRate{};
			float flCycle{};
			float flWeight{};
			float flWeightWithAirSmooth{};
			float flFeetWeight{};

			float flMovementSide{};
			Vector angMoveYaw{};
			Vector vecDirection{};

			LayerData_t(const CAnimationLayer& pLayer) {

				nSequence = pLayer.nSequence;
				flPlaybackRate = pLayer.flPlaybackRate;
				flCycle = pLayer.flCycle;
				flWeight = pLayer.flWeight;
			}

			LayerData_t() {};
		};

		LagRecord_t() = default;
		LagRecord_t(CBaseEntity* pEntity);

		void Apply(CBaseEntity* pEntity, bool Backup);
		void Restore(CBaseEntity* pEntity);
		void Apply(CBaseEntity* pEntity);
		void ApplyMatrix(CBaseEntity* pEntity, EMatrixType iType);
		void ApplyMatrix(CBaseEntity* pEntity, matrix3x4a_t* pMatrix);
		bool IsValid( );

		CBaseEntity* pEntity{};

		// 0 - Visual, 1 - Resolve, 2 - Left, 3 - Right, 4 - Center
		matrix3x4a_t pMatricies[MAX][256];
		// 0 - Left, 1 - Right
		matrix3x4a_t pSideMatrixes[2][256];
		LayerData_t LayerData[MAX];

		bool bBreakingLagcompensation{};
		bool bFakewalking{};
		bool bValid = true;
		bool bDormant{};
		bool bDidShot{};
		bool bRestoreData{};
		bool bFirstAfterDormant{};

		Vector vecVelocity{};
		Vector vecAbsVelocity{};
		Vector vecOrigin{};
		Vector vecAbsOrigin{};
		Vector vecMins{};
		Vector vecMaxs{};

		Vector vecRageSafePoint{};

		Vector vecEyeAngles{};
		Vector vecAbsAngles{};

		CAnimationLayer arrLayers[13];
		float flPoses[24];

		float flWalkToRunTransition{};
		float flSimulationTime{};
		float flOldSimulationTime{};
		float flInterpTime{};
		float flDuck{};
		float flLowerBodyYawTarget{};
		float flLastShotTime{};
		float flMaxSpeed{};
		float flAnimationVelocity{};
		float flDurationInAir{};
		float flActivityPlayback{};
		float flThirdPersonRecoil{};
		float flDesyncDelta{};
		float flResolveDelta{};
		float flLayerDifferences[MAX]{};
		float flGuessedYaw = 0.f;
		float flCollisionChangeTime = 0.f;
		float flCollisionChangeOrigin = 0.f;

		int nVelocityMode{};
		int iActivityTick{};
		int iActivityType{};

		int iFlags{};
		int iEFlags{};
		int iEffects{};
		int iEntIndex{};
		int iChoked{};
		int iResolveSide = 0;
	};

	enum EResolverMode
	{
		LEGIT = 0,
		DESYNC,
		OPPOSITE,
		SWAY,
		FAKE,
		OVERRIDE,
		ONSHOT,
		JITTER,
		ANIMATION
	};

	const char* EResolverModeToString(EResolverMode mode)
	{
		switch (mode)
		{
		case LEGIT:
			return "LEGIT";
		case DESYNC:
			return "DESYNC";
		case OPPOSITE:
			return "OPPOSITE";
		case SWAY:
			return "SWAY";
		case FAKE:
			return "FAKE";
		case OVERRIDE:
			return "OVERRIDE";
		case ONSHOT:
			return "ONSHOT";
		case JITTER:
			return "JITTER";
		case ANIMATION:
			return "ANIMATION";
		default:
			return "UNKNOWN";
		}
	}

	struct AnimationInfo_t
	{
		CBaseEntity* pEntity;

		bool bLeftDormancy{};

		int iLastValid = 0;
		int iLastResolve{};

		int iShotAmount{};
		int iHitAmount{};

		float flExploitTime;

		std::deque<Lagcompensation::LagRecord_t> pRecord;
		std::array<matrix3x4a_t, 256> pCachedMatrix{ matrix3x4a_t()};

		void ClearData() {

			pEntity = nullptr;
			bLeftDormancy = true;

			iShotAmount = 0;
			iHitAmount = 0;

			flExploitTime = 0.f;
			iLastValid = 0;
			pRecord.clear();
			pCachedMatrix = std::array<matrix3x4a_t, 256>();
		}

		void InvalidateRecords() {

			pRecord.clear();
			iLastValid = 0;
		}

		bool operator<(AnimationInfo_t& other) {

			if (playerList::arrPlayers[pEntity->EntIndex()].bPriority)
				return true;

			return pEntity->GetHealth() < other.pEntity->GetHealth();
		}
	};

	/* Everything will be ran inside this */
	void FrameStageNotify() noexcept;

	/* Those functions will run in createmove */
	void StartLagcompensation(CBaseEntity*);
	void FinishLagcompensation(CBaseEntity*);
	bool DataChanged(CBaseEntity*, Lagcompensation::LagRecord_t*);

	// get animation info
	AnimationInfo_t& GetLog(const int iEntIndex);
	// check if player is breaking lagcomp
	bool IsBreakingLagcompensation(Lagcompensation::LagRecord_t* pLagRecord);
	// fix tickcount so we can backtrack
	int FixTickCount(const float flSimulationTime);

	void UpdateIncomingSequences(INetChannel* pNetChannel);
	void ClearIncomingSequences();
	void AddLatencyToNetChannel(INetChannel* pNetChannel, float flLatency);

	// get client interp amount
	static float GetClientInterpAmount();

	// fuck interpolation
	void SetInterpolationFlags();
	void DisableInterpolation();

	// check if record is valid
	static bool IsValidRecord(float m_flSimulationTime, float m_flRange = 0.2f);

	// extrapolate players breaking lagcomp
	void ExtrapolatePlayer(CBaseEntity* m_pEntity, Lagcompensation::LagRecord_t* m_pCurrentRecord, Lagcompensation::LagRecord_t* m_pPrevious) const;

	std::array<std::pair<Lagcompensation::LagRecord_t, bool>, 65> arrBackupData;

private:
	// filter records after updating them
	void FilterRecords();


	// Values
	/* animation info */
	AnimationInfo_t pPlayerLogs[65];
	/* stored sequences */
	std::deque<SequenceObject_t> vecSequences = { };
	/* our real incoming sequences count */
	int nRealIncomingSequence = 0;
	/* count of incoming sequences what we can spike */
	int nLastIncomingSequence = 0;

	int nInvalidateFlags{};

};
inline Lagcompensation lagcomp;