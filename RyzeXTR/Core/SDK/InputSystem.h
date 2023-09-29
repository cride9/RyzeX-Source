#pragma once
// used: std::array
#include <array>
#include <Windows.h>

#include "../SDK/Menu/config.h"

enum class EKeyState : int
{
	NONE,
	DOWN,
	UP,
	RELEASED
};

enum EKeyInputType : int
{
	NO,
	HOLD,
	TOGGLE,
	ALWAYS_ON
};

/*
 * INPUT SYSTEM
 * listen and handle key states
 */
namespace IPT
{
	// Values
	/* current window */
	inline HWND	hWindow = nullptr;
	/* saved window messages handler */
	inline WNDPROC pOldWndProc = nullptr;
	/* last processed key states */
	inline std::array<EKeyState, 256U> arrKeyState = { };
	inline bool arrKeyBooleans[ 256 ];

	// Get
	/* set our window messages proccesor */
	bool Setup( );
	/* restore window messages processor and clear saved pointer */
	void Restore( );
	/* process input window message and save keys states in array */
	bool Process( UINT uMsg, WPARAM wParam, LPARAM lParam );

	/* is given key being held */
	inline bool IsKeyDown( const std::uint32_t uButtonCode )
	{
		return arrKeyState.at( uButtonCode ) == EKeyState::DOWN;
	}

	/* was given key released */
	inline bool IsKeyReleased( const std::uint32_t uButtonCode )
	{
		if ( arrKeyState.at( uButtonCode ) == EKeyState::RELEASED )
		{
			arrKeyState.at( uButtonCode ) = EKeyState::UP;
			return true;
		}

		return false;
	}

	inline bool HandleInput( const std::uint32_t uButtonCode )
	{
		if ( uButtonCode == 0 )
			return false;

		if ( cfg::m_iKeyStates[ uButtonCode ] == HOLD ) {
			return GetAsyncKeyState( uButtonCode );
		}
		else if ( cfg::m_iKeyStates[ uButtonCode ] == TOGGLE ) {

			if ( GetAsyncKeyState( uButtonCode ) & 1 )
				arrKeyBooleans[ uButtonCode ] = !arrKeyBooleans[ uButtonCode ];

			return arrKeyBooleans[ uButtonCode ]; //GetKeyState(uButtonCode);
		}
		else if ( cfg::m_iKeyStates[ uButtonCode ] == ALWAYS_ON ) {
			return true;
		}

		return false;

	}
}