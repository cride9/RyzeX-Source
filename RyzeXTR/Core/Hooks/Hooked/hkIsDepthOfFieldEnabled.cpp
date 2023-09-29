#include "../hooks.h"
#include "../../Features/Misc/misc.h"

bool __fastcall h::hkIsDepthOfFieldEnabled() {

	misc::MotionBlur(nullptr);
	return false;
}