#ifndef DirectXDevice_H
#define DirectXDevice_H

#include "ImGui/ImGui.h"
#include "ImGui/ImGui_Impl_Dx9.h"
#include "ImGui/ImGui_Impl_Win32.h"
#include <filesystem>


extern HWND hWindow;

static bool WasDone{ false };

const char* MenuFont[] = {
	"seguisym",
	"Arial",
	"Bahnschrift",
	"Gabriola",
	"Georgia",
	"Tahoma",
	"Taile",
	"Micross",
	"Himalaya",
	"Impact",
	"Times",
	"Trado",
};

void Initialize_GUI(IDirect3DDevice9* vDevice)
{
	ImGui::CreateContext();

	ImGui_ImplWin32_Init(hWindow);

	ImGui_ImplDX9_Init(vDevice);

	ImGui::StyleColorsClassic();

	// --

	ImGuiStyle* style = &ImGui::GetStyle();

	style->TabRounding = style->WindowRounding = style->WindowBorderSize = 0.0f;

	ImVec4* Colors = style->Colors;

	// --

	Colors[ImGuiCol_Text] = ImVec4(1, 1, 1, 1);

	Colors[ImGuiCol_WindowBg] = ImVec4(0.0f, 0.0f, 0.0f, 0.80f);

	Colors[ImGuiCol_PopupBg] = ImVec4(0.007843137254902f, 0.0117647058823529f, 0.0352941176470588f, 1.f);

	Colors[ImGuiCol_Border] = ImVec4(0.0f, 0.0f, 0.0f, 1.00f);

	Colors[ImGuiCol_CheckMark] = ImVec4(0.0f, 1, 0.02f, 1.00f);

	Colors[ImGuiCol_FrameBg] = ImVec4(0.001f, 0.001f, 0.001f, 1.f);

	Colors[ImGuiCol_TitleBg] = ImVec4(0.00f, 1.0f, 0.02f, 1.00f);

	Colors[ImGuiCol_TitleBgActive] = ImVec4(0.00f, 1.0f, 0.02f, 1.00f);

	Colors[ImGuiCol_TitleBgCollapsed] = ImVec4(0.00f, 1.0f, 0.02f, 1.00f);

	Colors[ImGuiCol_Button] = ImVec4(0.0392f, 0.0784f, 0.1803f, 1.f);

	Colors[ImGuiCol_ButtonHovered] = ImVec4(0.22f, 0.40f, 0.80f, 1.63f);

	Colors[ImGuiCol_ButtonActive] = ImVec4(0.00f, 0.00f, 0.00f, 1.63f);

	Colors[ImGuiCol_Tab] = ImVec4(0.0f, 0.0f, 0.0f, 1.00f);

	Colors[ImGuiCol_TabActive] = ImVec4(0.0f, 0.0f, 0.0f, 1.00f);

	//auto Font = ImGui::GetIO().Fonts->AddFontFromFileTTF("C:\\Windows\\Fonts\\seguisym.ttf"), 16.f);

	//ImGui::GetIO().Fonts->AddFontFromFileTTF("C:\\Windows\\Fonts\\Arialbi.ttf"), 16.f);

	//ImGui::GetIO().Fonts->AddFontFromFileTTF("C:\\Windows\\Fonts\\Ariblk.ttf"), 16.f);

	std::string Path = "C:\\Windows\\Fonts\\";

	for (auto FontName : MenuFont)
	{
		ImGui::GetIO().Fonts->AddFontFromFileTTF((Path + FontName + ".ttf").c_str(), 16.f);
	}
}

using EndScane_Fn = HRESULT(__stdcall*)(void*);

EndScane_Fn Original_EndScane;

HRESULT __stdcall EndScane(IDirect3DDevice9* Device)
{
	if (WasDone)
	{
		Menu::Render();
	}
	else
	{
		Initialize_GUI(Device);

		WasDone = !WasDone;
	}

	return Original_EndScane(Device);
}

using Reset_Fn = HRESULT(__stdcall*)(void*, void*);

Reset_Fn Original_Reset;

HRESULT __stdcall Reset(IDirect3DDevice9* vDevice, D3DPRESENT_PARAMETERS* Params)
{

	if (!WasDone)
	{
		return Original_Reset(vDevice, Params);
	}

	ImGui_ImplDX9_InvalidateDeviceObjects();

	HRESULT bReturn = Original_Reset(vDevice, Params);

	ImGui_ImplDX9_CreateDeviceObjects();

	return bReturn;
}

#endif