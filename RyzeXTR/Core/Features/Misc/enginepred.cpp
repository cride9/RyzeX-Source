#include "enginepred.h"
#include "../../globals.h"
#include "../Networking/networking.h"

void Prediction::OnFrameStage(CBaseEntity* pLocal) {

	// local must be alive and we also must receive an update
	if (!pLocal->IsAlive())
		return;

	// define const
	const int nSimulationTick = TIME_TO_TICKS(pLocal->GetSimulationTime());
	const int nOldSimulationTick = TIME_TO_TICKS(pLocal->GetOldSimulationTime());
	const int nCorrectionTicks = TIME_TO_TICKS(0.03f);
	const int nServerTick = i::ClientState->clockDriftMgr.nServerTick;

	// check time
	if (nSimulationTick <= nOldSimulationTick || abs(nSimulationTick - nServerTick) > nCorrectionTicks)
		return;

	// save last simulation ticks amount
	Tickbase_t.iSimulationTick = nSimulationTick - nServerTick;
}  

int Prediction::AdjustPlayerTimeBase(int iSimulationTick) {

	// get tickbase
	int nTickBase = g::pLocal->GetTickBase() + 1;

	// define const
	const int nCorrectionTicks = TIME_TO_TICKS(0.03f);
	const int nChokedCmds = i::ClientState->nChokedCommands;

	// if client gets ahead or behind of this, we'll need to correct.
	const int nTooFastLimit = nTickBase + nCorrectionTicks + nChokedCmds - Tickbase_t.iSimulationTick + 1;
	const int nTooSlowLimit = nTickBase - nCorrectionTicks + nChokedCmds - Tickbase_t.iSimulationTick + 1;

	// correct tick 
	if (nTickBase + 1 > nTooFastLimit || nTickBase + 1 < nTooSlowLimit)
		nTickBase += nCorrectionTicks + nChokedCmds - Tickbase_t.iSimulationTick;

	// save predicted tickbase
	return nTickBase + iSimulationTick;
}

void Prediction::Start(CUserCmd* pCmd, CBaseEntity* pLocal, int SequenceNumber )
{
	// @xref: "CPrediction::ProcessMovement"

	if (!pLocal->IsAlive() || i::MoveHelper == nullptr)
		return;

	localAnim->BeforePrediction(pLocal);
	if (CBaseCombatWeapon* pWeapon = pLocal->GetWeapon(); pWeapon) {

		pWeapon->UpdateAccuracyPenalty();

		flSpread = pWeapon->GetSpread();
		flInaccuracy = pWeapon->GetInaccuracy();
	}

	// start command
	*pLocal->GetCurrentCommand() = pCmd;
	pLocal->GetLastCommand() = *pCmd;

	// random seed was already generated in CInput::CreateMove
	*uPredictionRandomSeed = pCmd->iRandomSeed;
	// set ourselves as a predictable entity
	*pPredictionPlayer = pLocal;

	// backup globals
	flOldCurrentTime = i::GlobalVars->flCurrentTime;
	flOldFrameTime = i::GlobalVars->flFrameTime;
	iOldTickCount = i::GlobalVars->iTickCount;

	int SeqDiff = 0;
	int TickBase = 0;

	// the big ol tb fixer upper
	if ( !pLastCmd || pLastCmd->bHasBeenPredicted )
		SeqDiff = SequenceNumber - networking.GetCorrectedTickbase();

	TickBase = max(networking.GetCorrectedTickbase(),
		SequenceNumber - SeqDiff );

	pLastCmd = g::pCmd;

	// backup tickbase
	const int iOldTickBase = pLocal->GetTickBase();

	// backup prediction states
	const bool bOldIsFirstPrediction = i::Prediction->bIsFirstTimePredicted;
	const bool bOldInPrediction = i::Prediction->bInPrediction;

	// set corrected values
	i::GlobalVars->flCurrentTime = TICKS_TO_TIME( TickBase );
	i::GlobalVars->flFrameTime = i::Prediction->bEnginePaused ? 0.f : TICK_INTERVAL;
	i::GlobalVars->iTickCount = TickBase;

	i::Prediction->bIsFirstTimePredicted = false;
	i::Prediction->bInPrediction = true;

	/* skipped weapon select and vehicle predicts */

	// synchronize m_afButtonForced & m_afButtonDisabled
	pCmd->iButtons |= pLocal->GetButtonForced();
	pCmd->iButtons &= ~(pLocal->GetButtonDisabled());

	i::GameMovement->StartTrackPredictionErrors(pLocal);

	// update button state
	const int iButtons = pCmd->iButtons;
	const int nLocalButtons = *pLocal->GetButtons();
	const int nButtonsChanged = iButtons ^ nLocalButtons;

	// synchronize m_afButtonLast
	pLocal->GetButtonLast() = nLocalButtons;

	// synchronize m_nButtons
	*pLocal->GetButtons() = iButtons;

	// synchronize m_afButtonPressed
	pLocal->GetButtonPressed() = nButtonsChanged & iButtons;

	// synchronize m_afButtonReleased
	pLocal->GetButtonReleased() = nButtonsChanged & (~iButtons);

	// check if the player is standing on a moving entity and adjusts velocity and basevelocity appropriately
	i::Prediction->CheckMovingGround(pLocal, i::GlobalVars->flFrameTime);

	// copy angles from command to player
	i::Prediction->SetLocalViewAngles(pCmd->angViewPoint);

	// run prethink
	if (pLocal->PhysicsRunThink(THINK_FIRE_ALL_FUNCTIONS))
		pLocal->PreThink();

	// run think
	if (int* iNextThinkTick = pLocal->GetNextThink(); *iNextThinkTick > 0 && *iNextThinkTick <= TickBase )
	{
		*iNextThinkTick = TICK_NEVER_THINK;

		pLocal->Think();
	}

	// set host player
	i::MoveHelper->SetHost(pLocal);

	// setup move
	i::Prediction->SetupMove(pLocal, pCmd, i::MoveHelper, &moveData);
	i::GameMovement->ProcessMovement(pLocal, &moveData);

	// finish move
	i::Prediction->FinishMove(pLocal, pCmd, &moveData);
	i::MoveHelper->ProcessImpacts();

	// run post think
	pLocal->PostThink();

	// restore tickbase
	pLocal->GetTickBase() = iOldTickBase;

	// restore prediction states
	i::Prediction->bInPrediction = bOldInPrediction;
	i::Prediction->bIsFirstTimePredicted = bOldIsFirstPrediction;
}

void Prediction::End(CUserCmd* pCmd, CBaseEntity* pLocal) const
{
	if (!pLocal->IsAlive() || i::MoveHelper == nullptr)
		return;

	i::GameMovement->FinishTrackPredictionErrors(pLocal);

	// reset host player
	i::MoveHelper->SetHost(nullptr);

	// restore globals
	i::GlobalVars->flCurrentTime = flOldCurrentTime;
	i::GlobalVars->flFrameTime = flOldFrameTime;
	i::GlobalVars->iTickCount = iOldTickCount;

	// finish command
	*pLocal->GetCurrentCommand() = nullptr;

	// reset prediction seed
	*uPredictionRandomSeed = -1;

	// reset prediction entity
	*pPredictionPlayer = 0;

	// reset move
	i::GameMovement->Reset();
}

void Prediction::SaveNetvars( int iCommand, CBaseEntity* pLocal)
{
	if (!pLocal->IsAlive())
		return;

	pNetvarData[ iCommand % 150 ].iFlags = pLocal->GetFlags( );
	pNetvarData[ iCommand % 150 ].flDuckAmount = pLocal->GetDuckAmount( );
	pNetvarData[ iCommand % 150 ].flDuckSpeed = pLocal->GetDuckSpeed( );
	pNetvarData[ iCommand % 150 ].vecOrigin = pLocal->GetVecOrigin( );
	pNetvarData[ iCommand % 150 ].vecVelocity = pLocal->GetVelocity( );
	pNetvarData[ iCommand % 150 ].vecBaseVelocity = pLocal->GetVecBaseVelocity( );
	pNetvarData[ iCommand % 150 ].flFallVelocity = pLocal->GetFallVelocity( );
	pNetvarData[ iCommand % 150 ].vecViewOffset = pLocal->GetViewOffset( );
	pNetvarData[ iCommand % 150 ].vecAimPunchAngle = pLocal->GetAimPunch( );
	pNetvarData[ iCommand % 150 ].vecAimPunchAngleVel = pLocal->GetAimPunchVelocity( );
	pNetvarData[ iCommand % 150 ].vecViewPunchAngle = pLocal->GetViewPunch( );

	CBaseCombatWeapon* pWeapon = pLocal->GetWeapon( );
	if ( !pWeapon )
		return;

	pNetvarData[ iCommand % 150 ].flRecoilIndex = static_cast< CWeaponCSBase* >(pWeapon)->GetRecoilIndex( );
	pNetvarData[ iCommand % 150 ].flAccuracyPenalty = static_cast< CWeaponCSBase* >(pWeapon)->GetAccuracyPenalty( );
}

void Prediction::RestoreNetvars(int iCommand, CBaseEntity* pLocal)
{
	if (!pLocal->IsAlive())
		return;

	pLocal->GetFlags( ) = pNetvarData[ iCommand % 150 ].iFlags;
	pLocal->GetDuckAmount( ) = pNetvarData[ iCommand % 150 ].flDuckAmount;
	pLocal->GetDuckSpeed( ) = pNetvarData[ iCommand % 150 ].flDuckSpeed;
	pLocal->GetVecOrigin( ) = pNetvarData[ iCommand % 150 ].vecOrigin;
	pLocal->GetVelocity( ) = pNetvarData[ iCommand % 150 ].vecVelocity;
	pLocal->GetVecBaseVelocity( ) = pNetvarData[ iCommand % 150 ].vecBaseVelocity;
	pLocal->GetFallVelocity( ) = pNetvarData[ iCommand % 150 ].flFallVelocity;
	pLocal->GetViewOffset( ) = pNetvarData[ iCommand % 150 ].vecViewOffset;
	pLocal->GetAimPunch( ) = pNetvarData[ iCommand % 150 ].vecAimPunchAngle;
	pLocal->GetAimPunchVelocity( ) = pNetvarData[ iCommand % 150 ].vecAimPunchAngleVel;
	pLocal->GetViewPunch( ) = pNetvarData[ iCommand % 150 ].vecViewPunchAngle;

	CBaseCombatWeapon* pWeapon = pLocal->GetWeapon( );
	if ( !pWeapon )
		return;

	static_cast< CWeaponCSBase* >( pWeapon )->GetRecoilIndex( ) = pNetvarData[ iCommand % 150 ].flRecoilIndex;
	static_cast< CWeaponCSBase* >( pWeapon )->GetAccuracyPenalty( ) = pNetvarData[ iCommand % 150 ].flAccuracyPenalty;
}

void Prediction::SaveViewmodelData(CBaseEntity* pLocal)
{
	if (!pLocal)
		return;

	CBaseViewModel* const hViewmodel = ( CBaseViewModel* )i::EntityList->GetClientEntityFromHandle( pLocal->GetViewModel( ) );

	if ( !hViewmodel )
		return;

	iAnimationParity = hViewmodel->iAnimationParity( );
	iSequence = hViewmodel->iSequence( );
	//flCycle = hViewmodel->flCycle( );
	//flAnimTime = hViewmodel->flAnimTime( );
}

void Prediction::AdjustViewmodelData(CBaseEntity* pLocal)
{
	if (!pLocal)
		return;

	CBaseViewModel* const hViewmodel = ( CBaseViewModel* )i::EntityList->GetClientEntityFromHandle( pLocal->GetViewModel( ) );

	if ( !hViewmodel )
		return;

	if ( iSequence != hViewmodel->iSequence( ) || iAnimationParity != hViewmodel->iAnimationParity( ) )
		return;

	//hViewmodel->flCycle( ) = flCycle;
	//hViewmodel->flAnimTime( ) = flAnimTime;
}