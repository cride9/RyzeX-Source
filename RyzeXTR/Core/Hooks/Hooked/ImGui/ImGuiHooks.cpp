#include "../../hooks.h"
#include "../../../../Dependecies/ImGui/imgui_impl_dx9.h"
#include "../../../SDK/Menu/gui.h"
#include "../../../SDK/InputSystem.h"

HRESULT __stdcall h::Reset(IDirect3DDevice9* device, D3DPRESENT_PARAMETERS* params) {

	static auto original = detour::D3DReset.GetOriginal<decltype(&h::Reset)>();

	ImGui_ImplDX9_InvalidateDeviceObjects();

	const auto result = original(device, params);

	ImGui_ImplDX9_CreateDeviceObjects();

	return result;
}

long __stdcall h::EndScene(IDirect3DDevice9* device) {

	static auto original = detour::D3DEndScene.GetOriginal<decltype(&h::EndScene)>();

	static const auto  returnAddress = _ReturnAddress();

	const auto result = original(device);

	if (_ReturnAddress() == returnAddress)
		return result;

	if (!menu::setup)
		menu::SetupMenu(device);

	menu::Render();

	return result;
}

extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler( HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam );
long CALLBACK h::hkWndProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam )
{
	// process keys
	IPT::Process( uMsg, wParam, lParam );

	// switch window state on key click
	if ( IPT::IsKeyReleased( VK_INSERT ) )
		menu::open = !menu::open;

	if ( menu::open )
	{
		ImGui_ImplWin32_WndProcHandler( hWnd, uMsg, wParam, lParam );

		if ( wParam != 'R' && wParam != '1' && wParam != '2' && wParam != '3' && wParam != '4' && wParam != '5' && wParam != 'W' && wParam != 'A' && wParam != 'S' && wParam != 'D' && wParam != VK_SHIFT && wParam != VK_CONTROL && wParam != VK_TAB && wParam != VK_SPACE || ImGui::GetIO( ).WantTextInput )
			return true;
	}

	if ( menu::open && ( uMsg == WM_LBUTTONDOWN || uMsg == WM_LBUTTONUP || uMsg == WM_MOUSEMOVE ) )
		return false;

	// disable game input when menu is opened
	//i::InputSystem->EnableInput( !menu::open );

	/*
	 * @note: we can use imgui input handler to our binds if remove menu state check
	 * with ImGui::IsKeyDown, ImGui::IsKeyPressed, etc functions
	 * but imgui api's keys down durations doesnt have forward compatibility
	 * and i dont want spend a lot of time on recode it
	 */
	if ( menu::setup && menu::open && ImGui_ImplWin32_WndProcHandler( hWnd, uMsg, wParam, lParam ) )
		return 1L;

	// return input controls to the game
	return CallWindowProcW( IPT::pOldWndProc, hWnd, uMsg, wParam, lParam );
}