#pragma once
#include <cstdint>
#include "CRC32.h"
#include "DataTyes/Vector.h"
#include <../lua/embedding/sol/sol.hpp>

#define XorStr(str)(str)

enum ECommandButtons : int
{
	IN_ATTACK = (1 << 0),
	IN_JUMP = (1 << 1),
	IN_DUCK = (1 << 2),
	IN_FORWARD = (1 << 3),
	IN_BACK = (1 << 4),
	IN_USE = (1 << 5),
	IN_CANCEL = (1 << 6),
	IN_LEFT = (1 << 7),
	IN_RIGHT = (1 << 8),
	IN_MOVELEFT = (1 << 9),
	IN_MOVERIGHT = (1 << 10),
	IN_SECOND_ATTACK = (1 << 11),
	IN_RUN = (1 << 12),
	IN_RELOAD = (1 << 13),
	IN_LEFT_ALT = (1 << 14),
	IN_RIGHT_ALT = (1 << 15),
	IN_SCORE = (1 << 16),
	IN_SPEED = (1 << 17),
	IN_WALK = (1 << 18),
	IN_ZOOM = (1 << 19),
	IN_FIRST_WEAPON = (1 << 20),
	IN_SECOND_WEAPON = (1 << 21),
	IN_BULLRUSH = (1 << 22),
	IN_FIRST_GRENADE = (1 << 23),
	IN_SECOND_GRENADE = (1 << 24),
	IN_MIDDLE_ATTACK = (1 << 25),
	IN_USE_OR_RELOAD = (1 << 26)
};

class CUserCmd
{
public:
	virtual			~CUserCmd() { }		// 0x00
	int				iCommandNumber;		// 0x04
	int				iTickCount;			// 0x08
	Vector			angViewPoint;		// 0x0C
	Vector			vecAimDirection;	// 0x18
	float			flForwardMove;		// 0x24
	float			flSideMove;			// 0x28
	float			flUpMove;			// 0x2C
	int				iButtons;			// 0x30
	std::uint8_t	uImpulse;			// 0x34
	int				iWeaponSelect;		// 0x38
	int				iWeaponSubType;		// 0x3C
	int				iRandomSeed;		// 0x40
	short			sMouseDeltaX;		// 0x44
	short			sMouseDeltaY;		// 0x46
	bool			bHasBeenPredicted;	// 0x48
	Vector			vecHeadAngles;		// 0x4C
	Vector			vecHeadOffset;		// 0x58



	[[nodiscard]] CRC32_t GetChecksum() const
	{
		CRC32_t uHashCRC = 0UL;

		CRC32::Init(&uHashCRC);
		CRC32::ProcessBuffer(&uHashCRC, &iCommandNumber, sizeof(iCommandNumber));
		CRC32::ProcessBuffer(&uHashCRC, &iTickCount, sizeof(iTickCount));
		CRC32::ProcessBuffer(&uHashCRC, &angViewPoint, sizeof(angViewPoint));
		CRC32::ProcessBuffer(&uHashCRC, &vecAimDirection, sizeof(vecAimDirection));
		CRC32::ProcessBuffer(&uHashCRC, &flForwardMove, sizeof(flForwardMove));
		CRC32::ProcessBuffer(&uHashCRC, &flSideMove, sizeof(flSideMove));
		CRC32::ProcessBuffer(&uHashCRC, &flUpMove, sizeof(flUpMove));
		CRC32::ProcessBuffer(&uHashCRC, &iButtons, sizeof(iButtons));
		CRC32::ProcessBuffer(&uHashCRC, &uImpulse, sizeof(uImpulse));
		CRC32::ProcessBuffer(&uHashCRC, &iWeaponSelect, sizeof(iWeaponSelect));
		CRC32::ProcessBuffer(&uHashCRC, &iWeaponSubType, sizeof(iWeaponSubType));
		CRC32::ProcessBuffer(&uHashCRC, &iRandomSeed, sizeof(iRandomSeed));
		CRC32::ProcessBuffer(&uHashCRC, &sMouseDeltaX, sizeof(sMouseDeltaX));
		CRC32::ProcessBuffer(&uHashCRC, &sMouseDeltaY, sizeof(sMouseDeltaY));
		CRC32::Final(&uHashCRC);

		return uHashCRC;
	}

	// exclusive lua only
	sol::object _lua_get( sol::stack_object key, sol::this_state L ) {
		// we use stack_object for the arguments because we
		// know the values from Lua will remain on Lua's stack,
		// so long we we don't mess with it
		auto maybe_string_key
			= key.as< sol::optional< std::string > >( );
		if (maybe_string_key) {
			const std::string& k = *maybe_string_key;
			if (k == XorStr( "iCommandNumber" )) { return sol::object( L, sol::in_place, this->iCommandNumber ); }
			else if (k == XorStr( "iTickCount" )) { return sol::object( L, sol::in_place, this->iTickCount ); }
			else if (k == XorStr( "angViewPoint" )) { return sol::object( L, sol::in_place, this->angViewPoint ); }
			else if (k == XorStr( "vecAimDirection" )) { return sol::object( L, sol::in_place, this->vecAimDirection ); }
			else if (k == XorStr( "flForwardMove" )) { return sol::object( L, sol::in_place, this->flForwardMove ); }
			else if (k == XorStr( "flSideMove" )) { return sol::object( L, sol::in_place, this->flSideMove ); }
			else if (k == XorStr( "flUpMove" )) { return sol::object( L, sol::in_place, this->flUpMove ); }
			else if (k == XorStr( "iButtons" )) { return sol::object( L, sol::in_place, this->iButtons ); }
			else if (k == XorStr( "uImpulse" )) { return sol::object( L, sol::in_place, this->uImpulse ); }
			else if (k == XorStr( "iWeaponSelect" )) { return sol::object( L, sol::in_place, this->iWeaponSelect ); }
			else if (k == XorStr( "iWeaponSubType" )) { return sol::object( L, sol::in_place, this->iWeaponSubType ); }
			else if (k == XorStr( "iRandomSeed" )) { return sol::object( L, sol::in_place, this->iRandomSeed ); }
			else if (k == XorStr( "sMouseDeltaX" )) { return sol::object( L, sol::in_place, this->sMouseDeltaX ); }
			else if (k == XorStr( "sMouseDeltaY" )) { return sol::object( L, sol::in_place, this->sMouseDeltaY ); }
			else if (k == XorStr( "bHasBeenPredicted" )) { return sol::object( L, sol::in_place, this->bHasBeenPredicted ); }
			else if (k == XorStr( "vecHeadAngles" )) { return sol::object( L, sol::in_place, this->vecHeadAngles ); }
			else if (k == XorStr( "vecHeadOffset" )) { return sol::object( L, sol::in_place, this->vecHeadOffset ); }
		}

		// No valid key: push nil
		return sol::object( L, sol::in_place, sol::lua_nil );
	}

	// exclusive lua only
	void _lua_set( sol::stack_object key, sol::stack_object value,
		sol::this_state ) {
		// we use stack_object for the arguments because we
		// know the values from Lua will remain on Lua's stack,
		// so long we we don't mess with it
		auto maybe_string_key
			= key.as< sol::optional< std::string > >( );
		if (maybe_string_key) {
			const std::string& k = *maybe_string_key;

			if (k == XorStr( "iCommandNumber" )) { this->iCommandNumber = value.as< int >( ); }
			else if (k == XorStr( "iTickCount" )) { this->iTickCount = value.as< int >( ); }
			else if (k == XorStr( "angViewPoint" )) { this->angViewPoint = value.as< Vector >( ); }
			else if (k == XorStr( "vecAimDirection" )) { this->vecAimDirection = value.as< Vector >( ); }
			else if (k == XorStr( "flForwardMove" )) { this->flForwardMove = value.as< float >( ); }
			else if (k == XorStr( "flSideMove" )) { this->flSideMove = value.as< float >( ); }
			else if (k == XorStr( "flUpMove" )) { this->flUpMove = value.as< float >( ); }
			else if (k == XorStr( "iButtons" )) { this->iButtons = value.as< int >( ); }
			else if (k == XorStr( "uImpulse" )) { this->uImpulse = value.as< char >( ); }
			else if (k == XorStr( "iWeaponSelect" )) { this->iWeaponSelect = value.as< int >( ); }
			else if (k == XorStr( "iWeaponSubType" )) { this->iWeaponSubType = value.as< int >( ); }
			else if (k == XorStr( "iRandomSeed" )) { this->iRandomSeed = value.as< int >( ); }
			else if (k == XorStr( "sMouseDeltaX" )) { this->sMouseDeltaX = value.as< short >( ); }
			else if (k == XorStr( "sMouseDeltaY" )) { this->sMouseDeltaY = value.as< short >( ); }
			else if (k == XorStr( "bHasBeenPredicted" )) { this->bHasBeenPredicted = value.as< bool >( ); }
			else if (k == XorStr( "vecHeadAngles" )) { this->vecHeadAngles = value.as< Vector >( ); }
			else if (k == XorStr( "vecHeadOffset" )) { this->vecHeadOffset = value.as< Vector >( ); }
		}
	}
};

class CVerifiedUserCmd
{
public:
	CUserCmd	userCmd;	// 0x00
	CRC32_t		uHashCRC;	// 0x64
};

class CCommandContext
{
public:
	bool bNeedsProcessing;
	CUserCmd pCmd;
	int nCommandNumber;
};