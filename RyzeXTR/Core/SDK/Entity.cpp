#include "Entity.h"
#include "math.h"
#include "../globals.h"
#include "../Hooks/hooks.h"
#include "../Features/Networking/networking.h"
#include "../Features/Rage/autowall.h"

CBaseEntity* CBaseEntity::GetLocalPlayer()
{
	return (CBaseEntity*)i::EntityList->GetClientEntity(i::EngineClient->GetLocalPlayer());
}

int CBaseEntity::GetSequenceActivity(int iSequence)
{
	studiohdr_t* pStudioHdr = i::ModelInfo->GetStudioModel(this->GetModel());

	if (pStudioHdr == nullptr)
		return -1;

	using GetSequenceActivityFn = int(__fastcall*)(void*, void*, int);
	static auto oGetSequenceActivity = reinterpret_cast<GetSequenceActivityFn>(MEM::FindPattern(CLIENT_DLL, XorStr("55 8B EC 53 8B 5D 08 56 8B F1 83"))); // @xref: "Need to handle the activity %d\n"
	assert(oGetSequenceActivity != nullptr);

	return oGetSequenceActivity(this, pStudioHdr, iSequence);
}

void CBaseEntity::SetUpMovement() {

	if (!this || !this->AnimState())
		return;

	using SetUpMovementFn = void(__thiscall*)(CAnimState*);
	static auto oSetUpMovement = reinterpret_cast<SetUpMovementFn>(MEM::FindPattern(CLIENT_DLL, XorStr("55 8B EC 83 E4 F8 81 ? ? ? ? ? 56 57 8B ? ? ? ? ? 8B F1")));

	assert(oSetUpMovement != nullptr);

	oSetUpMovement(this->AnimState());
}

void CBaseEntity::SetUpVelocity() {

	if (!this->AnimState())
		return;

	using SetUpVelocityFn = void(__thiscall*)(CAnimState*);
	static auto oSetUpVelocity = reinterpret_cast<SetUpVelocityFn>(MEM::FindPattern(CLIENT_DLL, XorStr("55 8B EC 83 E4 F8 83 EC 30 56 57 8B ? ? ? ? ? 8B")));

	assert(oSetUpVelocity != nullptr);

	oSetUpVelocity(this->AnimState());
}

CBaseCombatWeapon* CBaseEntity::GetWeapon()
{
	if (auto pHandle = this->GetActiveWeaponHandle(); pHandle)
		return static_cast<CBaseCombatWeapon*>(i::EntityList->GetClientEntityFromHandle(pHandle));
	else
		return nullptr;
}

bool CBaseEntity::IsGrenade(CBaseCombatWeapon* bWeapon)
{
	if (bWeapon == nullptr) return false;
	int id = bWeapon->GetItemDefinitionIndex();
	static const std::vector<int> v = { WEAPON_FLASHBANG, WEAPON_HEGRENADE, WEAPON_SMOKEGRENADE, WEAPON_MOLOTOV, WEAPON_DECOY, WEAPON_INCGRENADE };
	return (std::find(v.begin(), v.end(), id) != v.end());
}

int CBaseEntity::GetMaxHealth()
{
	// check is dangerzone
	//if (i::GameTypes->GetCurrentGameType() == GAMETYPE_FREEFORALL)
	//	return 120;

	return 100;
}

std::optional<Vector> CBaseEntity::GetBonePosition(int iBone)
{
	assert(iBone > BONE_INVALID && iBone < MAXSTUDIOBONES); // given invalid bone index for getboneposition

	std::array<matrix3x4a_t, MAXSTUDIOBONES> arrBonesToWorld = { };

	if (this->SetupBones(arrBonesToWorld.data(), arrBonesToWorld.size(), BONE_USED_BY_ANYTHING, 0.f))
		return arrBonesToWorld.at(iBone).at(3);

	return std::nullopt;
}

int CBaseEntity::GetBoneByHash(const uint32_t uBoneHash) const
{
	if (const auto pModel = this->GetModel(); pModel != nullptr)
	{
		if (const auto pStudioHdr = i::ModelInfo->GetStudioModel(pModel); pStudioHdr != nullptr)
		{
			for (int i = 0; i < pStudioHdr->nBones; i++)
			{
				if (const auto pBone = pStudioHdr->GetBone(i); pBone != nullptr && fnv::Hash(pBone->GetName()) == uBoneHash)
					return i;
			}
		}
	}

	return BONE_INVALID;
}

Vector CBaseEntity::GetHitboxPosition(int hitbox, matrix3x4_t* matrix, float& flRadius, mstudiobbox_t* pStudioBox)
{
	if (hitbox >= HITBOX_MAX)
		return Vector(0, 0, 0);

	const Model_t* model = this->GetModel();
	if (!model)
		return Vector(0, 0, 0);

	studiohdr_t* studioHdr = i::ModelInfo->GetStudioModel(model);
	if (!studioHdr)
		return Vector(0, 0, 0);

	mstudiobbox_t* studioBox = studioHdr->GetHitboxSet(0)->GetHitbox(hitbox);
	if (!studioBox)
		return Vector(0, 0, 0);

	pStudioBox = studioBox;
	Vector min, max;

	min = M::VectorTransform(studioBox->vecBBMin, matrix[studioBox->iBone]);
	max = M::VectorTransform(studioBox->vecBBMax, matrix[studioBox->iBone]);

	flRadius = studioBox->flRadius;

	return (min + max) * 0.5f;

	return Vector(0, 0, 0);
}

Vector CBaseEntity::GetHitboxPosition(int hitbox, matrix3x4_t* matrix, Vector& vecMins, Vector& vecMaxs, float& flRadius)
{
	if (hitbox >= HITBOX_MAX)
		return Vector(0, 0, 0);

	const Model_t* model = this->GetModel();
	if (!model)
		return Vector(0, 0, 0);

	studiohdr_t* studioHdr = i::ModelInfo->GetStudioModel(model);
	if (!studioHdr)
		return Vector(0, 0, 0);

	mstudiobbox_t* studioBox = studioHdr->GetHitboxSet(0)->GetHitbox(hitbox);
	if (!studioBox)
		return Vector(0, 0, 0);

	vecMins = M::VectorTransform(studioBox->vecBBMin, matrix[studioBox->iBone]);
	vecMaxs = M::VectorTransform(studioBox->vecBBMax, matrix[studioBox->iBone]);

	flRadius = studioBox->flRadius;

	return (vecMins + vecMaxs) * 0.5f;

	return Vector(0, 0, 0);
}

Vector CBaseEntity::GetHitboxPosition(int hitbox, matrix3x4_t* matrix)
{
	if (hitbox >= HITBOX_MAX)
		return Vector(0, 0, 0);

	const Model_t* model = this->GetModel();
	if (!model)
		return Vector(0, 0, 0);

	studiohdr_t* studioHdr = i::ModelInfo->GetStudioModel(model);
	if (!studioHdr)
		return Vector(0, 0, 0);

	mstudiobbox_t* studioBox = studioHdr->GetHitboxSet(0)->GetHitbox(hitbox);
	if (!studioBox)
		return Vector(0, 0, 0);

	Vector min, max;

	min = M::VectorTransform(studioBox->vecBBMin, matrix[studioBox->iBone]);
	max = M::VectorTransform(studioBox->vecBBMax, matrix[studioBox->iBone]);

	return (min + max) * 0.5f;

	return Vector(0, 0, 0);
}

std::optional<Vector> CBaseEntity::GetHitboxPosition(const int iHitbox)
{
	assert(iHitbox > HITBOX_INVALID && iHitbox < HITBOX_MAX); // given invalid hitbox index for gethitboxposition

	std::array<matrix3x4a_t, MAXSTUDIOBONES> arrBonesToWorld = { };

	if (const auto pModel = this->GetModel(); pModel != nullptr)
	{
		if (const auto pStudioHdr = i::ModelInfo->GetStudioModel(pModel); pStudioHdr != nullptr)
		{
			if (const auto pHitbox = pStudioHdr->GetHitbox(iHitbox, 0); pHitbox != nullptr)
			{
				if (this->SetupBones(arrBonesToWorld.data(), arrBonesToWorld.size(), BONE_USED_BY_HITBOX, 0.f))
				{
					// get mins/maxs by bone
					const Vector vecMin = M::VectorTransform(pHitbox->vecBBMin, arrBonesToWorld.at(pHitbox->iBone));
					const Vector vecMax = M::VectorTransform(pHitbox->vecBBMax, arrBonesToWorld.at(pHitbox->iBone));

					// get center
					return (vecMin + vecMax) * 0.5f;
				}
			}
		}
	}

	return std::nullopt;
}

std::optional<Vector> CBaseEntity::GetHitboxPosition(const int iHitbox, Vector& vecMins, Vector& vecMaxs, float& flRadius)
{
	assert(iHitbox > HITBOX_INVALID && iHitbox < HITBOX_MAX); // given invalid hitbox index for gethitboxposition

	std::array<matrix3x4a_t, MAXSTUDIOBONES> arrBonesToWorld = { };

	if (const auto pModel = this->GetModel(); pModel != nullptr)
	{
		if (const auto pStudioHdr = i::ModelInfo->GetStudioModel(pModel); pStudioHdr != nullptr)
		{
			if (const auto pHitbox = pStudioHdr->GetHitbox(iHitbox, 0); pHitbox != nullptr)
			{
				if (this->SetupBones(arrBonesToWorld.data(), arrBonesToWorld.size(), BONE_USED_BY_HITBOX, 0.f))
				{
					// get mins/maxs by bone
					vecMins = M::VectorTransform(pHitbox->vecBBMin, arrBonesToWorld.at(pHitbox->iBone));
					vecMaxs = M::VectorTransform(pHitbox->vecBBMax, arrBonesToWorld.at(pHitbox->iBone));
					flRadius = pHitbox->flRadius;

					// get center
					return (vecMins + vecMaxs) * 0.5f;
				}
			}
		}
	}

	return std::nullopt;
}

std::optional<Vector> CBaseEntity::GetHitGroupPosition(const int iHitGroup)
{
	assert(iHitGroup >= HITGROUP_GENERIC && iHitGroup <= HITGROUP_GEAR); // given invalid hitbox index for gethitgroupposition

	std::array<matrix3x4a_t, MAXSTUDIOBONES> arrBonesToWorld = { };

	if (const auto pModel = this->GetModel(); pModel != nullptr)
	{
		if (const auto pStudioHdr = i::ModelInfo->GetStudioModel(pModel); pStudioHdr != nullptr)
		{
			if (const auto pHitboxSet = pStudioHdr->GetHitboxSet(this->GetHitboxSet()); pHitboxSet != nullptr)
			{
				if (this->SetupBones(arrBonesToWorld.data(), arrBonesToWorld.size(), BONE_USED_BY_HITBOX, 0.f))
				{
					mstudiobbox_t* pHitbox = nullptr;
					for (int i = 0; i < pHitboxSet->nHitboxes; i++)
					{
						pHitbox = pHitboxSet->GetHitbox(i);

						// check is reached needed group
						if (pHitbox->iGroup == iHitGroup)
							break;
					}

					if (pHitbox != nullptr)
					{
						// get mins/maxs by bone
						const Vector vecMin = M::VectorTransform(pHitbox->vecBBMin, arrBonesToWorld.at(pHitbox->iBone));
						const Vector vecMax = M::VectorTransform(pHitbox->vecBBMax, arrBonesToWorld.at(pHitbox->iBone));

						// get center
						return (vecMin + vecMax) * 0.5f;
					}
				}
			}
		}
	}

	return std::nullopt;
}

void CBaseEntity::ModifyEyePosition(const CAnimState* pAnimState, Vector* vecPosition) const
{
	// @ida modifyeyeposition: client.dll @ 55 8B EC 83 E4 F8 83 EC 70 56 57 8B F9 89 7C 24 38

	if (i::EngineClient->IsHLTV())
		return;

	CBaseEntity* pBaseEntity = pAnimState->pEntity;

	if (pBaseEntity == nullptr)
		return;

	IClientEntity* pGroundEntity = pBaseEntity->GetGroundEntity();

	if (!pAnimState->bHitGroundAnimation && pAnimState->flDuckAmount == 0.f && pGroundEntity != nullptr)
		return;

	matrix3x4_t* pMatrix = pBaseEntity->GetCachedBoneData().Base();
	Vector vecHeadPos = Vector
	(
		pMatrix[8][0][3],
		pMatrix[8][1][3],
		pMatrix[8][2][3] + 1.7f
	);

	if (const auto headPosition = vecHeadPos; headPosition != Vector(0, 0, 0))
	{
		Vector vecHead = headPosition;
		vecHead.z += 1.7f;

		if (vecHead.z < vecPosition->z)
		{
			float flFactor = 0.f;
			const float flDelta = std::fabsf(vecPosition->z - vecHead.z);
			const float flOffset = (flDelta - 4.0f) / 6.0f;

			if (flOffset >= 0.f)
				flFactor = min(flOffset, 1.0f);

			const float flFactorSquared = (flFactor * flFactor);
			vecPosition->z += ((vecHead.z - vecPosition->z) * ((flFactorSquared * 3.0f) - ((flFactorSquared * 2.0f) * flFactor)));
		}
	}
}

void CBaseEntity::PostThink()
{
	// @ida postthink: client.dll 56 8B 35 ? ? ? ? 57 8B F9 8B CE 8B 06 FF 90 ? ? ? ? 8B 07

	using PostThinkVPhysicsFn = bool(__thiscall*)(CBaseEntity*);
	static auto oPostThinkVPhysics = reinterpret_cast<PostThinkVPhysicsFn>(MEM::FindPattern(CLIENT_DLL, XorStr("55 8B EC 83 E4 F8 81 EC ? ? ? ? 53 8B D9 56 57 83 BB")));
	assert(oPostThinkVPhysics != nullptr);

	using SimulatePlayerSimulatedEntitiesFn = void(__thiscall*)(CBaseEntity*);
	static auto oSimulatePlayerSimulatedEntities = reinterpret_cast<SimulatePlayerSimulatedEntitiesFn>(MEM::FindPattern(CLIENT_DLL, XorStr("56 8B F1 57 8B BE ? ? ? ? 83 EF 01 78 74")));
	assert(oSimulatePlayerSimulatedEntities != nullptr);

	//i::MDLCache->BeginLock();

	if (this->IsAlive())
	{
		this->UpdateCollisionBounds();

		if (this->GetFlags() & FL_ONGROUND)
			this->GetFallVelocity() = 0.f;

		if (this->GetSequence() == -1)
			this->SetSequence(0);

		this->StudioFrameAdvance();
		oPostThinkVPhysics(this);
	}

	oSimulatePlayerSimulatedEntities(this);

	//i::MDLCache->EndLock();
}

bool CBaseEntity::InitializeAsClientEntity(const char* pszModelName, bool bRenderWithViewModels) {

	using InitializeAsClientEntityFn = bool(__thiscall*)(const char*, bool);
	static auto oInitializeAsClientEntity = reinterpret_cast<InitializeAsClientEntityFn>(MEM::FindPattern(CLIENT_DLL, XorStr("55 8B EC 8B 55 08 56 57 8B F9 85 D2 74 1B")));
	assert(oInitializeAsClientEntity != nullptr);

	return oInitializeAsClientEntity(pszModelName, bRenderWithViewModels);
}

bool CBaseEntity::IsEnemy(CBaseEntity* pEntity)
{
	// check is dangerzone
	//if (I::GameTypes->GetCurrentGameType() == GAMETYPE_FREEFORALL)
	//	// check is not teammate
	//	return (this->GetSurvivalTeam() != pEntity->GetSurvivalTeam());

	static CConVar* mp_teammates_are_enemies = i::ConVar->FindVar("mp_teammates_are_enemies");

	// check is deathmatch
	if (mp_teammates_are_enemies != nullptr && mp_teammates_are_enemies->GetBool() && this->GetTeam() == pEntity->GetTeam() && this != pEntity)
		return true;

	if (this->GetTeam() != pEntity->GetTeam())
		return true;

	return false;
}

bool CBaseEntity::IsTargetingLocal(CBaseEntity* pLocal)
{
	Vector vecForward = { };
	const Vector angView = this->GetEyeAngles();
	M::AngleVectors(angView, &vecForward);
	vecForward *= MAX_DISTANCE;

	const Vector vecStart = this->GetEyePosition();
	const Vector vecEnd = vecStart + vecForward;

	const Ray_t ray(vecStart, vecEnd);
	CTraceFilter filter(this);

	Trace_t trace = { };
	i::EngineTrace->TraceRay(ray, MASK_SHOT, &filter, &trace);

	if (trace.pHitEntity == pLocal)
		return true;

	return false;
}

bool CBaseEntity::CanShootLocalPlayer(CBaseEntity* pLocal) {

	Lagcompensation::LagRecord_t pLocalRecord(pLocal);
	std::memcpy(&pLocalRecord.pMatricies[RESOLVE], localAnim->GetRealMatrix().data(), sizeof(matrix3x4a_t) * 256);

	const Vector vecHeadPosition = pLocal->GetHitboxPosition(HITBOX_HEAD, pLocalRecord.pMatricies[RESOLVE]);
	Vector vecShootAngle = M::VectorAngles(vecHeadPosition - GetEyePosition(false));

	FireBulletData_t data;
	return autowall.SimulateFireBullet(this, this->GetWeapon(), data, &pLocalRecord);
}

bool CBaseEntity::CanShoot(CBaseCombatWeapon* pBaseWeapon, int iTickbase)
{
	if (!pBaseWeapon)
		return false;

	if (iTickbase == -1)
		iTickbase = this == g::pLocal ? networking.GetCorrectedTickbase() : this->GetTickBase();
	const float flServerTime = TICKS_TO_TIME(iTickbase);

	// check is have ammo
	if (pBaseWeapon->GetAmmo() <= 0)
		return false;

	// is player ready to shoot
	if (this->GetNextAttack() > flServerTime)
		return false;

	const short nDefinitionIndex = pBaseWeapon->GetItemDefinitionIndex();

	// check is weapon with burst mode
	if ((nDefinitionIndex == WEAPON_FAMAS || nDefinitionIndex == WEAPON_GLOCK) &&
		// check is burst mode
		pBaseWeapon->IsBurstMode() && pBaseWeapon->GetBurstShotsRemaining() > 0)
		return true;

	// is weapon ready to shoot
	if (pBaseWeapon->GetNextPrimaryAttack() > flServerTime)
		return false;

	// check for revolver cocking ready
	if (nDefinitionIndex == WEAPON_REVOLVER && pBaseWeapon->GetFireReadyTime() > flServerTime)
		return false;

	return true;
}

bool CBaseEntity::IsVisible(CBaseEntity* pEntity, const Vector& vecEnd, bool bSmokeCheck)
{
	const Vector vecStart = this->GetEyePosition(false);

	const Ray_t ray(vecStart, vecEnd);
	CTraceFilter filter(this);

	Trace_t trace = { };
	i::EngineTrace->TraceRay(ray, MASK_SHOT, &filter, &trace);

	// trace check
	if ((trace.IsVisible() || trace.pHitEntity == pEntity) /*&& !(bSmokeCheck && u::LineGoesThroughSmoke(vecStart, vecEnd))*/)
		return true;

	return false;
}

class IMultiplayerPhysics
{
public:
	virtual int		GetMultiplayerPhysicsMode() = 0;
	virtual float	GetMass() = 0;
	virtual bool	IsAsleep() = 0;
};

void CBaseEntity::InvalidateBoneCache() {

	static DWORD addr = (DWORD)MEM::FindPattern(CLIENT_DLL, XorStr("80 3D ? ? ? ? ? 74 16 A1 ? ? ? ? 48 C7 81"));

	*(int*)((uintptr_t)this + 0xA30) = i::GlobalVars->iFrameCount; //we'll skip occlusion checks now
	*(int*)((uintptr_t)this + 0xA28) = 0;//clear occlusion flags

	unsigned long g_iModelBoneCounter = **(unsigned long**)(addr + 10);
	*(unsigned int*)((DWORD)this + 0x2924) = 0xFF7FFFFF; // m_flLastBoneSetupTime = -FLT_MAX;
	*(unsigned int*)((DWORD)this + 0x2690) = (g_iModelBoneCounter - 1); // m_iMostRecentModelBoneCounter = g_iModelBoneCounter - 1;
}

float CBaseEntity::GetSequenceCycleRate(CStudioHdr* pStudioHdr, int iSequence) {

	return util::CallVFunc<float>( this, 222, pStudioHdr, iSequence );
}

float CBaseEntity::GetLayerSequenceCycleRate(CAnimationLayer* pLayer, int iSequence) {

	return util::CallVFunc < float >( this, 223, pLayer, iSequence );
}

#pragma runtime_checks( "", off )
float CBaseEntity::GetSequenceMoveDist(CStudioHdr* pStudioHdr, int iSequence) {

	Vector vecReturn;
	static auto lmao = MEM::FindPattern(CLIENT_DLL, XorStr("55 8B EC 83 EC 0C 56 8B F1 57 8B FA 85 F6 75 14"));
	using GetSequenceLinearMotionFn = int(__fastcall*)(CStudioHdr*, int, float*, Vector*);
	((GetSequenceLinearMotionFn)lmao)(pStudioHdr, iSequence, GetPoseParameter().data(), &vecReturn);
	__asm {
		add esp, 8
	}
	return vecReturn.Length();
}
#pragma runtime_checks( "", restore )

//float CBaseEntity::GetSequenceMoveDist( CStudioHdr* pStudioHdr, int iSequence ) {
//
//	Vector vecReturn;
//
//	using GetSequenceLinearMotionFn = int( __cdecl* )( CStudioHdr*, int, float*, Vector* );
//	static auto oGetSequenceLinearMotion = reinterpret_cast< GetSequenceLinearMotionFn >( util::FindSignature( "client.dll", "55 8B EC 83 EC 0C 56 8B F1 57 8B FA 85 F6 75 14 68" ) );
//	//assert( oGetSequenceLinearMotion != nullptr );
//
//	oGetSequenceLinearMotion( pStudioHdr, iSequence, GetPoseParameter( ).data(), &vecReturn );
//
//	return vecReturn.Length( );
//}

bool CBaseEntity::HandleBoneSetup(matrix3x4a_t* pBoneToWorldOut, int nBoneMask, float flCurrentTime)
{
	if (i::ClientState->iSignonState != SIGNONSTATE_FULL)
		return false;

	CStudioHdr* pStudioHdr = this->GetStudioHdr();
	if (!pStudioHdr || !pStudioHdr->pVirtualModel)
		return false;

	CMDLCacheCriticalSection mdlCacheCriticalSection(i::MDLCache);

	matrix3x4a_t* const matOldBones = this->GetBoneAccessor().matBones;
	const Vector vecOldAngles = this->GetAbsAngles();
	const Vector vecOldOrigin = this->GetAbsOrigin();

	CAnimationLayer arrLayers[13];
	float arrPoses[24];

	this->GetAnimationLayers(arrLayers);
	this->GetPoseParameters(arrPoses);

	matrix3x4_t matBaseMatrix;
	M::AngleMatrix(this->GetAbsAngles(), this->GetAbsOrigin(), matBaseMatrix);

	this->GetEffects() |= EF_NOINTERP;

	IKContext* IK_context = this->GetIKContext();
	if (IK_context)
	{
		Vector vecAbsAngles = const_cast<Vector&>(this->GetAbsAngles());

		IK_context->ClearTargets();
		IK_context->Init(pStudioHdr, vecAbsAngles, this->GetVecOrigin(), flCurrentTime, i::GlobalVars->iFrameCount, BONE_USED_BY_HITBOX | BONE_USED_BY_VERTEX_MASK);

		this->SetAbsAngles(vecAbsAngles);
	}

	Vector pos[256]{};
	BoneQuaternion q[256];
	CBoneBitList boneComputed;

	this->GetBoneAccessor().matBones = pBoneToWorldOut;
	this->StandardBlendingRules(pStudioHdr, pos, q, flCurrentTime, nBoneMask);

	if (IK_context)
	{
		this->UpdateIKLocks(flCurrentTime);
		IK_context->UpdateTargets(pos, q, pBoneToWorldOut, boneComputed);
		this->CalculateIKLocks(flCurrentTime);
		IK_context->SolveDependencies(pos, q, pBoneToWorldOut, boneComputed);
	}

	this->BuildTransformations(pStudioHdr, pos, q, matBaseMatrix, nBoneMask, boneComputed);
	this->ClampBonesInBBox(pBoneToWorldOut, nBoneMask);

	this->GetEffects() &= ~EF_NOINTERP;

	this->SetAnimationLayers(arrLayers);
	this->SetPoseParameters(arrPoses);
	this->SetAbsOrigin(vecOldOrigin);
	this->SetAbsAngles(vecOldAngles);
	this->GetBoneAccessor().matBones = matOldBones;

	return true;
}

bool CBaseEntity::IsBreakable()
{
	// @ida isbreakableentity: client.dll @ 55 8B EC 51 56 8B F1 85 F6 74 68

	const int iHealth = this->GetHealth();

	// first check to see if it's already broken
	if (iHealth < 0 && this->IsMaxHealth() > 0)
		return true;

	if (this->GetTakeDamage() != DAMAGE_YES)
	{
		const EClassIndex nClassIndex = this->GetClientClass()->nClassID;

		// force pass cfuncbrush
		if (nClassIndex != EClassIndex::CFuncBrush)
			return false;
	}

	if (const int nCollisionGroup = this->GetCollisionGroup(); nCollisionGroup != COLLISION_GROUP_PUSHAWAY && nCollisionGroup != COLLISION_GROUP_BREAKABLE_GLASS && nCollisionGroup != COLLISION_GROUP_NONE)
		return false;

	if (iHealth > 200)
		return false;

	if (IMultiplayerPhysics* pPhysicsInterface = dynamic_cast<IMultiplayerPhysics*>(this); pPhysicsInterface != nullptr)
	{
		if (pPhysicsInterface->GetMultiplayerPhysicsMode() != PHYSICS_MULTIPLAYER_SOLID)
			return false;
	}
	else {
	
		if (const char* szClassName = this->GetClassname(); !strcmp(szClassName, "func_breakable") || !strcmp(szClassName, "func_breakable_surf"))
		{
			if (!strcmp(szClassName, "func_breakable_surf"))
			{
				CBreakableSurface* pSurface = static_cast<CBreakableSurface*>(this);

				// don't try to break it if it has already been broken
				if (pSurface->IsBroken())
					return false;
			}
		}
		else if (this->PhysicsSolidMaskForEntity() & CONTENTS_PLAYERCLIP)
		{
			// hostages and players use CONTENTS_PLAYERCLIP, so we can use it to ignore them
			return false;
		}
	}

	if (IBreakableWithPropData* pBreakableInterface = dynamic_cast<IBreakableWithPropData*>(this); pBreakableInterface != nullptr)
	{
		// bullets don't damage it - ignore
		if (pBreakableInterface->GetDmgModBullet() <= 0.0f)
			return false;
	}

	return true;
}

mstudiobbox_t* CBaseEntity::StudioHitbox(int iHitbox) {

	if (!this)
		return nullptr;

	const Model_t* model = this->GetModel();
	if (!model)
		return nullptr;

	studiohdr_t* studioHdr = i::ModelInfo->GetStudioModel(model);
	if (!studioHdr)
		return nullptr;

	mstudiobbox_t* studioBox = studioHdr->GetHitboxSet(0)->GetHitbox(iHitbox);
	if (!studioBox)
		return nullptr;

	return studioBox;
}

bool CBaseEntity::IsFakeducking() {

	static float flStoredTicks;
	static float flCrouchedTicks[65];

	if (!this->AnimState())
		return false;

	float flDuckAmount = this->AnimState()->flDuckAmount;
	float flDuckSpeed = this->GetDuckSpeed();
	float iFlags = this->GetFlags();

	if (flDuckSpeed != 0.f && flDuckAmount != 0.f)
	{
		if (flDuckSpeed == 8.f && flDuckAmount <= 0.9f && flDuckAmount > 0.01f)
		{
			if (flStoredTicks != i::GlobalVars->iTickCount)
			{
				flCrouchedTicks[this->EntIndex()] = flCrouchedTicks[this->EntIndex()] + 1;
				flStoredTicks = i::GlobalVars->iTickCount;
			}
			return (flCrouchedTicks[this->EntIndex()] >= 5);
		}
		else
			flCrouchedTicks[this->EntIndex()] = 0;
	}

	return false;
}