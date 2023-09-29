#pragma once
#include "../Classes/CClientState.h"
#include "../../SDK/DataTyes/Vector.h"
#include "../../SDK/DataTyes/Matrix.h"
#include "../../SDK/CRC32.h"

enum EStage : int32_t
{
	FRAME_UNDEFINED = -1,
	FRAME_START,

	// a network packet is being recieved
	FRAME_NET_UPDATE_START,
	// data has been received and we are going to start calling postdataupdate
	FRAME_NET_UPDATE_POSTDATAUPDATE_START,
	// data has been received and called postdataupdate on all data recipients
	FRAME_NET_UPDATE_POSTDATAUPDATE_END,
	// received all packets, we can now do interpolation, prediction, etc
	FRAME_NET_UPDATE_END,
	// start rendering the scene
	FRAME_RENDER_START,
	// finished rendering the scene

	FRAME_RENDER_END,
	FRAME_NET_FULL_FRAME_UPDATE_ON_REMOVE
};

struct PlayerInfo_t
{
	std::uint64_t	ullVersion = 0ULL;
	union
	{
		std::uint64_t ullXuid;
		struct
		{
			std::uint32_t nXuidLow;
			std::uint32_t nXuidHigh;
		};
	};

	char			szName[128];
	int				nUserID;
	char			szSteamID[33];
	std::uint32_t	nFriendsID;
	char			szFriendsName[128];
	bool			bFakePlayer;
	bool			bIsHLTV;
	CRC32_t			uCustomFiles[4];
	std::uint8_t	dFilesDownloaded;
};

// https://github.com/perilouswithadollarsign/cstrike15_src/blob/f82112a2388b841d72cb62ca48ab1846dfcc11c8/public/cdll_int.h#L298
class IVEngineClient {

public:

	INetChannelInfo* GetNetChannelInfo() {

		return util::CallVFunc<INetChannelInfo*>(this, 78);
	}

	int GetLocalPlayer() {

		return util::CallVFunc<int>(this, 12);
	}

	void GetScreenSize(int& iWidth, int& iHeight) {

		util::CallVFunc<void>(this, 5, std::ref(iWidth), std::ref(iHeight));
	}

	bool GetPlayerInfo(int nEntityIndex, PlayerInfo_t* pInfo) {

		return util::CallVFunc<bool>(this, 8, nEntityIndex, pInfo);
	}

	int GetPlayerForUserID(int iUserID) {

		return util::CallVFunc<int>(this, 9, iUserID);
	}

	void GetViewAngles(Vector& angView){

		util::CallVFunc<void>(this, 18, std::ref(angView));
	}

	void SetViewAngles(Vector& angView){

		util::CallVFunc<void>(this, 19, std::ref(angView));
	}

	int GetMaxClients() {

		return util::CallVFunc<int>(this, 20);
	}

	bool IsInGame(){

		return util::CallVFunc<bool>(this, 26);
	}

	bool IsConnected() {

		return util::CallVFunc<bool>(this, 27);
	}

	void* GetBSPTreeQuery() {

		return util::CallVFunc<void*>(this, 43);
	}

	void* FireEvents( )
	{
		return util::CallVFunc<void*>( this, 59 );
	}

	bool IsHLTV() {

		return util::CallVFunc<bool>(this, 93);
	}

	void ExecuteClientCmd(const char* szCmdString) {

		util::CallVFunc<void>(this, 108, szCmdString);
	}

	void ClientCmdUnrestricted(const char* szCmdString, bool bFromConsoleOrKeybind = false) {

		util::CallVFunc<void>(this, 114, szCmdString, bFromConsoleOrKeybind);
	}

	bool IsVoiceRecording() {

		return util::CallVFunc<bool>(this, 224);
	}

	const ViewMatrix_t& WorldToScreenMatrix()
	{
		return util::CallVFunc<const ViewMatrix_t&>(this, 37);
	}
};