#pragma once

#include <string>
#include <Windows.h>
#include "bass.h"

static BOOL BASS_INIT_ONCE()
{
	if (HIWORD(BASS_GetVersion()) != BASSVERSION)
		return FALSE;

	if (!BASS_Init(-1, 44100, 0, nullptr, nullptr))
		return FALSE;

	BASS_SetConfig(BASS_CONFIG_NET_PLAYLIST, 1);
	BASS_SetConfig(BASS_CONFIG_NET_PREBUF, 0);

	return TRUE;
}

static void CALLBACK StatusProc(const void *buffer, DWORD length, void *user)
{
	if (buffer && !length && (DWORD) user == BASS::request_number) // got HTTP/ICY tags, and this is still the current request
	{
		//printf( "%s\n", buffer );
	}
	//( 32, WM_SETTEXT, 0, buffer ); // display status
}

static VOID BASS_OPEN_STREAM(const char* pszUrl)
{
	DWORD r = ++BASS::request_number;
	BASS_StreamFree(BASS::stream_handle);
	DWORD c = BASS_StreamCreateURL(pszUrl,
								   0, BASS_STREAM_BLOCK | BASS_STREAM_STATUS | BASS_STREAM_AUTOFREE, StatusProc, (void*) r); // open URL
	if (r != BASS::request_number) {
		if (c) BASS_StreamFree(c);
		return;
	}
	BASS::stream_handle = c;
}

static void DoMeta()
{
	const char *meta = BASS_ChannelGetTags(BASS::stream_handle, BASS_TAG_META);
	if (meta) { // got Shoutcast metadata
		const char *p = strstr(meta, XorStr("StreamTitle='")); // locate the title
		if (p) {
			const char *p2 = strstr(p, "';"); // locate the end of it
			if (p2) {
				char *t = _strdup(p + 13);
				t[p2 - (p + 13)] = 0;
				strcpy_s(BASS::bass_metadata, t);
				free(t);
			}
		}
	}
	else {
		meta = BASS_ChannelGetTags(BASS::stream_handle, BASS_TAG_OGG);
		if (meta) { // got Icecast/OGG tags
			const char *artist = NULL, *title = NULL, *p = meta;
			for (; *p; p += strlen(p) + 1) {
				if (!_strnicmp(p, XorStr("artist="), 7)) // found the artist
					artist = p + 7;
				if (!_strnicmp(p, XorStr("title="), 6)) // found the title
					title = p + 6;
			}
			if (title) {
				if (artist) {
					char text[100];
					_snprintf_s(text, sizeof(text), "%s - %s", artist, title);
					strcpy_s(BASS::bass_metadata, text);
				}
				else
					strcpy_s(BASS::bass_metadata, title);
			}
		}
	}
}

static void CALLBACK MetaSync(HSYNC handle, DWORD channel, DWORD data, void *user)
{
	DoMeta();
}

static void CALLBACK EndSync(HSYNC handle, DWORD channel, DWORD data, void *user)
{
}

static std::string string_after_delim(std::string const& str, std::string const& delim)
{
	return str.substr(str.find(delim) + delim.size());
}

static VOID BASS_PLAY_STREAM()
{
	QWORD progress = BASS_StreamGetFilePosition(BASS::stream_handle, BASS_FILEPOS_BUFFER)
		* 100 / BASS_StreamGetFilePosition(BASS::stream_handle, BASS_FILEPOS_END); // percentage of buffer filled
	if (progress > 75 || !BASS_StreamGetFilePosition(BASS::stream_handle, BASS_FILEPOS_CONNECTED)) // over 75% full (or end of download)
	{
		// get the broadcast name and URL
		const char *icy = BASS_ChannelGetTags(BASS::stream_handle, BASS_TAG_ICY);
		if (!icy) icy = BASS_ChannelGetTags(BASS::stream_handle, BASS_TAG_HTTP); // no ICY tags, try HTTP
		if (icy)
		{
			for (; *icy; icy += strlen(icy) + 1)
			{
				if (!_strnicmp(icy, XorStr("icy-name:"), 9))
				{
					strcpy_s(BASS::bass_channelinfo, string_after_delim(icy, "icy - name:").c_str());
				}
				if (!_strnicmp(icy, XorStr("icy-url:"), 8))
				{
					//strcpy_s( BASS::bass_channelinfo, string_after_delim( icy, "icy-url:" ).c_str() );
				}
			}
		}
		//printf( "playing...\n" )
		DoMeta();
		BASS_ChannelSetSync(BASS::stream_handle, BASS_SYNC_META, 0, &MetaSync, 0); // Shoutcast
		BASS_ChannelSetSync(BASS::stream_handle, BASS_SYNC_OGG_CHANGE, 0, &MetaSync, 0); // Icecast/OGG
		BASS_ChannelSetSync(BASS::stream_handle, BASS_SYNC_END, 0, &EndSync, 0);
		// play it!
		BASS_ChannelPlay(BASS::stream_handle, FALSE);
	}
	else
	{
		//printf( "buffering... %d%%", progress );
	}
}

static VOID BASS_STOP_STREAM()
{
	BASS_Pause();
}

static VOID BASS_START_STREAM()
{
	BASS_Start();
}

static VOID BASS_SET_VOLUME(DWORD channel, float cvar_volume)
{
	/*
	static float old_volume = 0.0f;
	if ( old_volume != cvar_volume )
	{
		old_volume = cvar_volume;
	}
	else
	{
		return;
	}
	*/
	BASS_ChannelSetAttribute(channel, BASS_ATTRIB_VOL, cvar_volume);
}

static VOID BASS_DESTROY()
{
	BASS_Free();
}