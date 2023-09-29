#pragma once
#include "../../../SDK/Entity.h"
#include "../../../globals.h"

struct AnimationRecord_t
{
	int iFlags = 0;
	int m_nButtons = 0;
	int iMoveType = 0;

	bool m_bIsShooting = false;

	Vector m_angRealAngles = Vector(0, 0, 0);
	Vector m_angFakeAngles = Vector(0, 0, 0);
	Vector m_angAimPunch = Vector(0, 0, 0);

	float m_flDuckAmount = 0.0f;
	float m_flDuckSpeed = 0.0f;

	Vector m_vecOrigin = Vector(0, 0, 0);
	Vector m_vecVelocity = Vector(0, 0, 0);
};

struct animationData_t {

	int iMoveType = 0;
	int iFlags = 0;

	std::array<CAnimationLayer, ANIMATION_LAYER_COUNT> arrLayers = { };
	std::array<float, MAXSTUDIOPOSEPARAM> arrPoses = { };

	Vector vecMatrixOrigin = Vector(0, 0, 0);
	std::array < matrix3x4a_t, MAXSTUDIOBONES > arrMatrix = { };

	CAnimState pAnimationState;
};

class C_LocalAnimations
{
public:
	virtual void OnCreateMove(bool& bSendPacket, CBaseEntity* pLocal);
	virtual void BeforePrediction(CBaseEntity* pLocal);
	virtual void StoreAnimationRecord(CBaseEntity* pLocal);
	virtual void ModifyEyePosition(Vector& vecInputEyePos, matrix3x4a_t* aMatrix);
	virtual void SetupPlayerBones(matrix3x4a_t* aMatrix, int nMask, CBaseEntity* pLocal);
	virtual void InterpolateMatricies();
	virtual void DoAnimationEvent(int nButtons, CBaseEntity* pLocal, bool bIsFakeAnimations = false);
	virtual void SimulateStrafe(int nButtons, CBaseEntity* pLocal);
	virtual void UpdateDesyncAnimations(CBaseEntity* pLocal);
	virtual void TransformateMatricies();
	virtual void CleanSnapshots();
	virtual bool CopyCachedMatrix(matrix3x4a_t* aInMatrix, int nBoneCount);
	virtual void SetupShootPosition(CBaseEntity* pLocal);
	virtual void CopyPlayerAnimationData(bool bFake, CBaseEntity* pLocal);
	virtual void AnimationBreaker(CBaseEntity*);
	virtual void SetupInterpolation(CBaseEntity* pLocal, bool bPostFrame);

	virtual float GetYawDelta()
	{
		return LocalData_t.flYawDelta;
	}
	virtual Vector GetShootPosition()
	{
		return LocalData_t.vecShootPosition;
	}
	virtual void ResetData();
	virtual std::array < matrix3x4a_t, MAXSTUDIOBONES > GetDesyncMatrix()
	{
		return LocalData_t.FakeData.arrMatrix;
	}
	virtual std::array < matrix3x4a_t, MAXSTUDIOBONES > GetRealMatrix()
	{
		return LocalData_t.RealData.arrMatrix;
	}
	Vector vecViewAngle = Vector(0, 0, 0);
private:


	struct
	{
		Vector vecAbsOrigin = Vector(0, 0, 0);
		int iFlags = 0;
		int iSimulationTick = 0;

		float flSpawnTime = 0.0f;
		float flYawDelta = 0.0f;
		float flInterpolationAmount = 0.f;

		std::array < AnimationRecord_t, 150 > arrAnimRecords;

		Vector vecShootPosition = Vector(0, 0, 0);
		animationData_t FakeData;
		animationData_t RealData;

		struct
		{
			std::array < matrix3x4a_t, MAXSTUDIOBONES > arrMatrix = { };
			std::array < CAnimationLayer, ANIMATION_LAYER_COUNT > arrLayers = { };
			std::array < float, MAXSTUDIOPOSEPARAM > arrPoses = { };
			CAnimState pAnimationState;
		} ShootData_t;

	} LocalData_t;
};
inline C_LocalAnimations* localAnim = new C_LocalAnimations();