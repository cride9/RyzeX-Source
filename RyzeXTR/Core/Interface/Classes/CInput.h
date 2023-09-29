#pragma once
#include "../../SDK/CUserCmd.h"

class CInput {

public:

	std::byte			pad0[0xC];				// 0x00
	bool				bTrackIRAvailable;		// 0x0C
	bool				bMouseInitialized;		// 0x0D
	bool				bMouseActive;			// 0x0E
	std::byte			pad1[0x9A];				// 0x0F
	bool				bCameraInThirdPerson;	// 0xA9
	std::byte			pad2[0x2];				// 0xAA
	Vector				vecCameraOffset;		// 0xAC
	std::byte			pad3[0x38];				// 0xB8
	CUserCmd* pCommands;				// 0xF0
	CVerifiedUserCmd* pVerifiedCommands;		// 0xF4

	CUserCmd* GetUserCmd(const int nSequenceNumber)
	{
		return &pCommands[nSequenceNumber % 150];
	}

	CVerifiedUserCmd* GetVerifiedCmd(const int nSequenceNumber)
	{
		return &pVerifiedCommands[nSequenceNumber % 150];
	}

	void ToThirdPerson()
	{
		return util::GetVFunc< void(__thiscall*)(decltype(this)) >(this, 35)(this);
	}

	void ToFirstPerson()
	{
		return util::GetVFunc< void(__thiscall*)(decltype(this)) >(this, 36)(this);
	}

	int CameraInThirdPerson(int slot = -1)
	{
		return util::GetVFunc< int(__thiscall*)(decltype(this), int) >(this, 32)(this, slot);
	}
};