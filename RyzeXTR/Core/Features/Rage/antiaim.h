#pragma once
#include "../../SDK/CUserCmd.h"
#include "../../globals.h"
#include "../../SDK/Menu/config.h"

enum EPITCH : int {

	OFF,
	UP,
	ZERO,
	DOWN
};

enum EYAW : int {

	FORWARD,
	BACKWARD
};

enum EDESYNC : int {

	DISABLED,
	STATIC,
	EXTENDED,
	JITTER,
	FLICK,
};

enum AATYPE : int {

	STANDING,
	MOVING,
	INAIR
};

namespace antiaim {

	inline bool needMicromovement = false;
	inline bool bHideFlick = false;

	inline bool shotInvert = false;
	void AntiAim(CUserCmd*, bool&);
	
	enum LBYUpdateType : int
	{
		LBYUPDATE_None,
		LBYUPDATE_Standing,
		LBYUPDATE_Moving
	};

	bool ShouldDisableAntiaim(CUserCmd* pCmd, bool& bSendPacket);
	
	bool FreeStandingDistance(CUserCmd*, Vector&);
	bool FreeStandingThreat(Vector& angle);
    void InvertOnShoot(CUserCmd* pCmd);
    void DoRytter(CUserCmd* pCmd, int type);
	int ClosestToLocal();
	int ClosesToCrosshair();
	void AtTarget(CUserCmd*, Vector&);

	bool LBYUpdate(CBaseEntity* pLocal);

	void DoAntiaim(CUserCmd* pCmd, bool& bSendPacket, AATYPE);
	bool ManualAA(CUserCmd* pCmd, bool type);
}