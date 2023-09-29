#pragma once
#include "../../SDK/Entity.h"
#include "../../globals.h"

class ServerSounds {

public:
	void Start();
	void Finish();

private:

	void AdjustPlayerBegin(CBaseEntity*);
	void AdjustPlayerFinish();
	void SetupAdjustPlayer(CBaseEntity*, SoundInfo_t&);

	bool ValidSound(SoundInfo_t&);

	struct SoundPlayer
	{
		void Override(SoundInfo_t& pSound) {
			iIndex = pSound.m_nSoundSource;
			vecOrigin = *pSound.m_vecOrigin;
			iReceiveTime = GetTickCount()/*i::GlobalVars->iTickCount*/;
		}

		int iIndex = 0;
		int iReceiveTime = 0;
		Vector vecOrigin = Vector(0, 0, 0);
		/* Restore data */
		int iFlags = 0;
		CBaseEntity* pEntity = nullptr;
		Vector vecAbsOrigin = Vector(0, 0, 0);
		bool bDormant = false;
	} arrSoundPlayers[64];

	CUtlVector<SoundInfo_t> vecSoundBuffer;
	CUtlVector<SoundInfo_t> vecCurSoundList;
	std::vector<SoundPlayer> vecRestorePlayers;
};
inline ServerSounds serversound;