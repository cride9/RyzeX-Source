#include "../../hooks.h"
#include "../../../SDK/Entity.h"
#include "../../../globals.h"
#include "../../../Features/Rage/Animations/Lagcompensation.h"
#include "../../../Features/Rage/Animations/EnemyAnimations.h"

bool __fastcall h::hkSetupBones(IClientRenderable* ecx, void* edx, matrix3x4a_t* matrix, int maxbones, int bonemask, float curtime) {

	static auto original = detour::setupBones.GetOriginal<decltype(&h::hkSetupBones)>();

	CBaseEntity* pEnt = ecx->GetIClientUnknown()->GetBaseEntity();

	bool bResult = true;
	if (!g::pLocal || !pEnt->IsAlive() || !pEnt->IsPlayer() || g::bUpdatingSkins || (!pEnt->IsEnemy(g::pLocal) && pEnt != g::pLocal))
		return original( ecx, edx, matrix, maxbones, bonemask, curtime);

	if (g::bSettingUpBones[pEnt->EntIndex()])
		return original(ecx, edx, matrix, maxbones, bonemask, curtime);;

	if (matrix)  
		bResult = (pEnt == g::pLocal ? localAnim->CopyCachedMatrix(matrix, maxbones) : anims.CopyCachedMatrix(pEnt, matrix, maxbones));

	return bResult;
}