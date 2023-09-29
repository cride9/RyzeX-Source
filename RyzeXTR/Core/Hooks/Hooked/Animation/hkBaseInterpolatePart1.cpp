#include "../../hooks.h"
#include "../../../Features/Rage/Animations/Lagcompensation.h"

enum
{
	INTERPOLATE_STOP = 0,
	INTERPOLATE_CONTINUE
};

int __fastcall h::hkBaseInterpolatePart1(CBaseEntity* pEntity, void* edx, float& currentTime, Vector& oldOrigin, Vector& oldAngles, int& bNoMoreChanges) {

	static auto original = detour::baseInterpolatePart1.GetOriginal<decltype(&hkBaseInterpolatePart1)>();

	if (!pEntity->IsPlayer())
		return original(pEntity, edx, currentTime, oldOrigin, oldAngles, bNoMoreChanges);

	using MoveToLastReceivedPositionFn = int(__thiscall*)(CBaseEntity*, char);
	static auto oMoveToLastReceivedPosition = reinterpret_cast<MoveToLastReceivedPositionFn>(MEM::FindPattern(CLIENT_DLL, XorStr("55 8B EC 51 53 56 8B F1 32 DB 8B 06")));
	oMoveToLastReceivedPosition(pEntity, 0);

	bNoMoreChanges = 2;
	return INTERPOLATE_STOP;

	//lagcomp.SetInterpolationFlags();
	return original(pEntity, edx, currentTime, oldOrigin, oldAngles, bNoMoreChanges);
}