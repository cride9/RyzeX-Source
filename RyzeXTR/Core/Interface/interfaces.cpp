#include "interfaces.h"
#include "Classes/CCSGameRulesProxy.h"
#include <stdexcept>

void i::SetupInterfaces() {

	ClientDll = GetInterface<IBaseClientDLL>(CLIENT_DLL, XorStr("VClient018"));
	EntityList = GetInterface<IClientEntityList>(CLIENT_DLL, XorStr("VClientEntityList003"));
	EngineClient = GetInterface<IVEngineClient>(ENGINE_DLL, XorStr("VEngineClient014"));
	Surface = GetInterface<ISurface>(VGUIMATSURFACE_DLL, XorStr("VGUI_Surface031"));
	Panel = GetInterface<IPanel>(VGUI2_DLL, XorStr("VGUI_Panel009"));
	StudioRender = GetInterface<IStudioRender>(STUDIORENDERER_DLL, XorStr("VStudioRender026"));
	Prediction = GetInterface<IPrediction>(CLIENT_DLL, XorStr("VClientPrediction001"));
	GameMovement = GetInterface<IGameMovement>(CLIENT_DLL, XorStr("GameMovement001"));
	EngineVGui = GetInterface<IEngineVGui>(ENGINE_DLL, XorStr("VEngineVGui001"));
	ModelRender = GetInterface<IVModelRender>(ENGINE_DLL, XorStr("VEngineModel016"));
	DebugOverlay = GetInterface<IVDebugOverlay>(ENGINE_DLL, XorStr("VDebugOverlay004"));
	MaterialSystem = GetInterface<IMaterialSystem>(MATERIALSYSTEM_DLL, XorStr("VMaterialSystem080"));
	ModelInfo = GetInterface<IVModelInfo>(ENGINE_DLL, XorStr("VModelInfoClient004"));
	ConVar = GetInterface<IConVar>(VSTDLIB_DLL, XorStr("VEngineCvar007"));
	EngineTrace = GetInterface<IEngineTrace>(ENGINE_DLL, XorStr("EngineTraceClient004"));
	PhysicsProps = GetInterface<IPhysicsSurfaceProps>(VPHYSICS_DLL, XorStr("VPhysicsSurfaceProps001"));
	GameEvent = GetInterface<IGameEventManager>(ENGINE_DLL, XorStr("GAMEEVENTSMANAGER002"));
	EngineSoundClient = GetInterface<IEngineSoundClient>(ENGINE_DLL, XorStr("IEngineSoundClient003"));
	InputSystem = GetInterface<IInputSystem>(INPUTSYSTEM_DLL, XorStr("InputSystemVersion001"));
	MDLCache = GetInterface<IMDLCache>(DATACACHE_DLL, XorStr("MDLCache004"));
	Localize = GetInterface<ILocalize>(LOCALIZE_DLL, XorStr("Localize_001"));
	Effects = GetInterface<IVEffects>(ENGINE_DLL, XorStr("VEngineEffects001"));
	StringContainer = GetInterface<CNetworkStringTableContainer>(ENGINE_DLL, XorStr("VEngineClientStringTable001"));
	LeafSystem = GetInterface<IClientLeafSystem>(CLIENT_DLL, XorStr("ClientLeafSystem002"));

	ClientState = **reinterpret_cast<CClientState***>(MEM::FindPattern(ENGINE_DLL, XorStr("A1 ? ? ? ? 8B 88 ? ? ? ? 85 C9 75 07")) + 0x1);
	if (ClientState == nullptr)
		throw std::runtime_error(XorStr("Failed to get CClientState"));

	KeyValuesSystem = reinterpret_cast<CKeyValuesSystem * (__cdecl*)()>(MEM::GetExportAddress(MEM::GetModuleBaseHandle(VSTDLIB_DLL), XorStr("KeyValuesSystem")))();
	if (KeyValuesSystem == nullptr)
		throw std::runtime_error(XorStr("Failed to get CKeyValuesSystem"));

	DirectDevice = **reinterpret_cast<IDirect3DDevice9***>(MEM::FindPattern(XorStr(L"shaderapidx9.dll"), XorStr("A1 ? ? ? ? 50 8B 08 FF 51 0C")) + 0x1); // @xref: "HandleLateCreation"
	if ( DirectDevice == nullptr )
		throw std::runtime_error(XorStr("Failed to get DirectDevice"));

	Input = *reinterpret_cast<CInput**>(MEM::FindPattern(CLIENT_DLL, XorStr("B9 ? ? ? ? F3 0F 11 04 24 FF 50 10")) + 0x1);
	if (Input == nullptr)
		throw std::runtime_error(XorStr("Failed to get CInput"));

	ClientModeShared = **reinterpret_cast<CClientModeShared***>(MEM::GetVFunc<std::uintptr_t>(ClientDll, 10) + 0x5);
	if (ClientModeShared == nullptr)
		throw std::runtime_error(XorStr("Failed to get CClientModeShared"));

	GlobalVars = **reinterpret_cast<CGlobalVarsBase***>(MEM::GetVFunc<std::uintptr_t>(ClientDll, 11) + 0xA);
	if (GlobalVars == nullptr)
		throw std::runtime_error(XorStr("Failed to get CGlobalVarsBase"));

	GlowObjectManager = *reinterpret_cast<IGlowObjectManager**>(MEM::FindPattern(CLIENT_DLL, XorStr("0F 11 05 ? ? ? ? 83 C8 01")) + 0x3);
	if (GlowObjectManager == nullptr)
		throw std::runtime_error(XorStr("Failed to get IGlowObjectManager"));

	RenderBeam = *reinterpret_cast<IViewRenderBeams**>(MEM::FindPattern(CLIENT_DLL, XorStr("B9 ? ? ? ? A1 ? ? ? ? FF 10 A1 ? ? ? ? B9")) + 1);
	if (RenderBeam == nullptr)
		throw std::runtime_error(XorStr("Failed to get IViewRenderBeams"));

	ClientMode = **reinterpret_cast<IClientMode***>((*reinterpret_cast<uintptr_t**>(ClientDll))[10] + 0x5);
	if (ClientMode == nullptr)
		throw std::runtime_error(XorStr("Failed to get IClientMode"));

	WeaponSystem = *(IWeaponSystem**)(MEM::FindPattern(CLIENT_DLL, XorStr("8B 35 ? ? ? ? FF 10 0F B7 C0")) + 2);
	if (WeaponSystem == nullptr)
		throw std::runtime_error(XorStr("Failed to get IWeaponSystem"));

	GameRules = *(CCSGameRulesProxy***)((DWORD)(MEM::FindPattern(CLIENT_DLL, XorStr("A1 ? ? ? ? 85 C0 0F 84 ? ? ? ? 80 B8 ? ? ? ? ? 74 7A"))) + 0x1);
	if (GameRules == nullptr)
		throw std::runtime_error(XorStr("Failed to get IGameRules"));

	MemAlloc = *static_cast<IMemAlloc**>(MEM::GetExportAddress(MEM::GetModuleBaseHandle(XorStr(L"tier0.dll")), XorStr("g_pMemAlloc")));
	if (MemAlloc == nullptr)
		throw std::runtime_error(XorStr("Failed to get IMemAlloc"));

	//ItemSystem = util::GetAbsoluteAddress<decltype(ItemSystem)>(util::FindSignature(CLIENT_DLL, "E8 ? ? ? ? 0F B7 0F") + 1);
	//if (ItemSystem == nullptr)
	//	throw std::runtime_error("Failed to get ItemSystem");

	util::Print(XorStr("Interfaces initialized!"));
}