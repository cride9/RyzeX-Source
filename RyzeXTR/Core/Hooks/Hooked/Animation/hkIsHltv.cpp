#include "../../hooks.h"
#include "../../../SDK/Entity.h"

bool __fastcall h::hkIsHltv(void* entityPointer, void* edx) {

	static auto original = detour::isHltv.GetOriginal<decltype(&h::hkIsHltv)>();

	if (i::ClientState->iSignonState != SIGNONSTATE_FULL)
		return original( entityPointer, edx);

	CBaseEntity* pEntity = static_cast<CBaseEntity*>(entityPointer);

	if (!pEntity || !pEntity->IsAlive())
		return original( entityPointer, edx );

	static const auto retVelocity = reinterpret_cast<void*>(MEM::FindPattern(CLIENT_DLL, XorStr("84 C0 75 38 8B 0D ? ? ? ? 8B 01 8B 80")));
	static const auto retLayers = reinterpret_cast<void*>(MEM::FindPattern(CLIENT_DLL, XorStr("84 C0 75 0D F6 87")));

	if (auto retAddress = _AddressOfReturnAddress(); retAddress == retVelocity || retAddress == retLayers)
		return true;

	return original( entityPointer, edx);
}