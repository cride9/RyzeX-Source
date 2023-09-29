#include "../hooks.h"
#include "../../Features/Misc/misc.h"
#include "../../Features/Rage/Animations/LocalAnimation.h"
#include "../../SDK/Menu/config.h"
#include "../../Features/Networking/networking.h"
#include "../../Features/Rage/Animations/Lagcompensation.h"
#include "../../Features/Rage/exploits.h"
#include "../../Features/Changers/SkinChanger.h"
#include "../../Lua/Lua.h"

/*

void OnRenderStart()
{
	CMatRenderContextPtr pRenderContext( materials );

	C_BaseAnimating::EnableInvalidateBoneCache( true );

	::partition->SuppressLists( PARTITION_ALL_CLIENT_EDICTS, true );
	C_BaseEntity::SetAbsQueriesValid( false );

	Rope_ResetCounters();

	{
		PREDICTION_TRACKVALUECHANGESCOPE( "interpolation" );
		C_BaseEntity::InterpolateServerEntities();
	}

	{
		VPROF( "OnRenderStart: dirty bone caches");
		C_BaseAnimating::InvalidateBoneCaches();
		C_BaseFlex::InvalidateFlexCaches();

		C_BaseEntity::SetAbsQueriesValid( true );
		C_BaseEntity::EnableAbsRecomputations( true );
		C_BaseAnimating::PushAllowBoneAccess( true, false, "OnRenderStart->CViewRender::SetUpView" ); // pops in CViewRender::SetUpView

		C_BaseEntity::MarkAimEntsDirty();
	}

	C_BaseAnimating::PopBoneAccess( "OnRenderStart->CViewRender::SetUpView" ); // pops the (true, false) bone access set in OnRenderStart

	C_BaseAnimating::PushAllowBoneAccess( true, true, "CViewRender::SetUpView->OnRenderEnd" ); // pop is in OnRenderEnd()

	C_BaseAnimating::UpdateClientSideAnimations();

	view->OnRenderStart();
	::partition->SuppressLists( PARTITION_ALL_CLIENT_EDICTS, false );

	ProcessOnDataChangedEvents();

	g_SmokeFogOverlayAlpha = 0;

	g_SmokeFogOverlayColor.Init( 0.0f, 0.0f, 0.0f );

	{
		PIXEVENT( pRenderContext, "SimulateEntities" );
		SimulateEntities();
	}

	{
		PIXEVENT( pRenderContext, "PhysicsSimulate" );
		PhysicsSimulate();
	}

	C_BaseAnimating::ThreadedBoneSetup();

	{
		VPROF_("Client TempEnts", 0, VPROF_BUDGETGROUP_CLIENT_SIM, false, BUDGETFLAG_CLIENT);
		engine->FireEvents();
		tempents->Update();
		beams->UpdateTempEntBeams();
		SetBeamCreationAllowed( false );
	}

	{
		VPROF_BUDGET( "ParticleMgr()->Simulate", VPROF_BUDGETGROUP_PARTICLE_SIMULATION );
		ParticleMgr()->Simulate( gpGlobals->frametime );
	}

	C_BaseEntity::CalcAimEntPositions();

	if ( ToolsEnabled() )
	{
		C_BaseEntity::ToolRecordEntities();
	}

	engine->UpdateDAndELights();
	C_BaseEntity::AddVisibleEntities();

	g_pClientLeafSystem->RecomputeRenderableLeaves();
	g_pClientShadowMgr->ReprojectShadows();
	g_pClientShadowMgr->AdvanceFrame();
	g_pClientLeafSystem->DisableLeafReinsertion( true );
}

*/

void hkPreFrameStageNotify(EStage curStage) {

	auto pLocal = CBaseEntity::GetLocalPlayer();
	if (!pLocal)
		return;

	skinChanger.AgentChanger(pLocal, curStage);
	switch (curStage) {

	case FRAME_START:

		break;

	case FRAME_RENDER_END:
		break;

	case FRAME_RENDER_START:
		if (exploits::bIsShiftingTicks)
			localAnim->SetupInterpolation(pLocal, false);

#ifdef _DEBUG
		//misc::ServerHitboxes();
#endif
		misc::BulletImpactFrameStage(pLocal);
		break;

	case FRAME_NET_UPDATE_END:
		lagcomp.FrameStageNotify();
		break;

	case FRAME_NET_UPDATE_POSTDATAUPDATE_END:
		break;

	case FRAME_NET_UPDATE_POSTDATAUPDATE_START:
		break;

	default:
		break;
	}
}

void hkPostFrameStageNotify(EStage curStage) {

	auto pLocal = CBaseEntity::GetLocalPlayer();
	if (!pLocal)
		return;

	switch (curStage) {

	case FRAME_NET_UPDATE_START:

		break;

	case FRAME_NET_UPDATE_POSTDATAUPDATE_START: // lmao
		skinChanger.Run(pLocal);
		break;

	case FRAME_NET_UPDATE_POSTDATAUPDATE_END:

		break;

	case FRAME_NET_UPDATE_END:
		prediction.OnFrameStage(pLocal);
		break;

	case FRAME_RENDER_START:
		if (exploits::bIsShiftingTicks)
			localAnim->SetupInterpolation(pLocal, true);
		break;

	default:
		break;
	}
}

void __fastcall h::hkFrameStageNotify(void* ecx, void* edx, EStage curStage) {

	static auto original = detour::frameStageNotify.GetOriginal<decltype(&h::hkFrameStageNotify)>();

	hkPreFrameStageNotify(curStage);
	original(ecx, edx, curStage);
	hkPostFrameStageNotify(curStage);
}