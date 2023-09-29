#include "../hooks.h"
#include "../../Interface/Classes/CConvar.h"
#include "../../SDK/Menu/config.h"
#include "../../Features/Rage/exploits.h"
#include "../../Interface/interfaces.h"
#include "../../Features/Networking/networking.h"

//https://github.com/perilouswithadollarsign/cstrike15_src/blob/f82112a2388b841d72cb62ca48ab1846dfcc11c8/engine/cl_main.cpp#L2736
void __cdecl h::hkClMove(float flSamples, bool bFinalTick) {

	// run exploits
	return exploits::OnCLMove( flSamples, bFinalTick );

	// this hook calls g_ClientDLL->CreateMove (that is hooked lmao)
	// g_ClientDLL->CreateMove( cl.lastoutgoingcommand + cl.chokedcommands + 1, host_state.interval_per_tick - flSamples, !cl.IsPaused() );

	// changing flSamples while doubletapping can cause some issues like freezing when doubletapping while our ticks are getting back to sync
	// and gl getting back ticks to synced values while we freeze :D (takes like 20-30sec to unfreeze the game)
}