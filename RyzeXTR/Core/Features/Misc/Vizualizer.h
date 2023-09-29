#pragma once
/* Used: flFrom, flTo, flDivider */
#include "../../SDK/Menu/config.h"

/* Used: ISurface */
#include "../../Interface/interfaces.h"

/* Used: CBaseEntity */
#include "../../SDK/Entity.h"

namespace vizualizer {

	inline bool bDoEffect{};

	void Initialize();
	void Draw(CBaseEntity* pLocal);
}