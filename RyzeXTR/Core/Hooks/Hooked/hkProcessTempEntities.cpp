#include "../hooks.h"

bool __fastcall h::hkProcessTempEntities(void* ecx, void* edx, void* pMsg) {

	int iBackup = i::GlobalVars->nMaxClients;
	
	i::GlobalVars->nMaxClients = 1;
	bool bResult = detour::processTempEntites.GetOriginal<decltype(&hkProcessTempEntities)>()(ecx, edx, pMsg);
	i::GlobalVars->nMaxClients = iBackup;

	return bResult;
}