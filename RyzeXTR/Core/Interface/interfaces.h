#pragma once
#include <Windows.h>
#include "../utilities.h"
#include "../SDK/Memory.h"

// Interfaces aka "abstract_classes" inside csgo leaked source
#include "Interfaces/IBaseClientDLL.h"
#include "Interfaces/IClientEntityList.h"
#include "Interfaces/IVEngineClient.h"
#include "Interfaces/ISurface.h"
#include "Interfaces/IPanel.h"
#include "Interfaces/IStudioRender.h"
#include "Interfaces/IPrediction.h"
#include "Interfaces/IEngineVGui.h"
#include "Interfaces/IMaterial.h"
#include "Interfaces/IVModelRender.h"
#include "Interfaces/IVDebugOverlay.h"
#include "Interfaces/IClientMode.h"
#include "Interfaces/IMaterialSystem.h"
#include "Interfaces/IMatRenderContext.h"
#include "Interfaces/IEngineTrace.h"
#include "Interfaces/IWeaponSystem.h"
#include "Interfaces/ISurfaceData.h"
#include "Interfaces/IGameEventManager.h"
#include "Interfaces/IGlowObjectManager.h"
#include "Interfaces/IEngineSoundClient.h"
#include "Interfaces/IViewRenderBeams.h"
#include "Interfaces/IInputSystem.h"
#include "Interfaces/IMdlCache.h"
#include "Interfaces/CLocalize.h"
#include "Interfaces/IItemSystem.h"
#include "Interfaces/IVEngineEffects.h"
#include "Interfaces/CNetworkStringTableContainer.h"
#include "Interfaces/IClientLeafSystem.h"
#include "Interfaces/MemAlloc.h"

// Classes aka that I can't find as interface in the source kbxdbdxjk
#include "Classes/CClientState.h"
#include "Classes/CKeyValuesSystem.h"
#include "Classes/CInput.h"
#include "Classes/CClientModeShared.h"
#include "Classes/CGlobalVarsBase.h"
#include "Classes/CStudio.h"
#include "Classes/CModelInfo.h"
#include "Classes/CTexture.h"
#include "Classes/CConvar.h"

namespace i {

	inline IBaseClientDLL* ClientDll;		// VClient018
	inline IClientEntityList* EntityList;	// VClientEntityList003
	inline IVEngineClient* EngineClient;	// VEngineClient014
	inline ISurface* Surface;				// VGUI_Surface031
	inline IPanel* Panel;					// VGUI_Panel009
	inline IStudioRender* StudioRender;		// VStudioRender026
	inline IPrediction* Prediction;			// VClientPrediction001
	inline IGameMovement* GameMovement;		// GameMovement001
	inline IEngineVGui* EngineVGui;			// VEngineVGui001
	inline IVModelRender* ModelRender;		// VEngineModel016
	inline IVDebugOverlay* DebugOverlay;	// VDebugOverlay004
	inline IClientMode* ClientMode;			// g_pClientDll[10] + 0x5
	inline IMaterialSystem* MaterialSystem;	// materialsystem.dll -> VMaterialSystem080
	inline IVModelInfo* ModelInfo;			// engine.dll -> VModelInfoClient004
	inline IConVar* ConVar;					// 
	inline IEngineTrace* EngineTrace;		// engine.dll -> EngineTraceClient004
	inline IWeaponSystem* WeaponSystem;		// 
	inline IPhysicsSurfaceProps* PhysicsProps;
	inline IGameEventManager* GameEvent;	// engine.dll -> GAMEEVENTSMANAGER002
	inline IGlowObjectManager* GlowObjectManager;
	inline IEngineSoundClient* EngineSoundClient;
	inline IViewRenderBeams* RenderBeam;
	inline IInputSystem* InputSystem;
	inline IMDLCache* MDLCache;
	inline ILocalize* Localize;
	inline IVEffects* Effects;				// engine.dll -> VEngineEffects001
	inline CNetworkStringTableContainer* StringContainer;
	inline IClientLeafSystem* LeafSystem;
	inline IMemAlloc* MemAlloc;

	inline IDirect3DDevice9* DirectDevice;
	inline IMoveHelper* MoveHelper;			// Runcommand parameter 

	inline CClientState* ClientState;
	inline CKeyValuesSystem* KeyValuesSystem;
	inline CInput* Input;
	inline CClientModeShared* ClientModeShared;
	inline CGlobalVarsBase* GlobalVars;

	inline std::add_pointer_t<IItemSystem* __cdecl()> ItemSystem;

	template <typename T>
	T* GetInterface(const wchar_t* szLib, const char* szName) {

		const auto moduleHandle = MEM::GetModuleBaseHandle(szLib);

		if (!moduleHandle) {

			util::Print("Failed to get interface: ", szName);
			return nullptr;
		}

		const auto funcAddress = MEM::GetExportAddress(moduleHandle, "CreateInterface");

		if (!funcAddress) {

			util::Print("Failed to get interface: ", szName);
			return nullptr;
		}		

		using Fn = T * (*)(const char*, int*);
		const auto StolenFunction = reinterpret_cast<Fn>(funcAddress);

		auto pBackupValue = StolenFunction(szName, nullptr);

		if (pBackupValue)
			util::Print("Found Interface: ", szName);
		else
			util::Print("Failed to get interface: ", szName);

		return pBackupValue;
	}

	void SetupInterfaces();
}