#include "../../hooks.h"
#include "../../../SDK/math.h"

void __fastcall h::hkSetupAliveLoop(void* ecx, void* edx) {

	static auto original = detour::setupAliveLoop.GetOriginal<decltype(&h::hkSetupAliveLoop)>();

	CAnimState* pState = static_cast <CAnimState*> (ecx);
	if (!pState->pEntity || pState->pEntity != g::pLocal)
		return;

	CAnimationLayer* pAliveLoop = &pState->pEntity->GetAnimationOverlays()[ANIMATION_LAYER_ALIVELOOP];
	if (!pAliveLoop)
		return;

	if (pAliveLoop->nSequence != 8 && pAliveLoop->nSequence != 9) {

		pState->SetLayerSequence(pAliveLoop, ACT_CSGO_ALIVE_LOOP);
		pState->SetLayerCycle(pAliveLoop, M::RandomFloat(0.0f, 1.0f));
		pState->SetLayerRate(pAliveLoop, pState->pEntity->GetLayerSequenceCycleRate(pAliveLoop, pAliveLoop->nSequence) * M::RandomFloat(0.8, 1.1f));
	}
	else {

		float flRetainCycle = pAliveLoop->flCycle;
		if (pState->pActiveWeapon != pState->pLastActiveWeapon) {

			pState->SetLayerSequence(pAliveLoop, ACT_CSGO_ALIVE_LOOP);
			pState->SetLayerCycle(pAliveLoop, flRetainCycle);
		}
		else if (pState->IsLayerSequenceFinished(pAliveLoop, pState->flLastUpdateIncrement))
			pState->SetLayerRate(pAliveLoop, pState->pEntity->GetLayerSequenceCycleRate(pAliveLoop, pAliveLoop->nSequence) * M::RandomFloat(0.8, 1.1f));
		else
			pState->SetLayerWeight(pAliveLoop, M::RemapValClamped(pState->flRunningSpeed, 0.55f, 0.9f, 1.0f, 0.0f));
	}

	return pState->IncrementLayerCycle(pAliveLoop, true);
}