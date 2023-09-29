#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <cstdint>
#include <thread>
#include <iostream>
#include "Hooks/hooks.h"
#include "SDK/Menu/gui.h"
#include "Interface/interfaces.h"
#include "SDK/NetVar/Netvar.h"
#include "hungarians.h"
#include "globals.h"
#include "SDK/math.h"
#include "SDK/Menu/config.h"
#include <mmsystem.h>
#include "memeSounds.h"
#include "SDK/InputSystem.h"
#include "Features/Rage/Animations/LocalAnimation.h"
#include "Features/Changers/SkinChanger.h"
#include "Hooks/Proxies/ProxyHooks.h"
#include "Features/Misc/misc.h"
#include "../Dependecies/BASS/API.h"
#include "SDK/Memory.h"
#include "xorstr.h"
#include "../Core/Features/Rage/Animations/EnemyAnimations.h"
#include "Lua/Lua.h"
#include "Features/Misc/Vizualizer.h"

DWORD WINAPI CheatThread(PVOID);

BOOL WINAPI DllMain(HINSTANCE hinstDLL, DWORD fdwReason, LPVOID lpReserved)
{
	if (fdwReason == DLL_PROCESS_ATTACH) {

		static auto InitSafeModuleFn = (void(__fastcall*)(void*, void*))MEM::FindPattern(CLIENT_DLL, XorStr("56 8B 71 3C B8"));
		if (InitSafeModuleFn)
			InitSafeModuleFn((void*)hinstDLL, nullptr);

		DisableThreadLibraryCalls(hinstDLL);

		auto thread = CreateThread(nullptr, 0, CheatThread, hinstDLL, 0, nullptr);

		if (thread)
			CloseHandle(thread);

		auto vizualizerThread = CreateThread(nullptr, 0, reinterpret_cast<LPTHREAD_START_ROUTINE>(vizualizer::Initialize), nullptr, 0, nullptr);

		if (vizualizerThread)
			CloseHandle(vizualizerThread);
	}

	return TRUE;
}

void OpenConsole() {

	AllocConsole();
	freopen_s((FILE**)stdin, XorStr("CONIN$"), XorStr("r"), stdin);
	freopen_s((FILE**)stdout, XorStr("CONOUT$"), XorStr("w"), stdout);
	SetConsoleTitleA(XorStr("Alpha Console"));
}

void CloseConsole() {

	fclose((FILE*)stdin);
	fclose((FILE*)stdout);

	HWND hw_ConsoleHwnd = GetConsoleWindow();
	FreeConsole();
	PostMessageW(hw_ConsoleHwnd, WM_CLOSE, 0, 0);
}

void SetupFonts() {

	g::fonts::NameESP = i::Surface->CreateFontGame();
	g::fonts::HealthESP = i::Surface->CreateFontGame();
	g::fonts::FlagESP = i::Surface->CreateFontGame();
	g::fonts::SkeetFont = i::Surface->CreateFontGame();
	g::fonts::DebugFont = i::Surface->CreateFontGame();

	i::Surface->SetFontGlyphSet(g::fonts::NameESP, XorStr("Verdana"), 12, FW_NORMAL, 0, 0, FONTFLAG_OUTLINE);
	i::Surface->SetFontGlyphSet(g::fonts::HealthESP, XorStr("Verdana"), 10, FW_NORMAL, 0, 0, FONTFLAG_OUTLINE);
	i::Surface->SetFontGlyphSet(g::fonts::FlagESP, XorStr("Small Fonts"), 11, FW_NORMAL, 0, 0, FONTFLAG_OUTLINE);
	i::Surface->SetFontGlyphSet(g::fonts::SkeetFont, XorStr("Verdana"), 25, FW_EXTRABOLD, 0, 0, FONTFLAG_DROPSHADOW | FONTFLAG_ANTIALIAS);
	i::Surface->SetFontGlyphSet(g::fonts::DebugFont, XorStr("Small Fonts"), 16, FW_NORMAL, 0, 0, FONTFLAG_OUTLINE);

	util::Print(XorStr("Fonts initialized"));
}

DWORD WINAPI CheatThread(PVOID hinstDLL) {

	while (!MEM::GetModuleBaseHandle(XorStr(L"serverbrowser.dll")))
		Sleep(200);

#if _DEBUG
	OpenConsole();
#endif

	MEM::Setup();
	i::SetupInterfaces();
	SetupFonts();
	n::SetupNetvars();
	//PlaySound(reinterpret_cast<LPCSTR>(memeSound::BUWAWA), NULL, SND_MEMORY | SND_ASYNC);
	skinChanger.Dump();
	menu::Setup();
	M::Setup();
	// Funny scripts 
	h::SetupHooks();
	//g::entityListener.Setup();
	IPT::Setup();
	p::Setup();

	g::bStartWelcome = true;
	misc::SetupRadio();
	// destroy radio
	BASS::bass_init = FALSE;
	BASS_Stop( );
	BASS_StreamFree( BASS::stream_handle );
	BASS_Free( );

	//g::entityListener.Destroy();
	menu::open = false;
	h::DestroyHooks();
	menu::Destroy();
	i::EngineClient->ClientCmdUnrestricted(XorStr("cl_fullupdate"));
	IPT::Restore( );
	p::Destroy();

	delete pConfig;
	delete localAnim;

#if NDEBUG || ALPHA

	while (true && !cfg::bDoUnload)
		Sleep(5000);
#endif

#if _DEBUG
	CloseConsole();
#endif

	FreeLibraryAndExitThread(static_cast<HMODULE>(hinstDLL), 0);

	return TRUE;
}
