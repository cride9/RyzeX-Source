#include "misc.h"
#include <fstream>
#include "../../SDK/Menu/config.h"
#include "../../SDK/math.h"
#include "../Rage/exploits.h"
#include "../Rage/antiaim.h"
#include "../../SDK/WavParser.h"
#include "../Rage/autowall.h"
#include "../Visuals/ESP.h"
#include "../Networking/networking.h"
#include "../../Interface/Classes/CCSGameRulesProxy.h"
#pragma comment(lib, "winmm.lib")

#include "../../SDK/InputSystem.h"

#include "../../../Dependecies/BASS/API.h"
#include "../../../Dependecies/BASS/string_obfuscation.h"
#include "../../SDK/Menu/gui.h"
#include "Playerlist.h"
#include "../Rage/Animations/EnemyAnimations.h"

void misc::SetupRadio( )
{
	BASS::bass_lib_handle = BASS::bass_lib.LoadFromMemory( bass_dll_image, sizeof( bass_dll_image ) );

	if ( BASS_INIT_ONCE( ) )
		BASS::bass_init = TRUE;

	static bool m_bBassNeedsReinit = false;

	static std::pair<std::string, char> channels[ ] =
	{
		__( XorStr("http://radio.infowaste.xyz:8010/radio.mp3") ),					  // Gabber
		__(XorStr("http://kathy.torontocast.com:3140/stream")),                     // Rock
		__(XorStr("http://radiosputnik.nl:8002/sputnik")),                          // Techno
		__(XorStr("http://64.20.39.8:8071/stream")),                                // Rap
		__(XorStr("http://radio4.vip-radios.fm:8020/stream128k-AAC-Chill_autodj")), // Chill
		__(XorStr("http://mp3.stream.tb-group.fm/clt.mp3")),                        // Club
		__(XorStr("http://mp3.stream.tb-group.fm/ht.mp3")),                         // House
		__(XorStr("http://69.195.153.34/cvgm192")),                                 // 8-Bit
		__(XorStr("http://8bit.fm:8000/live")),                                     // 8-Bit Alternative
		__(XorStr("http://ec2.yesstreaming.net:1910/stream")),                      // Lo-Fi
		__(XorStr("http://eurobeat.stream.laut.fm/eurobeat")),					  // Eurobeat
		__(XorStr("https://nightcore-berlin.stream.laut.fm/nightcore-berlin")),     // Nightcore
		__(XorStr("https://icast.connectmedia.hu/5202/live.mp3")),					  // Radio 1
		__(XorStr("http://stream-158.zeno.fm/71ntub27u18uv?zs=R4yvq6kaRPyekzJdUwP1VA")) // Phonk radio
	};

	bool bShouldDisable = false;

	while (!bShouldDisable)
	{
#ifdef NDEBUG
		if (cfg::bDoUnload)
			bShouldDisable = true;
#endif

#ifdef _DEBUG
		if (GetAsyncKeyState(VK_DELETE) || cfg::bDoUnload)
			bShouldDisable = true;
#endif
		std::this_thread::sleep_for( std::chrono::milliseconds( 100 ) );
		const auto desired_channel = cfg::misc::iRadioStation;

		if ( BASS::bass_init && desired_channel )
		{
			static auto current_channel = 0;

			if ( current_channel != desired_channel || m_bBassNeedsReinit )
			{
				m_bBassNeedsReinit = false;
				BASS_Start( );
				_rt( channel, channels[ desired_channel ] );
				BASS_OPEN_STREAM( channel );
				current_channel = desired_channel;
			}

			BASS_SET_VOLUME( BASS::stream_handle,
				IPT::HandleInput( cfg::misc::iRadioMuteHotKey ) || !cfg::misc::bEnableRadio
				? 0.f
				: cfg::misc::flRadioVolume / 200.f );
			BASS_PLAY_STREAM( );
		}
		else if ( BASS::bass_init )
		{
			m_bBassNeedsReinit = true;
			BASS_StreamFree( BASS::stream_handle );
		}
	}
}

void misc::CreateMove(CUserCmd* pCmd, Vector& vecViewAngle,bool& bSendPacket) {

	BunnyHop(pCmd);
	FakeLag(bSendPacket);
	AutoStrafe(vecViewAngle, pCmd);
	AspectRatio();
	Slowwalk(pCmd, cfg::antiaim::iFakeWalkSpeed); // need menu element && keybind
	FastStop(pCmd);
	FakeDuck(pCmd);
	SlideFix();
	BlockBot(pCmd);
	MoveToPosition();
	OnlyCheatLogs();
	RemovePostProcessing();
	FixScopeSens();
	ClanTag();
	LeftHandKnife();
	FogOptions();
	PreserveKillfeed(nullptr);

	g::pLocal->GetModelScale() = cfg::misc::bSkinnyBoy ? cfg::misc::iSkinnyBoy * 0.01f : 1.f;
	if (cfg::misc::bInfiniteDuck)
		pCmd->iButtons |= IN_BULLRUSH;
	RemoveShadows();
#if NDEBUG || ALPHA
	Security();
#endif
	//ViewModel();
}

void misc::SkyboxChanger() {

	static std::string szSkyboxes[] = {
	"",
XorStr("cs_baggage_skybox_"),
	XorStr("cs_tibet"),
	XorStr("vietnam"),
	XorStr("sky_lunacy"),
	XorStr("embassy"),
	XorStr("italy"),
	XorStr("jungle"),
	XorStr("office"),
	XorStr("sky_cs15_daylight01_hdr"),
	XorStr("sky_cs15_daylight02_hdr"),
	XorStr("sky_cs15_daylight03_hdr"),
	XorStr("sky_cs15_daylight04_hdr"),
	XorStr("sky_day02_05"),
	XorStr("nukeblank"),
	XorStr("dustblank"),
	XorStr("sky_venice"),
	XorStr("sky_csgo_cloudy01"),
	XorStr("sky_csgo_night02"),
	XorStr("sky_csgo_night02b"),
	XorStr("vertigo"),
	XorStr("vertigoblue_hdr"),
	XorStr("sky_dust"),
	XorStr("sky_hr_aztec")
	};

	static bool bRefreshNewGame = false;
	if (i::ClientState->iSignonState != SIGNONSTATE_FULL || !g::pLocal) {
		bRefreshNewGame = true;
		return;
	}

	static CConVar* sv_skyname = i::ConVar->FindVar(XorStr("sv_skyname"));
	static std::string szBackup;
	if (cfg::misc::iSkybox == 0) {

		util::SetSkybox(sv_skyname->GetString());
	}
	else if (szBackup.c_str() != szSkyboxes[cfg::misc::iSkybox] || bRefreshNewGame) {

		util::SetSkybox(szSkyboxes[cfg::misc::iSkybox].c_str());
		szBackup = szSkyboxes[cfg::misc::iSkybox];
		bRefreshNewGame = false;
	}
}

using namespace cachedEvents;
void misc::EventHandler(IGameEvent* pEvent) {

	//CheatLog(pEvent);

	const std::string& szEventName = pEvent->GetName();

	PreserveKillfeed(pEvent);
	if (szEventName.find(cachedEvents::playerHurt) != std::string_view::npos) {

		HandlePlayerHitEffects(pEvent);
		CapsuleHandler(pEvent);
	}
	if (szEventName.find(cachedEvents::playerDeath) != std::string_view::npos) {
		CapsuleHandler(pEvent);
		Killsay(pEvent);
		ThirdPersonDisableOnDeath(pEvent);
	}
	if (szEventName.find(cachedEvents::bBulletImpact) != std::string_view::npos) {
		WorldCrosshairHandler(pEvent);
		BulletImpact(pEvent);
		BulletTracer(pEvent);
	}
	if (szEventName.find(cachedEvents::weaponFire) != std::string_view::npos) {

	}
	if (szEventName.find(cachedEvents::roundStart) != std::string_view::npos) {
		BuyBot(pEvent);
		WalkBotHandler(pEvent);
		std::fill(anims.arrMissedShots.begin(), anims.arrMissedShots.end(), 0);
	}
	if (szEventName.find(cachedEvents::itemPurchase) != std::string_view::npos) {

	}
	//if (!strcmp(pEvent->GetName(), "bomb_beginplant")) {
	//	//CustomBombText("lmao");
	//}
}

CBaseEntity* UTIL_PlayerByIndex(int index)
{
	typedef CBaseEntity* (__fastcall* PlayerByIndex)(int);
	static PlayerByIndex UTIL_PlayerByIndex = reinterpret_cast<PlayerByIndex>(MEM::FindPattern(SERVER_DLL, XorStr("85 C9 7E 32 A1")));

	if (!UTIL_PlayerByIndex)
		return nullptr;

	return UTIL_PlayerByIndex(index);
}

void misc::ServerHitboxes() {

	static uintptr_t* pCall = (uintptr_t*)(MEM::FindPattern(SERVER_DLL, XorStr("55 8B EC 81 EC ? ? ? ? 53 56 8B 35 ? ? ? ? 8B D9 57 8B CE")));
	float fDuration = i::GlobalVars->flIntervalPerTick * 1.0f;

	PVOID pTEntity = nullptr;

	if ( g::pLocal == nullptr )
		return;

	for (size_t i = 1; i < i::GlobalVars->nMaxClients; i++) {

		CBaseEntity* pEntity = static_cast<CBaseEntity*>(i::EntityList->GetClientEntity(i));
		if ( !pEntity || !pEntity->IsAlive( ) || pEntity->IsDormant( ) || !pEntity->IsPlayer( ) || !pEntity->EntIndex( ) )
			continue;

		//if (!cfg::misc::bDrawServerHitboxOnAllEntities && !cfg::misc::bDrawServerHitbox)
		//	continue;

		//if (!cfg::misc::bDrawServerHitboxOnAllEntities && pEntity != g::pLocal)
		//	continue;

		//if (!cfg::misc::bDrawServerHitbox && pEntity == g::pLocal)
		//	continue;

		pTEntity = UTIL_PlayerByIndex(pEntity->EntIndex());
		
		if (pTEntity)
		{
			__asm
			{
				pushad
				movss xmm1, fDuration
				push 1
				mov ecx, pTEntity
				call pCall
				popad
			}
		}
	}
}

void misc::RemovePostProcessing() {

	static CConVar* mat_postprocess_enable = i::ConVar->FindVar(XorStr("mat_postprocess_enable"));

	mat_postprocess_enable->SetValue(cfg::misc::bRemovals[4] ? 0 : 1);
}

void misc::Security() {

	// dont even ask that
	static auto showerror = i::ConVar->FindVar(XorStr("cl_showerror"));

	if (showerror->GetInt() != 0)
		showerror->SetValue(0);
}

void misc::IdealTick(CUserCmd* pCmd, CBaseEntity* pLocal) {

	if (!pCmd || !pLocal)
		return;

	if (!cfg::antiaim::bAutoPeek)
		return;

	CBaseCombatWeapon* pWeapon = pLocal->GetWeapon();
	if (!pWeapon)
		return;

	CCSWeaponInfo* pWeaponData = pWeapon->GetCSWpnData();
	if (!pWeaponData)
		return;

	if (pWeaponData->nWeaponType >= 7)
		return;

	static bool bPositionSet;

	static Vector vecOrigin;

	static Vector vecOriginDelta;

	if (IPT::HandleInput(cfg::antiaim::iAutoPeek)) {

		if (!bPositionSet) {

			bPositionSet = true;
			vecOrigin = pLocal->GetVecOrigin();
			vecRecord = vecOrigin;
		}

		if (pCmd->iButtons & IN_ATTACK && antiaim::ShouldDisableAntiaim(pCmd, *g::bSendPacket) || bTeleportBack)
			bRetreat = true;
	}
	else {
		bPositionSet = false;
		vecOrigin = Vector(0, 0, 0);
		vecRecord = Vector(0, 0, 0);
		return;
	}

	if (bPositionSet && vecOrigin != Vector(0, 0, 0) && bRetreat) {

		Vector vecAngle;
		M::VectorAngles(vecOrigin - g::vecEyePosition, vecAngle);

		g::vecOriginalViewAngle.y = vecAngle.y;
		pCmd->flForwardMove = 450.f;
		pCmd->flSideMove = 0.f;

		if ((vecOrigin - pLocal->GetVecOrigin()).Length2D() < 2.f) {
			bRetreat = false;
			bTeleportBack = false;
		}
	}
}

void misc::OnlyCheatLogs() {

	// default variables
	static bool bSet = false;
	static bool bReset = false;
	con_filter_enable = i::ConVar->FindVar(XorStr("con_filter_enable"));
	con_filter_text = i::ConVar->FindVar(XorStr("con_filter_text"));

	if (cfg::misc::bOnlyCheatlog)
	{
		if (!bSet)
		{
			con_filter_enable->SetValue(true);
			con_filter_text->SetValue(XorStr("[RyzeX]"));

			bSet = true;
		}
		bReset = false;
	}
	else {

		if (!bReset) {

			con_filter_text->SetValue(XorStr(""));
			bReset = true;
		}
		bSet = false;
	}
}

void misc::SlideFix() {

	if (!g::pLocal || !g::pLocal->IsAlive() || g::pLocal->GetMoveType() == MOVETYPE_LADDER)
		return;

	g::pCmd->iButtons &= ~(IN_FORWARD | IN_BACK | IN_MOVERIGHT | IN_MOVELEFT);
}

void misc::BulletImpactFrameStage(CBaseEntity* pLocal) {

	if (!pLocal || !pLocal->IsAlive())
		return;

	try {
		CUtlVector<ClientHitVerify_t>& pImpactList = *(CUtlVector<ClientHitVerify_t>*)((uintptr_t)pLocal + 0x11C50);
		//CUtlVector<ClientHitVerify_t>* pImpactList = reinterpret_cast<CUtlVector<ClientHitVerify_t>*>(g::pLocal + 0x11C50);

		if (!&pImpactList || pImpactList.Size() == 0)
			return;

		if (!cfg::misc::bBulletImpact)
			return;

		static int iLastCount = 0;
		for (int i = pImpactList.Count(); i > iLastCount; --i) {

			i::DebugOverlay->AddBoxOverlay(
				Vector(pImpactList[i - 1].pos),
				Vector(-2.0f, -2.0f, -2.0f),
				Vector(2.0f, 2.0f, 2.0f),
				Vector(0.0f, 0.0f, 0.0f),
				cfg::misc::flImpactColor[1][0] * 255, // b
				cfg::misc::flImpactColor[1][1] * 255, // g
				cfg::misc::flImpactColor[1][2] * 255, // g
				cfg::misc::flImpactColor[1][3] * 255, // a
				4.f
			);
		}

		if (pImpactList.Count() != iLastCount)
			iLastCount = pImpactList.Count();
	}
	catch(std::exception) {
		throw std::runtime_error(std::format(XorStr("BulletImpactFrameStage crashed\nMemory at: {}\nLocalPlayer: {}\nEngine LocalPlayer: {}"), uintptr_t(pLocal + 0x11C50), uintptr_t(pLocal), uintptr_t(CBaseEntity::GetLocalPlayer())).c_str());
	}
}

void misc::BulletImpact(IGameEvent* pEvent) {

	auto pLocal = CBaseEntity::GetLocalPlayer();

	if (!pLocal || !pLocal->IsAlive())
		return;

	if (!cfg::misc::bBulletImpact)
		return;

	if (pEvent != nullptr) {

		auto iUser = i::EngineClient->GetPlayerForUserID(pEvent->GetInt(XorStr("userid")));

		if (iUser != i::EngineClient->GetLocalPlayer())
			return;

		Vector vecImpact = Vector(pEvent->GetInt(XorStr("x")), pEvent->GetInt(XorStr("y")), pEvent->GetInt(XorStr("z")));

		i::DebugOverlay->AddBoxOverlay(
			vecImpact,
			Vector(-2.0f, -2.0f, -2.0f),
			Vector(2.0f, 2.0f, 2.0f),
			Vector(0.0f, 0.0f, 0.0f),
			cfg::misc::flImpactColor[0][0] * 255, // b
			cfg::misc::flImpactColor[0][1] * 255, // g
			cfg::misc::flImpactColor[0][2] * 255, // g
			cfg::misc::flImpactColor[0][3] * 255, // a
			4.f
		);
	}
}

void misc::BuyBot(IGameEvent* event) { // need menu element

	if (!cfg::misc::bAutobuy)
		return;

	std::string buy = "";

	switch (cfg::misc::iPistols) {
	case 1: buy += XorStr("buy glock;buy usp_silencer;buy hkp2000;"); break;
	case 2: buy += XorStr("buy elite;"); break;
	case 3: buy += XorStr("buy p250;"); break;
	case 4: buy += XorStr("buy cz75a;buy fiveseven; buy tec9;"); break;
	case 5: buy += XorStr("buy deagle;buy revolver;"); break;
	}

	switch (cfg::misc::iSnipers) {
	case 1: buy += XorStr("buy ssg08;"); break;
	case 2: buy += XorStr("buy awp;"); break;
	case 3: buy += XorStr("buy scar20;buy g3sg1;"); break;
	}

	if (cfg::misc::bEquipments[0])
		buy += XorStr("buy vesthelm;");

	if (cfg::misc::bEquipments[1])
		buy += XorStr("buy taser;");

	if (cfg::misc::bEquipments[2])
		buy += XorStr("buy defuser;");

	if (cfg::misc::bGrenades[0])
		buy += XorStr("buy molotov;buy incgrenade;");

	if (cfg::misc::bGrenades[1])
		buy += XorStr("buy decoy;");

	if (cfg::misc::bGrenades[2])
		buy += XorStr("buy flashbang;");

	if (cfg::misc::bGrenades[3])
		buy += XorStr("buy hegrenade;");

	if (cfg::misc::bGrenades[4])
		buy += XorStr("buy smokegrenade;");

	i::EngineClient->ExecuteClientCmd(buy.c_str());
}

void misc::HandlePlayerHitEffects( IGameEvent* pEvent ) {

	IClientEntity* pAttacker = i::EntityList->GetClientEntity(i::EngineClient->GetPlayerForUserID(pEvent->GetInt(XorStr("attacker"))));

	if ( !pAttacker || pAttacker != g::pLocal )
		return;

	CBaseEntity* pEntity = reinterpret_cast<CBaseEntity*>(i::EntityList->GetClientEntity(i::EngineClient->GetPlayerForUserID(pEvent->GetInt(XorStr("userid")))));

	if ( !pEntity || pEntity == g::pLocal )
		return;

	// TODO: Add effects interface for cool hit effects :D
	/*if ( C::Get<bool>( Vars.bScreenHitEffects ) )
	{
		std::optional<Vector> vecPosition;
		vecPosition = pEntity->GetHitGroupPosition( pEvent->GetInt( "hitgroup" ) );

		if ( !vecPosition.has_value( ) )
			return;

		switch ( C::Get<int>( Vars.iScreenHitEffects ) )
		{
		case 0:
			i::Effects->Sparks( vecPosition.value( ), 10, 10 );
			break;
		case 1:
			i::Effects->Smoke( vecPosition.value( ), 1, 10.f, 60.f );
			break;
		case 2:
			i::Effects->EnergySplash( vecPosition.value( ), vecPosition.value( ) + 50, true );
			break;
		}
	}*/

	// play hit sound
	if ( cfg::misc::iHitSound == 1 ) {
		i::EngineSoundClient->EmitAmbientSound("buttons\\arena_switch_press_02.wav", cfg::misc::flHitSoundVolume / 100.f);
		// physics\\metal\\paintcan_impact_hard3.wav
	}
	else if ( cfg::misc::iHitSound == 2 && !cfg::misc::szWavPath.empty( ) ) {

		static bool m_bNeedsUpdate = true;
		static float m_flOldVolume = 0.f;
		static std::string m_szOldWavPath;
		// store the parsed hitsound bytes to a dummy byte.
		static BYTE* m_pParsedHitsound;

		// read the .wav file into memory.
		BYTE* m_pSoundBytes = util::ReadWavFileIntoMemory( cfg::misc::szWavPath );

		if ( cfg::misc::flHitSoundVolume != m_flOldVolume || m_szOldWavPath != cfg::misc::szWavPath )
			m_bNeedsUpdate = true;

		if ( m_bNeedsUpdate )
		{
			m_szOldWavPath = cfg::misc::szWavPath;
			m_flOldVolume = cfg::misc::flHitSoundVolume;
			m_bNeedsUpdate = false;

			// adjust the hitsound volume.
			m_pParsedHitsound = m_pSoundBytes;

			CWavParser::WavHeader_t header;
			header.ParseWavHeader( m_pParsedHitsound );
			wavparser.AdjustWavVolume( header, ( cfg::misc::flHitSoundVolume / 100.f ) );
		}
		// play the sound.
		if ( m_pParsedHitsound ) {

			PlaySoundA( LPCSTR( m_pParsedHitsound ), NULL, SND_MEMORY | SND_ASYNC );
		}
	}
}

using namespace cachedEvents;
void misc::PreserveKillfeed(IGameEvent* pEvent) { // need menu element

	static DWORD* _death_notice = reinterpret_cast<DWORD*>(util::FindHudElement(XorStr("CCSGO_HudDeathNotice")));
	static void(__thiscall * _clear_notices)(DWORD) = (void(__thiscall*)(DWORD))MEM::FindPattern(CLIENT_DLL, XorStr("55 8B EC 83 EC 0C 53 56 8B 71 58"));
	
	if (g::pLocal) {

		static float flSpawnTime = g::pLocal->GetSpawnTime();
		if (pEvent == nullptr && flSpawnTime != g::pLocal->GetSpawnTime()) {

			if (_death_notice)
				_death_notice = reinterpret_cast<DWORD*>(util::FindHudElement(XorStr("CCSGO_HudDeathNotice")));
			if (_clear_notices)
				_clear_notices(((DWORD)_death_notice - 20));

			flSpawnTime = g::pLocal->GetSpawnTime();
		}
	}

	if (!pEvent)
		return;

	const std::string_view& szEventName = pEvent->GetName();

	if (szEventName.find(playerDeath) != std::string_view::npos) {

		auto pAttacker = i::EntityList->GetClientEntity(i::EngineClient->GetPlayerForUserID(pEvent->GetInt(XorStr("attacker"))));

		if (!pAttacker || pAttacker != g::pLocal)
			return;

		int index = i::EngineClient->GetPlayerForUserID(pEvent->GetInt(XorStr("userid")));

		if (!index)
			return;

		if (i::EngineClient->IsConnected() && i::EngineClient->IsInGame())
			_death_notice = reinterpret_cast<DWORD*>(util::FindHudElement(XorStr("CCSGO_HudDeathNotice")));

		if (!_clear_notices)
			return;

		if (_death_notice)
			*(float*)((DWORD)_death_notice + 0x50) = cfg::misc::bPreserveKillfeed ? FLT_MAX : 1.5; // need menu element
	}
	else if (szEventName.find(roundStart) != std::string_view::npos) {

		localAnim->ResetData();
		for (size_t i = 0; i < i::GlobalVars->nMaxClients; i++) {
			visual::vecDormatPosition[i] = Vector(0, 0, 0);
		}

		if (_death_notice)
			_death_notice = reinterpret_cast<DWORD*>(util::FindHudElement(XorStr("CCSGO_HudDeathNotice")));
		if (_clear_notices)
			_clear_notices(((DWORD)_death_notice - 20));
	}
}

void misc::FakeDuck(CUserCmd* pCmd) {

	if (IPT::HandleInput(cfg::antiaim::iFakeDuckKey) && cfg::antiaim::bFakeDuck) {

		auto iChoke = i::ClientState->nChokedCommands;

		pCmd->iButtons |= IN_BULLRUSH;

		if (g::pLocal->GetFlags() & FL_ONGROUND) {

			if (iChoke <= ((*GameRules)->m_bIsValveDS() ? 3 : 7))
				pCmd->iButtons &= ~IN_DUCK;
			else
				pCmd->iButtons |= IN_DUCK;
		}
	}
}

void misc::FastStop(CUserCmd* pCmd) {

	if (!cfg::misc::bFastStop)
		return;

	if (!g::pLocal || !g::pLocal->IsAlive() || !pCmd || !pCmd->iCommandNumber)
		return;

	if (!(g::pLocal->GetFlags() & FL_ONGROUND))
		return;

	auto bPressedKey = pCmd->iButtons & IN_FORWARD || pCmd->iButtons & IN_MOVELEFT || pCmd->iButtons & IN_BACK || pCmd->iButtons & IN_MOVERIGHT || pCmd->iButtons & IN_JUMP;

	if (bPressedKey)
		return;

	auto vecVelocity = g::pLocal->GetVelocity();

	if (vecVelocity.Length2D() > 20.0f)
	{
		Vector vecDirection;
		Vector vecView;

		M::VectorAngles(vecVelocity, vecDirection);
		i::EngineClient->GetViewAngles(vecView);

		vecDirection.y = vecView.y - vecDirection.y;

		Vector vecForward;
		M::AngleVectors(vecDirection, &vecForward);

		static auto cl_forwardspeed = i::ConVar->FindVar(XorStr("cl_forwardspeed"));
		static auto cl_sidespeed = i::ConVar->FindVar(XorStr("cl_sidespeed"));

		auto negative_forward_speed = -cl_forwardspeed->GetFloat();
		auto negative_side_speed = -cl_sidespeed->GetFloat();

		auto negative_forward_direction = vecForward * negative_forward_speed;
		auto negative_side_direction = vecForward * negative_side_speed;

		pCmd->flForwardMove = negative_forward_direction.x;
		pCmd->flSideMove = negative_side_direction.y;
	}
}

void misc::Slowwalk(CUserCmd* pCmd, float flSpeed) {

	if (!g::pLocal || !g::pLocal->IsAlive() || !pCmd || !pCmd->iCommandNumber)
		return;

	if (flSpeed <= 0.f)
		return;

	if (!IPT::HandleInput(cfg::antiaim::iFakeWalkKey))
		return;

	float flMinSpeed = (float)(sqrt((pCmd->flForwardMove * pCmd->flForwardMove) + (pCmd->flSideMove * pCmd->flSideMove) + (pCmd->flUpMove * pCmd->flUpMove)));
	if (flMinSpeed <= 0.f)
		return;

	if (pCmd->iButtons & IN_DUCK)
		flSpeed *= 2.94117647f;

	if (flMinSpeed <= flSpeed)
		return;

	float flMove = flSpeed / flMinSpeed;

	pCmd->flForwardMove *= flMove;
	pCmd->flSideMove *= flMove;
	pCmd->flUpMove *= flMove;
}

void misc::AspectRatio() {

	static CConVar* r_aspectratio = i::ConVar->FindVar(XorStr("r_aspectratio"));

	if (!cfg::misc::bAspectRatio) {
		r_aspectratio->SetValue(0);
		return;
	}

	float ratio = (cfg::misc::iAspectRatio * 0.1) / 2;
	if (ratio > 0.001)
		r_aspectratio->SetValue(ratio); //ayyware hhhh
	else
		r_aspectratio->SetValue((35 * 0.1f) / 2);
}

Vector VecAngle( Vector vec )
{
	Vector ret;
	if ( vec.x == 0.0f && vec.y == 0.0f )
	{
		ret.x = ( vec.z > 0.0f ) ? 270.0f : 90.0f;
		ret.y = 0.0f;
	}
	else
	{
		ret.x = M_RAD2DEG( std::atan2f( -vec.z, vec.Length2D( ) ) );
		ret.y = M_RAD2DEG( std::atan2f( vec.y, vec.x ) );
		if ( ret.y < 0.0f ) ret.y += 360.0f;
		if ( ret.x < 0.0f ) ret.x += 360.0f;
	}
	ret.z = 0.0f;
	return ret;
}

void misc::AutoStrafe(Vector& vecView, CUserCmd* pCmd) {

	if (!cfg::misc::bAutoStrafe)
		return;

	// check if local player is valid
	if (!g::pLocal || !g::pLocal->IsAlive() || !pCmd || !pCmd->iCommandNumber)
		return;

	// check ladder and ground
	if (g::pLocal->GetMoveType() == MOVETYPE_LADDER || g::pLocal->GetFlags() & FL_ONGROUND || g::pLocal->GetMoveType() == MOVETYPE_NOCLIP)
		return;

	if (g::pLocal->GetVelocity().Length2D() < 5.0f && !pCmd->flForwardMove && !pCmd->flSideMove)
		return;

	float flYawAdd = 0.f;
	static const auto cl_sidespeed = i::ConVar->FindVar( XorStr( "cl_sidespeed" ) )->GetFloat( );
	if ( !( g::pLocal->GetFlags( ) & FL_ONGROUND ) )
	{
		bool bBack = pCmd->iButtons & IN_BACK;
		bool bForward = pCmd->iButtons & IN_FORWARD;
		bool bRight = pCmd->iButtons & IN_MOVELEFT;
		bool bLeft = pCmd->iButtons & IN_MOVERIGHT;

		if ( bBack )
		{
			flYawAdd = -180.f;
			if ( bRight )
				flYawAdd -= 45.f;
			else if ( bLeft )
				flYawAdd += 45.f;
		}
		else if ( bRight )
		{
			flYawAdd = 90.f;
			if ( bBack )
				flYawAdd += 45.f;
			else if ( bForward )
				flYawAdd -= 45.f;
		}
		else if ( bLeft )
		{
			flYawAdd = -90.f;
			if ( bBack )
				flYawAdd -= 45.f;
			else if ( bForward )
				flYawAdd += 45.f;
		}
		else
		{
			flYawAdd = 0.f;
		}

		vecView.y += flYawAdd;
		pCmd->flForwardMove = 0.f;
		pCmd->flSideMove = 0.f;
		const auto delta = M::NormalizeAngle( vecView.y - M_RAD2DEG( std::atan2f( g::pLocal->GetVelocity( ).y, g::pLocal->GetVelocity( ).x ) ) );
		pCmd->flSideMove = delta > 0.f ? -cl_sidespeed : cl_sidespeed;
		vecView.y = M::NormalizeAngle( vecView.y - delta );

		if ( vecView.x == 0 && vecView.y == 0 && vecView.z == 0 )
			i::EngineClient->GetViewAngles( vecView );

		const Vector vecMove = Vector( pCmd->flForwardMove, pCmd->flSideMove, pCmd->flUpMove );
		const Vector vecAngle = VecAngle( vecMove );
		const float flSpeed = vecMove.Length2D( );
		const float flRotation = M_DEG2RAD( pCmd->angViewPoint.y - vecView.y + vecAngle.y );

		pCmd->flForwardMove = std::cosf( flRotation ) * flSpeed;
		pCmd->flSideMove = std::sinf( flRotation ) * flSpeed;
	}
}

void misc::MovementFix(CUserCmd* pCmd, Vector& oldang) {

	//Vector vMovements(pCmd->flForwardMove, pCmd->flSideMove, 0.f);

	//if (vMovements.Length2D() == 0)
	//	return;

	//CUserCmd* pUserCmd = static_cast <CUserCmd*> (pCmd);
	//if (!pUserCmd || !pUserCmd->iCommandNumber)
	//	return;

	///* get wish angles */
	//Vector angWishAngles;
	//i::EngineClient->GetViewAngles(angWishAngles);

	//Vector PureForward, PureRight, PureUp, CurrForward, CurrRight, CurrUp;
	//M::AngleVectors(angWishAngles, &PureForward, &PureRight, &PureUp);
	//M::AngleVectors(pUserCmd->angViewPoint, &CurrForward, &CurrRight, &CurrUp);

	//PureForward[2] = PureRight[2] = CurrForward[2] = CurrRight[2] = 0.f;

	//auto VectorNormalize = [](Vector& vec)->float {
	//	float radius = sqrtf(vec.x * vec.x + vec.y * vec.y + vec.z * vec.z);
	//	float iradius = 1.f / (radius + FLT_EPSILON);

	//	vec.x *= iradius;
	//	vec.y *= iradius;
	//	vec.z *= iradius;

	//	return radius;
	//};
	//VectorNormalize(PureForward);
	//VectorNormalize(PureRight);
	//VectorNormalize(CurrForward);
	//VectorNormalize(CurrRight);
	//Vector PureWishDir;
	//for (auto i = 0u; i < 2; i++)
	//	PureWishDir[i] = PureForward[i] * pUserCmd->flForwardMove + PureRight[i] * pUserCmd->flSideMove;
	//PureWishDir[2] = 0.f;

	//Vector CurrWishDir;
	//for (auto i = 0u; i < 2; i++)
	//	CurrWishDir[i] = CurrForward[i] * pUserCmd->flForwardMove + CurrRight[i] * pUserCmd->flSideMove;
	//CurrWishDir[2] = 0.f;

	//if (PureWishDir != CurrWishDir) {
	//	pUserCmd->flForwardMove = (PureWishDir.x * CurrRight.y - CurrRight.x * PureWishDir.y) / (CurrRight.y * CurrForward.x - CurrRight.x * CurrForward.y);
	//	pUserCmd->flSideMove = (PureWishDir.y * CurrForward.x - CurrForward.y * PureWishDir.x) / (CurrRight.y * CurrForward.x - CurrRight.x * CurrForward.y);
	//}

	static CConVar* cl_forwardspeed = i::ConVar->FindVar("cl_forwardspeed");
	static CConVar* cl_sidespeed = i::ConVar->FindVar("cl_sidespeed");

	float flForwardDelta = cl_forwardspeed->GetFloat() - pCmd->flForwardMove;
	float flSidewayDelta = cl_sidespeed->GetFloat() - pCmd->flSideMove;
	float flMoveDelta = flForwardDelta < flSidewayDelta ? pCmd->flForwardMove / cl_forwardspeed->GetFloat() : pCmd->flSideMove / cl_sidespeed->GetFloat();

	Vector vRealF, vRealR;
	Vector aRealDir = pCmd->angViewPoint;
	aRealDir.Normalize();
	aRealDir.Clamp();

	M::AngleVectors(aRealDir, &vRealF, &vRealR, nullptr);
	vRealF[2] = 0;
	vRealR[2] = 0;

	vRealF.VectorNormalize();
	vRealR.VectorNormalize();

	Vector aWishDir = oldang;
	aWishDir.Normalize();
	aWishDir.Clamp();

	Vector vWishF, vWishR;
	M::AngleVectors(aWishDir, &vWishF, &vWishR, nullptr);	

	vWishF[2] = 0;
	vWishR[2] = 0;

	vWishF.VectorNormalize();
	vWishR.VectorNormalize();

	Vector vWishVel;
	vWishVel[0] = vWishF[0] * pCmd->flForwardMove + vWishR[0] * pCmd->flSideMove;
	vWishVel[1] = vWishF[1] * pCmd->flForwardMove + vWishR[1] * pCmd->flSideMove;
	vWishVel[2] = 0;

	float a = vRealF[0], b = vRealR[0], c = vRealF[1], d = vRealR[1];
	float v = vWishVel[0], w = vWishVel[1];

	float flDivide = (a * d - b * c);
	float x = (d * v - b * w) / flDivide;
	float y = (a * w - c * v) / flDivide;

	pCmd->flForwardMove = std::clamp(x + (flMoveDelta * pCmd->angViewPoint.z), -450.f, 450.f);
	pCmd->flSideMove = std::clamp(y + (flMoveDelta * pCmd->angViewPoint.z), -450.f, 450.f);

}

void misc::BunnyHop(CUserCmd* pCmd) {
	
	if (!g::pLocal || !g::pLocal->IsAlive())
		return;

	if (!cfg::misc::bBunnyHop)
		return;

	static bool bLastJumped = false, bShouldFake = false;

	if (!bLastJumped && bShouldFake) {

		bShouldFake = false;
		pCmd->iButtons |= IN_JUMP;
	}
	else if (pCmd->iButtons & IN_JUMP) {

		if (g::pLocal->GetFlags() & FL_ONGROUND || g::pLocal->GetFlags() & FL_PARTIALGROUND) {

			bLastJumped = true;
			bShouldFake = true;
		}
		else {

			pCmd->iButtons &= ~IN_JUMP;
			bLastJumped = false;
		}
	}
	else {

		bLastJumped = false;
		bShouldFake = false;
	}
}

void misc::ThirdPerson() {

	auto pLocal = CBaseEntity::GetLocalPlayer();

	if (!pLocal)
		return;

	Vector vecOffset;
	Vector vecEyePosition, vecForward;
	static CTraceFilter filter(pLocal, TRACE_WORLD_ONLY);
	Trace_t trace;

	// for whatever reason override_view also gets called from the main menu.
	if (!i::EngineClient->IsInGame() || i::ClientState->iSignonState != SIGNONSTATE_FULL || !pLocal)
		return;

	if (pLocal->GetTeam() == TEAM_UNASSIGNED)
		return;

	// check if we have a local player and it is alive.
	bool bAlive = pLocal && pLocal->IsAlive();

	static float flKurvaAnyad = 0.f;

	if (!i::Input->bCameraInThirdPerson)
		flKurvaAnyad = 0.f;
	
	// camera should be in thirdperson.
	if (IPT::HandleInput(cfg::misc::iThirdPersonKey) && cfg::misc::bThirdPerson)
	{
		// if bAlive and not in thirdperson already switch to thirdperson.
		if (bAlive && !i::Input->CameraInThirdPerson())
			i::Input->ToThirdPerson();

		// if dead and spectating in firstperson switch to thirdperson.
		else if (pLocal->GetObserverMode() == 4)
		{
			// if in thirdperson, switch to firstperson.
			// we need to disable thirdperson to spectate properly.
			if (i::Input->CameraInThirdPerson())
			{
				i::Input->ToFirstPerson();
				i::Input->vecCameraOffset.z = 0.f;
			}

			pLocal->GetObserverMode() = 5;
		}
	}

	// camera should be in firstperson.
	else if (i::Input->CameraInThirdPerson())
	{
		i::Input->ToFirstPerson();
		i::Input->vecCameraOffset.z = 0.f;
	}
	// if after all of this we are still in thirdperson.
	if (i::Input->CameraInThirdPerson())
	{
		// get camera angles.
		i::EngineClient->GetViewAngles(vecOffset);

		// get our viewangle's vecForward directional vector.
		M::AngleVectors(vecOffset, &vecForward);

		// cam_idealdist convar.
		vecOffset.z = static_cast<float>(cfg::misc::iThirdPersonDistance);

		if (vecOffset.z == 0) {

			vecEyePosition = pLocal->GetEyePosition(false);
			Vector vecHeadPosition = pLocal->GetHitboxPosition(HITBOX_HEAD).value();
			float flDifference = ((vecEyePosition - vecHeadPosition).Length2D()) * 0.1f;

			Vector vecViewPunch = g::pLocal->GetViewPunch();
			Vector vecAimPunch = g::pLocal->GetAimPunch();

			vecOffset.x += (vecViewPunch[0] + (vecAimPunch[0] * 2 * 0.4499999f));
			vecOffset.y += (vecViewPunch[1] + (vecAimPunch[1] * 2 * 0.4499999f));
			//pSetup->angView[2] -= (vecViewPunch[2] + (vecAimPunch[2] * 2 * 0.4499999f));

			i::Input->vecCameraOffset = { vecOffset.x + flDifference, vecOffset.y + flDifference, vecEyePosition.z - vecHeadPosition.z };
			//i::Input->vecCameraOffset = { vecOffset.x + (vecEyePosition - vecHeadPosition).x + cfg::debugSlider, vecOffset.y + (vecEyePosition - vecHeadPosition).y, vecEyePosition.z - vecHeadPosition.z};
			return;
		}

		// start pos.
		vecEyePosition = pLocal->GetEyePosition(false);

		// setup trace filter and trace.
		i::EngineTrace->TraceRay(Ray_t(vecEyePosition, vecEyePosition - (vecForward * vecOffset.z), { -16.f, -16.f, -16.f }, { 16.f, 16.f, 16.f }), MASK_SOLID, &filter, &trace);

		// adapt distance to travel time.
		trace.flFraction = max(0.f, min(trace.flFraction, 1.f));
		vecOffset.z *= trace.flFraction;

		// override camera angles.
		i::Input->vecCameraOffset = { vecOffset.x, vecOffset.y, vecOffset.z };
	}
}

void misc::FakeLag(bool& bSendPacket) {

	enum FAKELAGTYPE {

		MAXIMUM,
		ADAPTIVE,
		JITTER,
		RANDOM
	};

	if ((*GameRules)->m_bFreezePeriod())
		return;
	
	if (i::EngineClient->IsVoiceRecording())
		return;

	if (!g::pLocal || !g::pLocal->IsAlive() || !cfg::antiaim::iFakelag || !cfg::antiaim::bFakelag) {
		bSendPacket = true;
		return;
	} 

	if ( IPT::HandleInput(cfg::antiaim::iFakeDuckKey) && cfg::antiaim::bFakeDuck) {
		bSendPacket = i::ClientState->nChokedCommands >= ((*GameRules)->m_bIsValveDS() ? 6 : 14);
		return;
	}

	if (exploits::bDoubleTapEnabled && exploits::iShiftAmount > 0 && cfg::rage::bDoubletap && IPT::HandleInput(cfg::rage::iDoubletapKey))
		return;

	static int iCurrentChoke = 0;
	static bool bSwitch = false;

	static CConVar* sv_maxspeed = i::ConVar->FindVar(XorStr("sv_maxspeed"));
	float flVelocity = g::pLocal->GetVelocity().Length2D();
	float flMaxVelocity = g::pLocal->GetWeapon() ? g::pLocal->IsScoped() ? g::pLocal->GetWeapon()->GetCSWpnData()->flMaxSpeed[1] : g::pLocal->GetWeapon()->GetCSWpnData()->flMaxSpeed[0] : sv_maxspeed->GetFloat();

	int iMin = cfg::antiaim::iFakelagMin;
	int iMax = cfg::antiaim::iFakeLagMax;

	if ((*GameRules)->m_bIsValveDS()) {

		iMin = min(6, iMin);
		iMax = min(6, iMax);
	}

	if (cfg::antiaim::iFakelag) {

		switch (cfg::antiaim::iFakeLagType) {

		case MAXIMUM:

			iCurrentChoke = cfg::antiaim::iFakelag;
			break;

		case ADAPTIVE:

			iCurrentChoke = max(1, 15 * (flVelocity / flMaxVelocity));
			break;

		case JITTER:

			if (bSendPacket) {
				bSwitch = !bSwitch;
				iCurrentChoke = bSwitch ? iMin : iMax;
			}
				

			break;
		case RANDOM:

			if (bSendPacket)
				iCurrentChoke = M::RandomInt(iMin, iMax);

			break;
		}
	}
	else
		iCurrentChoke = cfg::antiaim::iFakelag;

	iMax = ((cfg::rage::bDoubletap && IPT::HandleInput(cfg::rage::iDoubletapKey)) || (cfg::rage::bHideshot && IPT::HandleInput(cfg::rage::iHideShotKey))) ? 1 : iMax;
	networking.LagcompensatedTicks = min(iMax, max(iMin, iCurrentChoke));
	iRestChoke = networking.LagcompensatedTicks - i::ClientState->nChokedCommands;
	bSendPacket = i::ClientState->nChokedCommands >= networking.LagcompensatedTicks;
}

void misc::DrawBream(Vector vecSource, Vector vecEnd, Color color) {

	BeamInfo_t info;
	info.m_nType = TE_BEAMPOINTS;
	//info.m_pszModelName = "sprites/purplelaser1.vmt";
	info.m_pszModelName = XorStr("sprites/white.vmt");
	info.m_nModelIndex = -1;
	info.m_flHaloScale = -1.0f;
	info.m_flLife = 3.0f;
	info.m_flWidth = 0.1;
	info.m_flEndWidth = 1.0f;
	info.m_flFadeLength = 0.0f;
	info.m_flAmplitude = 2.0f;
	info.m_flBrightness = color[3];
	info.m_flSpeed = 0.5f;
	info.m_nStartFrame = 0.f;
	info.m_flFrameRate = 0.f;
	info.m_flRed = color[0];
	info.m_flGreen = color[1];
	info.m_flBlue = color[2];
	info.m_nSegments = 2;
	info.m_bRenderable = true;
	info.iFlags = FBEAM_ONLYNOISEONCE | FBEAM_NOTILE | FBEAM_HALOBEAM;
	info.m_vecStart = vecSource;
	info.m_vecEnd = vecEnd;

	Beam_t* pBeam = i::RenderBeam->CreateBeamPoints(info);
	if (pBeam)
		i::RenderBeam->DrawBeam(pBeam);
}

void misc::BulletTracer(IGameEvent* pEvent) {
	
	if (!g::pLocal)
		return;

	/* Get this once, so the beams won't deform bcs of multiple impact -> multiple position */
	auto iUser = i::EngineClient->GetPlayerForUserID(pEvent->GetInt(XorStr("userid")));
	CBaseEntity* pEntity = static_cast<CBaseEntity*>(i::EntityList->GetClientEntity(iUser));

	if (!pEntity)
		return;

	Vector vecImpact = Vector(pEvent->GetInt(XorStr("x")), pEvent->GetInt(XorStr("y")), pEvent->GetInt(XorStr("z")));

	if (pEntity == g::pLocal && cfg::visual::bBulletTracer[LOCAL])
		DrawBream(pEntity->GetEyePosition(), vecImpact, cfg::visual::flBulletTracerColor[LOCAL]);
	
	else if (!pEntity->IsEnemy(g::pLocal) && cfg::visual::bBulletTracer[TEAM] && !pEntity->IsDormant())
		DrawBream(pEntity->GetEyePosition(), vecImpact, cfg::visual::flBulletTracerColor[TEAM]);
	
	else if (pEntity->IsEnemy(g::pLocal) && cfg::visual::bBulletTracer[ENEMY] && !pEntity->IsDormant())
		DrawBream(pEntity->GetEyePosition(), vecImpact, cfg::visual::flBulletTracerColor[ENEMY]);
	
}

void misc::WorldCrosshairHandler(IGameEvent* pEvent) {

	if (!g::pLocal || !g::pLocal->IsAlive() || !cfg::misc::bWorldCrosshair)
		return;

	auto iUser = i::EngineClient->GetPlayerForUserID(pEvent->GetInt(XorStr("userid")));

	if (iUser != g::pLocal->EntIndex())
		return;

	static int i = 0;

	cfg::misc::flWorldCrosshairColor[3] = 1.f;
	visual::flWorldCrosshairLength[i] = i::GlobalVars->flCurrentTime;
	visual::vecWorldCrosshair[i] = Vector(pEvent->GetInt(XorStr("x")), pEvent->GetInt(XorStr("y")), pEvent->GetInt(XorStr("z")));
	i = ( i + 1 ) % 5;
}

void misc::FixScopeSens() {

	// zoom_sensitivity_ratio_mouse 
	static CConVar* zoom_sensitivity_ratio_mouse = i::ConVar->FindVar(XorStr("zoom_sensitivity_ratio_mouse"));

	if (!g::pLocal || !g::pLocal->GetWeapon()) {
		zoom_sensitivity_ratio_mouse->SetValue(1);
	}

	if (!g::pLocal->IsScoped() || !i::EngineClient->IsInGame()) {
		zoom_sensitivity_ratio_mouse->SetValue(1);
		return;
	}
	
	
	zoom_sensitivity_ratio_mouse->SetValue(g::pLocal->IsScoped() ? cfg::misc::bRemovals[3] ? 0 : 1 : 1);
}

void misc::AutoPistol(CUserCmd* pCmd, CBaseEntity* pLocal) {

	if (!pLocal || !pLocal->IsAlive() || !pLocal->GetWeapon())
		return;

	if (pLocal->IsGrenade(pLocal->GetWeapon()))
		return;

	if (pLocal->GetWeapon()->GetItemDefinitionIndex() == EItemDefinitionIndex::WEAPON_C4 ||
		pLocal->GetWeapon()->GetItemDefinitionIndex() == EItemDefinitionIndex::WEAPON_REVOLVER)
		return;

	if ((pCmd->iButtons & IN_ATTACK) && (pLocal->GetWeapon()->GetNextPrimaryAttack() >= TICKS_TO_TIME(networking.GetCorrectedTickbase())))
		pCmd->iButtons &= ~IN_ATTACK;
}

void misc::RemoveSmoke() {
	static int flCurrentTime = i::GlobalVars->flCurrentTime;

	if (!cfg::misc::bRemovals[0] || !g::pLocal || flCurrentTime > (i::GlobalVars->flCurrentTime - TICKS_TO_TIME(32)))
		return;

	flCurrentTime = i::GlobalVars->flCurrentTime;

    if (static auto sigLineGoesThroughSmoke = MEM::FindPattern(CLIENT_DLL, XorStr("55 8B EC 83 EC 08 8B 15 ? ? ? ? 0F 57 C0")); sigLineGoesThroughSmoke) {
        int* smokeCount = reinterpret_cast<int*>(*reinterpret_cast<DWORD*>(sigLineGoesThroughSmoke + 0x8));
        *smokeCount = 0;
    }

    static const char* vecSmokeWireframe = XorStr("particle/vistasmokev1/vistasmokev1_smokegrenade");

    IMaterial* wireframeMaterial = i::MaterialSystem->FindMaterial(vecSmokeWireframe, XorStr("Other textures"));
    if (wireframeMaterial) {
        wireframeMaterial->SetMaterialVarFlag(MATERIAL_VAR_WIREFRAME, true);
    }

    static std::array<const char*, 3> vecSmokeNoDraw = {
        XorStr("particle/vistasmokev1/vistasmokev1_fire"),
        XorStr("particle/vistasmokev1/vistasmokev1_emods"),
        XorStr("particle/vistasmokev1/vistasmokev1_emods_impactdust")
    };

    for (const char* smokeMaterialName : vecSmokeNoDraw) {
        IMaterial* smokeMaterial = i::MaterialSystem->FindMaterial(smokeMaterialName, XorStr("Other textures"));
        if (smokeMaterial) {
            smokeMaterial->SetMaterialVarFlag(MATERIAL_VAR_NO_DRAW, true);
        }
    }
}

void misc::WalkBotHandler(IGameEvent* pEvent) {

	//if (!strcmp(pEvent->GetName(), roundStart))
	//	bNewRound = true;
}

//void TraceRayBot(CUserCmd* pCmd);
//void misc::WalkBot(CUserCmd* pCmd) {
//
//	static int positionID = 0;
//	static int randomLmao = 0;
//	static std::vector<Vector> moveHere;
//	static Vector vecLastAngle = g::vecOriginalViewAngle;
//
//	if (!g::pLocal)
//		return;
//
//	if (!cfg::debugSwitch2 || !g::pLocal->IsAlive() || bNewRound) {
//		AIVizualization.clear();
//		moveHere.clear();
//		positionID = 0;
//		randomLmao = M::RandomInt(0, 6);
//		bNewRound = false;
//
//		int iLocalTeam = g::pLocal->GetTeam();
//		switch (randomLmao)
//		{
//		case 1: moveHere = iLocalTeam == TEAM_CT ? walkbotPositions::MirageCTB : walkbotPositions::MirageTBRush;
//			break;
//		case 2: moveHere = iLocalTeam == TEAM_CT ? walkbotPositions::MirageCTNinja : walkbotPositions::MirageTMidRush;
//			break;
//		case 3: moveHere = iLocalTeam == TEAM_CT ? walkbotPositions::MirageCTNinja : walkbotPositions::MirageTPallaceRush;
//			break;
//		case 4: moveHere = iLocalTeam == TEAM_CT ? walkbotPositions::MirageCTStairs : walkbotPositions::MirageTStairs;
//			break;
//		case 5: moveHere = iLocalTeam == TEAM_CT ? walkbotPositions::MirageCTSpawnBox : walkbotPositions::MirageTUnderGroundRush;
//			break;
//
//		default: moveHere = iLocalTeam == TEAM_CT ? walkbotPositions::MirageCTSpawnBox : walkbotPositions::MirageTUnderGroundRush;
//			break;
//		}
//
//		return;
//	}
//
//	if (moveHere.empty())
//		return;
//
//	//int iClosestIndex = antiaim::ClosestToLocal();
//	//CBaseEntity* pClosestTarget = nullptr;
//
//	//if (iClosestIndex != -1)
//	//	pClosestTarget = reinterpret_cast<CBaseEntity*>(i::EntityList->GetClientEntity(iClosestIndex));
//
//	//if (pClosestTarget != nullptr) {
//
//	//	Vector vecInterpolatedEyePosition = ragebot.InterpolateLocalEyePosition(g::pLocal->GetEyePosition(), 3);
//
//	//	FireBulletData_t data = { };
//	//	data.vecPosition = vecInterpolatedEyePosition;
//	//	data.vecDirection = (pClosestTarget->GetHitboxPosition(HITBOX_UPPER_CHEST).value() - vecInterpolatedEyePosition).Normalized();
//
//	//	Vector vecCalcAngle;
//	//	M::VectorAngles(pClosestTarget->GetHitboxPosition(HITBOX_STOMACH).value() - g::pLocal->GetEyePosition(), vecCalcAngle);
//	//	Vector vecDistanceBetween = (vecLastAngle.NormalizeAngle() - vecCalcAngle.NormalizeAngle());
//
//	//	float flFinalFov = vecDistanceBetween.y;
//
//	//	vecLastAngle.y += std::clamp(flFinalFov, -30.f, 30.f);
//
//	//	if (flFinalFov < 35)
//	//		vecLastAngle.y = vecCalcAngle.y;
//
//	//	M::NormalizeAngle(vecLastAngle.y);
//	//	i::EngineClient->SetViewAngles(vecLastAngle);
//
//	//	if (autowall.SimulateFireBullet(g::pLocal, g::pLocal->GetWeapon(), data))
//	//		return;
//	//}
//
//	if (moveHere.size() > positionID) {
//
//		MoveToPosition(moveHere.at(positionID));
//		AIVizualization.push_front(moveHere.at(positionID));
//
//		if ((moveHere.at(positionID) - g::pLocal->GetAbsOrigin()).Length2D() < 10.f) {
//			positionID++;
//		}
//
//		while (AIVizualization.size() >= 32)
//			AIVizualization.pop_back();
//	}
//	//else {
//	//	TraceRayBot(pCmd);
//	//}
//}
//
//void TraceRayBot(CUserCmd* pCmd) // wip
//{
//	using namespace misc;
//
//	auto GRD_TO_BOG = [&](float GRD) -> float {
//		return (M_PI / 180) * GRD;
//	};
//	auto ProjectPoint = [&](Vector vecOrigin, float flYaw, float flDistance) -> Vector {
//		return (vecOrigin + Vector((flDistance * sin(GRD_TO_BOG(flYaw))), -(flDistance * cos(GRD_TO_BOG(flYaw))), 0));
//	};
//
//	float flBestDistance = 0.f;
//	float flBestAngle = -9999999.f;
//
//	pCmd->flForwardMove = 450.f;
//
//	CTraceFilter filter1(g::pLocal);
//	Vector vecOrigin = g::pLocal->GetVecOrigin() + Vector(0, 0, 10);
//
//	Vector vecEyeAngle;
//	i::EngineClient->GetViewAngles(vecEyeAngle);
//
//	Trace_t TraceFront;
//	Vector vecProjectFirstPoint = ProjectPoint(vecOrigin, M::NormalizeYaw((vecEyeAngle.y + 90)), 40);
//	i::EngineTrace->TraceRay(Ray_t(vecOrigin, vecProjectFirstPoint), MASK_SOLID, &filter1, &TraceFront);
//	AIVizualization.push_front(TraceFront.vecEnd);
//
//	static bool bJumped = false;
//
//	if (!(g::pLocal->GetFlags() & FL_ONGROUND))
//		pCmd->iButtons |= IN_DUCK;
//	else
//		bJumped = false;
//
//	if (TraceFront.flFraction != 1.0f && (g::pLocal->GetFlags() & FL_ONGROUND) && !bJumped) {
//		pCmd->iButtons |= IN_JUMP;
//		bJumped = true;
//	}
//
//	static bool bRandomDirect = true;
//	bRandomDirect = false;
//	srand(time(NULL));
//	int iRandomStart = rand() % 36;
//	int i = iRandomStart;
//	bool bDoubleStart = false;
//	bool bLoop = true;
//	while (bLoop) {
//
//		i += bRandomDirect ? 1 : -1;
//
//		if (bRandomDirect && i > 36) {
//			i = 0;
//			bDoubleStart = true;
//		}
//		else if (!bRandomDirect && i < 0) {
//			i = 36;
//			bDoubleStart = true;
//		}
//
//		if (bRandomDirect && i >= iRandomStart && bDoubleStart) {
//			bLoop = false;
//			break;
//		}
//		else if (!bRandomDirect && i <= iRandomStart && bDoubleStart) {
//			bLoop = false;
//			break;
//		}
//
//		Trace_t TraceInit;
//		Vector vecProjectSecondPoint = ProjectPoint(vecOrigin, M::NormalizeYaw(i * 10), 300) + Vector(0, 0, 90);
//		i::EngineTrace->TraceRay(Ray_t(vecOrigin, vecProjectSecondPoint), MASK_SOLID, &filter1, &TraceInit);
//		AIVizualization.push_front(TraceInit.vecEnd);
//
//		if (TraceInit.flFraction > flBestDistance) {
//			flBestAngle = M::NormalizeYaw(i * 10);
//			flBestDistance = TraceInit.flFraction;
//		}
//	}
//
//	flBestAngle = M::NormalizeYaw(flBestAngle - 90);
//
//	float flDelta = M::NormalizeYaw(flBestAngle - vecEyeAngle.y);
//
//	if (flDelta >= 0)
//		vecEyeAngle.y = M::NormalizeYaw(vecEyeAngle.y + ((abs(flDelta) <= 5) ? abs(flDelta) : 5));
//	else
//		vecEyeAngle.y = M::NormalizeYaw(vecEyeAngle.y - ((abs(flDelta) <= 5) ? abs(flDelta) : 5));
//
//	Trace_t TraceLeft;
//	Vector vecProjectThirdPoint = ProjectPoint(vecOrigin, M::NormalizeYaw((vecEyeAngle.y - 90) - 45), 300);
//	i::EngineTrace->TraceRay(Ray_t(vecOrigin, vecProjectThirdPoint), MASK_SOLID, &filter1, &TraceLeft);
//	AIVizualization.push_front(TraceLeft.vecEnd);
//
//	Trace_t TraceRight;
//	Vector vecProjectFourthPoint = ProjectPoint(vecOrigin, M::NormalizeYaw((vecEyeAngle.y - 90) + 45), 300);
//	i::EngineTrace->TraceRay(Ray_t(vecOrigin, vecProjectFourthPoint), MASK_SOLID, &filter1, &TraceRight);
//	AIVizualization.push_front(TraceRight.vecEnd);
//
//
//	if (TraceLeft.flFraction > TraceRight.flFraction)
//		pCmd->flSideMove = -100.f;
//	else
//		pCmd->flSideMove = 100.f;
//
//	vecEyeAngle.x = 0.f;
//
//	i::EngineClient->SetViewAngles(vecEyeAngle);
//
//	while (AIVizualization.size() >= 32)
//		AIVizualization.pop_back();
//}

void misc::MoveToPosition() {

	Vector vecExtrapolatedLocalPos = (g::pLocal->GetVecOrigin() + (g::pLocal->GetVelocity() * (i::GlobalVars->flIntervalPerTick * 3)));
	if (playerList::iFollowPlayerIndex == -1)
		return;

	for (size_t i = 0; i < i::GlobalVars->nMaxClients; i++) {

		CBaseEntity* pEntity = static_cast<CBaseEntity*>(i::EntityList->GetClientEntity(i));
		if (!pEntity)
			continue;

		if (abs((vecExtrapolatedLocalPos - pEntity->GetVecOrigin()).Length2D()) > 0.75f/*1.29217472f*/) {

			Vector vecAngle;
			M::VectorAngles(pEntity->GetVecOrigin() - vecExtrapolatedLocalPos, vecAngle);

			g::vecOriginalViewAngle.y = vecAngle.y;
			g::pCmd->flForwardMove = g::pCmd->iButtons & IN_DUCK ? 450.f * 3 : 450.f;
			g::pCmd->flSideMove = 0.f;
		}
	}
}

void Friction(float flFriction, Vector* vecVelocity)
{
	static CConVar* sv_friction = i::ConVar->FindVar(XorStr("sv_friction"));
	static CConVar* sv_stopspeed = i::ConVar->FindVar(XorStr("sv_stopspeed"));
	float	speed, newspeed, control;
	float	friction;
	float	drop;

	// Calculate speed
	speed = g::pLocal->GetVelocity().Length();

	// If too slow, return
	if (speed < 0.1f)
	{
		return;
	}

	drop = 0;

	friction = sv_friction->GetFloat() * flFriction;

	control = (speed < sv_stopspeed->GetFloat()) ? sv_stopspeed->GetFloat() : speed;

	drop += control * friction * i::GlobalVars->flFrameTime;

	newspeed = speed - drop;
	if (newspeed < 0)
		newspeed = 0;

	if (newspeed != speed)
		newspeed /= speed;

	*vecVelocity -= (g::pLocal->GetVelocity() * (1.f - newspeed));
}

void misc::BlockBot(CUserCmd* pCmd) {

	static Vector vecOriginalViewAngle = Vector(0, 0, 0);
	if (!g::pLocal || !g::pLocal->IsAlive() || !cfg::misc::bBlockbot || !IPT::HandleInput(cfg::misc::iBlockbotKey)) {
		vecOriginalViewAngle = Vector(FP_ZERO, FP_ZERO, FP_ZERO);
		return;
	}
	Vector vecExtrapolatedLocalPos = (g::pLocal->GetVecOrigin() + (g::pLocal->GetVelocity() * (i::GlobalVars->flIntervalPerTick * 3)));

	if (CBaseEntity* pBlockedPlayer = g::pLocal->GetGroundEntity(); 
		pBlockedPlayer != nullptr && 
		pBlockedPlayer->IsPlayer()) {


		// allowed difference before we fall down cuz head has a bigger surface (idk why)
		// so if player is going in small circles, we won't fall (or doing small changes that could kill most blockbots)
		if (abs((vecExtrapolatedLocalPos - pBlockedPlayer->GetVecOrigin()).Length2D()) > 0.75f/*1.29217472f*/) {

			Vector vecAngle;
			M::VectorAngles(pBlockedPlayer->GetVecOrigin() - vecExtrapolatedLocalPos, vecAngle);

			g::vecOriginalViewAngle.y = vecAngle.y;
			g::pCmd->flForwardMove = pCmd->iButtons & IN_DUCK ? 450.f * 3 : 450.f;
			g::pCmd->flSideMove = 0.f; 
		}
	}
	else {

		if (vecOriginalViewAngle.IsZero()) {
			i::EngineClient->GetViewAngles(vecOriginalViewAngle);
			vecOriginalViewAngle.NormalizeAngle();
		}

		float flBestDistance = 250.f;
		CBaseEntity* pTarget = nullptr;

		for (size_t i = 1; i < i::GlobalVars->nMaxClients; i++)
		{
			CBaseEntity* pEntity = static_cast<CBaseEntity*>(i::EntityList->GetClientEntity(i));

			if (!pEntity || !pEntity->IsAlive() || pEntity->IsDormant() || pEntity == g::pLocal)
				continue;

			float flDistance = g::pLocal->GetVecOrigin().DistTo(pEntity->GetVecOrigin());

			if (flDistance < flBestDistance) {

				flBestDistance = flDistance;
				pTarget = pEntity;
			}
		}

		if (!pTarget)
			return;

		Vector vecAngle;
		M::VectorAngles(pTarget->GetVecOrigin() - vecExtrapolatedLocalPos, vecAngle);

		vecAngle.y -= vecOriginalViewAngle.y;
		vecAngle.NormalizeAngle();

		//if (!(pCmd->iButtons & IN_FORWARD))
		g::vecOriginalViewAngle = vecOriginalViewAngle;

		if (vecAngle.y < -0.75f)
			pCmd->flSideMove = 450.f;
		else if (vecAngle.y > 0.75f)
			pCmd->flSideMove = -450.f;
	}
}

void misc::ClanTag() {

	static bool bShouldPrint = true;
	INetChannelInfo* pNetChannel = i::EngineClient->GetNetChannelInfo();

	if (!pNetChannel)
		return;

	if (!bShouldPrint && !cfg::misc::bClantag)
		return;

	//std::vector<std::string> vecClantagString = util::AnimateText("RyzeXTR");

	static float flTime = 1;
	int iTicks = TIME_TO_TICKS(pNetChannel->GetAvgLatency(FLOW_OUTGOING)) + (float)i::GlobalVars->iTickCount;
	float intervals = 0.4f / i::GlobalVars->flIntervalPerTick;
	int iMainTime = (int)(iTicks / intervals) % 17; // 17

	if (iMainTime != flTime)
	{
		if (cfg::misc::bClantag) {
			bShouldPrint = true;
			if (cfg::misc::szCustomClantagBuffer.length() > 0)
			{
				const char* clanTagSteps = cfg::misc::szCustomClantagBuffer.c_str();

				util::SetClan(clanTagSteps);
			}
			else
			{
				const char* clanTagSteps[17] = {
					XorStr("R"), XorStr("TR"), XorStr("XTR"), XorStr("eXTR"), XorStr("zeXTR"), XorStr("yzeXTR"),
					XorStr("RyzeXTR"), XorStr("RyzeXTR"), XorStr("RyzeXTR"), XorStr("RyzeXTR"),
					XorStr("RyzeXT"), XorStr("RyzeX"), XorStr("Ryze"), XorStr("Ryz"), XorStr("Ry"), XorStr("R"), XorStr("")
				};
				
				util::SetClan(clanTagSteps[iMainTime]);
			}
			

			
		}
		else {
			bShouldPrint = false;
			util::SetClan(" ");
		}
	}
	flTime = iMainTime;
}

void misc::Killsay(IGameEvent* pEvent) {

	if (!cfg::misc::bKillsay)
		return;

	int iUserID = i::EngineClient->GetPlayerForUserID(pEvent->GetInt(XorStr("userid")));
	int iAttacker = i::EngineClient->GetPlayerForUserID(pEvent->GetInt(XorStr("attacker")));
	if (int iLocalIndex = i::EngineClient->GetLocalPlayer(); iUserID != iLocalIndex && iAttacker == iLocalIndex) {
		i::EngineClient->ExecuteClientCmd(std::format(XorStr("say \"{}\""), cfg::misc::szKillsayBuffer).c_str());
	}
}

void misc::ThirdPersonDisableOnDeath(IGameEvent* pEvent) {

	int iUserID = i::EngineClient->GetPlayerForUserID(pEvent->GetInt(XorStr("userid")));

	if (iUserID == i::EngineClient->GetLocalPlayer() && i::Input->bCameraInThirdPerson)
		i::Input->ToFirstPerson();
}

void misc::CapsuleHandler(IGameEvent* pEvent) {

	if (!g::pLocal || !cfg::misc::bDrawCapsule)
		return;

	int iUserID = i::EngineClient->GetPlayerForUserID(pEvent->GetInt(XorStr("userid")));
	int iAttacker = i::EngineClient->GetPlayerForUserID(pEvent->GetInt(XorStr("attacker")));
	int iHitgroup = pEvent->GetInt(XorStr("hitgroup"));
	if (int iLocalIndex = i::EngineClient->GetLocalPlayer(); iUserID != iLocalIndex && iAttacker == iLocalIndex) {
		CapsuleOnHit(iUserID, iHitgroup, Color(cfg::misc::flDrawCapsuleColor), 3.f);
	}
}

void misc::CapsuleOnHit(int pEntity, int iHitgroup, Color arrColor, float flDuration) {

	if (pEntity >= 64 || pEntity < 1)
		return;

	auto pLog = &lagcomp.GetLog(pEntity);
	if (!pLog->pEntity || pLog->pRecord.empty()) 
		return;
	
	const auto pModel = pLog->pEntity->GetModel();
	if (!pModel)
		return;

	studiohdr_t* pStudioModel = i::ModelInfo->GetStudioModel(pModel);
	if (!pStudioModel)
		return;

	for (size_t i = 0; i < pStudioModel->nBones; i++) {

		mstudiobbox_t* pHitbox = pStudioModel->GetHitbox(i, 0);
		if (!pHitbox)
			continue;

		Vector vMin, vMax;
		vMin = M::VectorTransform(pHitbox->vecBBMin, pLog->pCachedMatrix[pHitbox->iBone]);
		vMax = M::VectorTransform(pHitbox->vecBBMax, pLog->pCachedMatrix[pHitbox->iBone]);

		if (pHitbox->flRadius > -1) {
			if (pHitbox->iGroup == iHitgroup) 
				i::DebugOverlay->AddCapsuleOverlay(vMin, vMax, pHitbox->flRadius, Color(cfg::misc::flDrawCapsuleColorHit), flDuration);
			else
				i::DebugOverlay->AddCapsuleOverlay(vMin, vMax, pHitbox->flRadius, arrColor, flDuration);
		}
	}
}

std::string misc::GetHitgroupName(int iHitgroup) {

	switch (iHitgroup)
	{
	case HITGROUP_GENERIC: return XorStr("None");
	case HITGROUP_HEAD: return XorStr("Head");
	case HITGROUP_CHEST: return XorStr("Chest");
	case HITGROUP_STOMACH: return XorStr("Stomach");
	case HITGROUP_LEFTARM: return XorStr("Left arm");
	case HITGROUP_RIGHTARM: return XorStr("Right arm");
	case HITGROUP_LEFTLEG: return XorStr("Left leg");
	case HITGROUP_RIGHTLEG: return XorStr("Right leg");
	case HITGROUP_NECK: return XorStr("Neck");
	case HITGROUP_GEAR: return XorStr("Gear");
	}
}

std::string misc::GetMatrixName(int iType) {

	switch (iType)
	{
	case VISUAL: return XorStr("VISUAL");
	case RESOLVE: return XorStr("RESOLVE");
	case LEFT: return XorStr("LEFT");
	case RIGHT: return XorStr("RIGHT");
	case CENTER: return XorStr("CENTER");
	case MAX: return XorStr("MAX");
	default: return XorStr("UNKNOWN");
	}
}

void misc::LeftHandKnife() {
	
	if (!g::pLocal || !g::pLocal->IsAlive())
		return;

	static bool bSaveAgain = false;
	if (!cfg::misc::bInvertKnife) {

		bSaveAgain = true;
		return;
	}

	static CConVar* convar = i::ConVar->FindVar(XorStr("cl_righthand"));
	static int iBackupValue = convar->GetInt();
	if (bSaveAgain) {
		iBackupValue = convar->GetInt();
		bSaveAgain = false;
	}

	if (CBaseCombatWeapon* pWeapon = g::pLocal->GetWeapon(); pWeapon)
		convar->SetValue(pWeapon->IsKnife() ? iBackupValue ? 0 : 1 : iBackupValue);
}

static __declspec(naked) void __cdecl Invoke_NET_SetConVar(void* pfn, const char* cvar, const char* value)
{
	__asm
	{
		push    ebp
		mov     ebp, esp
		and esp, 0FFFFFFF8h
		sub     esp, 44h
		push    ebx
		push    esi
		push    edi
		mov     edi, cvar
		mov     esi, value
		jmp     pfn
	}
}
void DECLSPEC_NOINLINE NET_SetConVar(const char* value, const char* cvar)
{
	// "\x8D\x4C\x24\x1C\xE8\x00\x00\x00\x00\x56"
	static DWORD setaddr = (DWORD)MEM::FindPattern(ENGINE_DLL, XorStr("8D 4C 24 1C E8 ? ? ? ? 56"));
	if (setaddr != 0)
	{
		void* pvSetConVar = reinterpret_cast< void* >( setaddr );
		Invoke_NET_SetConVar( pvSetConVar, cvar, value );
	}
}

void change_name(const char* name)
{
	if (i::EngineClient->IsInGame() && i::EngineClient->IsConnected())
		NET_SetConVar(name, XorStr("name"));
}

bool misc::ChangeName(bool bReconnect, const char* szName) {

	if (!exploitInitialized && i::EngineClient->IsInGame()) {
		if (PlayerInfo_t playerInfo; g::pLocal && i::EngineClient->GetPlayerInfo(i::EngineClient->GetLocalPlayer(), &playerInfo) && (!strcmp(playerInfo.szName, "?empty") || !strcmp(playerInfo.szName, "\n\xAD\xAD\xAD"))) {
			exploitInitialized = true;
		}
		else {
			change_name(XorStr("\n\xAD\xAD\xAD"));
			return false;
		}
	}
	change_name(szName);

	return true;
}

bool misc::ResetName(bool bReconnect, const char* szName) {

	static CConVar* pNameVar = i::ConVar->FindVar(XorStr("name"));
	return ChangeName(false, pNameVar->GetString());
}

void misc::RevolverRunCommand(CBaseEntity* pEntity) {

	CBaseCombatWeapon* pCombatWeapon = pEntity->GetWeapon();
	static int iActivity = 0, iTickbase = 0;
	if (pCombatWeapon)
	{
		if (pCombatWeapon->GetItemDefinitionIndex() == WEAPON_REVOLVER)
		{
			int nActivity = pCombatWeapon->GetActivity();
			if (iActivity != nActivity)
				if (nActivity == 208)
					iTickbase = networking.GetCorrectedTickbase() + 1;

			if (nActivity == 208)
				if (iTickbase == networking.GetCorrectedTickbase() - 1)
					pCombatWeapon->GetFireReadyTime() = TICKS_TO_TIME(iTickbase) + 0.2f;

			iActivity = nActivity;
		}
	}
}

bool misc::CanFireWeapon(float curtime, bool bRevolverSecondary, bool bSkipExtraRevolverChecks)
{
	/*if ( IExploits::Get( )->m_ExploitData.m_bShifting )
		return false;*/

	if (!g::pLocal || !g::pLocal->IsAlive())
		return false;

	if (g::pLocal->GetFlags() & FL_FROZEN)
		return false;

	CBaseCombatWeapon* pWeapon = g::pLocal->GetWeapon();
	if (!pWeapon)
		return false;

	if (pWeapon->IsGrenade())
		return false;

	CCSWeaponInfo* weapon_data = pWeapon->GetCSWpnData();
	if (!weapon_data)
		return false;

	if (weapon_data->nWeaponType != WEAPONTYPE_KNIFE && pWeapon->GetAmmo() < 1)
		return false;

	float flNextAttack = g::pLocal->GetNextAttack();
	float flNextPrimaryAttack = pWeapon->GetNextPrimaryAttack();
	float flNextSecondaryAttack = pWeapon->GetNextSecondaryAttack();

	// @onetap - delay 2 ticks when using exploits
	// we don't need to delay burst shot, we don't use exploits with burst
	//if (IExploits::Get()->m_ExploitData.m_iExploitType != EXPLOIT_TYPE_NONE
	//	&& IExploits::Get()->m_ExploitData.m_bCanExploit
	//	&& IExploits::Get()->m_ExploitData.m_iTicksAllowedForProcessing >= 5
	//	&& IExploits::Get()->m_ExploitData.m_bCanExploit)
	//{
	//	//bool bWasOffensiveShift = features::exploits::m_Data.m_iCommandNumberOnOffensiveShift >= features::exploits::m_Data.m_iCommandNumberOnShift;
	//	//int iLastShiftAmount = bWasOffensiveShift ? features::exploits::m_Data.m_iLastShiftAmount [ SHIFT_TYPE_OFFENSIVE ] : features::exploits::m_Data.m_iLastShiftAmount [ SHIFT_TYPE_DEFENSIVE ];

	//	float delay_time = TICKS_TO_TIME(2 /*+ iLastShiftAmount*/);
	//	flNextAttack += delay_time;
	//	flNextPrimaryAttack += delay_time;
	//	flNextSecondaryAttack += delay_time;
	//}

	if (!(curtime >= flNextAttack))
		return false;

	//if ((pWeapon->GetItemDefinitionIndex() == WEAPON_GLOCK || pWeapon->GetItemDefinitionIndex() == WEAPON_FAMAS) && pWeapon->m_iBurstShotsRemaining() > 0) {
	//	if (curtime >= pWeapon->m_fNextBurstShot())
	//		return true;
	//}

	if (pWeapon->GetItemDefinitionIndex() == WEAPON_REVOLVER && bRevolverSecondary)
	{
		if (curtime >= flNextSecondaryAttack)
			return true;

		return false;
	}

	if (!(curtime >= flNextPrimaryAttack))
		return false;

	if (pWeapon->GetItemDefinitionIndex() != WEAPON_REVOLVER || bSkipExtraRevolverChecks)
		return true;

	// only for REVOLVER
	// @onetap
	if (!(curtime >= pWeapon->GetFireReadyTime()))
		return false;

	// only for REVOLVER
	// @onetap
	if (((CBaseEntity*)pWeapon)->GetSequence() != 5)
		return false;

	return true;
}

void misc::RevolverCreateMove() {

	if (!cfg::rage::bEnable)
		return;

	if (!g::pLocal)
		return;

	CBaseCombatWeapon* pWeapon = g::pLocal->GetWeapon();
	if (!pWeapon)
		return;

	CCSWeaponInfo* pWeaponInfo = pWeapon->GetCSWpnData();
	if (!pWeaponInfo)
		return;

	// don't remove IN_ATTACK on these weapons
	if (pWeapon->IsGrenade() || pWeapon->GetItemDefinitionIndex() == WEAPON_C4 || pWeapon->GetItemDefinitionIndex() == WEAPON_HEALTHSHOT)
		return;

	float curtime = TICKS_TO_TIME(networking.GetCorrectedTickbase());

	// we do something else for revolver
	if (pWeapon->GetItemDefinitionIndex() != WEAPON_REVOLVER)
	{
		if (!CanFireWeapon(curtime))
		{
			g::pCmd->iButtons &= ~IN_ATTACK;
		}

		return;
	}

	g::pCmd->iButtons &= ~IN_SECOND_ATTACK;

	if (!CanFireWeapon(curtime, false, true))
	{
		g::pCmd->iButtons &= ~IN_ATTACK;
		return;
	}

	if (pWeapon->GetFireReadyTime() <= curtime)
	{
		if (pWeapon->GetNextSecondaryAttack() > curtime)
		{
			g::pCmd->iButtons |= IN_SECOND_ATTACK;
		}
	}
	else
	{
		g::pCmd->iButtons |= IN_ATTACK;
	}

	/*if (g::pLocal->GetFlags() & FL_FROZEN)
		return;

	CBaseCombatWeapon* pCombatWeapon = g::pLocal->GetWeapon();
	if (!pCombatWeapon || !cfg::rage::enable || pCombatWeapon->GetItemDefinitionIndex() != WEAPON_REVOLVER)
		return;

	g::pCmd->iButtons &= ~IN_SECOND_ATTACK;
	if (g::pCmd->iButtons & IN_ATTACK)
		return;

	if (pCombatWeapon->GetFireReadyTime() > TICKS_TO_TIME(g::pLocal->GetTickBase()))
		g::pCmd->iButtons |= IN_ATTACK;*/
}

#pragma runtime_checks( "", off )
void misc::CustomBombText(const char* szText) {

	if (!g::pLocal || !g::pLocal->IsAlive())
		return;

	if (!g::pLocal->GetWeapon() || g::pLocal->GetWeapon()->GetItemDefinitionIndex() != WEAPON_C4)
		return;

	C4* pWeapon = reinterpret_cast<C4*>(reinterpret_cast<CWeaponCSBase*>(g::pLocal->GetWeapon()));
	CBaseViewModel* pViewmodel = reinterpret_cast<CBaseViewModel*>(i::EntityList->GetClientEntityFromHandle(g::pLocal->GetViewModel()));
	
	if (!pWeapon || !pViewmodel)
		return;

	pWeapon->OnFireEvent(pViewmodel, Vector(0, 0, 0), Vector(0, 0, 0), 7002, szText);
}
#pragma runtime_checks( "", restore )

void misc::Print(const std::string text, ...)
{
	va_list     list;
	int         size;
	std::string buf;

	if (text.empty())
		return;

	va_start(list, text);

	// count needed size.
	size = std::vsnprintf(0, 0, text.c_str(), list);

	// allocate.
	buf.resize(size);

	// print to buffer.
	std::vsnprintf(buf.data(), size + 1, text.c_str(), list);

	va_end(list);

	// print to console.
	i::ConVar->ConsoleColorPrintf(RYZEXCOLOR, XorStr("[RyzeX] "));
	i::ConVar->ConsoleColorPrintf(Color(240, 240, 240, 255), buf.append(XorStr("\n")).c_str());
}

void misc::FogOptions() {

	static CConVar* fog_override = i::ConVar->FindVar(XorStr("fog_override"));
	static CConVar* fog_start = i::ConVar->FindVar(XorStr("fog_start"));
	static CConVar* fog_end = i::ConVar->FindVar(XorStr("fog_end"));
	static CConVar* fog_color = i::ConVar->FindVar(XorStr("fog_color"));

	using namespace cfg::visual;

	fog_override->SetValue(bOverrideFog);
	if (!bOverrideFog)
		return;

	fog_start->SetValue(iFogStart);
	fog_end->SetValue(iFogEnd);
	fog_color->SetValue(std::format(XorStr("{} {} {}"), flFogColor[0] * 255, flFogColor[1] * 255, flFogColor[2] * 255).c_str());
}

void misc::RemoveShadows() {

	static CConVar* r_shadows = i::ConVar->FindVar("cl_csm_shadows");
	if (r_shadows)
		r_shadows->SetValue(cfg::misc::bRemovals[6] ? 0 : 1);
}

void misc::BoostMovement(CUserCmd* pCmd) {

	if (!exploits::bIsShiftingTicks)
		return;

	float& flForwardMove = pCmd->flForwardMove;
	float& flSideMove = pCmd->flSideMove;
	int& nButtons = pCmd->iButtons;

	if (flForwardMove > 5.0f) {
		nButtons |= IN_FORWARD;
		nButtons &= ~IN_BACK;

		flForwardMove = 450.0f;
	}
	else if (flForwardMove < -5.0f) {
		nButtons |= IN_BACK;
		nButtons &= ~IN_FORWARD;

		flForwardMove = -450.0f;
	}

	if (flSideMove > 5.0f) {

		nButtons |= IN_MOVERIGHT;
		nButtons &= ~IN_MOVELEFT;

		flSideMove = 450.0f;
	}
	else if (flSideMove < -5.0f) {

		nButtons |= IN_MOVELEFT;
		nButtons &= ~IN_MOVERIGHT;

		flSideMove = -450.0f;
	}

	nButtons &= ~IN_SPEED;
}

void misc::MotionBlur(CViewSetup* pSetup) {

	using namespace cfg::misc;
	if (!bMotionBlur)
		return;

	if (i::ClientState->iSignonState != SIGNONSTATE_FULL)
		return;

	if (!g::pLocal)
		return;

	static std::tuple<float, float, float, Vector, float> tupBlurHistory{};
	static std::array<float, 4U> arrMotionBlurValues = { 0.0f, 0.0f, 0.0f, 0.0f };

	if (pSetup) {

		const float flElapsedTime = i::GlobalVars->flRealTime - std::get<0>(tupBlurHistory);

		const Vector vecViewAngles = pSetup->angView;
		float flCurrentPitch = vecViewAngles.x;
		float flCurrentYaw = vecViewAngles.y;

		M::NormalizeYaw(flCurrentPitch);
		M::NormalizeYaw(flCurrentYaw);

		Vector vecCurrentSideVector;
		Vector vecCurrentForwardVector;
		Vector vecCurrentUpVector;
		M::AngleVectors(pSetup->angView, &vecCurrentForwardVector, &vecCurrentSideVector, &vecCurrentUpVector);

		Vector vecCurrentPosition = pSetup->vecOrigin;
		Vector vecPositionChange = std::get<3>(tupBlurHistory) - vecCurrentPosition;

		if (((vecPositionChange.Length() > 30.0f) && (flElapsedTime >= 0.5f)) || flElapsedTime > (1.0f / 15.0f))
			std::fill(arrMotionBlurValues.begin(), arrMotionBlurValues.end(), 0.f);
		
		else if (vecPositionChange.Length() > 50.0f) 
			std::get<4>(tupBlurHistory) = i::GlobalVars->flRealTime + 1.0f;
		
		else {

			const float flHorizontalFov = pSetup->flFOV;
			const float flVerticalFov = (pSetup->flAspectRatio <= 0.0f) ? (pSetup->flFOV) : (pSetup->flFOV / pSetup->flAspectRatio);
			const float flViewDotMotion = vecCurrentForwardVector.DotProduct(vecPositionChange);

			arrMotionBlurValues[2] = flViewDotMotion;

			const float flSideDotMotion = vecCurrentSideVector.DotProduct(vecPositionChange);
			float flYawDifference = std::get<2>(tupBlurHistory) - flCurrentYaw;
			if (((std::get<2>(tupBlurHistory) - flCurrentYaw > 180.0f) || (std::get<2>(tupBlurHistory) - flCurrentYaw < -180.0f)) &&
				((std::get<2>(tupBlurHistory) + flCurrentYaw > -180.0f) && (std::get<2>(tupBlurHistory) + flCurrentYaw < 180.0f)))
				flYawDifference = std::get<2>(tupBlurHistory) + flCurrentYaw;

			float flYawDifferenceAdjusted = flYawDifference + (flSideDotMotion / 3.0f);

			if (flYawDifference < 0.0f)
				flYawDifferenceAdjusted = std::clamp(flYawDifferenceAdjusted, flYawDifference, 0.0f);
			else
				flYawDifferenceAdjusted = std::clamp(flYawDifferenceAdjusted, 0.0f, flYawDifference);

			const float flUnDampenedYaw = flYawDifferenceAdjusted / flHorizontalFov;
			arrMotionBlurValues[0] = flUnDampenedYaw * (1.0f - (fabsf(flCurrentPitch) / 90.0f));

			const float flPitchCompensateMask = 1.0f - ((1.0f - fabsf(vecCurrentForwardVector.z)) * (1.0f - fabsf(vecCurrentForwardVector.z)));
			const float flPitchDifference = std::get<1>(tupBlurHistory) - flCurrentPitch;
			float flPitchDifferenceAdjusted = flPitchDifference;

			if (flCurrentPitch > 0.0f)
				flPitchDifferenceAdjusted = flPitchDifference - ((flViewDotMotion / 2.0f) * flPitchCompensateMask);
			else
				flPitchDifferenceAdjusted = flPitchDifference + ((flViewDotMotion / 2.0f) * flPitchCompensateMask);

			if (flPitchDifference < 0.0f)
				flPitchDifferenceAdjusted = std::clamp(flPitchDifferenceAdjusted, flPitchDifference, 0.0f);
			else
				flPitchDifferenceAdjusted = std::clamp(flPitchDifferenceAdjusted, 0.0f, flPitchDifference);

			arrMotionBlurValues[1] = flPitchDifferenceAdjusted / flVerticalFov;
			arrMotionBlurValues[3] = flUnDampenedYaw;
			arrMotionBlurValues[3] *= (fabs(flCurrentPitch) / 90.0f) * (fabs(flCurrentPitch) / 90.0f) * (fabs(flCurrentPitch) / 90.0f);

			if (flElapsedTime > 0.0f)
				arrMotionBlurValues[2] /= flElapsedTime * 30.0f;
			else
				arrMotionBlurValues[2] = 0.0f;

			arrMotionBlurValues[2] = std::clamp((fabsf(arrMotionBlurValues[2]) - flFallingMin) / (flFallingMax - flFallingMin), 0.0f, 1.0f) * (arrMotionBlurValues[2] >= 0.0f ? 1.0f : -1.0f);
			arrMotionBlurValues[2] /= 30.0f;
			arrMotionBlurValues[0] *= flRotationIntensity * .15f * flStrength;
			arrMotionBlurValues[1] *= flRotationIntensity * .15f * flStrength;
			arrMotionBlurValues[2] *= flRotationIntensity * .15f * flStrength;
			arrMotionBlurValues[3] *= flRotationIntensity * .15f * flStrength;
		}

		if (i::GlobalVars->flRealTime < std::get<4>(tupBlurHistory)) {
			arrMotionBlurValues[0] = 0.0f;
			arrMotionBlurValues[1] = 0.0f;
			arrMotionBlurValues[3] = 0.0f;
		}
		else 
			std::get<4>(tupBlurHistory) = 0.0f;
		
		std::get<0>(tupBlurHistory) = i::GlobalVars->flRealTime;
		std::get<1>(tupBlurHistory) = flCurrentPitch;
		std::get<2>(tupBlurHistory) = flCurrentYaw;
		std::get<3>(tupBlurHistory) = vecCurrentPosition;

		return;
	}

	const auto pMaterial = i::MaterialSystem->FindMaterial(("dev/motion_blur"), ("rendertargets"), false);
	if (!pMaterial) 
		return;

	IMaterialVar* const pMotionBlurInternal = pMaterial->FindVar("$motionblurinternal", nullptr, false);
	IMaterialVar* const pMotionBlurViewPortInternal = pMaterial->FindVar("$motionblurviewportinternal", nullptr, false);

	if (!pMotionBlurInternal || !pMotionBlurViewPortInternal)
		return;

	pMotionBlurInternal->SetVectorComponent(arrMotionBlurValues[0], 0);
	pMotionBlurInternal->SetVectorComponent(arrMotionBlurValues[1], 1);
	pMotionBlurInternal->SetVectorComponent(arrMotionBlurValues[2], 2);
	pMotionBlurInternal->SetVectorComponent(arrMotionBlurValues[3], 3);

	pMotionBlurViewPortInternal->SetVectorComponent(0.0f, 0);
	pMotionBlurViewPortInternal->SetVectorComponent(0.0f, 1);
	pMotionBlurViewPortInternal->SetVectorComponent(1.0f, 2);
	pMotionBlurViewPortInternal->SetVectorComponent(1.0f, 3);

	static uint8_t* uDrawScreenEffectsSig = (MEM::FindPattern(CLIENT_DLL, "55 8B EC 83 E4 F8 83 EC 1C 53 56 57 8D"));
	if (!uDrawScreenEffectsSig)
		return;

	int iWidth, iHeight;
	i::EngineClient->GetScreenSize(iWidth, iHeight);

	__asm
	{
		push iHeight
		push iWidth
		push 0
		xor edx, edx
		mov ecx, pMaterial
		call uDrawScreenEffectsSig
		add esp, 12
	}

}