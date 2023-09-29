#pragma once
#include <deque>
#include "../../SDK/entity.h"
#include "../../Interface/interfaces.h"
#include "../../globals.h"
#include "../Misc/enginepred.h"

struct networkData_t {

	int iTickRate;
	int iMaxChoke;
	int iSequence;
	int iServerTick;
	int iCompensatedServerTick;

	bool bSkipDataGram;

	float flLatency;
};

class CNetworking
{
public:
	void OnPacketEnd( CClientState* pClientState );
	void SaveNetvarData( int iCommand );
	void RestoreNetvarData( int iCommand );

	int GetServerTick();
	int GetCorrectedTickbase();

    CNetvarData* GetRecord();

	void StartNetworking();
	void FinishNetworking();

	int LagcompensatedTicks;

	void ResetNetwork();
	networkData_t data;

private:
	std::array < CNetvarData, 150 > pCompressData = { };
	int iLastCommandNumber;
};
inline CNetworking networking;

class CPacketManager
{
public:
	bool ShouldProcessPacketStart( int iCommand );
	std::deque<int> pCommandList;
};
inline CPacketManager packetManager;