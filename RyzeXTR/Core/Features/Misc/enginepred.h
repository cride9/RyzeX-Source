#pragma once
#include "../Rage/Animations/LocalAnimation.h"
#include "../../Interface/interfaces.h"
#include "../../SDK/Entity.h"

struct TickbaseRecord_t
{
	TickbaseRecord_t()
	{
		iTickbase = 0;
		bIsValid = false;
	}
	TickbaseRecord_t(int nTick)
	{
		iTickbase = nTick;
		bIsValid = true;
	}

	int iTickbase = 0;
	bool bIsValid = false;
};

struct CNetvarData
{
	float flRecoilIndex = 0.f;
	float flAccuracyPenalty = 0.f;
	float flDuckAmount = 0.f;
	float flDuckSpeed = 0.f;
	float flFallVelocity = 0.f;

	int nRenderMode = 0;
	int nTickbase = 0;
	int iFlags = 0;

	Vector vecOrigin = Vector( 0.f, 0.f, 0.f );
	Vector vecVelocity = Vector( 0.f, 0.f, 0.f );
	Vector vecBaseVelocity = Vector( 0.f, 0.f, 0.f );
	Vector vecViewOffset = Vector( 0.f, 0.f, 0.f );

	Vector vecAimPunchAngleVel = Vector( 0.f, 0.f, 0.f );
	Vector vecAimPunchAngle = Vector( 0.f, 0.f, 0.f );
	Vector vecViewPunchAngle = Vector( 0.f, 0.f, 0.f );
};

enum EThinkMethods : int
{
	THINK_FIRE_ALL_FUNCTIONS = 0,
	THINK_FIRE_BASE_ONLY,
	THINK_FIRE_ALL_BUT_BASE,
};

class Prediction {

public:
	Prediction() {
		uPredictionRandomSeed = *reinterpret_cast<int**>(MEM::FindPattern(CLIENT_DLL, XorStr("8B 0D ? ? ? ? BA ? ? ? ? E8 ? ? ? ? 83 C4 04")) + 2);
		pPredictionPlayer = *reinterpret_cast<CBaseEntity***>(MEM::FindPattern(CLIENT_DLL, XorStr("89 35 ? ? ? ? F3 0F 10 48 20")) + 0x2);
	}

	void Start(CUserCmd* pCmd, CBaseEntity* pLocal, int SequenceNumber );
	void End(CUserCmd* pCmd, CBaseEntity* pLocal) const;

	CNetvarData GetNetvars( int iCommand ) { return this->pNetvarData[ iCommand % 150 ]; };

	void SaveNetvars( int iCommand , CBaseEntity* pLocal);
	void RestoreNetvars( int iCommand, CBaseEntity* pLocal);
	void SaveViewmodelData(CBaseEntity* pLocal);
	void AdjustViewmodelData(CBaseEntity* pLocal);
	int AdjustPlayerTimeBase(int iSimulationTick);

	float flSpread = 0.f;
	float flInaccuracy = 0.f;

	std::array < CNetvarData, 150 > pNetvarData = { };
	CUserCmd* pLastCmd = nullptr;

	struct
	{
		int iSimulationTick = 0;

		std::array < TickbaseRecord_t, 150 > arrTickbase{};
		std::array < int, 150 > arrGameTickbase{};
	} Tickbase_t;

	inline void ResetData()
	{
		Tickbase_t.iSimulationTick = 0;
		Tickbase_t.arrTickbase = { };
		Tickbase_t.arrGameTickbase = { };
	}
	inline int GetEngineTickbase(int nCommand) { return Tickbase_t.arrGameTickbase[nCommand % 150]; };
	inline void SetTickbase(int nCommand, int nTickBase)
	{
		Tickbase_t.arrTickbase[nCommand % 150] = TickbaseRecord_t(nTickBase);
	};
	inline TickbaseRecord_t* GetTickbaseRecord(int nCommand)
	{
		return &Tickbase_t.arrTickbase[nCommand % 150];
	}
	void OnFrameStage(CBaseEntity* pLocal);

private:

	int* uPredictionRandomSeed = nullptr;

	CBaseEntity** pPredictionPlayer = nullptr;

	CMoveData moveData = { };

	float flOldCurrentTime = 0.f;
	float flOldFrameTime = 0.f;
	int iOldTickCount = 0;

	float flCycle = 0.f;
	float flAnimTime = 0.f;
	int	iSequence = 0;
	int	iAnimationParity = 0;
};
inline Prediction prediction;