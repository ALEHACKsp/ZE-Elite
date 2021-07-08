#ifndef General_H
#define General_H

#include "include.h"
#include "Menu.h"
#include "SDK.h"

#include <fstream>
#include <iomanip>
#include "MaterialManager.h"
#include "IKeyValues.h"
#include "CvarQuery.h"
#include "SteamID/SteamID.hpp"

#include "Esp.h"
#include "Assorted.h"
#include "HitSound.h"
#pragma comment(lib, "Winmm.lib") 

HWND hWindow;

bool PressedKeys[256];

WNDPROC Original_Wnd_Proc;

// ------------ \\

IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND, UINT, WPARAM, LPARAM);

using PaintTraverse_Type = void(__thiscall*)(void*, unsigned int, bool, bool);

PaintTraverse_Type Original_PaintTraverse;

void __fastcall PaintTraverse(PVOID pPanels, int edx, unsigned int vguiPanel, bool forceRepaint, bool allowForce) noexcept
{
	Original_PaintTraverse(pPanels, vguiPanel, forceRepaint, allowForce);

	static bool WasEnabled{ false }, Unload_Process_WasDone{ false };

	static unsigned int vguiFocusOverlayPanel(0);

	if (vguiFocusOverlayPanel == NULL)
	{
		const char* szName = Panel->GetName(vguiPanel);

		if (szName[0] == 'F' && szName[5] == 'O' && szName[12] == 'P')
		{
			vguiFocusOverlayPanel = vguiPanel;

			gMat.Initialize();
		}
	}


	if (vguiFocusOverlayPanel == vguiPanel)
	{
		if (!Esp::Fonts_Were_Initialized)
		{
			Esp::Initialize();
		}

		Esp::Render();

		if (Menu::Get.General.Unload)
		{
			if (!Unload_Process_WasDone)
			{
				Panel->SetMouseInputEnabled(vguiPanel, false);

				Unload_Process_WasDone = !Unload_Process_WasDone;
			}
		}
		else
		{
			//extern bool Draw_Map_Notification_Question;

			if (!WasEnabled && (Menu::Get.Menu.On/* || Draw_Map_Notification_Question*/))
			{
				Panel->SetMouseInputEnabled(vguiPanel, true);

				WasEnabled = !WasEnabled;
			}
			else if (WasEnabled && !Menu::Get.Menu.On /*&& !Draw_Map_Notification_Question*/)
			{
				Panel->SetMouseInputEnabled(vguiPanel, false);

				WasEnabled = !WasEnabled;
			}
		}
	}
}

static unsigned __int32 __stdcall Wnd_Proc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) noexcept
{
	switch (uMsg)
	{
	case WM_LBUTTONDOWN:
		PressedKeys[VK_LBUTTON] = true;
		break;
	case WM_LBUTTONUP:
		PressedKeys[VK_LBUTTON] = false;
		break;
	case WM_RBUTTONDOWN:
		PressedKeys[VK_RBUTTON] = true;
		break;
	case WM_RBUTTONUP:
		PressedKeys[VK_RBUTTON] = false;
		break;
	case WM_KEYDOWN:
		PressedKeys[wParam] = true;
		break;
	case WM_KEYUP:
		PressedKeys[wParam] = false;
		break;
	default: break;
	}


	static bool IsDown = false, IsClicked = false;

	if (PressedKeys[VK_INSERT])
	{
		IsClicked = false;
		IsDown = true;
	}
	else if (!PressedKeys[VK_INSERT] && IsDown)
	{
		IsClicked = true;
		IsDown = false;
	}
	else
	{
		IsClicked = false;
		IsDown = false;
	}

	if (IsClicked)
	{
		Menu::Get.Menu.On = !Menu::Get.Menu.On;
	}

	//extern bool Draw_Map_Notification_Question;

	if (Menu::Get.Menu.On/* || Draw_Map_Notification_Question*/)
	{
		ImGui_ImplWin32_WndProcHandler(hWnd, uMsg, wParam, lParam);

		return WM_CREATE;
	}

	static auto Voice_RecordStart = reinterpret_cast<bool(__cdecl*)(const char*, const char*, const char*)>(
		CallableFromRelative(Tools::FindPattern("engine.dll", "E8 ? ? ? ? 83 C4 0C 5F 5E C3")));

	static auto Dirctory = std::string(DirectoryPath).append("\\cstrike\\sound\\ZE-Elite\\");
	
	for (auto& SoundInfo : SoundList)
	{
		auto Key = SoundInfo.GetKey();

		if (PressedKeys[Key] && !SoundInfo.KeyWasPressed)
		{
			SoundInfo.EndTime = Globals->curtime + SoundInfo.SoundDuration;

			Engine->ClientCmd_Unrestricted("voice_loopback 1");

			Voice_RecordStart(NULL, NULL, std::string(Dirctory + SoundInfo.GetName() + ".wav").c_str());

			SoundInfo.KeyWasPressed = true;
		}
	}

	return CallWindowProc(Original_Wnd_Proc, hWnd, uMsg, wParam, lParam);
}

using DrawModelExecute_Type = void(__thiscall*)(
	void*,
	const DrawModelState_t&,
	const ModelRenderInfo_t&,
	matrix3x4*
	);

DrawModelExecute_Type Original_DrawModelExecute;

void __stdcall DrawModelExecute(const DrawModelState_t& state, const ModelRenderInfo_t& pInfo, matrix3x4* pCustomBoneToWorld) noexcept
{
	auto LocalPlayer{ CBaseEntity::GetLocalPlayer() };

	static std::string PlayerModel{ "models/player" }, weaponModel{ "models/weapons/" }, WeaponModel{ "models/Weapons/" };

	auto ModelName(GetModelName(pInfo.pModel));

	if (!LocalPlayer || (pInfo.entity_index == -1 && (strstr(ModelName, weaponModel.c_str()) || strstr(ModelName, WeaponModel.c_str()))))
		return;

	auto pModelEntity{ GetEntity(pInfo.entity_index) };

	static std::string DroppedWeapon{ "_dropped" };

	if (LocalPlayer->IsAlive() && pModelEntity)
	{
		//const auto* ModelName = GetModelName(pModelEntity->GetModel());

		const bool IsPlayerModel = strstr(ModelName, PlayerModel.c_str());

		const bool IsWeaponModel = strstr(ModelName, weaponModel.c_str()) && !strstr(ModelName, DroppedWeapon.c_str());

		if (IsPlayerModel)
		{
			if (Menu::Get.Visuals.Chams.Player.CMP_Enabled)
			{
				if (pModelEntity->IsAlive() && pModelEntity->IsPlayer() && pModelEntity->IsEnemy(LocalPlayer))
				{
					if (auto Player_Material = gMat.Chams[Menu::Get.Visuals.Chams.Player.CMP_Type]; Player_Material)
					{
						if (Player_Material->GetMaterialVarFlag(MATERIAL_VAR_WIREFRAME) != Menu::Get.Visuals.Chams.Player.CMP_Wireframe)
							Player_Material->SetMaterialVarFlag(MATERIAL_VAR_WIREFRAME, Menu::Get.Visuals.Chams.Player.CMP_Wireframe);

						if (Player_Material->GetMaterialVarFlag(MATERIAL_VAR_IGNOREZ) != Menu::Get.Visuals.Chams.Player.CMP_Ignorez)
							Player_Material->SetMaterialVarFlag(MATERIAL_VAR_IGNOREZ, Menu::Get.Visuals.Chams.Player.CMP_Ignorez);

						ForceMaterial(Menu::Get.Colors.Chams.Player, Player_Material);

						Original_DrawModelExecute(MdlRender, state, pInfo, pCustomBoneToWorld);

						gMat.ResetMaterial();

						return;
					}
				}
			}
		}


		if (IsWeaponModel)
		{
			if (Menu::Get.Visuals.Chams.Weapon.CMW_Enabled)
			{
				if (auto OWNER = GetOwner(pModelEntity); !OWNER && pModelEntity->GetOwner() != 255)
				{
					if (auto Weapon_Material = gMat.Chams[Menu::Get.Visuals.Chams.Weapon.CMW_Type]; Weapon_Material)
					{
						if (Weapon_Material->GetMaterialVarFlag(MATERIAL_VAR_WIREFRAME) != Menu::Get.Visuals.Chams.Weapon.CMW_Wireframe)
							Weapon_Material->SetMaterialVarFlag(MATERIAL_VAR_WIREFRAME, Menu::Get.Visuals.Chams.Weapon.CMW_Wireframe);

						if (Weapon_Material->GetMaterialVarFlag(MATERIAL_VAR_IGNOREZ) != Menu::Get.Visuals.Chams.Weapon.CMW_Ignorez)
							Weapon_Material->SetMaterialVarFlag(MATERIAL_VAR_IGNOREZ, Menu::Get.Visuals.Chams.Weapon.CMW_Ignorez);

						ForceMaterial(Menu::Get.Colors.Chams.CWeapon, Weapon_Material);

						Original_DrawModelExecute(MdlRender, state, pInfo, pCustomBoneToWorld);

						gMat.ResetMaterial();

						return;
					}
				}
			}
		}
	}

	Original_DrawModelExecute(MdlRender, state, pInfo, pCustomBoneToWorld);
}

using OverrideView_Type = void(__thiscall*)(void*, void*);

OverrideView_Type Original_OverrideView;

void __fastcall OverrideView(void* _this, void* _edx, CViewSetup* pSetup) noexcept// credits, ActualCheats and outi - plasma
{
	Original_OverrideView(_this, pSetup); //Call the original.

	const auto current_Fov = pSetup->m_fov;

	if (Menu::Get.Visuals.FieldOfView NotEquals 0
		&& current_Fov NotEquals 40.0f
		&& current_Fov NotEquals 10.0f
		&& current_Fov NotEquals 15.0f
		&& current_Fov NotEquals Menu::Get.Visuals.FieldOfView)
	{
		pSetup->m_fov = current_Fov + Menu::Get.Visuals.FieldOfView;
	}
}

using GetViewModelFov_Type = float(__thiscall*)();

GetViewModelFov_Type Original_GetViewModelFov;

float __fastcall GetViewModelFov() noexcept
{
	return Original_GetViewModelFov() + Menu::Get.Visuals.ViewModelFov;
}
 

void __fastcall Hooked_EmitSound
(
	void* ECX,
	void* EDX,
	void*& filter,
	int iEntIndex,
	int iChannel,
	const char* pSample,
	float flVolume,
	float flAttenuation,
	int iFlags = 0,
	int iPitch = 100,
	int iSpecialDSP = 0,
	const Vector* pOrigin = NULL,
	const Vector* pDirection = NULL,
	CUtlVector< Vector >* pUtlVecOrigins = NULL,
	bool bUpdatePositions = true,
	float soundtime = 0.0f,
	int speakerentity = -1
);

using EmitSound_Type = decltype(&Hooked_EmitSound); EmitSound_Type Original_EmitSound;

struct EmitSound_t { int m_nChannel; char const* m_pSoundName; float m_flVolume; };

void __fastcall Hooked_EmitSound(void* ECX, void* EDX, void*& filter, int iEntIndex, int iChannel, const char* pSample,
	float flVolume, float flAttenuation, int iFlags, int iPitch, int iSpecialDSP,
	const Vector* pOrigin, const Vector* pDirection, CUtlVector< Vector >* pUtlVecOrigins, bool bUpdatePositions, float soundtime, int speakerentity)
{
	if (strstr(pSample, "weapons/")) { flVolume = Menu::Get.Misc.Sounds.WeaponsAudio * 0.01f; }
	else 
	if (strstr(pSample, "footsteps")) { flVolume = Menu::Get.Misc.Sounds.FootStepsVolume * 0.01f; }

	Original_EmitSound(ECX, EDX, filter, iEntIndex, iChannel, pSample, flVolume, flAttenuation, iFlags, iPitch, iSpecialDSP, pOrigin, pDirection, pUtlVecOrigins, bUpdatePositions, soundtime, speakerentity);
}

// Detours \\


class DetourHookInfo
{
public:
	PVOID Original;

	DetourHookInfo(uintptr_t Addr, PVOID Fn, size_t offset) : Location(PVOID(Addr)) {

		Original = nullptr;

		if(Addr) Redirection_Manager::Redirect_Function(Original, offset, Location, 1, Fn);
	}

	~DetourHookInfo() {

		if (Original) Redirection_Manager::Restore_Function(1, Location, Original, 0);
	}

private:

	PVOID Location;
};

DetourHookInfo* Warning;
DetourHookInfo* CheckCRCs;
DetourHookInfo* DrawModel;
DetourHookInfo* EmitSound;
DetourHookInfo* GetFgColor;
DetourHookInfo* ReadWavFile;
DetourHookInfo* GetCvarValue;
DetourHookInfo* GetPMaterial;
DetourHookInfo* SetViewModel;
DetourHookInfo* ApplyConVars;
DetourHookInfo* CheckWhitelist;
DetourHookInfo* ProcessFixAngle;
DetourHookInfo* DrawSpriteModel;
DetourHookInfo* FireEventIntern;
DetourHookInfo* CL_QueueDownload;
DetourHookInfo* SimulateEntities;
DetourHookInfo* ProcessStringCmd;
DetourHookInfo* ConsistencyCheck;
DetourHookInfo* CalcViewModelView;
DetourHookInfo* OnScreenSizeChanged;
DetourHookInfo* WriteListenEventList;
DetourHookInfo* C_ParticleSmokeGrenade;
DetourHookInfo* ProcessMuzzleFlashEvent;
DetourHookInfo* C_BaseAnimating_DrawModel;
DetourHookInfo* CSpriteTrail_GetRenderOrigin;
DetourHookInfo* ClientModeShared_FireGameEvent;
DetourHookInfo* CSteam3Client_InitiateConnection;

int __fastcall Hooked_C_BaseAnimating_DrawModel(CBaseEntity* ECX, void*, int flags)
{
	static auto Original = reinterpret_cast<int(__thiscall*)(void*, int)>(C_BaseAnimating_DrawModel->Original);

	if (Menu::Get.Visuals.NoTeammates)
	{
		if (auto pLocal = CBaseEntity::GetLocalPlayer(); pLocal && pLocal->IsAlive())
		{
			auto pEntity = (ECX - 4);

			if (!pEntity->IsEnemy(pLocal) && pLocal != pEntity)
				return 0;
		}
	}

	return Original(ECX, flags);
}

const Vector& __fastcall Hooked_CSpriteTrail_GetRenderOrigin(void* ECX)
{
	static auto Original = reinterpret_cast<const Vector & (__thiscall*)(void*)>(CSpriteTrail_GetRenderOrigin->Original);

	static Vector Origin; Origin = Original(ECX);

	Origin.z += 10.0f;

	return Origin;
}

void __fastcall Hooked_ProcessMuzzleFlashEvent(void* ECX)
{
	static auto Original = reinterpret_cast<void(__thiscall*)(void*)>(ProcessMuzzleFlashEvent->Original);

	if (Menu::Get.Visuals.Disable_Players_MuzzleFlash) return;

	Original(ECX);
}

void __fastcall Hooked_SetViewModel(void* ECX)
{
	static auto Original = reinterpret_cast<void(__thiscall*)(void*)>(SetViewModel->Original);

	Original(ECX);

	if (auto pLocal = CBaseEntity::GetLocalPlayer(); pLocal)
	{
		if (auto ActiveWeapon = pLocal->GetActiveWeapon(); ECX == ActiveWeapon)
		{
			SkinChanger(pLocal);
		}
	}
}

void Hooked_DrawSpriteModel
(
	CSprite* baseentity,
	void* psprite,
	const Vector& origin,
	float fscale,
	float frame,
	int rendermode,
	int r, int g, int b, int a,
	const Vector& forward,
	const Vector& right,
	const Vector& up,
	float flHDRColorScale)
{
	static auto Original = reinterpret_cast<decltype(&Hooked_DrawSpriteModel)>(DrawSpriteModel->Original);

	auto EditedOrigin = origin; bool IsFoundInSpriteEntities(CSprite * Entity);

	if (HealthSprite::IsFoundInSpriteList(baseentity)) EditedOrigin.z += 80.f;

	Original(baseentity, psprite, EditedOrigin, fscale, frame, rendermode, r, g, b, a, forward, right, up, flHDRColorScale);
}

char __fastcall Hooked_ApplyConVars(void* ECX, void*, void* Args)
{
	static auto Original = reinterpret_cast<void(__thiscall*)(void*, void*)>(ApplyConVars->Original);

	return 1;
}

Color& __fastcall Hooked_GetFgColor(void* Self, void* EDX, void* UnknownArg)
{
	static auto Original = reinterpret_cast<Color & (__thiscall*)(void*, void*)>(GetFgColor->Original);

	static auto GePanelName = Get_vFunction<const char* (__thiscall*)(void*)>(
		Self, *reinterpret_cast<BYTE*>(Tools::FindPattern("client.dll", "E8 ? ? ? ? EB 94") + 0x15) / 4);

	for (auto& Element : HudList)
	{
		if (_stricmp(Element, GePanelName(Self)) == 0)
		{
			*(Color*&)UnknownArg = Menu::Get.Colors.General.HudColor;

			return *(Color*&)UnknownArg;
		}
	}

	return Original(Self, UnknownArg);
}

bool __cdecl Hooked_ReadWavFile
(
	const char* pFilename,
	char*& pData,
	int& nDataBytes,
	int& wBitsPerSample,
	int& nChannels,
	int& nSamplesPerSec
)
{
	std::vector<unsigned char>Temp;

	unsigned char x;

	std::ifstream input(pFilename, std::ios::in | std::ios::binary);

	input >> std::noskipws;

	while (input >> x) {

		Temp.push_back(x);
	}

	if (auto size = Temp.size(); size)
	{
		nDataBytes = size;

		pData = new char[size];

		mmcopy(pData, Temp.data(), size);
	}

	return true;
}

void __stdcall Hooked_OnScreenSizeChanged(int OldWidht, int OldHeight) noexcept
{
	static auto Original = reinterpret_cast<void(__thiscall*)(void*, int, int)>(
		OnScreenSizeChanged->Original);

	Original(Surface, OldWidht, OldHeight);

	Esp::Initialize();
}

void __cdecl Hooked_SimulateEntities()
{
	static auto Original = reinterpret_cast<decltype(&Hooked_SimulateEntities)>(SimulateEntities->Original);

	if (SpriteEntity) SpriteEntity->ClientThink(); Original();
}

bool __fastcall Hooked_CheckCRCs(void*, void*, const char* szMap) noexcept
{
	if (Menu::Get.Misc.Sv_Pure_Bypass)
	{
		return true;
	}

	static auto Original = reinterpret_cast<bool(__cdecl*)(const char*)>(CheckCRCs->Original);

	return Original(szMap);
}

void __fastcall Hooked_CheckWhitelist(void*, void*, int iUnknown) noexcept
{
	if (Menu::Get.Misc.Sv_Pure_Bypass)
	{
		return;
	}

	static auto Original = reinterpret_cast<void(__cdecl*)(int)>(CheckWhitelist->Original);

	Original(iUnknown);
}

void __fastcall Hooked_ConsistencyCheck(void* pThis, void*, bool bChanged) noexcept
{
	if (Menu::Get.Misc.Sv_Pure_Bypass)
	{
		return;
	}

	static auto Original = reinterpret_cast<void(__thiscall*)(void*, int)>(ConsistencyCheck->Original);

	Original(pThis, bChanged);
}

class SVC_FixAngle {
	char pad[16];
public:

	bool m_bRelative;
	QAngle m_Angle;
};

bool __fastcall Hooked_ProcessFixAngle(void* ECX, void* EDX, SVC_FixAngle* msg)
{
	static auto Original = reinterpret_cast<bool(__thiscall*)(void*, SVC_FixAngle*)>(ProcessFixAngle->Original);

	if (msg->m_Angle.IsValid()/*Server might crash you ?*/&& (!Menu::Get.Misc.NoDrug || msg->m_bRelative))
		return Original(ECX, msg);

	return true;
}

void __fastcall Hooked_SmokeGrenade_Start(CBaseEntity* ecx, void* edx, void* pParticleMgr, void* pArgs)
{
	static auto Original = reinterpret_cast<void(__thiscall*)(void*, void*,void*)>(
		C_ParticleSmokeGrenade->Original);

	if (Menu::Get.Visuals.NoSmoke) { 

		return; 
	}

	Original(ecx, pParticleMgr, pArgs);
}

void __stdcall Hooked_ClientModeShared_FireGameEvent(IGameEvent* event)
{
	static auto Original = reinterpret_cast<void(__stdcall*)(void*)>(
		ClientModeShared_FireGameEvent->Original);

	const auto EventName(event->GetName());

	if (!strcmp(EventName, "player_connect_client"))
	{
		ChatElement->ChatPrintf(0, 0, "\x04Player \x03%s \x04has joined the game", event->GetString("name"));

		return;
	}

	if (!strcmp(EventName, "player_disconnect"))
	{
		ChatElement->ChatPrintf(0, 0, "\x04Player \x03%s \x04left the game (\x03%s)", event->GetString("name"), event->GetString("reason"));

		int Disconnected_Index = Engine->GetPlayerForUserID(event->GetInt("userid"));

		if (Disconnected_Index > 0 && Disconnected_Index < 65)
		{
			if (HealthSprite::SpriteEntities[Disconnected_Index].Entity)
			{
				HealthSprite::SpriteEntities[Disconnected_Index].Entity->Remove();

				HealthSprite::SpriteEntities[Disconnected_Index].Entity = nullptr;
			}
		}

		return;
	}

	Original(event);
}


void __fastcall Hooked_CalcViewModelView(CBaseEntity* pThis, void*, CBaseEntity* owner, const Vector& eyePosition, const QAngle& eyeAngles)
{
	auto pLocal = CBaseEntity::GetLocalPlayer();

	static auto Original = reinterpret_cast<void(__thiscall*)(void*, void*, const Vector&, const Vector&)>(CalcViewModelView->Original);

	QAngle vmangles = eyeAngles;

	Vector vmorigin = eyePosition;

	Vector vecRight, vecUp, vecForward;

	Math::AngleVectors(vmangles, &vecForward, &vecRight, &vecUp);

	vmangles += Menu::Get.Visuals.ViewModel.Angles;

	vmorigin += (vecForward * Menu::Get.Visuals.ViewModel.Origin.x) +
		(vecUp * Menu::Get.Visuals.ViewModel.Origin.z) +
		(vecRight * Menu::Get.Visuals.ViewModel.Origin.y);

	Original(pThis, owner, vmorigin, vmangles);
}

void __cdecl Hooked_Warning(char* msg, ...)
{
	static auto Original(reinterpret_cast<void(__cdecl*)(char* msg, ...)>(
		Warning->Original));

	if (Menu::Get.Misc.Disable_Game_Console_Warnings) return void();
	{
		va_list va_alist;

		char buf[1000];

		va_start(va_alist, msg);

		_vsnprintf(buf, sizeof(buf), msg, va_alist);

		va_end(va_alist);

		Original(buf);
	}
}

void __cdecl Hooked_CL_QueueDownload(const char* Name) noexcept
{
	static auto Original(reinterpret_cast<void(__cdecl*)(const char*)>(
		CL_QueueDownload->Original));

	constexpr static auto& Filter = Menu::Get.Misc.DownloadManagerFilter;

	enum { All, NoSounds, MapsOnly, None, CustomFileExtension };

	if (Filter == None
		|| (Filter == MapsOnly && !strstr(Name, ".bsp"))
		|| (Filter == NoSounds && (strstr(Name, ".wav") || strstr(Name, ".mp3")))
		|| (Filter == CustomFileExtension && !FindStringCS(std::string(Name), std::string(Menu::Get.Misc.CustomizeableFiles))))
		return;

	Original(Name);
}

bool __stdcall Hooked_ProcessStringCmd(NET_StringCmd* msg)
{
	static auto Original(reinterpret_cast<bool(__stdcall*)(void*)>(
		ProcessStringCmd->Original));

	// don't let server execute commands in your console.

	return true;
}

int __fastcall Hooked_DrawModel(CBaseEntity* pThis, void*, int flags)
{
	static const auto Original(reinterpret_cast<int(__thiscall*)(CBaseEntity*, int)>(
		DrawModel->Original));

	if (pThis) {

		auto Name{ GetEffectName(PVOID(UINT(pThis) + 8)) };

		if (Name && ((Menu::Get.Visuals.Disable_Fire_Particles && strstr(Name, "burning_character")) || strstr(Name, "explosion_huge"))) {

			return 0;
		}

		return Original(pThis, flags);
	}

	return 0;
}

char CustomMaterialName[64]{ "effects/muzzleflashX" };

void* __fastcall Hooked_GetPMaterial(void* pThis, void* EDX, const char* MaterialName)
{
	static auto Original = reinterpret_cast<void* (__thiscall*)(void*, const char*)>(
		GetPMaterial->Original);

	if (strstr(MaterialName, "muzzleflash") && Menu::Get.Visuals.MuzzleFlash > 0)
	{
		char Buffer[64]{ 0 };

		std::string Effects("effects/");

		if (Menu::Get.Visuals.MuzzleFlash > EffectMaterials.size() - 2)
		{
			return Original(pThis, CustomMaterialName);
		}

		return Original(pThis, Effects.append(EffectMaterials[Menu::Get.Visuals.MuzzleFlash]).c_str());
	}

	return Original(pThis, MaterialName);
}

float PointScale = 0.03f; bool __fastcall Hooked_FireEventIntern(void* pThis, void* EDX, IGameEvent* event, bool bServerOnly, bool bClientOnly)
{
	static auto Original = reinterpret_cast<bool(__thiscall*)(void*, void*, bool, bool)>(FireEventIntern->Original);

	if (auto pLocal = CBaseEntity::GetLocalPlayer(); pLocal && event && !Menu::Get.General.Panic) {

		auto EventName = event->GetName();

		if (strstr(EventName, "player_spawn"))
		{
			int Spawned_INDEX = Engine->GetPlayerForUserID(event->GetInt("userid"));

			if (Spawned_INDEX == Engine->GetLocalPlayer()) {

				Global_Jumps = Perfect_Jumps = 0;
			}

			if (auto Entity = GetEntity(Spawned_INDEX); Entity)
			{
				if (Spawned_INDEX > 0 && Spawned_INDEX < HealthSprite::SpriteEntities.size()) {

					HealthSprite::SpriteEntities[Spawned_INDEX].MaxHealth = Entity->GetHealth();

					if (Entity->IsEnemy(pLocal) && pLocal->IsAlive())
					{
						if (Menu::Get.Visuals.Health && Menu::Get.Visuals.HealthType == 2 && !HealthSprite::SpriteEntities[Spawned_INDEX].Entity)
						{
							HealthSprite::CreateSpriteForEntity(Entity);
						}
					}

					HealthSprite::OnPlayerHurt(HealthSprite::SpriteEntities[Spawned_INDEX], Entity->GetHealth());
				}
			}
		}

		if (strstr(EventName, "bullet_impact"))
		{
			if (auto PlayerIndex = Engine->GetPlayerForUserID(event->GetInt("userid")); PlayerIndex == Engine->GetLocalPlayer()) {

				Vector Position(event->GetFloat("x"), event->GetFloat("y"), event->GetFloat("z"));

				constexpr auto& Bullet_Impact{ Menu::Get.Colors.General.BulletImpact };

				if (Bullet_Impact.a()) {

					DebugOverlay->AddBoxOverlay(Position, Vector(-2, -2, -2), Vector(2, 2, 2), QAngle(0, 0, 0), Bullet_Impact.r(), Bullet_Impact.g(), Bullet_Impact.b(), Bullet_Impact.a(), 4);
				}

				if (Menu::Get.Visuals.Tracers.TR_Enabled) {

					DrawBeamd(pLocal->GetEyePosition() - Vector(0, 0, PointScale), Position, Menu::Get.Colors.General.Tracers);
				}
			}
		}


		if (strstr(EventName, "player_hurt"))
		{
			int attacker = event->GetInt("attacker");

			int dmg_health = event->GetInt("dmg_health");

			int Hurted_INDEX = Engine->GetPlayerForUserID(event->GetInt("userid"));

			if (Engine->GetPlayerForUserID(attacker) == Engine->GetLocalPlayer())
			{
				if (Menu::Get.Visuals.DamageIndicator.DI_Enabled)
				{
					DamageIndicator::data = DamageIndicator_t{ dmg_health, pLocal->GetTickBase() * Globals->interval_per_tick + 1.f };
				}

				if (Menu::Get.Visuals.Hitmarker.HM_Enabled)
				{
					if (Menu::Get.Visuals.HitSound)
					{
						auto Temp = HitSounds.size() - 2;

						Engine->ClientCmd_Unrestricted_Formatted("play \"HitSounds\\%s\"", HitSounds[Menu::Get.Visuals.HitSound >= Temp ? RandomInt(1, Temp) : Menu::Get.Visuals.HitSound].c_str());
					}

					if (Menu::Get.Visuals.Mode)
					{
						Engine->ClientCmd_Unrestricted_Formatted("r_screenoverlay \"overlays\\hitmarkers\\hitmarker%i\"", Menu::Get.Visuals.Overlay);

						Overlay_Triggered = true;
					}

					_flHurtTime = Globals->curtime;
				}
			}
		}

		if (strstr(EventName, "player_death"))
		{
			int DIED_INDEX = Engine->GetPlayerForUserID(event->GetInt("userid")), KILLER_INDEX = Engine->GetPlayerForUserID(event->GetInt("attacker"));

			if (DIED_INDEX == Engine->GetLocalPlayer())
			{
				Engine->ClientCmd_Unrestricted_Formatted("r_screenoverlay \"\"");

				HealthSprite::ClearSprites();

				if (SpriteEntity)
				{
					SpriteEntity->Remove();

					SpriteEntity = nullptr;
				}
			}
			else
			{
				if (DIED_INDEX > 0 && DIED_INDEX < 65)
				{
					if (HealthSprite::SpriteEntities[DIED_INDEX].Entity)
					{
						HealthSprite::SpriteEntities[DIED_INDEX].Entity->Remove();

						HealthSprite::SpriteEntities[DIED_INDEX].Entity = nullptr;
					}
				}
			}

			CBaseEntity* Died_Player = GetEntity(DIED_INDEX), * Killer = GetEntity(KILLER_INDEX);

			if (Died_Player && Killer && Killer == pLocal) {

				if (Menu::Get.Misc.KillMessage && Killmessages.size())
				{
					std::string message = Killmessages[std::rand() % Killmessages.size()];

					if (strstr(message.c_str(), "$nick"))
					{
						std::string dead_player_name = std::string(Died_Player->GetName());

						dead_player_name.erase(std::remove(dead_player_name.begin(), dead_player_name.end(), ';'), dead_player_name.end());
						dead_player_name.erase(std::remove(dead_player_name.begin(), dead_player_name.end(), '"'), dead_player_name.end());
						dead_player_name.erase(std::remove(dead_player_name.begin(), dead_player_name.end(), '\n'), dead_player_name.end());

						message = ReplaceString(message, "$nick", dead_player_name);
					}

					Engine->ClientCmd_Unrestricted_Formatted("say \"%s\"", message.c_str());
				}

				if (Menu::Get.Visuals.KillEffect_Type > 0) {

					auto HeadHitBox(Died_Player->GetHitboxPosition(12));

					auto Angles = Died_Player->GetAbsAngles();

					static CTeslaInfo teslaInfo;

					static CEffectData data;

					[[maybe_unused]] static const auto Once = []()
					{
						data.m_fFlags = 6;
						data.m_vNormal.Init(-1.f, 1.f, -2.f); //some shit I don't care about
						data.m_flScale = 2.f;
						data.m_nColor = 3; //not actual color, but what kind of blood spray that would be (headshot, knife impact, etc)

						teslaInfo.m_pszSpriteName = "sprites/physbeam.vmt";

						teslaInfo.m_flBeamWidth = 5.f;

						teslaInfo.m_flRadius = 100.f;

						teslaInfo.m_vColor.Init(1.f, 1.f, 1.f); //your value up to 255 / 255.f

						teslaInfo.m_flTimeVisible = 0.75f;

						teslaInfo.m_nBeams = 12;

						return 0;
					}();

					data.m_nEntIndex = teslaInfo.m_nEntIndex = DIED_INDEX;

					data.m_vAngles = teslaInfo.m_vAngles = Angles;

					data.m_vOrigin = teslaInfo.m_vPos = HeadHitBox;

					static auto DispatchEffect = reinterpret_cast<void(*)(const char*, const CEffectData&)>(
						CallableFromRelative(Tools::FindPattern("client.dll", "E8 ? ? ? ? 83 C4 08 EB 15")));

					if (Menu::Get.Visuals.KillEffect_Type == 1)
					{
						static auto FX_Tesla = reinterpret_cast<void(__cdecl*)(const  CTeslaInfo&)>(Tools::FindPattern("client.dll", "8B 7B 08 FF 77 18") - 0x24);

						FX_Tesla(teslaInfo);

						DispatchEffect("csblood", data);
					}
					else
					{
						DispatchEffect(KillEffects[Menu::Get.Visuals.KillEffect_Type], data);
					}

					Surface->Play_Sound("ambient/energy/zap3.wav");
				}
			}
		}
	}

	return Original(EventManager, event, bServerOnly, bClientOnly);
}

CLC_RespondCvarValue* CLC_RespondCvarValue_Table;

extern std::vector<DesiredConVarsValueInfo> DesiredConVarsValue;

bool __fastcall Hooked_GetCvarValue(void* ECX, void* EDX, SVC_GetCvarValue* msg)
{
	if (!ClientState->m_NetChannel) return false;

	//static auto Original = reinterpret_cast<bool(__thiscall*)(void*, SVC_GetCvarValue*)>(GetCvarValue->Original);

	// Prepare the response.
	CLC_RespondCvarValue returnMsg; *(void**)&returnMsg = CLC_RespondCvarValue_Table;

	returnMsg.m_iCookie = msg->m_iCookie;
	returnMsg.m_szCvarName = msg->m_szCvarName;
	returnMsg.m_szCvarValue = "";
	returnMsg.m_eStatusCode = eQueryCvarValueStatus_CvarNotFound;

	char tempValue[256];

	// Does any ConCommand exist with this name?

	const auto pVar = Cvar->FindVar(msg->m_szCvarName);

	if (pVar)
	{
		if (pVar->IsFlagSet(FCVAR_SERVER_CANNOT_QUERY))
		{
			// The server isn't allowed to query this.
			returnMsg.m_eStatusCode = eQueryCvarValueStatus_CvarProtected;
		}
		else
		{
			returnMsg.m_eStatusCode = eQueryCvarValueStatus_ValueIntact;

			if (pVar->IsFlagSet(FCVAR_NEVER_AS_STRING))
			{
				// The cvar won't store a string, so we have to come up with a string for it ourselves.
				if (fabs(pVar->GetFloat() - pVar->GetInt()) < 0.001f)
				{
					snprintf(tempValue, sizeof(tempValue), "%d", pVar->GetInt());
				}
				else
				{
					snprintf(tempValue, sizeof(tempValue), "%f", pVar->GetFloat());
				}

				returnMsg.m_szCvarValue = tempValue;
			}
			else
			{
				// The easy case..
				returnMsg.m_szCvarValue = pVar->GetString();
			}
		}
	}
	else
	{
		if (Cvar->FindCommand(msg->m_szCvarName))
			returnMsg.m_eStatusCode = eQueryCvarValueStatus_NotACvar; // It's a command, not a cvar.
		else
			returnMsg.m_eStatusCode = eQueryCvarValueStatus_CvarNotFound;
	}

#ifdef SafetyMode
	bool HasDesiredValue = false;
#endif

	for (int i = 0; i < DesiredConVarsValue.size(); i++)
	{
		if (strcmp(returnMsg.m_szCvarName, DesiredConVarsValue.at(i).GetName()) == 0)
		{
			returnMsg.m_szCvarValue = DesiredConVarsValue.at(i).GetValue();

			returnMsg.m_eStatusCode = (EQueryCvarValueStatus)DesiredConVarsValue.at(i).GetStatus(); 
			
#ifdef SafetyMode
			HasDesiredValue = true;
#endif
			break;
		}
	}

#ifdef SafetyMode
	if (!HasDesiredValue && pVar)
	{
		returnMsg.m_szCvarValue = pVar->GetDefault();
	}
#endif

	// Send back.
	ClientState->m_NetChannel->SendNetMsg((void*&)returnMsg);

	return true;
}

CLC_ListenEvents* CLC_ListenEvents_Table;

void __stdcall Hooked_WriteListenEventList(void* msg) noexcept
{
	static auto Original = reinterpret_cast<void(__thiscall*)(PVOID, PVOID)>(WriteListenEventList->Original);

	Original(EventManager, msg);

	bool BulletImpact_Options = Menu::Get.Colors.General.BulletImpact.a() || Menu::Get.Visuals.Tracers.TR_Enabled;

	bool PlayerHurt_Options = Menu::Get.Visuals.Hitmarker.HM_Enabled || Menu::Get.Visuals.DamageIndicator.DI_Enabled;

	static auto m_EventArray_Set = [](PVOID msg, int eventid) { *(unsigned long*)((unsigned int)msg + 4 * (eventid >> 5) + 16) |= 1 << (eventid & 0x1F); };

	if (PlayerHurt_Options) {

		m_EventArray_Set(msg, 24);
	}

	if (BulletImpact_Options) {

		m_EventArray_Set(msg, 98);
	}
}

void __stdcall Hooked_CSteam3Client_InitiateConnection(void* data, int nMaxData, int& nOutSize, long nIP, short nPort, unsigned long long nGSSteamID, bool bSecure)
{
	auto  CSteam3Client_InitiateConnection_Type = reinterpret_cast<void(__stdcall*)(
		void*, int, int&, long, short, unsigned long long, bool)>(CSteam3Client_InitiateConnection->Original);

	auto pdata = (long*)data;

	CSteam3Client_InitiateConnection_Type(pdata, nMaxData, nOutSize, nIP, nPort, nGSSteamID, bSecure);

	if (Menu::Get.Misc.SteamIDSpoofer.SIP_Enabled)
	{
		uint32_t nSteamID = Menu::Get.Misc.SteamIDSpoofer.RandomSteamID ? (__rdtsc() % INT_MAX) : Menu::Get.Misc.SteamIDSpoofer.SteamID;

		uint32_t revHash;

		nOutSize = GenerateRevEmu2013(Transpose(data, 8), nSteamID >> 1, &revHash);

		if (!nOutSize) return;

		revHash = revHash << 1;

		pdata[0] = revHash | (nSteamID & 1);
	}
}

#endif
