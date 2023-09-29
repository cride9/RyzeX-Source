#pragma once
#include <d3d9.h>
#include <stdexcept>

#include "../../../Dependecies/ImGui/imgui.h"
#include "../../../Dependecies/ImGui/imgui_impl_win32.h"
#include "../../../Dependecies/ImGui/imgui_impl_dx9.h"

#include "config.h"
#include "../../Features/Misc/Playerlist.h"

#define ANIMATIONSPEED 8.f

namespace clr {

	// general
	inline ImVec4 theme = ImColor(92, 103, 191);

	// text clr
	inline ImVec4 text_active = ImColor(212, 212, 212);
	inline ImVec4 text_hov = ImColor(122, 122, 123, 255);
	inline ImVec4 text_off = ImColor(92, 92, 93, 255);
	inline ImVec4 text_preview = ImColor(255, 255, 255, 15);

	// combo selectable clr
	inline ImVec4 text_selectable = ImColor(145, 92, 191);
	inline ImVec4 bg_selectable = ImColor(52, 51, 77, 90);
	inline ImVec4 bg_selectableHov = ImColor(52, 51, 77, 50);
	inline ImVec4 bg_selectableAct = ImColor(52, 51, 77, 50);
	// windows
	inline ImVec4 window = ImColor(92, 92, 93, 255);

	// childs
	inline ImVec4 child = ImColor(92, 92, 93, 255);
	inline ImVec4 childBorder = ImColor(52, 51, 77);
	inline ImVec4 childBG = ImColor(7, 0, 13, 120);

	// frames
	inline ImVec4 frame_active = ImColor(7, 0, 13, 75);
	inline ImVec4 frame_hov = ImColor(7, 0, 13, 75);
	inline ImVec4 frame = ImColor(7, 0, 13, 75);

	// elements
	inline ImVec4 element_bg = ImColor(7, 0, 13, 75);
	inline ImVec4 element_bg_active = ImColor(7, 0, 13, 75);
	inline ImVec4 element_frame = ImColor(52, 51, 77, 125);
	inline ImVec4 element_frame_active = ImColor(52, 51, 77, 255);

	// buttons
	inline ImVec4 button_bg = ImColor(7, 0, 13, 0);
	inline ImVec4 button_bg_active = ImColor(7, 0, 13, 125);
	inline ImVec4 button_frame = ImColor(52, 51, 77, 0);
	inline ImVec4 button_frame_active = ImColor(88, 86, 130, 255);
	inline ImVec4 button_frame_hover = ImColor(88, 86, 130, 200);

	// skins
	inline ImVec4 rarity_text[6] = { ImColor(212, 212, 212) /*1*/, ImColor(81, 141, 156) /*2*/, ImColor(35, 92, 145) /*3*/, ImColor(84, 73, 140)/*4*/, ImColor(135, 81, 156) /*5*/, ImColor(140, 53, 61) /*6*/ };
}

enum ETabs : int {

	UNDEFINED = -1,
	RAGE_TAB,
	ANTIAIM_TAB,
	VISUAL_TAB,
	MISC_TAB,
	SKIN_TAB,
	CONFIG_TAB,
	PLAYERLST
};

enum EEntity : int {

	ENEMY,
	TEAM,
	LOCAL,
	WORLD,
	VIEWMODEL
};

enum EEspFlags : int {

	NAME,
	HEALTH,
	ARMOR,
	AMMO,
	MONEY,
	WEAPON,
	RESOLVER,
	FAKEDUCK,
	AIMBOT,
	FLAGMAX
};

enum EVISUAL : int {

	ESP,
	MODEL
};

enum EWEAPON : int {

	AUTO,
	SCOUT,
	AWP,
	PISTOL,
	HEAVY_PISTOL,
	OTHER,
	ZEUS,
	WEAPON_MAX
};

namespace menu {

	inline ImFont* tabIcons = nullptr;
	inline ImFont* tabIconsPreview = nullptr;
	inline ImFont* weaponIcons = nullptr;
	inline ImFont* defaultFont = nullptr;
	inline ImFont* defaultFontBigger = nullptr;
	inline int iSelectedTab = 0;

	inline bool open = true;
	inline bool setup = false;
	inline bool bPressedSave = false;
	inline bool bWarningMethod = false;

	inline HWND window = nullptr;
	inline WNDCLASSEX windowClass = {};

	inline LPDIRECT3DDEVICE9 device = nullptr;
	inline LPDIRECT3D9 d3d9 = nullptr;

	bool SetupWindowClass(const char* szWindowClassName) noexcept;
	void DestroyWindowClass() noexcept;

	bool SetupWindow(const char* szWindowName) noexcept;
	void DestroyWindow() noexcept;

	bool SetupDirectX() noexcept;
	void DestroyDirectX() noexcept;

	void Setup();

	void SetupMenu(LPDIRECT3DDEVICE9 device) noexcept;
	void Destroy() noexcept;

	bool DrawCustomChildRounding(const char* str_id, const ImVec2& size_arg, bool border, ImGuiWindowFlags extra_flags, ImDrawFlags drawflags);
	void DrawRandomTexture(const ImVec2& center, float radius, ImU32 col, int num_segments, float randomness);
	void DrawBackgroundTexture(float i);
	bool ButtonCenteredOnLine(const char* label, float alignment, bool bSelected);
	void Text(const char* label, float alignment, bool vertically = false);
	bool Checkbox(const char* label, bool* bValue, float alignment);

	void Style();
	void Render() noexcept;

	void Rage(ImVec2 savedCursorPosition);
	void AntiAim(ImVec2 savedCursorPosition);
	void Visual(ImVec2 savedCursorPosition);
	void Misc(ImVec2 savedCursorPosition);
	void Skins(ImVec2 savedCursorPosition);
	void Config(ImVec2 savedCursorPosition);
	void PlayerList(ImVec2 savedCursorPosition);
	void SaveWarning(bool& saved, bool type) noexcept;
	void HandleMenuElements() noexcept;
}


static const int KeyCodes[] = {
	0x0,  //Undefined
	0x01,
	0x02,
	0x03,
	0x04,
	0x05,
	0x06,
	0x07, //Undefined
	0x08,
	0x09,
	0x0A, //Reserved
	0x0B, //Reserved
	0x0C,
	0x0D,
	0x0E, //Undefined
	0x0F, //Undefined
	0x10,
	0x11,
	0x12,
	0x13,
	0x14,
	0x15,
	0x16, //IME On
	0x17,
	0x18,
	0x19,
	0x1A, //IME Off
	0x1B,
	0x1C,
	0x1D,
	0x1E,
	0x1F,
	0x20,
	0x21,
	0x22,
	0x23,
	0x24,
	0x25,
	0x26,
	0x27,
	0x28,
	0x29,
	0x2A,
	0x2B,
	0x2C,
	0x2D,
	0x2E,
	0x2F,
	0x30,
	0x31,
	0x32,
	0x33,
	0x34,
	0x35,
	0x36,
	0x37,
	0x38,
	0x39,
	0x3A, //Undefined
	0x3B, //Undefined
	0x3C, //Undefined
	0x3D, //Undefined
	0x3E, //Undefined
	0x3F, //Undefined
	0x40, //Undefined
	0x41,
	0x42,
	0x43,
	0x44,
	0x45,
	0x46,
	0x47,
	0x48,
	0x49,
	0x4A,
	0x4B,
	0x4C,
	0x4B,
	0x4E,
	0x4F,
	0x50,
	0x51,
	0x52,
	0x53,
	0x54,
	0x55,
	0x56,
	0x57,
	0x58,
	0x59,
	0x5A,
	0x5B,
	0x5C,
	0x5D,
	0x5E, //Rservered
	0x5F,
	0x60, //Numpad1
	0x61, //Numpad2
	0x62, //Numpad3
	0x63, //Numpad4
	0x64, //Numpad5
	0x65, //Numpad6
	0x66, //Numpad7
	0x67, //Numpad8
	0x68, //Numpad8
	0x69, //Numpad9
	0x6A,
	0x6B,
	0x6C,
	0x6D,
	0x6E,
	0x6F,
	0x70, //F1
	0x71, //F2
	0x72, //F3
	0x73, //F4
	0x74, //F5
	0x75, //F6
	0x76, //F7
	0x77, //F8
	0x78, //F9
	0x79, //F10
	0x7A, //F11
	0x7B, //F12
	0x7C, //F13
	0x7D, //F14
	0x7E, //F15
	0x7F, //F16
	0x80, //F17
	0x81, //F18
	0x82, //F19
	0x83, //F20
	0x84, //F21
	0x85, //F22
	0x86, //F23
	0x87, //F24
	0x88, //Unkown
	0x89, //Unkown
	0x8A, //Unkown
	0x8B, //Unkown
	0x8C, //Unkown
	0x8D, //Unkown
	0x8E, //Unkown
	0x8F, //Unkown
	0x90,
	0x91,
	0x92, //OEM Specific
	0x93, //OEM Specific
	0x94, //OEM Specific
	0x95, //OEM Specific
	0x96, //OEM Specific
	0x97, //Unkown
	0x98, //Unkown
	0x99, //Unkown
	0x9A, //Unkown
	0x9B, //Unkown
	0x9C, //Unkown
	0x9D, //Unkown
	0x9E, //Unkown 
	0x9F, //Unkown
	0xA0,
	0xA1,
	0xA2,
	0xA3,
	0xA4,
	0xA5
};

const char* const KeyNames[] = {
	"None",
	"M1",
	"M2",
	"Esc",
	"M3",
	"M4",
	"M5",
	"None",
	"Back",
	"Tab",
	"None",
	"None",
	"BSpace",
	"Enter",
	"None",
	"None",
	"Shift",
	"Ctrl",
	"Alt",
	"PB",
	"CL",
	"VK_KANA",
	"Unknown",
	"VK_JUNJA",
	"VK_FINAL",
	"VK_KANJI",
	"Unknown",
	"Esc",
	"VK_CONVERT",
	"VK_NONCONVERT",
	"VK_ACCEPT",
	"VK_MODECHANGE",
	"Space",
	"Page Up",
	"Page Down",
	"End",
	"Home",
	"Left",
	"Up",
	"Right",
	"Down",
	"VK_SELECT",
	"VK_PRINT",
	"VK_EXECUTE",
	"Print Screen",
	"Ins",
	"Del",
	"VK_HELP",
	"0",
	"1",
	"2",
	"3",
	"4",
	"5",
	"6",
	"7",
	"8",
	"9",
	"Unknown",
	"Unknown",
	"Unknown",
	"Unknown",
	"Unknown",
	"Unknown",
	"Unknown",
	"A",
	"B",
	"C",
	"D",
	"E",
	"F",
	"G",
	"H",
	"I",
	"J",
	"K",
	"L",
	"M",
	"N",
	"O",
	"P",
	"Q",
	"R",
	"S",
	"T",
	"U",
	"V",
	"W",
	"X",
	"Y",
	"Z",
	"Left Windows",
	"Right Windows",
	"VK_APPS",
	"Unknown",
	"VK_SLEEP",
	"NUMPAD0",
	"NUMPAD1",
	"NUMPAD2",
	"NUMPAD3",
	"NUMPAD4",
	"NUMPAD5",
	"NUMPAD6",
	"NUMPAD7",
	"NUMPAD8",
	"NUMPAD9",
	"Multiply",
	"+",
	"Separator",
	"Subtract",
	"-",
	"/",
	"F1",
	"F2",
	"F3",
	"F4",
	"F5",
	"F6",
	"F7",
	"F8",
	"F9",
	"F10",
	"F11",
	"F12",
	"F13",
	"F14",
	"F15",
	"F16",
	"F17",
	"F18",
	"F19",
	"F20",
	"F21",
	"F22",
	"F23",
	"F24",
	"Unknown",
	"Unknown",
	"Unknown",
	"Unknown",
	"Unknown",
	"Unknown",
	"Unknown",
	"Unknown",
	"Num Lock",
	"Scroll lock",
	"VK_OEM_NEC_EQUAL",
	"VK_OEM_FJ_MASSHOU",
	"VK_OEM_FJ_TOUROKU",
	"VK_OEM_FJ_LOYA",
	"VK_OEM_FJ_ROYA",
	"Unknown",
	"Unknown",
	"Unknown",
	"Unknown",
	"Unknown",
	"Unknown",
	"Unknown",
	"Unknown",
	"Unknown",
	"LShift",
	"RShift",
	"LCtrl",
	"RCtrl",
	"LAlt",
	"RAlt"
};
