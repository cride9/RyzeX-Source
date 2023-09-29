#include "../hooks.h"
#include "../../globals.h"
#include "../../Features/Visuals/chams.h"
#include "../../Features/Misc/misc.h"

void __fastcall h::hkDrawModel(IStudioRender* thisptr, int edx, DrawModelResults_t* pResults, const DrawModelInfo_t& info, matrix3x4a_t* pBoneToWorld, float* flFlexWeights, float* flFlexDelayedWeights, const Vector& vecModelOrigin, int nFlags) {

	static auto original = detour::drawModel.GetOriginal<decltype(&h::hkDrawModel)>();

	if (i::ClientState->iSignonState != SIGNONSTATE_FULL)
		return original(thisptr, edx, pResults, info, pBoneToWorld, flFlexWeights, flFlexDelayedWeights, vecModelOrigin, nFlags);

	static CConVar* r_drawothermodels = i::ConVar->FindVar(XorStr("r_drawothermodels"));
	if (r_drawothermodels->GetInt() != 3)
		r_drawothermodels->SetValue(3);

	bool bClearOverride = false;

	if (g::pLocal) {

		misc::RemoveSmoke();
		bClearOverride = chams::DrawChams(g::pLocal, pResults, info, pBoneToWorld, flFlexWeights, flFlexDelayedWeights, vecModelOrigin, nFlags);
	}

	original(thisptr, edx, pResults, info, pBoneToWorld, flFlexWeights, flFlexDelayedWeights, vecModelOrigin, nFlags);

	if (bClearOverride)
		i::StudioRender->ForcedMaterialOverride(nullptr);
}

void __fastcall h::hkDrawModelMDL(void* ecx, int edx, IMatRenderContext* ctx, const DrawModelState_t& state, const ModelRenderInfo_t& info, matrix3x4_t* bonetoworld) {

	static auto original = detour::drawModelMdl.GetOriginal<decltype(&h::hkDrawModelMDL)>();

	original(ecx, edx, ctx, state, info, bonetoworld);
}

int __fastcall h::hkDrawModelAnimating(void* ecx, int edx, int flags, RenderableInstance_t& instance) {

	static auto original = detour::drawModelAnimating.GetOriginal<decltype(&h::hkDrawModelAnimating)>();

	int returnValue = original(ecx, edx, flags, instance);

	return returnValue;
}