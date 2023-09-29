#include "serversounds.h"
#include "../Visuals/ESP.h"

void ServerSounds::Start()
{
	vecCurSoundList.RemoveAll();
	i::EngineSoundClient->GetActiveSounds(vecCurSoundList);

	// No active sounds.
	if (!vecCurSoundList.Count())
		return;

	// Accumulate sounds for esp correction
	for (int iter = 0; iter < vecCurSoundList.Count(); iter++)
	{
		SoundInfo_t& sound = vecCurSoundList[iter];
		if (sound.m_nSoundSource == 0 || // World
			sound.m_nSoundSource > 64)   // Most likely invalid
			continue;

		CBaseEntity* player = (CBaseEntity*)i::EntityList->GetClientEntity(sound.m_nSoundSource);

		if (!player || player == g::pLocal || sound.m_vecOrigin->IsZero())
			continue;

		if (!ValidSound(sound))
			continue;

		/*
			Missing scoreboard life/team checks.
		*/

		SetupAdjustPlayer(player, sound);

		arrSoundPlayers[sound.m_nSoundSource].Override(sound);
		visual::vecDormatPosition[sound.m_nSoundSource] = *sound.m_vecOrigin;
	}

	for (int iter = 1; iter < i::EntityList->GetHighestEntityIndex(); iter++)
	{
		CBaseEntity* player = (CBaseEntity*)i::EntityList->GetClientEntity(iter);
		if (!player || !player->IsDormant() || !player->IsAlive())
			continue;

		AdjustPlayerBegin(player);
	}

	vecSoundBuffer = vecCurSoundList;
}

void ServerSounds::SetupAdjustPlayer(CBaseEntity* player, SoundInfo_t& sound)
{
	Vector src3D, dst3D;
	Trace_t tr;
	CTraceFilter filter(player);
	src3D = (*sound.m_vecOrigin) + Vector(0, 0, 1); // So they dont dig into ground incase shit happens /shrug
	dst3D = src3D - Vector(0, 0, 100);
	Ray_t ray(src3D, dst3D);

	i::EngineTrace->TraceRay(ray, MASK_PLAYERSOLID, &filter, &tr);

	// step = (tr.fraction < 0.20)
	// shot = (tr.fraction > 0.20)
	// stand = (tr.fraction > 0.50)
	// crouch = (tr.fraction < 0.50)

	/* Corrects origin and important flags. */

	// Player stuck, idk how this happened
	if (tr.bAllSolid)
	{
		arrSoundPlayers[sound.m_nSoundSource].iReceiveTime = -1;
	}

	*sound.m_vecOrigin = ((tr.flFraction < 0.97) ? tr.vecEnd : *sound.m_vecOrigin);
	arrSoundPlayers[sound.m_nSoundSource].iFlags = player->GetFlags();
	arrSoundPlayers[sound.m_nSoundSource].iFlags |= (tr.flFraction < 0.50f ? FL_DUCKING : 0) | (tr.flFraction != 1 ? FL_ONGROUND : 0);   // Turn flags on
	arrSoundPlayers[sound.m_nSoundSource].iFlags &= (tr.flFraction > 0.50f ? ~FL_DUCKING : 0) | (tr.flFraction == 1 ? ~FL_ONGROUND : 0); // Turn flags off
}

void ServerSounds::Finish()
{
	// Do any finishing code here. If we add smtn like sonar radar this will be useful.

	AdjustPlayerFinish();
}

void ServerSounds::AdjustPlayerFinish()
{
	// Restore and clear saved players for next loop.
	for (auto& RestorePlayer : vecRestorePlayers)
	{
		auto player = RestorePlayer.pEntity;
		player->GetFlags() = RestorePlayer.iFlags;
		player->GetVecOrigin() = RestorePlayer.vecOrigin;
		player->SetAbsOrigin(RestorePlayer.vecAbsOrigin);
		//*(bool*)((DWORD)player + 0xED) = RestorePlayer.bDormant; // dormant check
	}
	vecRestorePlayers.clear();
}

void ServerSounds::AdjustPlayerBegin(CBaseEntity* player)
{
	// Adjusts player's origin and other vars so we can show full-ish esp.
	//if (!g_Options.esp_farther)
	//	return;

	constexpr int EXPIRE_DURATION = 450; // miliseconds-ish?
	auto& sound_player = arrSoundPlayers[player->EntIndex()];
	bool sound_expired = GetTickCount() - sound_player.iReceiveTime > EXPIRE_DURATION;
	if (sound_expired /*&& Visuals::ESP_Fade[player->EntIndex()] == 0.f*/)
		return;

	SoundPlayer current_player;
	current_player.pEntity = player;
	current_player.bDormant = true;
	current_player.iFlags = player->GetFlags();
	current_player.vecOrigin = player->GetVecOrigin();
	current_player.vecAbsOrigin = player->GetAbsOrigin();
	vecRestorePlayers.emplace_back(current_player);

	//if (!sound_expired)
	//	*(bool*)((DWORD)player + 0xED) = false; // dormant check
	player->GetFlags() = sound_player.iFlags;
	player->GetVecOrigin() = sound_player.vecOrigin;
	player->SetAbsOrigin(sound_player.vecOrigin);
}

bool ServerSounds::ValidSound(SoundInfo_t& sound)
{
	// Use only server dispatched sounds.
	//if (!sound.m_bFromServer)
	//	return false;

	// We don't want the sound to keep following client's predicted origin.
	for (int iter = 0; iter < vecSoundBuffer.Count(); iter++)
	{
		SoundInfo_t& cached_sound = vecSoundBuffer[iter];
		if (cached_sound.m_nGuid == sound.m_nGuid)
		{
			return false;
		}
	}

	return true;
}