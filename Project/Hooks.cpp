#include <Windows.h>
#include "Config.h"
#include <memory>
#include <Psapi.h>
#include <chrono>
#include <thread>
#include <d3d9.h>

//Credits to TraitCore <3
#define Bits_32
#include "\
Redirection_Manager/\
Redirection_Manager.hpp"

#include "SDK.h"
#include "Hooks.h"
#include "Client.hpp"
#include "Interfaces.h"
#include "DirectXDevice.hpp"

void* sv_cheats;
void* fog_override;
void* DirectXDevice{ nullptr };

VMTHook* Hooks::Panel_Table;
VMTHook* Hooks::Engine_Table;
VMTHook* Hooks::Client_Table;
VMTHook* Hooks::Surface_Table;
VMTHook* Hooks::ClientMod_Table;
VMTHook* Hooks::D3Ddevice_Table;
VMTHook* Hooks::NetChannel_Table;
VMTHook* Hooks::RenderView_Table;
VMTHook* Hooks::EngineSound_Table;
VMTHook* Hooks::ModelRender_Table;
VMTHook* Hooks::GameMovement_Table;
VMTHook* Hooks::BSPTreeQuery_Table;
VMTHook* Hooks::MatSystemOther_Table;
VMTHook* Hooks::GameEventManager_Table;

void OnLevelInit();

void mmcopy
(
	void* address,
	const void* value,
	size_t bytes
);

#include "General.hpp"
#include "NetvarHooks.h"

void Initialize_Loactions() noexcept {

	sv_cheats = malloc(72); assert(sv_cheats);

	fog_override = malloc(72); assert(fog_override);

	hWindow = FindWindowA("Valve001", nullptr); assert(hWindow);

	if (auto r_shadows(Cvar->FindVar("r_shadows")); r_shadows)
		r_shadows->SetValue(0);

	if (auto r_3dsky(Cvar->FindVar("r_3dsky")); r_3dsky)
		r_3dsky->SetValue(0);

	m_fEffects = new NetVarHookInfo("DT_BaseEntity", "m_fEffects", m_fEffectsHook); assert(m_fEffects->Original_Fn);

	m_VecForceProxy = new NetVarHookInfo("DT_CSRagdoll", "m_vecForce", VecForce); assert(m_VecForceProxy->Original_Fn);

	m_iHealthProxy = new NetVarHookInfo("DT_BasePlayer", "m_iHealth", m_iHealthHook); assert(m_iHealthProxy->Original_Fn);

	DirectXDevice = **(void***)(Tools::FindPattern("shaderapidx9.dll", "8B 0D ? ? ? ? F7 DF") + 2);

	Create_Console_Variable_Location = (void*)(Tools::FindPattern("engine.dll", "55 8B EC D9 EE 56 FF 75 18"));

	CLC_ListenEvents_Table = *(CLC_ListenEvents**)(Tools::FindPattern("engine.dll", "C7 01 ? ? ? ? 85 C0") + 0x2);

	Warning = new DetourHookInfo(uintptr_t(GetProcAddress(GetModuleHandle("tier0.dll"), "Warning")), Hooked_Warning, 2);

	DrawModel = new DetourHookInfo(Tools::FindPattern("client.dll", "55 8B EC 83 EC 30 53 33 D2"), Hooked_DrawModel, 0);

	CLC_RespondCvarValue_Table = *(CLC_RespondCvarValue**)(Tools::FindPattern("engine.dll", "8B 01 52 8B 40 34") - 0x1E);

	SetViewModel = new DetourHookInfo(Tools::FindPattern("client.dll", "C2 04 00 56 57 8B F9") + 0x3, Hooked_SetViewModel, 4);

	ReadWavFile = new DetourHookInfo(Tools::FindPattern("engine.dll", "51 56 68 ? ? ? ? FF 75 08") - 0x3, Hooked_ReadWavFile, 4);

	CheckWhitelist = new DetourHookInfo(Tools::FindPattern("engine.dll", "55 8B EC 83 3D ? ? ? ? ? 7E 5E"), Hooked_CheckWhitelist, 4);

	CalcViewModelView = new DetourHookInfo(Tools::FindPattern("client.dll", "55 8B EC 83 EC 24 8B 55 10"), Hooked_CalcViewModelView, 0);

	ConsistencyCheck = new DetourHookInfo(Tools::FindPattern("engine.dll", "81 EC A0 02 ? ? 53 8B D9") - 0x3, Hooked_ConsistencyCheck, 3);

	FireEventIntern = new DetourHookInfo(Tools::FindPattern("engine.dll", "55 8B EC 83 EC 34 53 8B 5D 08 57"), Hooked_FireEventIntern, 0);

	ProcessStringCmd = new DetourHookInfo(Tools::FindPattern("engine.dll", "55 8B EC 80 B9 ? ? ? ? ? 74 2F"), Hooked_ProcessStringCmd, 4);

	GetPMaterial = new DetourHookInfo(CallableFromRelative(Tools::FindPattern("client.dll", "E8 ? ? ? ? 8D 5F F8")), Hooked_GetPMaterial, 0);

	SimulateEntities = new DetourHookInfo(Tools::FindPattern("client.dll", "83 EC 10 8B 0D ? ? ? ? 53 56") - 0x3, Hooked_SimulateEntities, 0);

	GetCvarValue = new DetourHookInfo(Tools::FindPattern("engine.dll", "55 8B EC 81 EC ? ? ? ? 56 57 8B 7D 08 89 4D FC"), Hooked_GetCvarValue, 3);

	CheckCRCs = new DetourHookInfo(CallableFromRelative(Tools::FindPattern("engine.dll", "E8 ? ? ? ? 83 C4 04 84 C0 75 21")), Hooked_CheckCRCs, 0);

	GetFgColor = new DetourHookInfo(CallableFromRelative(Tools::FindPattern("client.dll", "E8 ? ? ? ? 8B 45 08 5D C2 04 00")), Hooked_GetFgColor, 0);

	ProcessFixAngle = new DetourHookInfo(Tools::FindPattern("engine.dll", "55 8B EC 8B 45 08 83 EC 08 F3 0F 10 15 ? ? ? ?"), Hooked_ProcessFixAngle, 0);

	C_ParticleSmokeGrenade = new DetourHookInfo(Tools::FindPattern("client.dll", "55 8B EC 81 EC 00 01 ? ? 56 57 8B F9"), Hooked_SmokeGrenade_Start, 3);

	ProcessMuzzleFlashEvent = new DetourHookInfo(Tools::FindPattern("client.dll", "55 8B EC 83 EC 68 53 56 57 8B F9"), Hooked_ProcessMuzzleFlashEvent, 0);

	DrawSpriteModel = new DetourHookInfo(CallableFromRelative(Tools::FindPattern("client.dll", "E8 ? ? ? ? 83 C4 38 83 FE 03")), Hooked_DrawSpriteModel, 2);

	OnScreenSizeChanged = new DetourHookInfo(Tools::FindPattern("vguimatsurface.dll", "50 64 89 25 ? ? ? ? 83 EC 10 56") - 0x10, Hooked_OnScreenSizeChanged, 4);

	CL_QueueDownload = new DetourHookInfo(CallableFromRelative(Tools::FindPattern("engine.dll", "E8 ? ? ? ? 83 C4 04 8B 8B ? ? ? ?")), Hooked_CL_QueueDownload, 7);

	CSteam3Client_InitiateConnection = new DetourHookInfo(Tools::FindPattern("engine.dll", "55 8B EC 83 EC 10 8B 45 1C"), Hooked_CSteam3Client_InitiateConnection, 0);

	WriteListenEventList = new DetourHookInfo(CallableFromRelative(Tools::FindPattern("engine.dll", "E8 ? ? ? ? 8B 4E 10 8D 55 A4")), Hooked_WriteListenEventList, 0);

	C_BaseAnimating_DrawModel = new DetourHookInfo(CallableFromRelative(Tools::FindPattern("client.dll", "E8 ? ? ? ? 8D 5E FC")), Hooked_C_BaseAnimating_DrawModel, 0);

	ClientModeShared_FireGameEvent = new DetourHookInfo(Tools::FindPattern("client.dll", "55 8B EC 81 EC ? ? ? ? 56 57 89 4D FC"), Hooked_ClientModeShared_FireGameEvent, 3);

	CSpriteTrail_GetRenderOrigin = new DetourHookInfo(Tools::FindPattern("client.dll", "55 8B EC 57 FF 75 08 8B F9 E8 ? ? ? ? 83 7D 08 00 75 1B") - 0xD0, Hooked_CSpriteTrail_GetRenderOrigin, 2);

	Create_Console_Variable(fog_override, "fog_override", "0", FCVAR_CHEAT | FCVAR_CLIENTDLL, "", nullptr); assert(Cvar->FindVar("fog_override"));

	Create_Console_Variable(sv_cheats, "sv_cheats", "0", FCVAR_NOTIFY | FCVAR_REPLICATED, "Allow cheats on server", nullptr); assert(Cvar->FindVar("sv_cheats"));

	if (auto NetChannel = ClientState->m_NetChannel; NetChannel) {

		CLC_ListenEvents msg;

		*(void**)&msg = CLC_ListenEvents_Table;

		Hooked_WriteListenEventList(&msg);

		ClientState->m_NetChannel->SendNetMsg((void*&)msg);
	}
}

void Hooks::Initialize(HMODULE MyDLL) noexcept
{
	Precache_Custom_Stuff(MyDLL);

	Config::LoadConfig(); OnLevelInit(); Initialize_Loactions();

	Client_Table = new VMTHook(BaseClientDLL);
	{
		CHLClient::Original_Init = Client_Table->Hook<CHLClient::Init_Type>(6, CHLClient::LevelInitPostEntity); assert(CHLClient::Original_Init);

		CHLClient::Original_Shutdown = Client_Table->Hook<CHLClient::Shutdown_Type>(7, CHLClient::LevelShutdown); assert(CHLClient::Original_Shutdown);

		CHLClient::Original_CreateMove = Client_Table->Hook<CHLClient::CreateMove_Type>(21, CHLClient::Hooked_CreateMove); assert(CHLClient::Original_CreateMove);

		CHLClient::Original_FrameStageNotify = Client_Table->Hook<CHLClient::FrameStageNotify_Type>(35, CHLClient::FrameStageNotify); assert(CHLClient::Original_FrameStageNotify);

		CHLClient::Original_DispatchUserMessage = Client_Table->Hook<CHLClient::DispatchUserMessage_Type>(36, CHLClient::DispatchUserMessage); assert(CHLClient::Original_DispatchUserMessage);
	}

	ClientMod_Table = new VMTHook(ClientMode);
	{
		Original_OverrideView = ClientMod_Table->Hook<OverrideView_Type>(16, OverrideView); assert(Original_OverrideView);

		Original_GetViewModelFov = ClientMod_Table->Hook<GetViewModelFov_Type>(32, GetViewModelFov); assert(Original_GetViewModelFov);
	}

	D3Ddevice_Table = new VMTHook(DirectXDevice);
	{
		Original_Reset = D3Ddevice_Table->Hook<Reset_Fn>(16, Reset); assert(Original_Reset);

		Original_EndScane = D3Ddevice_Table->Hook<EndScane_Fn>(42, EndScane); assert(Original_EndScane);
	}

	Panel_Table = new VMTHook(Panel);
	{
		Original_PaintTraverse = Panel_Table->Hook<PaintTraverse_Type>(41, PaintTraverse); assert(Original_PaintTraverse);
	}

	ModelRender_Table = new VMTHook(MdlRender);
	{
		Original_DrawModelExecute = ModelRender_Table->Hook<DrawModelExecute_Type>(19, DrawModelExecute); assert(Original_DrawModelExecute);
	}

	EngineSound_Table = new VMTHook(EngineSound);
	{
		Original_EmitSound = EngineSound_Table->Hook<EmitSound_Type>(4, Hooked_EmitSound);
	}
	
	//Surface_Table = new VMTHook(Surface);
	{
		 //Surface_Table->Hook(15, Hooked_DrawLine);
	}

	//RenderView_Table = new VMTHook(RenderView);
	{
		//Original_DrawLights = RenderView_Table->Hook<DrawLights_fn>(16, DrawLights);
	}

	//MatSystemOther_Table = new VMTHook(MatSystemOther);
	{
		//Original_FindMaterial = MatSystemOther_Table->Hook<FindMaterial_Type>(71, FindMaterial); assert(Original_FindMaterial);
	}

	if(hWindow) Original_Wnd_Proc = WNDPROC((SetWindowLongPtr(hWindow, GWLP_WNDPROC, LONG_PTR(Wnd_Proc)))); assert(Original_Wnd_Proc);
}

void Hooks::Uninitialize() noexcept
{
	if (auto NetChannel = ClientState->m_NetChannel; NetChannel) {

		CLC_ListenEvents msg; *(void**)&msg = CLC_ListenEvents_Table; reinterpret_cast<void(__thiscall*)(PVOID, PVOID)>(WriteListenEventList->Original)(EventManager, &msg);

		ClientState->m_NetChannel->SendNetMsg((void*&)msg);
	}

	// Netvars \\

	delete m_fEffects;

	delete m_iHealthProxy;

	delete m_VecForceProxy;

	// Hooks

	delete Warning;

	delete DrawModel;

	delete EmitSound;

	delete CheckCRCs;

	delete GetFgColor;

	delete ReadWavFile;

	delete ApplyConVars;

	delete GetCvarValue;

	delete GetPMaterial;

	delete SetViewModel;

	delete CheckWhitelist;

	delete DrawSpriteModel;

	delete FireEventIntern;

	delete ProcessFixAngle;

	delete ProcessStringCmd;

	delete SimulateEntities;

	delete ConsistencyCheck;

	delete CL_QueueDownload;

	delete CalcViewModelView;

	delete OnScreenSizeChanged;

	delete WriteListenEventList;

	delete C_ParticleSmokeGrenade;

	delete ProcessMuzzleFlashEvent;

	delete C_BaseAnimating_DrawModel;

	delete CSpriteTrail_GetRenderOrigin;

	delete ClientModeShared_FireGameEvent;

	delete CSteam3Client_InitiateConnection;

	delete Panel_Table;

	delete Engine_Table;

	delete Client_Table;

	delete Surface_Table;

	delete ClientMod_Table;

	delete D3Ddevice_Table;

	delete RenderView_Table;

	delete EngineSound_Table;

	delete NetChannel_Table;

	delete ModelRender_Table;

	delete GameMovement_Table;

	delete BSPTreeQuery_Table;

	delete MatSystemOther_Table;

	delete GameEventManager_Table;

	if (hWindow) SetWindowLongPtr(hWindow, GWLP_WNDPROC, LONG_PTR(Original_Wnd_Proc));
}

void mmcopy(void* address, const void* value, size_t bytes)
{
	static auto Protection_Backup{ TickCount };

	VirtualProtect(address, bytes, PAGE_EXECUTE_READWRITE, &Protection_Backup);

	memcpy(address, value, bytes);

	VirtualProtect(address, bytes, Protection_Backup, nullptr);
}
