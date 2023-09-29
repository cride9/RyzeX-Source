#include "../hooks.h"
#include "../../SDK/Menu/config.h"
#include "../../SDK/Entity.h"

int __fastcall h::hkListLeavesInBox(void* thisptr, void* edx, const Vector* vecMins, const Vector* vecMaxs, unsigned short* puList, int nListMax) {

	static auto original = detour::listLeaves.GetOriginal<decltype(&h::hkListLeavesInBox)>();

	static const auto pInsertIntoTreeReturn = MEM::FindPattern(CLIENT_DLL, XorStr("56 52 FF 50 18")) + 0x5;

	if (const volatile auto pReturnAddress = static_cast<std::uint8_t*>(_ReturnAddress());
		// check is called from 'CClientLeafSystem::InsertIntoTree()'
		pReturnAddress != pInsertIntoTreeReturn)
		return original(thisptr, edx, vecMins, vecMaxs, puList, nListMax);

	const volatile auto pStackFrame = static_cast<std::uint8_t*>(_AddressOfReturnAddress());

	if (const auto pInfo = *reinterpret_cast<RenderableInfo_t**>(pStackFrame + 0x14); pInfo != nullptr)
	{
		if (const auto pRenderable = pInfo->pRenderable; pRenderable != nullptr)
		{
			CBaseEntity* pLocal = CBaseEntity::GetLocalPlayer();

			// check is disabling occlusion for enemy players
			if (const auto pCSPlayer = static_cast<CBaseEntity*>(pRenderable->GetIClientUnknown()->GetBaseEntity()); pCSPlayer != nullptr && pCSPlayer->IsPlayer() && pLocal != nullptr && pCSPlayer->IsEnemy(pLocal))
			{
				// @test: seems like valve fixed render order themselves or somewhat else and now chams draw perfect through decals w/o any fixes

				static auto m_bCanUseFastPath = (n::netvars[fnv::HashConst("CBaseAnimating->m_flFrozen")]).uOffset + 0x4;
				*(bool*)((uintptr_t)pCSPlayer + m_bCanUseFastPath) = false;

				//pInfo->uFlags &= ~RENDER_FLAGS_FORCE_OPAQUE_PASS;
				//pInfo->uFlags2 |= RENDER_FLAGS_BOUNDS_ALWAYS_RECOMPUTE;

				// @test: since just 'constexpr' without 'static' doesn't guarantee it gonna be inside rdata, is it ok to pass pointer to stack vars here? check original method 
				constexpr Vector vecUnlimitedMins = { -MAX_COORD_FLOAT, -MAX_COORD_FLOAT, -MAX_COORD_FLOAT };
				constexpr Vector vecUnlimitedMaxs = { MAX_COORD_FLOAT, MAX_COORD_FLOAT, MAX_COORD_FLOAT };
				return original(thisptr, edx, &vecUnlimitedMins, &vecUnlimitedMaxs, puList, nListMax);
			}
		}
	}

	return original(thisptr, edx, vecMins, vecMaxs, puList, nListMax);
}