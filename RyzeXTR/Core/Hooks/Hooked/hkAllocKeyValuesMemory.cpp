#include "../hooks.h"

void* __fastcall h::hkAllocKeyValuesMemory(CKeyValuesSystem* thisptr, int edx, int iSize) {

	static auto original = detour::allocKeyValues.GetOriginal<decltype(&h::hkAllocKeyValuesMemory)>();

	static const auto uAllocKeyValuesEngine = MEM::GetAbsoluteAddress(MEM::FindPattern(ENGINE_DLL, XorStr("E8 ? ? ? ? 83 C4 08 84 C0 75 10 FF 75 0C")) + 0x1) + 0x4A;
	static const auto uAllocKeyValuesClient = MEM::GetAbsoluteAddress(MEM::FindPattern(CLIENT_DLL, XorStr("E8 ? ? ? ? 83 C4 08 84 C0 75 10")) + 0x1) + 0x3E;

	if (const std::uint8_t* uReturnAddress = reinterpret_cast<std::uint8_t*>(_ReturnAddress()); uReturnAddress == uAllocKeyValuesEngine || uReturnAddress == uAllocKeyValuesClient)
		return nullptr;

	return original(thisptr, edx, iSize);
}