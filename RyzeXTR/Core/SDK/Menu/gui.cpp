#include "gui.h"
#include "fontBytes.h"	
#include "../../globals.h"
#include "config.h"
#include <string>
#include "../../memeSounds.h"
#include "../../SDK/math.h"

#include "../InputSystem.h"
#include "../../../Dependecies/ImGui/imgui_freetype.h"
#include "../../SDK/Draw.h"
#include "../../../Dependecies/ImGui/imgui_internal.h"

void menu::Style() {


	ImGuiIO& io = ImGui::GetIO();
	ImGuiStyle& style = ImGui::GetStyle();
	auto& color = style.Colors;

	io.IniFilename = NULL;

	defaultFont = io.Fonts->AddFontFromFileTTF(XorStr("C:\\Windows\\Fonts\\consola.ttf"), 13);
	defaultFontBigger = io.Fonts->AddFontFromFileTTF(XorStr("C:\\Windows\\Fonts\\consolab.ttf"), 18);
	tabIcons = io.Fonts->AddFontFromMemoryCompressedTTF(byteFonts::FuckMyLife_compressed_data, byteFonts::FuckMyLife_compressed_size, 35.f);
	tabIconsPreview = io.Fonts->AddFontFromMemoryCompressedTTF(byteFonts::FuckMyLife_compressed_data, byteFonts::FuckMyLife_compressed_size, 110.f);
	weaponIcons = io.Fonts->AddFontFromMemoryCompressedTTF(byteFonts::font_compressed_data, byteFonts::font_compressed_size, 55.f);

	// window properties
	style.WindowRounding = 10.f;
	style.WindowPadding = ImVec2(0.f, 0.f);
	style.WindowMinSize = ImVec2(800, 600);

	// frame
	style.FrameRounding = 10.f;
	style.FramePadding = ImVec2(15, 15);
	style.ItemInnerSpacing = ImVec2(5.f, 2.f);
	style.PopupRounding = 3.f;
	style.PopupBorderSize = 2.f;
	style.ScrollbarSize = 1.f;

	color[ImGuiCol_WindowBg] = ImColor(7, 0, 13, 255);
	color[ImGuiCol_Border] = ImColor(52, 51, 77);
	color[ImGuiCol_ChildBg] = ImColor(7, 0, 13, 185);
	color[ImGuiCol_Button] = ImColor(0, 0, 0, 0);
	color[ImGuiCol_ButtonHovered] = ImColor(7, 0, 13, 50);
	color[ImGuiCol_ButtonActive] = ImColor(7, 0, 13, 255);
	color[ImGuiCol_FrameBg] = ImColor(7, 0, 13, 50);
	color[ImGuiCol_FrameBgHovered] = ImColor(7, 0, 13, 50);
	color[ImGuiCol_FrameBgActive] = ImColor(172, 171, 255, 255);
	color[ImGuiCol_CheckMark] = ImColor(145, 92, 191, 255);
	color[ImGuiCol_SliderGrab] = ImColor(172, 171, 255, 255);
	color[ImGuiCol_SliderGrabActive] = ImColor(172, 171, 255, 255);
	color[ImGuiCol_Text] = ImColor(212, 212, 212, 255);
	color[ImGuiCol_ResizeGrip] = ImColor(52, 51, 77, 120);
	color[ImGuiCol_ResizeGripActive] = ImColor(255, 255, 255, 255);
	color[ImGuiCol_ResizeGripHovered] = ImColor(255, 255, 255, 255);
	color[ImGuiCol_PopupBg] = ImColor(7, 0, 13, 255);
	color[ImGuiCol_Header] = ImColor(52, 51, 77, 90);
	color[ImGuiCol_HeaderHovered] = ImColor(52, 51, 77, 50);
	color[ImGuiCol_HeaderActive] = ImColor(52, 51, 77, 50);
}

void menu::Render() noexcept {

	ImGui_ImplDX9_NewFrame();
	ImGui_ImplWin32_NewFrame();

	ImGui::NewFrame();
	
	ImDrawList* pBackgroundDrawList = ImGui::GetBackgroundDrawList( );
	//D::RenderDrawData( pBackgroundDrawList );
	
	if (menu::open) {

		HandleMenuElements();
	}

	ImGui::EndFrame();
	ImGui::Render();
	ImGui_ImplDX9_RenderDrawData(ImGui::GetDrawData());
}

bool menu::SetupWindowClass(const char* szWindowClassName) noexcept {

	windowClass.cbSize = sizeof(WNDCLASSEX);
	windowClass.style = CS_HREDRAW | CS_VREDRAW | CS_DROPSHADOW;
	windowClass.lpfnWndProc = DefWindowProc;
	windowClass.cbClsExtra = 0;
	windowClass.cbWndExtra = 0;
	windowClass.hInstance = GetModuleHandle(NULL);
	windowClass.hIcon = NULL;
	windowClass.hCursor = NULL;
	windowClass.hbrBackground = NULL;
	windowClass.lpszMenuName = NULL;
	windowClass.lpszClassName = szWindowClassName;
	windowClass.hIconSm = NULL;

	if (!RegisterClassEx(&windowClass))
		return false;

	return true;
}

void menu::DestroyWindowClass() noexcept {

	UnregisterClass(windowClass.lpszClassName, windowClass.hInstance);
}

bool menu::SetupWindow(const char* szWindowName) noexcept {

	//                    lpClassName,				lpWindowName, dwStyle, x, y, nWidth, nHeight, hWndParent, hMenu, hInstance, lpParam
	window = CreateWindow(windowClass.lpszClassName, szWindowName, WS_OVERLAPPEDWINDOW, 0, 0, 500, 700, 0, 0, windowClass.hInstance, 0);

	if (!window)
		return false;

	return true;
}

void menu::DestroyWindow() noexcept {

	if (window)
		DestroyWindow(window);
}

bool menu::SetupDirectX() noexcept {

	const auto handle = GetModuleHandle("d3d9.dll");

	if (!handle)
		return false;

	using Fn = LPDIRECT3D9(__stdcall*)(UINT);
	const auto create = reinterpret_cast<Fn>(GetProcAddress(handle, "Direct3DCreate9"));

	if (!create)
		return false;

	d3d9 = create(D3D_SDK_VERSION);

	if (!d3d9)
		return false;

	D3DPRESENT_PARAMETERS parameters = {};
	parameters.BackBufferWidth = 0;
	parameters.BackBufferHeight = 0;
	parameters.BackBufferFormat = D3DFMT_UNKNOWN;
	parameters.BackBufferCount = 0;
	parameters.MultiSampleType = D3DMULTISAMPLE_NONE;
	parameters.MultiSampleQuality = NULL;
	parameters.SwapEffect = D3DSWAPEFFECT_DISCARD;
	parameters.hDeviceWindow = window;
	parameters.Windowed = 1;
	parameters.EnableAutoDepthStencil = 0;
	parameters.AutoDepthStencilFormat = D3DFMT_UNKNOWN;
	parameters.Flags = NULL;
	parameters.FullScreen_RefreshRateInHz = 0;
	parameters.PresentationInterval = 0;

	if (d3d9->CreateDevice(D3DADAPTER_DEFAULT, D3DDEVTYPE_NULLREF, window, D3DCREATE_SOFTWARE_VERTEXPROCESSING | D3DCREATE_DISABLE_DRIVER_MANAGEMENT, &parameters, &device) < 0)
		return false;

	return true;
}

void menu::DestroyDirectX() noexcept {

	if (device) {
		device->Release();
		device = NULL;
	}

	if (d3d9) {
		d3d9->Release();
		d3d9 = NULL;
	}
}

void menu::Setup() {

	if (!SetupWindowClass("CoolHack001"))
		throw std::runtime_error("Failed to create window class");

	if (!SetupWindow("CoolHack Window"))
		throw std::runtime_error("Failed to create window");

	if (!SetupDirectX())
		throw std::runtime_error("Failed to create directx");

	DestroyWindow();
	DestroyWindowClass();

	util::Print("Menu initialized");
}

void menu::SetupMenu(LPDIRECT3DDEVICE9 device) noexcept {

	auto parameters = D3DDEVICE_CREATION_PARAMETERS{};
	device->GetCreationParameters(&parameters);

	window = parameters.hFocusWindow;

	ImGui::CreateContext();

	Style();
	//D::SetupFonts( );

	ImGui_ImplWin32_Init(window);
	ImGui_ImplDX9_Init(device);

	setup = true;
}

void menu::Destroy() noexcept {

	ImGui_ImplDX9_Shutdown();
	ImGui_ImplWin32_Shutdown();
	ImGui::DestroyContext();

	SetWindowLongPtr(window, GWLP_WNDPROC, reinterpret_cast<LONG_PTR>( IPT::pOldWndProc ));

	DestroyDirectX();
}


bool menu::DrawCustomChildRounding(const char* str_id, const ImVec2& size_arg, bool border, ImGuiWindowFlags extra_flags, ImDrawFlags drawflags) {

	ImGuiWindow* window = ImGui::GetCurrentWindow(); // ImVec4(7 / 255.f, 0, 13 / 255.f, 255)

	auto start = ImVec2(window->Pos.x + ImGui::GetCursorPos().x, window->Pos.y + ImGui::GetCursorPos().y);
	window->DrawList->AddRectFilled(start, ImVec2(start.x + size_arg.x, start.y + size_arg.y), ImGui::GetColorU32(ImGuiCol_ChildBg), ImGui::GetStyle().WindowRounding, drawflags);
	window->DrawList->AddRect(start, ImVec2(start.x + size_arg.x, start.y + size_arg.y), ImGui::GetColorU32(ImGuiCol_Border), ImGui::GetStyle().WindowRounding, drawflags);

	ImGui::PushStyleColor(ImGuiCol_ChildBg, ImVec4(0, 0, 0, 0));
	ImGui::PushStyleColor(ImGuiCol_Border, ImVec4(0, 0, 0, 0));
	auto ret = ImGui::BeginChildEx(str_id, window->GetID(str_id), size_arg, border, extra_flags | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse | ImGuiWindowFlags_NoCollapse);
	ImGui::PopStyleColor(2);

	return ret;
}

void menu::DrawRandomTexture(const ImVec2& center, float radius, ImU32 col, int num_segments, float randomness) {

	ImGuiWindow* window = ImGui::GetCurrentWindow();

	auto& draw = window->DrawList;

	float flStep = (2 * IM_PI) / num_segments;

	std::vector<ImVec2> dontknow{};
	float lmao = randomness;
	for (float rotation = 0; rotation < (IM_PI * 2.0); rotation += flStep) {

		ImVec2 something = ImVec2(
			radius * cos(rotation) + center.x + sin(lmao) * (radius / 10.f),
			radius * sin(rotation) + center.y + sin(lmao) * (radius / 10.f));

		dontknow.push_back(something);
		lmao += 0.5f;
	}
	dontknow.back() = dontknow.front();

	draw->AddPolyline(dontknow.data(), dontknow.size(), col, 0, 2.f);
}

void menu::DrawBackgroundTexture(float i) {

	DrawRandomTexture(ImVec2(ImGui::GetWindowPos().x + 200, ImGui::GetWindowPos().y + 200), 10.f, ImGui::GetColorU32(ImGuiCol_ResizeGrip), 64, 2 + i);
	DrawRandomTexture(ImVec2(ImGui::GetWindowPos().x + 200, ImGui::GetWindowPos().y + 200), 30.f, ImGui::GetColorU32(ImGuiCol_ResizeGrip), 64, 2 + i);
	DrawRandomTexture(ImVec2(ImGui::GetWindowPos().x + 200, ImGui::GetWindowPos().y + 200), 50.f, ImGui::GetColorU32(ImGuiCol_ResizeGrip), 64, 2 + i);
	DrawRandomTexture(ImVec2(ImGui::GetWindowPos().x + 200, ImGui::GetWindowPos().y + 200), 70.f, ImGui::GetColorU32(ImGuiCol_ResizeGrip), 64, 2 + i);

	DrawRandomTexture(ImVec2(ImGui::GetWindowPos().x + 500, ImGui::GetWindowPos().y + 100), 20.f, ImGui::GetColorU32(ImGuiCol_ResizeGrip), 54, 6 + i);
	DrawRandomTexture(ImVec2(ImGui::GetWindowPos().x + 500, ImGui::GetWindowPos().y + 100), 50.f, ImGui::GetColorU32(ImGuiCol_ResizeGrip), 54, 6 + i);

	DrawRandomTexture(ImVec2(ImGui::GetWindowPos().x + 200, ImGui::GetWindowPos().y + 500), 10.f, ImGui::GetColorU32(ImGuiCol_ResizeGrip), 64, 42 + i);
	DrawRandomTexture(ImVec2(ImGui::GetWindowPos().x + 200, ImGui::GetWindowPos().y + 500), 30.f, ImGui::GetColorU32(ImGuiCol_ResizeGrip), 64, 43 + i);
	DrawRandomTexture(ImVec2(ImGui::GetWindowPos().x + 200, ImGui::GetWindowPos().y + 500), 50.f, ImGui::GetColorU32(ImGuiCol_ResizeGrip), 64, 44 + i);
	DrawRandomTexture(ImVec2(ImGui::GetWindowPos().x + 200, ImGui::GetWindowPos().y + 500), 70.f, ImGui::GetColorU32(ImGuiCol_ResizeGrip), 64, 45 + i);
	DrawRandomTexture(ImVec2(ImGui::GetWindowPos().x + 200, ImGui::GetWindowPos().y + 500), 90.f, ImGui::GetColorU32(ImGuiCol_ResizeGrip), 64, 46 + i);
	DrawRandomTexture(ImVec2(ImGui::GetWindowPos().x + 200, ImGui::GetWindowPos().y + 500), 120.f, ImGui::GetColorU32(ImGuiCol_ResizeGrip), 64, 47 + i);
	DrawRandomTexture(ImVec2(ImGui::GetWindowPos().x + 200, ImGui::GetWindowPos().y + 500), 150.f, ImGui::GetColorU32(ImGuiCol_ResizeGrip), 64, 48 + i);

	DrawRandomTexture(ImVec2(ImGui::GetWindowPos().x + 700, ImGui::GetWindowPos().y + 600), 10.f, ImGui::GetColorU32(ImGuiCol_ResizeGrip), 55, 84 + i);
	DrawRandomTexture(ImVec2(ImGui::GetWindowPos().x + 700, ImGui::GetWindowPos().y + 600), 30.f, ImGui::GetColorU32(ImGuiCol_ResizeGrip), 55, 85 + i);
	DrawRandomTexture(ImVec2(ImGui::GetWindowPos().x + 700, ImGui::GetWindowPos().y + 600), 50.f, ImGui::GetColorU32(ImGuiCol_ResizeGrip), 55, 86 + i);
	DrawRandomTexture(ImVec2(ImGui::GetWindowPos().x + 700, ImGui::GetWindowPos().y + 600), 70.f, ImGui::GetColorU32(ImGuiCol_ResizeGrip), 55, 87 + i);

	DrawRandomTexture(ImVec2(ImGui::GetWindowPos().x + 800, ImGui::GetWindowPos().y + 350), 10.f, ImGui::GetColorU32(ImGuiCol_ResizeGrip), 55, 84 + 5 + i);
	DrawRandomTexture(ImVec2(ImGui::GetWindowPos().x + 800, ImGui::GetWindowPos().y + 350), 30.f, ImGui::GetColorU32(ImGuiCol_ResizeGrip), 55, 85 + 5 + i);
	DrawRandomTexture(ImVec2(ImGui::GetWindowPos().x + 800, ImGui::GetWindowPos().y + 350), 50.f, ImGui::GetColorU32(ImGuiCol_ResizeGrip), 55, 86 + 5 + i);
	DrawRandomTexture(ImVec2(ImGui::GetWindowPos().x + 800, ImGui::GetWindowPos().y + 350), 70.f, ImGui::GetColorU32(ImGuiCol_ResizeGrip), 55, 87 + 5 + i);

	DrawRandomTexture(ImVec2(ImGui::GetWindowPos().x + 550, ImGui::GetWindowPos().y + 350), 5.f, ImGui::GetColorU32(ImGuiCol_ResizeGrip), 55, 4 + i);
	DrawRandomTexture(ImVec2(ImGui::GetWindowPos().x + 550, ImGui::GetWindowPos().y + 350), 13.f, ImGui::GetColorU32(ImGuiCol_ResizeGrip), 55, 4 + i);
	DrawRandomTexture(ImVec2(ImGui::GetWindowPos().x + 550, ImGui::GetWindowPos().y + 350), 24.f, ImGui::GetColorU32(ImGuiCol_ResizeGrip), 55, 4 + i);
	DrawRandomTexture(ImVec2(ImGui::GetWindowPos().x + 550, ImGui::GetWindowPos().y + 350), 52.f, ImGui::GetColorU32(ImGuiCol_ResizeGrip), 55, 4 + i);

	DrawRandomTexture(ImVec2(ImGui::GetWindowPos().x + 1200, ImGui::GetWindowPos().y + 375), 10.f, ImGui::GetColorU32(ImGuiCol_ResizeGrip), 55, 84 + 5 + i);
	DrawRandomTexture(ImVec2(ImGui::GetWindowPos().x + 1200, ImGui::GetWindowPos().y + 375), 30.f, ImGui::GetColorU32(ImGuiCol_ResizeGrip), 55, 85 + 5 + i);
	DrawRandomTexture(ImVec2(ImGui::GetWindowPos().x + 1200, ImGui::GetWindowPos().y + 375), 50.f, ImGui::GetColorU32(ImGuiCol_ResizeGrip), 55, 86 + 5 + i);
	DrawRandomTexture(ImVec2(ImGui::GetWindowPos().x + 1200, ImGui::GetWindowPos().y + 375), 70.f, ImGui::GetColorU32(ImGuiCol_ResizeGrip), 55, 87 + 5 + i);

	DrawRandomTexture(ImVec2(ImGui::GetWindowPos().x + 920, ImGui::GetWindowPos().y + 635), 12.f, ImGui::GetColorU32(ImGuiCol_ResizeGrip), 55, 84 + 12 + i);
	DrawRandomTexture(ImVec2(ImGui::GetWindowPos().x + 920, ImGui::GetWindowPos().y + 635), 35.f, ImGui::GetColorU32(ImGuiCol_ResizeGrip), 55, 85 + 12 + i);
	DrawRandomTexture(ImVec2(ImGui::GetWindowPos().x + 920, ImGui::GetWindowPos().y + 635), 52.f, ImGui::GetColorU32(ImGuiCol_ResizeGrip), 55, 86 + 12 + i);
	DrawRandomTexture(ImVec2(ImGui::GetWindowPos().x + 920, ImGui::GetWindowPos().y + 635), 75.f, ImGui::GetColorU32(ImGuiCol_ResizeGrip), 55, 87 + 12 + i);

	DrawRandomTexture(ImVec2(ImGui::GetWindowPos().x + 1000, ImGui::GetWindowPos().y + 150), 12.f, ImGui::GetColorU32(ImGuiCol_ResizeGrip), 55, 84 + i);
	DrawRandomTexture(ImVec2(ImGui::GetWindowPos().x + 1000, ImGui::GetWindowPos().y + 150), 23.f, ImGui::GetColorU32(ImGuiCol_ResizeGrip), 55, 85 + i);
	DrawRandomTexture(ImVec2(ImGui::GetWindowPos().x + 1000, ImGui::GetWindowPos().y + 150), 34.f, ImGui::GetColorU32(ImGuiCol_ResizeGrip), 55, 86 + i);
	DrawRandomTexture(ImVec2(ImGui::GetWindowPos().x + 1000, ImGui::GetWindowPos().y + 150), 52.f, ImGui::GetColorU32(ImGuiCol_ResizeGrip), 55, 87 + i);
}

bool menu::ButtonCenteredOnLine(const char* label, float alignment, bool bSelected)
{
	ImGuiStyle& style = ImGui::GetStyle();

	float size = ImGui::CalcTextSize(label).x + style.FramePadding.x * 2.0f;
	float avail = ImGui::GetContentRegionAvail().x;

	float off = (avail - size) * alignment;
	if (off > 0.0f)
		ImGui::SetCursorPosX(ImGui::GetCursorPosX() + off);

	return ImGui::Button(label, ImVec2(0, 0), bSelected);
}

void menu::Text(const char* label, float alignment, bool vertically)
{
	ImGuiStyle& style = ImGui::GetStyle();

	if (vertically) {

		float size2 = ImGui::CalcTextSize(label).y + style.FramePadding.y * 2.0f;
		float avail2 = ImGui::GetContentRegionAvail().y;

		float off2 = (avail2 - size2) * 0.5f;
		if (off2 > 0.0f)
			ImGui::SetCursorPosY(ImGui::GetCursorPosY() + off2);
	}

	float size = ImGui::CalcTextSize(label).x + style.FramePadding.x * 2.0f;
	float avail = ImGui::GetContentRegionAvail().x;

	float off = (avail - size) * alignment;
	if (off > 0.0f)
		ImGui::SetCursorPosX(ImGui::GetCursorPosX() + off);

	return ImGui::Text(label);
}

bool menu::Checkbox(const char* label, bool* bValue, float alignment)
{
	ImGui::Text(label);
	ImGui::SameLine();

	ImGuiStyle& style = ImGui::GetStyle();

	float size = ImGui::CalcTextSize(label).x + style.FramePadding.x * 2.0f;
	float avail = ImGui::GetContentRegionAvail().x;

	float off = (avail - size) * alignment;
	if (off > 0.0f)
		ImGui::SetCursorPosX(ImGui::GetCursorPosX() + off);

	return ImGui::Checkbox(std::format("##{}", label).c_str(), bValue);
}
