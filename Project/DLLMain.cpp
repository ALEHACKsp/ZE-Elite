#include "SDK.h"
#include <string>
#include <fstream>
#include <intrin.h>
#include "include.h"
#include "ClientEntityListener.h"
#include "ImGui/imgui_impl_dx9.h"
#include "ImGui/imgui_impl_win32.h"

using namespace std;

CUserCmd* GlobalVars::cmd{ nullptr };

DWORD WINAPI Main_Function(void* DLL)
{
	//AllocConsole();

	//SetConsoleTitleA("ZE-Elite");

	//freopen("CONIN$"), "r"), stdin);

	//freopen("CONOUT$"), "w"), stdout);

	while (!(GetModuleHandleA("serverbrowser.dll")))Sleep(200);

	Interfaces::Initialize(); Offsets(BaseClientDLL->GetAllClasses()); Hooks::Initialize((HMODULE)DLL);

	ClientEntityListner::Singleton()->Initialize();

	static bool PressedPanicKey{ false };

	while (1)
	{
		if (IsVirtualKeyPressed(Menu::Get.Keys.PanicKey) && !PressedPanicKey)
		{
			Menu::Get.General.Panic = !Menu::Get.General.Panic;

			PressedPanicKey = !PressedPanicKey;
		}
		else if (!IsVirtualKeyPressed(Menu::Get.Keys.PanicKey) && PressedPanicKey)
		{
			PressedPanicKey = !PressedPanicKey;
		}

		if (IsVirtualKeyPressed(Menu::Get.Keys.Unload) || Menu::Get.General.Unload)
		{
			Menu::Get.General.Unload = true;

			Sleep(50);

			break;
		}

		Sleep(100);

#define OWNER

#ifndef OWNER
		if (FindDebugger())
		{
			break;
		}
#endif
	}

Lable:

	ClientEntityListner::Singleton()->End(); Hooks::Uninitialize(); 

	void OnUnload(); OnUnload(); 
	
	ImGui_ImplDX9_Shutdown();

	ImGui_ImplWin32_Shutdown();

	ImGui::DestroyContext();

	//FreeConsole();

	Sleep(500);

	//SendMessageA(FindWindowA(NULL, "ZE-Elite"), WM_CLOSE, NULL, NULL);

	FreeLibraryAndExitThread((HMODULE)DLL, EXIT_SUCCESS);
}

BOOL __stdcall DllMain
(
	_In_ HINSTANCE hinstDLL,
	_In_ DWORD     fdwReason,
	_In_ LPVOID    lpvReserved
)
{
	if (fdwReason == DLL_PROCESS_ATTACH)
	{
		if (hinstDLL)
		{
			DisableThreadLibraryCalls(hinstDLL);
		}

		CreateThread(NULL, NULL, Main_Function, hinstDLL, NULL, NULL);
	}

	return TRUE;
}
