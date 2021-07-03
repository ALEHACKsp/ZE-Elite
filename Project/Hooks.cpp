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

	unsigned long EngineModuleHandle = EngineModule; unsigned long ClientModuleHandle = ClientModule;

	CLC_ListenEvents_Table = (CLC_ListenEvents*)(EngineModuleHandle + 0x2F0D74);

	CLC_RespondCvarValue_Table = (void*)(EngineModuleHandle + 0x2F0E00);

	FireEventIntern = new DetourHookInfo(EngineModuleHandle + 0x195E90, Hooked_FireEventIntern, 0);

	DrawSpriteModel = new DetourHookInfo(ClientModuleHandle + 0xC9EE0, Hooked_DrawSpriteModel, 2);

	CheckCRCs = new DetourHookInfo(EngineModuleHandle + 0xBA6A0, Hooked_CheckCRCs, 0);

	ReadWavFile = new DetourHookInfo(EngineModuleHandle + 0x936D0, Hooked_ReadWavFile, 4);

	CheckWhitelist = new DetourHookInfo(EngineModuleHandle + 0xBA840, Hooked_CheckWhitelist, 4);

	ConsistencyCheck = new DetourHookInfo(EngineModuleHandle + 0xCFA90, Hooked_ConsistencyCheck, 3);

	C_BaseAnimating_DrawModel = new DetourHookInfo(ClientModuleHandle + 0x81D30, Hooked_C_BaseAnimating_DrawModel, 0);

	ProcessMuzzleFlashEvent = new DetourHookInfo(ClientModuleHandle + 0x1D1E20, Hooked_ProcessMuzzleFlashEvent, 0);

	CSpriteTrail_GetRenderOrigin = new DetourHookInfo(ClientModuleHandle + 0x1948F0, Hooked_CSpriteTrail_GetRenderOrigin, 2);

	OnScreenSizeChanged = new DetourHookInfo(Tools::FindPattern("vguimatsurface.dll", "50 64 89 25 ? ? ? ? 83 EC 10 56") - 0x10, Hooked_OnScreenSizeChanged, 4);

	ProcessFixAngle = new DetourHookInfo(EngineModuleHandle + 0x1EEEF0, Hooked_ProcessFixAngle, 0);

	SetViewModel = new DetourHookInfo(ClientModuleHandle + 0x64000, Hooked_SetViewModel, 4);

	SimulateEntities = new DetourHookInfo(ClientModuleHandle + 0xD5A60, Hooked_SimulateEntities, 0);

	GetCvarValue = new DetourHookInfo(EngineModuleHandle + 0x9F750, Hooked_GetCvarValue, 3);

	GetPMaterial = new DetourHookInfo(ClientModuleHandle + 0x163CD0, Hooked_GetPMaterial, 0);

	ClientModeShared_FireGameEvent = new DetourHookInfo(ClientModuleHandle + 0x1D4D40, Hooked_ClientModeShared_FireGameEvent, 3);

	CalcViewModelView = new DetourHookInfo(ClientModuleHandle + 0x6B840, Hooked_CalcViewModelView, 0);

	Warning = new DetourHookInfo(uintptr_t(GetProcAddress(GetModuleHandle("tier0.dll"), "Warning")), Hooked_Warning, 2);

	ProcessStringCmd = new DetourHookInfo(EngineModuleHandle + 0x9FEE0, Hooked_ProcessStringCmd, 4);

	CL_QueueDownload = new DetourHookInfo(EngineModuleHandle + 0x1840D0, Hooked_CL_QueueDownload, 7);

	DrawModel = new DetourHookInfo(ClientModuleHandle + 0x162A60, Hooked_DrawModel, 0);

	C_ParticleSmokeGrenade = new DetourHookInfo(ClientModuleHandle + 0x233050, Hooked_SmokeGrenade_Start, 3);

	CSteam3Client_InitiateConnection = new DetourHookInfo(EngineModuleHandle + 0xC2E00, Hooked_CSteam3Client_InitiateConnection, 0);

	WriteListenEventList = new DetourHookInfo(EngineModuleHandle + 0x197560, Hooked_WriteListenEventList, 0);

	DirectXDevice = **(void***)(Tools::FindPattern("shaderapidx9.dll", "8B 0D ? ? ? ? F7 DF") + 2); assert(DirectXDevice);

	Create_Console_Variable_Location = (void*)(EngineModuleHandle + 0x246610);

	Create_Console_Variable(fog_override, "fog_override", "0", FCVAR_CHEAT | FCVAR_CLIENTDLL, "", nullptr); assert(Cvar->FindVar("fog_override"));

	Create_Console_Variable(sv_cheats, "sv_cheats", "0", FCVAR_NOTIFY | FCVAR_REPLICATED, "Allow cheats on server", nullptr); assert(Cvar->FindVar("sv_cheats"));

	if (auto NetChannel = ClientState->m_NetChannel; NetChannel) {

		CLC_ListenEvents msg;

		*(void**)&msg = CLC_ListenEvents_Table;

		Hooked_WriteListenEventList(&msg);

		ClientState->m_NetChannel->SendNetMsg((void*&)msg);
	}
}

void Hooks::Initialize() noexcept
{
	Precache_materials_and_models();

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