#include "../../hooks.h"
#include "../../../SDK/Entity.h"

void __fastcall h::hkBuildTransformation(void* entityPointer, void* edx, CStudioHdr* hdr, Vector* pos, Quaternion* q, const matrix3x4a_t& transform, int mask, uint8_t* computed) {

	static auto original = detour::buildTransform.GetOriginal<decltype(&h::hkBuildTransformation)>();

	const auto pEnt = reinterpret_cast<CBaseEntity*>(entityPointer);
	if (!pEnt || !pEnt->IsAlive())
		return original( entityPointer, edx, hdr, pos, q, transform, mask, computed);

	auto backup = hdr->vecBoneFlags;
	float flCurtime = i::GlobalVars->flCurrentTime;
	i::GlobalVars->flCurrentTime = pEnt->GetSimulationTime();
	
	for (auto i = 0; i < hdr->vecBoneFlags.Count(); i++)
		hdr->vecBoneFlags.Element(i) &= ~BONE_ALWAYS_PROCEDURAL;

	original(entityPointer, edx, hdr, pos, q, transform, mask, computed);

	hdr->vecBoneFlags = backup;
	i::GlobalVars->flCurrentTime = flCurtime;
}