#include "../../hooks.h"
#include "../../../Features/Rage/Animations/Lagcompensation.h"
#include "../../../Features/Networking/networking.h"
/*
void ___thiscall C_CSPlayer::BuildTransformations (DWORD this, int studioHdr, int a3, int a4, int a5, int boneMask, int boneComputed)
{
	C_BaseFlex::BuildTransformations((int *)this, studioHdr, a3, a4, a5, boneMask);
	v8 = *(_DWORD *)(this + 0x26A4);

	if (v8) {
		v9 = *(_DWORD *)(v8 +0x2950); // maybe studiohdr?

		if ( v9 &&*(_DWORD *)v9 && (**(_DWORD **)(v9 + 0x30) & *(_DWORD *)(this + 0x26B0)) != 0 && !bonemask ) {
			
			// this = entity pointer
			// *(_DWORD *)(this+0x26A8) = entity->m_dwBoneMatrix
			// boneMask = bonemask

			C_CSPlayer::ClampBonesInBBox(CBaseEntity* pEntity, matrix3x4_t* pMatrix, int iBoneMask);
		}
	}
}
*/

void __fastcall h::hkClampBonesInBBox(CBaseEntity* pEntity, void* edx, matrix3x4a_t* pMatrix, int iBoneMask) {

	static auto original = detour::clampBonesInBBox.GetOriginal<decltype(&hkClampBonesInBBox)>();

	if (!g::pLocal || (!pEntity->IsEnemy(g::pLocal) && g::pLocal != pEntity))
		return original(pEntity, edx, pMatrix, iBoneMask);

	float flCurtime = i::GlobalVars->flCurrentTime;

	i::GlobalVars->flCurrentTime = pEntity->GetSimulationTime();

	original( pEntity, edx, pMatrix, iBoneMask);

	i::GlobalVars->flCurrentTime = flCurtime;
}