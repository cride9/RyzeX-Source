#pragma once
#include "../../SDK/CUserCmd.h"
#include "../../globals.h"
#include "../../SDK/Enums.h"
#include "../Rage/Animations/LocalAnimation.h"
#include <deque>

enum ECONFIG : int {

	SEMIRAGE,
	HVH,
	BAIM,
	HEADSHOT,
};

namespace misc {

	inline bool bNewRound = false;
	inline bool bBreakLagcompensation = false;
	inline bool bFilter = false;
	inline bool bRetreat = false, bTeleportBack = false;
	inline bool exploitInitialized = false;
	inline Vector vecRecord = Vector(0.f, 0.f, 0.f);
	inline Vector vecEyePosition = Vector(0.f, 0.f, 0.f);
	inline int iRestChoke = 0;
	inline CConVar* con_filter_text = nullptr;
	inline CConVar* con_filter_enable = nullptr;

	void SetupRadio( );

	void CreateMove(CUserCmd*, Vector&, bool&);
	bool CanFireWeapon(float curtime, bool bRevolverSecondary = false, bool bSkipExtraRevolverChecks = false);

	void BunnyHop(CUserCmd* pCmd);
	void FakeLag(bool&);
	void ThirdPerson();
	void MovementFix(CUserCmd*, Vector&);
	void AutoStrafe(Vector&, CUserCmd*);
	void AspectRatio();
	void Slowwalk(CUserCmd*, float);
	void FastStop(CUserCmd*);
	void FakeDuck(CUserCmd*);
	void BulletImpact(IGameEvent*);
	void BulletImpactFrameStage(CBaseEntity* pLocal);
	void SlideFix();
	void SkyboxChanger();
	void OnlyCheatLogs();
	void IdealTick(CUserCmd*, CBaseEntity* pLocal);
	void ServerHitboxes();
	void RemovePostProcessing();
	void FixScopeSens();
	void AutoPistol(CUserCmd* pCmd, CBaseEntity* pLocal);
	void RemoveSmoke();
	void WalkBot(CUserCmd*);
	void WalkBotHandler(IGameEvent*);
	void MoveToPosition();
	void ClanTag();
	void CapsuleHandler(IGameEvent*);
	void CapsuleOnHit(int, int, Color, float);
	void LeftHandKnife();
	bool ChangeName(bool, const char*);
	bool ResetName(bool, const char*);
	void FogOptions();
	void RemoveShadows();
	void MotionBlur(CViewSetup* pSetup);

	void Killsay(IGameEvent*);

    void ThirdPersonDisableOnDeath(IGameEvent* pEvent);

	void DrawBream(Vector, Vector, Color);
	void WorldCrosshairHandler(IGameEvent*);

	void Security();

	void EventHandler(IGameEvent*);
	void HandlePlayerHitEffects(IGameEvent*);
	void PreserveKillfeed(IGameEvent*);
	void BuyBot(IGameEvent*);
	void BulletTracer(IGameEvent*);
	void BlockBot(CUserCmd*);
	void BoostMovement(CUserCmd*);

	void CustomBombText(const char*);
	std::string GetHitgroupName(int iHitgroup);
	std::string GetMatrixName(int iType);

	void RevolverRunCommand(CBaseEntity*);
    bool CanFireWeapon(float curtime, bool bRevolverSecondary, bool bSkipExtraRevolverChecks);
	void RevolverCreateMove();
	void Print(const std::string text, ...);
}