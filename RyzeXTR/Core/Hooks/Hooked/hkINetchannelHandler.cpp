#include "../hooks.h"
#include "../../globals.h"
#include "../../Features/Networking/networking.h"	

void __fastcall h::hkPacketStart( void* ecx, void* edx, int sequence, int outgoing )
{
	static auto original = detour::packetStart.GetOriginal<decltype( &hkPacketStart )>( );

	if (packetManager.ShouldProcessPacketStart(outgoing))
		return original(ecx, edx, sequence, outgoing);
}

void __fastcall h::hkPacketEnd( void* ecx, void* edx )
{
	static auto original = detour::packetEnd.GetOriginal<decltype( &hkPacketEnd )>( );

	//int nCommandsAcknowledged = i::ClientState->iCommandAck - i::ClientState->iLastCommandAck;
	//util::Print(std::format("{} - {}", i::ClientState->iCommandAck, i::ClientState->iLastCommandAck).c_str());
	//if (nCommandsAcknowledged <= 0) {
	//	return original(ecx, edx);
	//}
	//
	//CClientState* ClientState = static_cast<CClientState*>(ecx);

	//if (ClientState->pNetChannel && !(ClientState->nChokedCommands % 4)) {

	//	const auto current_choke = ClientState->pNetChannel->iChokedPackets;
	//	ClientState->pNetChannel->iChokedPackets = 0;
	//	ClientState->pNetChannel->SendDatagram(0);
	//	--ClientState->pNetChannel->iOutSequenceNr;
	//	ClientState->pNetChannel->iChokedPackets = current_choke;
	//}

	networking.OnPacketEnd(static_cast<CClientState*>(ecx));
	return original(ecx, edx);
}

bool __fastcall h::hkTemptEntities( void* ecx, void* edx, void* msg )
{
	static auto original = detour::temptEntities.GetOriginal<decltype( &hkTemptEntities )>( );

	int iBackup = i::GlobalVars->nMaxClients;

	i::GlobalVars->nMaxClients = 1;
	bool bResult = original(ecx, edx, msg);
	i::GlobalVars->nMaxClients = iBackup;

	return bResult;

	if (!g::pLocal || !i::EngineClient->IsInGame())
		return original( ecx, edx, msg);

	// filtering events
	if ( !g::pLocal->IsAlive( ) )
		return original(ecx, edx, msg);

	CEventInfo* pEventInfo = i::ClientState->pEvents; //0x4DEC
	CEventInfo* pNextEvent = nullptr;

	if ( !pEventInfo )
		return original(ecx, edx, msg);

	do
	{
		pNextEvent = pEventInfo->pNext;

		int classID = pEventInfo->iClassID - 1;

		auto m_pCreateEventFn = pEventInfo->pClientClass->pCreateEventFn;
		if ( !m_pCreateEventFn )
			continue;

		IClientNetworkable* pCE = m_pCreateEventFn( );
		if ( !pCE )
			continue;

		if ( classID == ( int )EClassIndex::CTEFireBullets )
		{
			// set fire_delay to zero to send out event so its not here later.
			pEventInfo->flFireDelay = 0.0f;
		}
		pEventInfo = pNextEvent;
	} 
	while ( pNextEvent != nullptr );

	return original(ecx, edx, msg);
}
