#pragma once
#include "../../SDK/DataTyes/Vector.h"
#include "../../Interface/Classes/CClientModeShared.h"
#define FLOW_OUTGOING	0
#define FLOW_INCOMING	1
#define MAX_FLOWS		2		// in & out

#define SIGNONSTATE_NONE        0    // no state yet, about to connect
#define SIGNONSTATE_CHALLENGE    1    // client challenging server, all OOB packets
#define SIGNONSTATE_CONNECTED    2    // client is connected to server, netchans ready
#define SIGNONSTATE_NEW            3    // just got serverinfo and string tables
#define SIGNONSTATE_PRESPAWN    4    // received signon buffers
#define SIGNONSTATE_SPAWN        5    // ready to receive entity packets
#define SIGNONSTATE_FULL        6    // we are fully connected, first non-delta packet received (in-game check)
#define SIGNONSTATE_CHANGELEVEL    7    // server is changing level, please wait

class bf_write
{
public:
	bf_write() = default;

	// the current buffer
	unsigned long* pData;
	int				nDataBytes;
	int				nDataBits;

	// where we are in the buffer
	int				iCurrentBit;
};

class bf_read
{
public:
	std::uintptr_t uBaseAddress;
	std::uintptr_t uCurrentOffset;

	bf_read(std::uintptr_t uAddress) : uBaseAddress(uAddress), uCurrentOffset(0U) {}

	void SetOffset(std::uintptr_t uOffset)
	{
		uCurrentOffset = uOffset;
	}

	void Skip(std::uintptr_t uLength)
	{
		uCurrentOffset += uLength;
	}

	int ReadByte()
	{
		char dValue = *reinterpret_cast<char*>(uBaseAddress + uCurrentOffset);
		++uCurrentOffset;
		return dValue;
	}

	bool ReadBool()
	{
		bool bValue = *reinterpret_cast<bool*>(uBaseAddress + uCurrentOffset);
		++uCurrentOffset;
		return bValue;
	}

	const char* ReadString()
	{
		char szBuffer[256];
		char chLength = *reinterpret_cast<char*>(uBaseAddress + uCurrentOffset);
		++uCurrentOffset;
		memcpy(szBuffer, reinterpret_cast<void*>(uBaseAddress + uCurrentOffset), chLength > 255 ? 255 : chLength);
		szBuffer[chLength > 255 ? 255 : chLength] = '\0';
		uCurrentOffset += chLength + 1;
		return szBuffer;
	}
};

class INetChannelInfo
{
public:
	enum
	{
		GENERIC = 0,	// must be first and is default group
		LOCALPLAYER,	// bytes for local player entity update
		OTHERPLAYERS,	// bytes for other players update
		ENTITIES,		// all other entity bytes
		SOUNDS,			// game sounds
		EVENTS,			// event messages
		TEMPENTS,		// temp entities
		USERMESSAGES,	// user messages
		ENTMESSAGES,	// entity messages
		VOICE,			// voice data
		STRINGTABLE,	// a stringtable update
		MOVE,			// client move cmds
		STRINGCMD,		// string command
		SIGNON,			// various signondata
		PAINTMAP,		// paintmap data
		ENCRYPTED,		// encrypted data
		TOTAL,			// must be last and is not a real group
	};

	virtual const char* GetName() const = 0;			// get channel name
	virtual const char* GetAddress() const = 0;			// get channel IP address as string
	virtual float		GetTime() const = 0;			// current net time
	virtual float		GetTimeConnected() const = 0;	// get connection time in seconds
	virtual int			GetBufferSize() const = 0;		// netchannel packet history size
	virtual int			GetDataRate() const = 0;		// send data rate in byte/sec

	virtual bool		IsLoopback() const = 0;			// true if loopback channel
	virtual bool		IsTimingOut() const = 0;		// true if timing out
	virtual bool		IsPlayback() const = 0;			// true if demo playback
	virtual float		GetLatency(int iFlow) const = 0; // current latency (RTT), more accurate but jittering
	virtual float		GetAvgLatency(int iFlow) const = 0; // average packet latency in seconds
	virtual float		GetAvgLoss(int iFlow) const = 0; // avg packet loss[0..1]
	virtual float		GetAvgChoke(int iFlow) const = 0; // avg packet choke[0..1]
	virtual float		GetAvgData(int iFlow) const = 0; // data flow in bytes/sec
	virtual float		GetAvgPackets(int iFlow) const = 0; // avg packets/sec
	virtual int			GetTotalData(int iFlow) const = 0; // total flow in/out in bytes
	virtual int			GetTotalPackets(int iFlow) const = 0;
	virtual int			GetSequenceNr(int iFlow) const = 0; // last send seq number
	virtual bool		IsValidPacket(int iFlow, int nFrame) const = 0; // true if packet was not lost/dropped/chocked/flushed
	virtual float		GetPacketTime(int iFlow, int nFrame) const = 0; // time when packet was send
	virtual int			GetPacketBytes(int iFlow, int nFrame, int group) const = 0; // group size of this packet
	virtual bool		GetStreamProgress(int iFlow, int* piReceived, int* piTotal) const = 0; // TCP progress if transmitting
	virtual float		GetTimeSinceLastReceived() const = 0; // get time since last received packet in seconds
	virtual	float		GetCommandInterpolationAmount(int iFlow, int nFrame) const = 0;
	virtual void		GetPacketResponseLatency(int iFlow, int frame_number, int* pnLatencyMsecs, int* pnChoke) const = 0;
	virtual void		GetRemoteFramerate(float* pflFrameTime, float* pflFrameTimeStdDeviation, float* pflFrameStartTimeStdDeviation) const = 0;
	virtual float		GetTimeoutSeconds() const = 0;
};

class INetChannel
{
public:
	std::byte	pad0[0x14];				//0x0000
	bool		bProcessingMessages;	//0x0014
	bool		bShouldDelete;			//0x0015
	bool		bStopProcessing;		//0x0016
	std::byte	pad1[0x1];				//0x0017
	int			iOutSequenceNr;			//0x0018 last send outgoing sequence number
	int			iInSequenceNr;			//0x001C last received incoming sequence number
	int			iOutSequenceNrAck;		//0x0020 last received acknowledge outgoing sequence number
	int			iOutReliableState;		//0x0024 state of outgoing reliable data (0/1) flip flop used for loss detection
	int			iInReliableState;		//0x0028 state of incoming reliable data
	int			iChokedPackets;			//0x002C number of choked packets
	std::byte	pad2[0x414];			//0x0030

	int	SendDatagram(bf_write* pDatagram)
	{
		return util::CallVFunc<int>(this, 46, pDatagram);
	}

	bool Transmit(bool bOnlyReliable = false)
	{
		return util::CallVFunc<bool>(this, 49, bOnlyReliable);
	}
}; // Size: 0x0444

// @credits: https://github.com/ValveSoftware/source-sdk-2013/blob/master/sp/src/public/inetmessage.h
class INetMessage
{
public:
	virtual					~INetMessage() { }
	virtual void			SetNetChannel(void* pNetChannel) = 0;
	virtual void			SetReliable(bool bState) = 0;
	virtual bool			Process() = 0;
	virtual	bool			ReadFromBuffer(bf_read& buffer) = 0;
	virtual	bool			WriteToBuffer(bf_write& buffer) = 0;
	virtual bool			IsReliable() const = 0;
	virtual int				GetType() const = 0;
	virtual int				GetGroup() const = 0;
	virtual const char*		GetName() const = 0;
	virtual INetChannel*	GetNetChannel() const = 0;
	virtual const char*		ToString() const = 0;
	virtual std::size_t		GetSize() const = 0;
};

class CCLCMsg_Move
{
private:
	std::byte pad0[0x8];
public:
	int nBackupCommands;
	int nNewCommands;
};

template <typename T>
class CNetMessagePB : public INetMessage, public T { };
using CCLCMsg_Move_t = CNetMessagePB<CCLCMsg_Move>;

// @credits: https://github.com/ValveSoftware/source-sdk-2013/blob/master/mp/src/public/inetmsghandler.h
class INetChannelHandler
{
public:
	virtual			~INetChannelHandler() { }
	virtual void	ConnectionStart(INetChannel* pChannel) = 0; // called first time network channel is established
	virtual void	ConnectionStop() = 0; // called first time network channel is established
	virtual void	ConnectionClosing(const char* szReason) = 0; // network channel is being closed by remote site
	virtual void	ConnectionCrashed(const char* szReason) = 0; // network error occured
	virtual void	PacketStart(int iIncomingSequence, int iOutgoingAcknowledged) = 0;	// called each time a new packet arrived
	virtual void	PacketEnd() = 0; // all messages has been parsed
	virtual void	FileRequested(const char* szFileName, unsigned int uTransferID, bool bReplayDemoFile) = 0; // other side request a file for download
	virtual void	FileReceived(const char* szFileName, unsigned int uTransferID, bool bReplayDemoFile) = 0; // we received a file
	virtual void	FileDenied(const char* szFileName, unsigned int uTransferID, bool bReplayDemoFile) = 0; // a file request was denied by other side
	virtual void	FileSent(const char* szFileName, unsigned int uTransferID, bool bReplayDemoFile) = 0; // we sent a file
	virtual bool	ChangeSplitscreenUser(int nSplitScreenUserSlot) = 0; // interleaved networking used by SS system is changing the SS player slot that the subsequent messages pertain to
};

class CClockDriftMgr
{
public:
	float flClockOffsets[ 17 ]; //0x0000
	uint32_t iCurClockOffset; //0x0044
	uint32_t nServerTick;     //0x0048
	uint32_t nClientTick;     //0x004C
}; //Size: 0x0050

class CClientState {
public:
	std::byte		pad0[0x9C];				// 0x0000
	INetChannel* pNetChannel;			// 0x009C
	int				iChallengeNr;			// 0x00A0
	std::byte		pad1[0x64];				// 0x00A4
	int				iSignonState;			// 0x0108
	std::byte		pad2[0x8];				// 0x010C
	float			flNextCmdTime;			// 0x0114
	int				nServerCount;			// 0x0118
	int				iCurrentSequence;		// 0x011C
	char			pad_0120[ 0x4 ];               //0x0120
	CClockDriftMgr	clockDriftMgr; //0x0128
	int				iDeltaTick;				// 0x0174
	bool			bPaused;				// 0x0178
	std::byte		pad4[0x7];				// 0x0179
	int				iViewEntity;			// 0x0180
	int				iPlayerSlot;			// 0x0184
	char			szLevelName[MAX_PATH];	// 0x0188
	char			szLevelNameShort[80];	// 0x028C
	char			szMapGroupName[80];		// 0x02DC
	char			szLastLevelNameShort[80]; // 0x032C
	std::byte		pad5[0xC];				// 0x037C
	int				nMaxClients;			// 0x0388 
	std::byte		pad6[0x498C];			// 0x038C
	float			flLastServerTickTime;	// 0x4D18
	bool			bInSimulation;			// 0x4D1C
	std::byte		pad7[0x3];				// 0x4D1D
	int				iOldTickcount;			// 0x4D20
	float			flTickRemainder;		// 0x4D24
	float			flFrameTime;			// 0x4D28
	int				iLastOutgoingCommand;	// 0x4D2C
	int				nChokedCommands;		// 0x4D30
	int				iLastCommandAck;		// 0x4D34
	int				iCommandAck;			// 0x4D38
	int				iSoundSequence;			// 0x4D3C
	bool			bIsHltv;				// lmao
	std::byte		pad8[0x4F];				// 0x4D40
	Vector			angViewPoint;			// 0x4D90
	std::byte		pad9[0xD0];				// 0x4D9C
	CEventInfo*		pEvents;				// 0x4E6C
};