#pragma once
#include <deque>
#include "SDK/CUserCmd.h"
#include "SDK/Entity.h"
#include "Interface/Interfaces/entityListener.h"

#define _DEBUGTEXT "Cride Is A Nigger :kekw:"

namespace g {

	inline CUserCmd* pCmd;
	inline CBaseEntity* pLocal;
	inline Vector vecOriginalViewAngle;
	inline Vector vecEyePosition;

	inline float flVelocityModifier;

	inline float flColorBuffer[4];

	inline bool bAllowAnimations[65];
	inline bool bUpdatingSkins = false;
	inline bool bStartWelcome = false;
	inline bool* bSendPacket = nullptr;

	inline bool bSettingUpBones[65];

	inline std::deque<Vector> drawList{};

	namespace fonts {
		inline HFont HealthESP;
		inline HFont NameESP;
		inline HFont FlagESP;
		inline HFont SkeetFont;
		inline HFont DebugFont;
	}
}