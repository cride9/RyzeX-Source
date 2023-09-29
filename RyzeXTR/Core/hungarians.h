#pragma once
#define DLL_FOLYAMAT_CSATOLAS   1    
#define DLL_CERNA_CSATOLAS    2    
#define DLL_CERNA_LECSATOLAS    3    
#define DLL_FOLYAMAT_LECSATOLAS   0    
#define NORMALHIVAS __stdcall
#define IGAZ 1
#define HAMIS 0

using HPELDA = HINSTANCE;
using DSZO = DWORD;
using LPURES = LPVOID;
using PURES = PVOID;
using IGAZHAMIS = BOOL;
using HEGYSEG = HMODULE;
using FOGANTYU = HANDLE;

void Szalkonyvtarhivasokletiltasa(HEGYSEG peldaDll) {

	DisableThreadLibraryCalls(peldaDll);
}

FOGANTYU CsinaljSzalat(LPTHREAD_START_ROUTINE lpKezdoCim, LPURES lpTenyezo) {

	return CreateThread(nullptr, 0, lpKezdoCim, lpTenyezo, 0, nullptr);
}

void FoganytuBezaras(FOGANTYU hFogantyu) {

	CloseHandle(hFogantyu);
}