#pragma once

class IHudChat;
class CClientModeShared 
{
public:
	std::byte	pad0[ 0x1B ];
	void*		pViewport;
	IHudChat*	pChatElement;
	//HCursor		hCursorNone; compiler error, don't have time to look at this rn. Fix it yourself
	void*		pWeaponSelection;
	int			nRootSize[ 2 ];
};

class CEventInfo
{
public:
	enum
	{
		EVENT_INDEX_BITS = 8,
		EVENT_DATA_LEN_BITS = 11,
		MAX_EVENT_DATA = 192,
	};

	inline CEventInfo( )
	{
		iClassID = 0;
		flFireDelay = 0.0f;
		nFlags = 0;
		pSendTable = 0;
		pClientClass = 0;
		iPacked = 0;
	}

	short				iClassID{};
	short				pad{};
	float				flFireDelay{};
	const void*			pSendTable{};
	const CBaseClient*  pClientClass{};
	int					iPacked{};
	int					nFlags{};
	int					aFilters[ 8 ]{};
	CEventInfo*			pNext{};
};
