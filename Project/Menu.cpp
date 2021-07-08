#include "include.h"
#include "SDK.h"
#include "Menu.h"
#include <string>
#include <vector>
#include <array>

#include "Assorted.h"
#include "MaterialManager.h"
#include "ImGui/ImGui_impl_win32.h"
#include "ImGui/ImGui_impl_dx9.h"
#define IMGUI_DEFINE_MATH_OPERATORS
#include "ImGui/ImGui_internal.h"
#include "ImGui/Imgui.h"
#include "SkinChanger.h"
#include "CvarQuery.h"
#include "Soundpad.h"
#include "Config.h"
#include "SteamID/steamid.hpp"

#define Push() ImGui::PushID(__LINE__);

#define Pop() ImGui::PopID();

#define PushW(W) ImGui::PushItemWidth(W);

#define PopW() ImGui::PopItemWidth();

#define SpawnTestVar() static float Var(100);\
\
Push();\
ImGui::SliderFloat("", &Var, 20, 500);\
Pop();

constexpr auto windowFlags = ImGuiWindowFlags_NoCollapse
| ImGuiWindowFlags_NoResize
| ImGuiWindowFlags_NoScrollbar
| ImGuiWindowFlags_AlwaysAutoResize
| ImGuiWindowFlags_NoScrollWithMouse;

extern void mmcopy
(
	void* address,
	const void* value,
	size_t bytes
);

extern float Since_Last_Used;

extern std::vector<const char*>ClanTagInfo;

void __stdcall Hooked_WriteListenEventList(PVOID);

void HotKey(unsigned char& key) noexcept
{
	std::string KeyName = InputSystem->VirtualKeyToString(key);

	std::transform(KeyName.begin(), KeyName.begin() + 1, KeyName.begin(), ::toupper);

	if (KeyName.size() > 2)
	{
		std::transform(KeyName.begin() + 1, KeyName.end(), KeyName.begin() + 1, ::tolower);
	}

	key ? ImGui::Text("[ %s ]", KeyName.c_str()) : ImGui::TextUnformatted("[ Key ]");

	if (!ImGui::IsItemHovered())
		return;

	ImGui::SetTooltip("Press Key");

	ImGuiIO& io = ImGui::GetIO();

	for (int i = 0; i < IM_ARRAYSIZE(io.KeysDown); i++)
	{
		if (ImGui::IsKeyPressed(i) && i != VK_INSERT)
		{
			key = i != VK_ESCAPE ? i : 0;
		}
	}

	for (int i = 0; i < IM_ARRAYSIZE(io.MouseDown); i++)
	{
		if (ImGui::IsMouseDown(i) && i + (i > 1 ? 2 : 1) != VK_INSERT)
		{
			key = i + (i > 1 ? 2 : 1);
		}
	}
}

void DrawRectRainbow(int x, int y, int width, int height, float flSpeed, float& flRainbow) noexcept
{
	ImDrawList* windowDrawList = ImGui::GetWindowDrawList();

	Color colColor(0, 0, 0, 255);

	flRainbow += flSpeed;

	if (flRainbow > 1.f) flRainbow = 0.f;

	for (int i = 0; i < width; i++)
	{
		float hue = (1.f / (float)width) * i;
		hue -= flRainbow;
		if (hue < 0.f) hue += 1.f;

		Color colRainbow = colColor.FromHSB(hue, 1.f, 1.f);

		windowDrawList->AddRectFilled(ImVec2(x + i, y), ImVec2(width, height), colRainbow.GetU32());
	}
}

bool ColorPopup(const char* name, std::array<float, 4>& color,int ID) noexcept
{
	bool ReturnV(false);

	ImGui::PushID(ID);

	ImColor Colorr{ color[0],color[1] ,color[2] ,color[3] };

	bool openPopup = ImGui::ColorButton("", Colorr, ImGuiColorEditFlags_NoTooltip | ImGuiColorEditFlags_AlphaPreview);

	if (ImGui::BeginDragDropTarget())
	{
		if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload(IMGUI_PAYLOAD_TYPE_COLOR_4F))
			color = *(std::array<float, 4>*)payload->Data;
		ImGui::EndDragDropTarget();
	}

	ImGui::SameLine(0.0f, 5.0f);

	ImGui::TextUnformatted(name);

	if (openPopup)
		ImGui::OpenPopup("");

	if (ImGui::BeginPopup(""))
	{
		ReturnV = ImGui::ColorPicker4("", color.data(), ImGuiColorEditFlags_NoSidePreview | ImGuiColorEditFlags_AlphaPreview | ImGuiColorEditFlags_AlphaBar);

		ImGui::EndPopup();
	}
	
	Pop();

	return ReturnV;
}

std::vector<DesiredConVarsValueInfo> DesiredConVarsValue;

void Menu::ZE() noexcept
{
	[[maybe_unused]] static auto Once = []()
	{
		const char* ClanTags[] = { "ZE-Elite","UKnownCheats","Banned" };

		for (auto ClanTag : ClanTags)
		{
			ClanTagInfo.push_back(ClanTag);
		}

		return true;
	}();

	ImGui::Checkbox("Auto Laser", &Menu::Get.General.AutoLaser);

	ImGui::Checkbox("Auto Kevlar", &Menu::Get.Misc.AutoKevlar);

	if(ImGui::Checkbox("Wireframe Lasers", &Menu::Get.General.WireFrame_Lasers)) {

		if (sci_fi_lvl2_28) {

			sci_fi_lvl2_28->SetMaterialVarFlag(MATERIAL_VAR_WIREFRAME, Menu::Get.General.WireFrame_Lasers);
		}

		if (blueblacklargebeam) {

			blueblacklargebeam->SetMaterialVarFlag(MATERIAL_VAR_WIREFRAME, Menu::Get.General.WireFrame_Lasers);
		}
	}

	if (ImGui::Checkbox("Make Items Visible", &Menu::Get.Visuals.Special_Item)) {

		if (static auto ItemsMat(MatSystemOther->FindMaterial("particle/particle_glow_05_add_15ob_minsize", NULL)); ItemsMat)
		{
			ItemsMat->SetMaterialVarFlag(MATERIAL_VAR_IGNOREZ, Menu::Get.Visuals.Special_Item);
		}
	}

	ImGui::PushItemWidth(100);

	ImGui::Text("Filter");

	Push();

	ImGui::Combo("", &Menu::Get.General.Filter, "Jump Only\0Duck Only\0Both\0");

	Pop();

	ImGui::PopItemWidth();

	ImGui::Separator();

	if (ImGui::Button("Save", ImVec2(100, 20)))
	{
		Config::SaveConfig();
	}

	if (ImGui::Button("Load", ImVec2(100, 20)))
	{
		Config::LoadConfig();

		void OnLevelInit(); OnLevelInit();
	}

	if (ImGui::Button("Panic", ImVec2(100, 22)))
	{
		Menu::Get.General.Panic = !Menu::Get.General.Panic;
	}

	ImGui::SameLine();

	HotKey(Menu::Get.Keys.PanicKey);

	if (ImGui::Button("Unload", ImVec2(100, 22)))
	{
		Menu::Get.General.Unload = true;
	}

	ImGui::SameLine();

	HotKey(Menu::Get.Keys.Unload);
}

void Menu::Aimbot() noexcept
{
	ImGui::Columns(2, nullptr, false);

	ImGui::SetColumnOffset(1, 200);

	ImGui::SetColumnWidth(0, 230);

	ImGui::Checkbox("Active", &Menu::Get.Aimbot.Ragebot.Enabled);

	ImGui::Checkbox("Anti Smac", &Menu::Get.Aimbot.AntiSmac);

	ImGui::Checkbox("Auto Shoot", &Menu::Get.Aimbot.Ragebot.AutoShoot);

	ImGui::Checkbox("Auto Stop", &Menu::Get.Aimbot.Ragebot.AutoStop);

	ImGui::Checkbox("Auto Crouch", &Menu::Get.Aimbot.Ragebot.AutoCrouch);

	ImGui::Checkbox("Friendly Fire", &Menu::Get.Aimbot.Ragebot.FriendlyFire);

	ImGui::Checkbox("Predicted Postion", &Menu::Get.Aimbot.Ragebot.Predicted);

	ImGui::NextColumn();

	ImGui::PushItemWidth(130);

	ImGui::SliderInt("FOV", &Menu::Get.Aimbot.Ragebot.FOV, 1, 360);

	ImGui::SliderFloat("Smooth", &Menu::Get.Aimbot.Ragebot.SmoothVal, 1, 35);

	ImGui::SliderInt("Timeout", &Menu::Get.Aimbot.Ragebot.Timeout, 0, 1000, "%d ms");

	static int IHitBox{ 0 };

	if (ImGui::Combo("HitBox", &IHitBox, "Head\0Neck\0Chest\0Stomach\0"))
	{
		if (IHitBox == 0)
			Menu::Get.Aimbot.Ragebot.HitBox = 12;
		else if (IHitBox == 1)
			Menu::Get.Aimbot.Ragebot.HitBox = 11;
		else if (IHitBox == 2)
			Menu::Get.Aimbot.Ragebot.HitBox = 10;
		else
			Menu::Get.Aimbot.Ragebot.HitBox = 9;
	}

	ImGui::Combo("Silent", &Menu::Get.Aimbot.Ragebot.Silent, "Off\0" "Client Side\0""Server Side\0");

	ImGui::Combo("Selection", &Menu::Get.Aimbot.Ragebot.TargetSelection, "Distance\0" "FOV\0");

	ImGui::Combo("Usage", &Menu::Get.Aimbot.Ragebot.Usage, "Always\0" "On Attack\0");

	ImGui::PopItemWidth();

	ImGui::Columns(1);
}

void Menu::TriggerBot() noexcept
{
	PushW(100);

	ImGui::Checkbox("Active", &Menu::Get.Misc.Triggerbot.TB_Enabled);

	Push();

	ImGui::Combo("", &Menu::Get.Misc.Triggerbot.TB_Usage, "Auto\0""On Key\0");

	Pop();

	ImGui::SameLine();

	HotKey(Menu::Get.Keys.Triggerbot);

	PopW();

	ImGui::Text("Filter");

	ImGui::Separator();

	ImGui::Selectable(" Head", &Menu::Get.Misc.Triggerbot.Filter[0]);

	ImGui::Selectable(" Chest", &Menu::Get.Misc.Triggerbot.Filter[1]);

	ImGui::Selectable(" Stomach", &Menu::Get.Misc.Triggerbot.Filter[2]);

	ImGui::Selectable(" Arms", &Menu::Get.Misc.Triggerbot.Filter[3]);

	ImGui::Selectable(" Legs", &Menu::Get.Misc.Triggerbot.Filter[4]);
}

void RenderChamsTab() noexcept
{
	CreateColorFunction(PlayerChams);

	CreateColorFunction(WeaponChams);

	CreateColorFunction(BacktrackChams);

	ImGui::Checkbox("Player", &Menu::Get.Visuals.Chams.Player.CMP_Enabled);
	{
		ImGui::SameLine(90.5f);

		PlayerChams(Menu::Get.Colors.Chams.Player, "");
		{
			ImGui::SameLine(121.1F);

			Push();

			if (ImGui::Button("...", ImVec2(50, 20)))
			{
				ImGui::OpenPopup("CMPlayer");
			}

			if (ImGui::BeginPopup("CMPlayer"))
			{
				ImGui::Checkbox("Wireframe", &Menu::Get.Visuals.Chams.Player.CMP_Wireframe);

				ImGui::Checkbox("Ignore-Z", &Menu::Get.Visuals.Chams.Player.CMP_Ignorez);

				ImGui::PushItemWidth(105.f);

				ImGui::Combo("Type", &Menu::Get.Visuals.Chams.Player.CMP_Type, "Normal\0Flat\0Debug\0Water\0");

				ImGui::PopItemWidth();

				ImGui::EndPopup();
			}

			Pop();
		}
	}


	ImGui::Checkbox("Weapon", &Menu::Get.Visuals.Chams.Weapon.CMW_Enabled); 
	{
		ImGui::SameLine(90.5f);

		WeaponChams(Menu::Get.Colors.Chams.CWeapon, "");
		{
			ImGui::SameLine(121.1F);

			Push();

			if (ImGui::Button("...", ImVec2(50, 20)))
			{
				ImGui::OpenPopup("CMWeapon");
			}

			if (ImGui::BeginPopup("CMWeapon"))
			{
				ImGui::Checkbox("Wireframe", &Menu::Get.Visuals.Chams.Weapon.CMW_Wireframe);

				ImGui::Checkbox("Ignore-Z", &Menu::Get.Visuals.Chams.Weapon.CMW_Ignorez);

				ImGui::PushItemWidth(105.f);

				ImGui::Combo("Type", &Menu::Get.Visuals.Chams.Weapon.CMW_Type, "Normal\0Flat\0Debug\0Water\0Glass 1\0Glass 2\0Glass 3\0Concrete\0");

				ImGui::PopItemWidth();

				ImGui::EndPopup();
			}

			Pop();
		}
	}

	/*ImGui::Checkbox("Backtrack", &Menu::Get.Visuals.Chams.Backtrack.CMB_Enabled);
	{
		ImGui::SameLine(90.5f);

		BacktrackChams(Menu::Get.Colors.Chams.Backtrack, "");
		{
			ImGui::SameLine(121.1F);

			Push();

			if (ImGui::Button("...", ImVec2(50, 20)))
			{
				ImGui::OpenPopup("CMBacktrack");
			}

			if (ImGui::BeginPopup("CMBacktrack"))
			{

				ImGui::Checkbox("Wireframe", &Menu::Get.Visuals.Chams.Backtrack.CMB_Wireframe);

				ImGui::Checkbox("Ignore-Z", &Menu::Get.Visuals.Chams.Backtrack.CMB_Ignorez);

				ImGui::PushItemWidth(105.f);

				ImGui::Combo("Type", &Menu::Get.Visuals.Chams.Backtrack.CMB_Type, "Normal\0Flat\0Debug\0Water\0");

				ImGui::PopItemWidth();

				ImGui::EndPopup();
			}

			Pop();
		}
	}*/

	ImGui::Columns(1);
}

namespace ImGui
{
	static auto vector_getter = [](void* vec, int idx, const char** out_text)
	{
		auto& vector = *static_cast<std::vector<std::string>*>(vec);
		if (idx < 0 || idx >= static_cast<int>(vector.size())) { return false; }
		*out_text = vector.at(idx).c_str();
		return true;
	};

	bool Combo(const char* label, int* currIndex, std::vector<std::string>& values)
	{
		if (values.empty()) { return false; }
		return Combo(label, currIndex, vector_getter,
			static_cast<void*>(&values), values.size());
	}

	bool ListBox(const char* label, int* currIndex, std::vector<std::string>& values)
	{
		if (values.empty()) { return false; }
		return ListBox(label, currIndex, vector_getter,
			static_cast<void*>(&values), values.size());
	}

}

char SkyBoxName[64];

int SkyBoxIndex{ 0 };

extern float PointScale;

extern int m_iScopeDustTexture;

extern CBaseEntity* RainEntity;

extern CSpriteTrail* SpriteEntity;

extern char CustomMaterialName[64];

extern char Backup_Of_Scope_Len_Bytes[4];

std::array<HealthSprite::MyCSprite, 65> HealthSprite::SpriteEntities;

void ForceFullUpdate()
{
	static auto ForceFullUpdate = reinterpret_cast<void(__thiscall*)(void*)>(
		Tools::FindPattern("engine.dll", "56 8B F1 83 BE A0 01"));

	ForceFullUpdate(ClientState);
}

void Menu::Visuals() noexcept
{
	static int CurrentTab{ 0 };

	if (ImGui::Button("Player Esp", ImVec2(159.f, 25)))
	{
		if (CurrentTab)
			CurrentTab = 0;
	}

	ImGui::SameLine();

	if (ImGui::Button("Assorted", ImVec2(159.f, 25)))
	{
		if (CurrentTab != 1)
			CurrentTab = 1;
	}

	ImGui::SameLine();

	if (ImGui::Button("Skin Changer", ImVec2(159.f, 25)))
	{
		if (CurrentTab != 2)
			CurrentTab = 2;
	}

	ImGui::Separator();

	if (CurrentTab == NULL)
	{
		CreateColorFunction(Box);

		CreateColorFunction(Name);

		CreateColorFunction(Health);

		CreateColorFunction(Weapon);

		CreateColorFunction(Snapline);

		CreateColorFunction(Skeleton);

		ImGui::Checkbox("Box", &Menu::Get.Visuals.Box);
		{
			ImGui::SameLine(89.0f);

			Box(Menu::Get.Colors.PlayerEsp.Box, "");
			{
				ImGui::SameLine(119.1F);

				Push();

				if (ImGui::Button("...", ImVec2(50, 20)))
				{
					ImGui::OpenPopup("Box");
				}

				if (ImGui::BeginPopup("Box"))
				{
					ImGui::PushItemWidth(105.f);

					ImGui::Combo("Type", &Menu::Get.Visuals.BoxType, "2D\0" "2D corners\0");

					ImGui::PopItemWidth();

					ImGui::EndPopup();
				}

				Pop();
			}
		}

		ImGui::Checkbox("Name", &Menu::Get.Visuals.Name);
		{
			ImGui::SameLine(89.0f);

			Name(Menu::Get.Colors.PlayerEsp.Name, "");
			{
				ImGui::SameLine(119.1F);

				Push();

				if (ImGui::Button("...", ImVec2(50, 20)))
				{
					ImGui::OpenPopup("Box");
				}

				if (ImGui::BeginPopup("Box"))
				{
					ImGui::PushItemWidth(105.f);

					ImGui::Combo("Font", &Menu::Get.Visuals.NameFont, "Tahoma [14]\0Tahoma [25, 15, 1]\0Arial [16]\0Verdana [12]\0");

					ImGui::PopItemWidth();

					ImGui::EndPopup();
				}

				Pop();
			}
		}

		if (ImGui::Checkbox("Health", &Menu::Get.Visuals.Health))
		{
			if (auto pLocal = CBaseEntity::GetLocalPlayer(); pLocal && pLocal->IsAlive())
			{
				HealthSprite::ValidateSprites(pLocal);
			}
		}

		ImGui::SameLine(89.0f);

		Health(Menu::Get.Colors.PlayerEsp.Health, "");
		{
			ImGui::SameLine(119.1F);

			Push();

			if (ImGui::Button("...", ImVec2(50, 20)))
			{
				ImGui::OpenPopup("Health");
			}

			if (ImGui::BeginPopup("Health"))
			{
				ImGui::PushItemWidth(105.f);

				if (ImGui::Combo("Mode", &Menu::Get.Visuals.HealthType, "Text\0Bar\0Sprite Bar\0"))
				{
					if (auto pLocal = CBaseEntity::GetLocalPlayer(); pLocal && pLocal->IsAlive())
					{
						HealthSprite::ValidateSprites(pLocal);
					}
				}

				if (Menu::Get.Visuals.HealthType == 2)
				{
					if (ImGui::SliderFloat("Sprite Scale", &Menu::Get.Visuals.HP_SpriteScale, 0.05, 1))
					{
						HealthSprite::UpdateSpritesScale();
					}
				}
				else if (Menu::Get.Visuals.HealthType == 0)
				{
					ImGui::Combo("Font", &Menu::Get.Visuals.HealthFont, "Tahoma [14]\0Tahoma [25, 15, 1]\0Arial [16]\0Verdana [12]\0");
				}

				ImGui::PopItemWidth();

				ImGui::EndPopup();
			}

			Pop();
		}
		


		ImGui::Checkbox("Weapon", &Menu::Get.Visuals.Weapon);
		{
			ImGui::SameLine(89.0f);

			Weapon(Menu::Get.Colors.PlayerEsp.Weapon, "");
			{
				ImGui::SameLine(119.1F);

				Push();

				if (ImGui::Button("...", ImVec2(50, 20)))
				{
					ImGui::OpenPopup("Weapon");
				}

				if (ImGui::BeginPopup("Weapon"))
				{
					ImGui::PushItemWidth(105.f);

					ImGui::Combo("Render Type", &Menu::Get.Visuals.WeaponRenderingMode, "Name\0Icon\0");

					if (Menu::Get.Visuals.WeaponRenderingMode == 0)
					{
						ImGui::Combo("Font", &Menu::Get.Visuals.WeaponFont, "Tahoma [14]\0Tahoma [25, 15, 1]\0Arial [16]\0Verdana [12]\0");
					}

					ImGui::PopItemWidth();

					ImGui::EndPopup();
				}

				Pop();
			}
		}


		ImGui::Checkbox("Snapline", &Menu::Get.Visuals.SnapLine);
		{
			ImGui::SameLine(89.0f);

			Snapline(Menu::Get.Colors.PlayerEsp.SnapLine, "");
		}

		ImGui::Checkbox("Skeleton", &Menu::Get.Visuals.Skeleton);
		{
			ImGui::SameLine(89.0f);

			Skeleton(Menu::Get.Colors.PlayerEsp.Skeleton, "");
		}


	}
	else if (CurrentTab == 1)
	{
		ImGui::Columns(2, nullptr, false);

		ImGui::SetColumnOffset(1, 200);

		ImGui::SetColumnWidth(0, 300.f);

		PushW(135.F);

		if (ImGui::Combo("Hands", &Menu::Get.Visuals.Hands, "Normal\0Wireframe\0Hidden\0")) {

			if (static auto HandsMat(MatSystemOther->FindMaterial("models/weapons/v_models/hands/v_hands.vtf", NULL)); HandsMat) {

				HandsMat->SetMaterialVarFlag(MATERIAL_VAR_WIREFRAME, Menu::Get.Visuals.Hands == 1);

				HandsMat->SetMaterialVarFlag(MATERIAL_VAR_NO_DRAW, Menu::Get.Visuals.Hands == 2);
			}
		}

		Push();

		if (ImGui::Combo("Scopes", &Menu::Get.Visuals.ScopeLen, ScopeOverlays))
		{
			void UpdateScopeLens(); UpdateScopeLens();
		}

		Pop();

		Push();

		ImGui::Combo("Kill Effect", &Menu::Get.Visuals.KillEffect_Type, KillEffects.data(), KillEffects.size());

		Pop();

		Push();

		if (ImGui::Combo("", &SkyBoxIndex, SkyBoxes)) {

			static auto sv_skyname(Cvar->FindVar("sv_skyname"));

			switch (SkyBoxIndex)
			{
			case 0:
				LoadSkyBox(sv_skyname->GetString());
				break;
			default:
				LoadSkyBox(SkyBoxes[SkyBoxIndex].c_str());
				break;
			}
		}Pop();

		ImGui::SameLine();

		CreateColorFunction(SkyBox);

		if (SkyBox(Menu::Get.Colors.General.SkyBox))
		{
			static std::array<float, 4> SkyBoxCurrentColor{};

			Menu::Get.Colors.General.SkyBox.GetColor(SkyBoxCurrentColor, 255.f);

			for (auto i = MatSystemOther->FirstMaterial(); i != MatSystemOther->InvalidMaterial(); i = MatSystemOther->NextMaterial(i)) {

				auto pMaterial = MatSystemOther->GetMaterial(i);

				if (!pMaterial || !pMaterial->IsPrecached())
					continue;

				if (strstr(pMaterial->GetTextureGroupName(), TEXTURE_GROUP_SKYBOX)) {

					pMaterial->ColorModulate(SkyBoxCurrentColor[0], SkyBoxCurrentColor[1], SkyBoxCurrentColor[2]); pMaterial->AlphaModulate(SkyBoxCurrentColor[3]);
				}
			}
		}

		Push();

		CreateColorFunction(DLights);

		ImGui::Combo("", &Menu::Get.Visuals.DLights, "Off\0My Player\0Enemies\0Teammates\0All\0");

		ImGui::SameLine();

		DLights(Menu::Get.Colors.PlayerEsp.DLights, "Dynamic-Lights");

		Pop();

		PopW();

		CreateColorFunction(Hud);

		Hud(Menu::Get.Colors.General.HudColor);

		CreateColorFunction(World);

		if (World(Menu::Get.Colors.General.World))
		{
			static std::array<float, 4> WorldCurrentColor{};

			Menu::Get.Colors.General.World.GetColor(WorldCurrentColor, 255.f);

			for (auto i = MatSystemOther->FirstMaterial(); i != MatSystemOther->InvalidMaterial(); i = MatSystemOther->NextMaterial(i)) {

				auto pMaterial = MatSystemOther->GetMaterial(i);

				if (!pMaterial || !pMaterial->IsPrecached())
					continue;

				if (strstr(pMaterial->GetTextureGroupName(), TEXTURE_GROUP_WORLD)) {

					pMaterial->ColorModulate(WorldCurrentColor[0], WorldCurrentColor[1], WorldCurrentColor[2]); pMaterial->AlphaModulate(WorldCurrentColor[3]);
				}
			}
		}

		CreateColorFunction(Nightvision);

		if (Nightvision(Menu::Get.Colors.General.Nightvision))
		{
			mmcopy(overlaycolor, Menu::Get.Colors.General.Nightvision, 4);

			if (auto pLocal = CBaseEntity::GetLocalPlayer(); pLocal)
			{
				pLocal->IsNightVisionOn() = Menu::Get.Colors.General.Nightvision.a();

				pLocal->GetNightVisionAlpha() = Menu::Get.Colors.General.Nightvision.a();
			}
		}

		if (ImGui::IsItemHovered() || ImGui::IsItemActive())
		{
			ImGui::SameLine();

			ImGui::Text("Set alpha to active");
		}

		CreateColorFunction(NadeTracer);

		NadeTracer(Menu::Get.Colors.General.NadeTracer, "Nade Tracer");

		if (ImGui::IsItemHovered() || ImGui::IsItemActive())
		{
			ImGui::SameLine();

			ImGui::Text("Set alpha to active");
		}

		CreateColorFunction(Bullet_Impact);

		if (Bullet_Impact(Menu::Get.Colors.General.BulletImpact, "Bullet Impact"))
		{
			if (auto NetChannel = ClientState->m_NetChannel; NetChannel) {

				CLC_ListenEvents msg; *(void**)&msg = CLC_ListenEvents_Table;

				Hooked_WriteListenEventList(&msg);

				ClientState->m_NetChannel->SendNetMsg((void*&)msg);
			}
		}

		if (ImGui::IsItemHovered() || ImGui::IsItemActive())
		{
			ImGui::SameLine();

			ImGui::Text("Set alpha to active");
		}

		if (ImGui::Checkbox("Full Bright", &Menu::Get.Visuals.FullBright))
		{
			if (static auto mat_fullbright(Cvar->FindVar("mat_fullbright")); mat_fullbright)
				mat_fullbright->SetMemValue(Menu::Get.Visuals.FullBright);
		}

		if (ImGui::Checkbox("Disable Fog", &Menu::Get.Visuals.NoFog))
		{
			if (static auto Fog_Enable(Cvar->FindVar("Fog_Enable")); Fog_Enable)
				Fog_Enable->SetMemValue(!Menu::Get.Visuals.NoFog);

			if (static auto Fog_Override(Cvar->FindVar("NewFogOverride")); Fog_Override)
				Fog_Override->SetMemValue(Menu::Get.Visuals.NoFog);
		}

		ImGui::Checkbox("Disable Smoke", &Menu::Get.Visuals.NoSmoke);

		ImGui::Checkbox("Disable Fire Particles", &Menu::Get.Visuals.Disable_Fire_Particles);

		if (ImGui::IsItemHovered()) {

			ImGui::SameLine();

			ImGui::TextUnformatted("Burning Character");
		}

		ImGui::Checkbox("Disable Visual Recoil", &Menu::Get.Visuals.NoVisualRecoil);

		ImGui::Checkbox("Hide Teammates Skins", &Menu::Get.Visuals.NoTeammates);

		if (ImGui::Checkbox("Disable Players FlashLight", &Menu::Get.Visuals.NoFlashLight)) {

			if (Menu::Get.Visuals.NoFlashLight && Engine->IsInGame())
			{
				LoopTroughPlayers()
				{
					if (auto Entity(GetEntity(i)); Entity && Entity->IsPlayer())
						if (Entity->GetEffects() & EF_DIMLIGHT)
							Entity->GetEffects() &= ~EF_DIMLIGHT;

				}
			}
		}

		ImGui::Checkbox("Disable Players MuzzleFlash", &Menu::Get.Visuals.Disable_Players_MuzzleFlash);

		ImGui::NextColumn();

		PushW(100.f);

		Push();

		ImGui::SliderInt("Viewmodel fov", &Menu::Get.Visuals.ViewModelFov, -60, 60, "%d");

		Pop();

		Push();

		ImGui::SliderInt("Field of view", &Menu::Get.Visuals.FieldOfView, -60, 60, "%d");

		Pop();

		Push();

		ImGui::SliderInt("Flash reduction", &Menu::Get.Visuals.FlashPercentage, 0.f, 100.f, "%d%%");

		Pop(); void Nightmod();

		if (ImGui::SliderInt("Night mode", &Menu::Get.Visuals.NightMode, 1, 100, "%d%%")) {

			Nightmod();
		}

		PopW();


		ImGui::Text("\n\n");

		ImGui::Text("Fog");
		{
			ImGui::SameLine(107.f);

			Push();

			if (ImGui::Button("...", ImVec2(50, 20)))
			{
				ImGui::OpenPopup("Fog");
			}

			if (ImGui::BeginPopup("Fog"))
			{
				if (ImGui::Checkbox("Active", &Menu::Get.Visuals.FOG.Enabled))
				{
					if (static auto fog_override = Cvar->FindVar("NewFogOverride"); fog_override)
					{
						fog_override->SetMemValue(Menu::Get.Visuals.FOG.Enabled);
					}

					if (static auto fog_enable = Cvar->FindVar("fog_enable"); fog_enable)
					{
						fog_enable->SetMemValue(Menu::Get.Visuals.FOG.Enabled && !Menu::Get.Visuals.NoFog);
					}
				}

				ImGui::SameLine();

				CreateColorFunction(Fog);

				if (Fog(Menu::Get.Colors.General.Fog, ""))
				{
					if (static auto fog_color = Cvar->FindVar("fog_color"); fog_color)
					{
						char Buffer[64];

						snprintf(Buffer, sizeof Buffer, "%i %i %i", Menu::Get.Colors.General.Fog[0], Menu::Get.Colors.General.Fog[1], Menu::Get.Colors.General.Fog[2]);

						fog_color->SetValue(Buffer);
					}
				}

				ImGui::PushItemWidth(91.f);

				if (ImGui::SliderFloat("start", &Menu::Get.Visuals.FOG.start, -1, 9999))
				{
					if (static auto fog_start = Cvar->FindVar("fog_start"); fog_start)
					{
						fog_start->SetMemValue(Menu::Get.Visuals.FOG.start);
					}
				}

				if (ImGui::SliderFloat("end", &Menu::Get.Visuals.FOG.end, -1, 9999))
				{
					if (static auto fog_end = Cvar->FindVar("fog_end"); fog_end)
					{
						fog_end->SetMemValue(Menu::Get.Visuals.FOG.end);
					}
				}

				if (ImGui::SliderFloat("Max density", &Menu::Get.Visuals.FOG.maxdensity, 0.f, 1.f))
				{
					if (static auto fog_maxdensity = Cvar->FindVar("fog_maxdensity"); fog_maxdensity)
					{
						fog_maxdensity->SetMemValue(Menu::Get.Visuals.FOG.maxdensity);
					}
				}

				ImGui::PopItemWidth();

				ImGui::EndPopup();
			}

			Pop();
		}

		ImGui::Text("Debug");

		ImGui::SameLine(107.f);

		Push();

		if (ImGui::Button("...", ImVec2(50, 20)))
		{
			ImGui::OpenPopup("Debug");
		}

		if (ImGui::BeginPopup("Debug"))
		{
			PushW(300);

			ImGui::Checkbox("Index", &Menu::Get.Visuals.Debug.INDEX);

			ImGui::Checkbox("Classnmae", &Menu::Get.Visuals.Debug.Classname);

			ImGui::Checkbox("Modelname", &Menu::Get.Visuals.Debug.ModelName);

			ImGui::Checkbox("Model Index", &Menu::Get.Visuals.Debug.ModelIndex);

			ImGui::Checkbox("Origin", &Menu::Get.Visuals.Debug.Origin);

			ImGui::Checkbox("Velocity", &Menu::Get.Visuals.Debug.Velocity);

			PopW();

			PushW(180.f);

			ImGui::Text("Ignore:");

			ImGui::Separator();

			ImGui::Checkbox("Players", &Menu::Get.Visuals.Debug.Filter[0]);

			ImGui::Checkbox("Weapons", &Menu::Get.Visuals.Debug.Filter[1]);

			ImGui::Checkbox("Sprites", &Menu::Get.Visuals.Debug.Filter[2]);

			ImGui::Checkbox("Motionless", &Menu::Get.Visuals.Debug.Filter[3]);

			ImGui::SliderInt("", &Menu::Get.Visuals.Debug.MaxDistance, 0, 5000, "if distance over: %d unit");

			PopW();

			ImGui::EndPopup();
		}

		Pop();

		ImGui::Text("Trails");
		{
			ImGui::SameLine(107.f);

			Push();

			if (ImGui::Button("...", ImVec2(50, 20)))
			{
				ImGui::OpenPopup("Trails");
			}

			if (ImGui::BeginPopup("Trails"))
			{
				constexpr auto& TrailsOptions = Menu::Get.Visuals.Trails;

				if (ImGui::Checkbox("Active", &TrailsOptions.TR_Enabled))
				{
					if (auto pLocal = CBaseEntity::GetLocalPlayer(); pLocal)
					{
						if (!TrailsOptions.TR_Enabled || !pLocal->IsAlive())
						{
							if (SpriteEntity)
							{
								SpriteEntity->Remove();

								SpriteEntity = nullptr;
							}
						}
						else { void ExpensiveTrail(CSpriteTrail*&, CBaseEntity*); ExpensiveTrail(SpriteEntity, pLocal); }
					}
				}

				ImGui::SameLine();

				CreateColorFunction(Trails);

				if (Trails(Menu::Get.Colors.General.Trails, ""))
				{
					if (SpriteEntity)
					{
						SpriteEntity->GetRenderColor() = Menu::Get.Colors.General.Trails;
					}
				}

				ImGui::PushItemWidth(91.f);

				//ImGui::Combo("Price", &TrailsOptions.Price, "Cheap\0Expensive\0");

				if (ImGui::Combo("Texture", &TrailsOptions.SpriteTexture, TrailMaterials) && SpriteEntity && TrailMaterials.size())
				{
					std::string Sprite("sprites/trails/");

					Sprite.append(TrailMaterials[Menu::Get.Visuals.Trails.SpriteTexture]);

					Sprite.append(".vmt");

					auto ModelIndex = ModelInfo->GetModelIndex(Sprite.c_str());

					if (ModelIndex == -1)
					{
						if (PrecacheModel(Sprite.c_str()))
						{
							ModelIndex = ModelInfo->GetModelIndex(Sprite.c_str());
						}
					}

					if (ModelIndex != -1)
					{
						SpriteEntity->SetModelByIndex(ModelIndex);
					}
				}

				if (ImGui::Combo("Render Mode", &TrailsOptions.Expensive.RenderMode, "Normal\0TransColor\0TransTexture\0Glow\0TransAlpha\0TransAdd\0"))
				{
					if (SpriteEntity)
					{
						SpriteEntity->GetRenderMode() = TrailsOptions.Expensive.RenderMode;
					}
				}

				if (ImGui::InputFloat("Width", &TrailsOptions.Expensive.Width))
				{
					if (SpriteEntity)
					{
						SpriteEntity->GetStartWidth() = TrailsOptions.Expensive.Width;
					}
				}

				if (ImGui::InputFloat("EndWidth", &TrailsOptions.Expensive.EndWidth))
				{
					if (SpriteEntity)
					{
						SpriteEntity->GetEndWidth() = TrailsOptions.Expensive.EndWidth;
					}
				}

				if (ImGui::InputFloat("Lifetime", &TrailsOptions.Expensive.LifeTime))
				{
					if (SpriteEntity)
					{
						SpriteEntity->GetLifeTime() = TrailsOptions.Expensive.LifeTime;
					}
				}


				ImGui::PopItemWidth();

				ImGui::EndPopup();
			}

			Pop();
		}

		ImGui::Text("Tracers");
		{
			ImGui::SameLine(107.f);

			Push();

			if (ImGui::Button("...", ImVec2(50, 20)))
			{
				ImGui::OpenPopup("Tracers");
			}

			if (ImGui::BeginPopup("Tracers"))
			{
				if(ImGui::Checkbox("Active", &Menu::Get.Visuals.Tracers.TR_Enabled))
				{
					if (auto NetChannel = ClientState->m_NetChannel; NetChannel) {

						CLC_ListenEvents msg; *(void**)&msg = CLC_ListenEvents_Table;

						Hooked_WriteListenEventList(&msg);

						ClientState->m_NetChannel->SendNetMsg((void*&)msg);
					}
				}

				ImGui::SameLine();

				CreateColorFunction(Tracers);

				Tracers(Menu::Get.Colors.General.Tracers, "");

				ImGui::PushItemWidth(91.f);

				ImGui::Combo("Texture", &Menu::Get.Visuals.Tracers.SpriteTexture, TracerSprites.data(), TracerSprites.size());

				ImGui::SliderFloat("Point Scale", &PointScale, -1.f, 1.f);

				ImGui::SameLine();

				if (ImGui::Button("Reset"))
				{
					PointScale = 0.03f;
				}

				ImGui::InputFloat("HaloScale", &Menu::Get.Visuals.Tracers.m_flHaloScale);

				ImGui::InputFloat("Life", &Menu::Get.Visuals.Tracers.m_flLife);

				ImGui::InputFloat("Width", &Menu::Get.Visuals.Tracers.m_flWidth);

				ImGui::InputFloat("EndWidth", &Menu::Get.Visuals.Tracers.m_flEndWidth);

				ImGui::InputFloat("FadeLength", &Menu::Get.Visuals.Tracers.m_flFadeLength);

				ImGui::InputFloat("Amplitude", &Menu::Get.Visuals.Tracers.m_flAmplitude);

				ImGui::InputFloat("Speed", &Menu::Get.Visuals.Tracers.m_flSpeed);

				ImGui::InputInt("Segments", &Menu::Get.Visuals.Tracers.m_nSegments, 0, 0);

				ImGui::InputInt("Flags", &Menu::Get.Visuals.Tracers.m_nFlags, 0, 0);

				ImGui::PopItemWidth();

				ImGui::EndPopup();
			}

			Pop();
		}

		ImGui::Text("Weather");
		{
			ImGui::SameLine(107.f);

			Push();

			if (ImGui::Button("...", ImVec2(50, 20)))
			{
				ImGui::OpenPopup("Weather");
			}

			if (ImGui::BeginPopup("Weather"))
			{
				ImGui::Checkbox("Enabled", &Menu::Get.Visuals.Weather.Active);

				//ImGui::SameLine();

				//CreateColorFunction(Tracers);

				//Tracers(Menu::Get.Colors.General.Tracers, "");

				ImGui::PushItemWidth(91.f);

				if (ImGui::Combo("Type", &Menu::Get.Visuals.Weather.Type, "Rain\0Snow\0"))
				{
					if (RainEntity)
					{
						RainEntity->GetPrecipType() = Menu::Get.Visuals.Weather.Type;
					}
				}

				if(ImGui::SliderFloat("Width", &Menu::Get.Visuals.Weather.Width, 0.5f, 5.f))
				{
					if (r_rainwidth)
					{
						r_rainwidth->SetMemValue(Menu::Get.Visuals.Weather.Width);
					}
				}

				if (ImGui::SliderFloat("Speed", &Menu::Get.Visuals.Weather.Speed, 600.f, 2000.f))
				{
					if (r_rainspeed)
					{
						r_rainspeed->SetMemValue(Menu::Get.Visuals.Weather.Speed);
					}
				}

				/*if (ImGui::SliderFloat("Radius", &Menu::Get.Visuals.Weather.Radius, 500.f, 2000.f))
				{
					if (r_RainRadius)
					{
						r_RainRadius->SetMemValue(Menu::Get.Visuals.Weather.Radius);
					}
				}*/

				if (ImGui::SliderFloat("Length", &Menu::Get.Visuals.Weather.Length, 0.1f, 0.8f))
				{
					if (r_rainlength)
					{
						r_rainlength->SetMemValue(Menu::Get.Visuals.Weather.Length);
					}
				}

				ImGui::PopItemWidth();

				ImGui::EndPopup();
			}

			Pop();
		}


		ImGui::Text("Crosshair");
		{
			ImGui::SameLine(107.f);

			Push();

			if (ImGui::Button("...", ImVec2(50, 20)))
			{
				ImGui::OpenPopup("Crosshair");
			}

			if (ImGui::BeginPopup("Crosshair"))
			{
				//SpawnTestVar();

				constexpr auto& CrosshairStruct = Menu::Get.Visuals.Crosshair;

				constexpr auto& CrosshairColor = Menu::Get.Colors.General.Crosshair;

				static auto CCCrosshair{ Cvar->FindVar("crosshair") };

				ImGui::Checkbox("Recoil Crosshair", &CrosshairStruct.Recoil);

				constexpr auto& Circle_Shape = CrosshairStruct.Circle_Shape;

				constexpr auto& Plus_Shape = CrosshairStruct.Plus_Shape;

				constexpr auto& X_Shape = CrosshairStruct.X_Shape;

				ImGui::Text("Pluse Shape");
				{
					ImGui::SameLine(80.f);

					Push();

					if (ImGui::Button("...", ImVec2(40, 20)))
					{
						ImGui::OpenPopup("Plus_Shape");
					}

					if (ImGui::BeginPopup("Plus_Shape"))
					{
						if (ImGui::Checkbox("Active", &Plus_Shape.Enabled))
						{
							if (CCCrosshair)
							{
								CCCrosshair->SetValue(!Plus_Shape.Enabled && !X_Shape.Enabled && !Circle_Shape.Enabled);
							}
						}

						ImGui::SameLine();

						CreateColorFunction(Plus_Shape_Color);

						Plus_Shape_Color(CrosshairColor.Plus_Shape, "");

						ImGui::PushItemWidth(90.f);

						ImGui::SliderInt("length", &Plus_Shape.Length, 0, 20, "%d");

						ImGui::SliderInt("gap", &Plus_Shape.Gap, 0, 20, "%d");

						ImGui::PopItemWidth();

						ImGui::EndPopup();
					}

					Pop();
				}

				ImGui::Text("X Shape");
				{
					ImGui::SameLine(80.f);

					Push();

					if (ImGui::Button("...", ImVec2(40, 20)))
					{
						ImGui::OpenPopup("X_Shape");
					}

					if (ImGui::BeginPopup("X_Shape"))
					{
						if (ImGui::Checkbox("Active", &X_Shape.Enabled))
						{
							if (CCCrosshair)
							{
								CCCrosshair->SetValue(!Plus_Shape.Enabled && !X_Shape.Enabled && !Circle_Shape.Enabled);
							}
						}

						ImGui::SameLine();

						CreateColorFunction(X_Shape_Color);

						X_Shape_Color(CrosshairColor.X_Shape, "");

						ImGui::PushItemWidth(90.f);

						ImGui::SliderInt("length", &X_Shape.Length, 0, 20, "%d");

						ImGui::SliderInt("gap", &X_Shape.Gap, 0, 20, "%d");

						ImGui::PopItemWidth();

						ImGui::EndPopup();
					}

					Pop();
				}

				ImGui::Text("Circle");
				{
					ImGui::SameLine(80.f);

					Push();

					if (ImGui::Button("...", ImVec2(40, 20)))
					{
						ImGui::OpenPopup("Circle_Shape");
					}

					if (ImGui::BeginPopup("Circle_Shape"))
					{

						if (ImGui::Checkbox("Active", &Circle_Shape.Enabled))
						{
							if (CCCrosshair)
							{
								CCCrosshair->SetValue(!Plus_Shape.Enabled && !X_Shape.Enabled && !Circle_Shape.Enabled);
							}
						}

						ImGui::SameLine();

						CreateColorFunction(Circle_Shape_Color);

						Circle_Shape_Color(CrosshairColor.Circle_Shape, "");

						ImGui::PushItemWidth(90.f);

						ImGui::SliderInt("Radius", &Circle_Shape.Radius, 0, 20, "%d");

						ImGui::SliderInt("Segments", &Circle_Shape.Segments, 0, 20, "%d");

						ImGui::PopItemWidth();

						ImGui::EndPopup();
					}

					Pop();
				}

				ImGui::Text("You can use more than one shape, Customize a cool crosshair");

				ImGui::EndPopup();
			}

			Pop();
		}

		ImGui::Text("Hitmarker");
		{
			ImGui::SameLine(107.f);

			Push();

			if (ImGui::Button("...", ImVec2(50, 20)))
			{
				ImGui::OpenPopup("Hitmarker");
			}

			if (ImGui::BeginPopup("Hitmarker"))
			{
				if(ImGui::Checkbox("Active", &Menu::Get.Visuals.Hitmarker.HM_Enabled))
				{
					if (auto NetChannel = ClientState->m_NetChannel; NetChannel) {

						CLC_ListenEvents msg; *(void**)&msg = CLC_ListenEvents_Table;

						Hooked_WriteListenEventList(&msg);

						ClientState->m_NetChannel->SendNetMsg((void*&)msg);
					}
				}

				ImGui::SameLine();

				CreateColorFunction(Hitmarker);

				Hitmarker(Menu::Get.Colors.General.Hitmarker, "");

				ImGui::PushItemWidth(90.f);

				ImGui::Combo("Mode", &Menu::Get.Visuals.Mode, "Normal\0Overlay\0");

				ImGui::Combo("Sound", &Menu::Get.Visuals.HitSound, HitSounds);

				if (!Menu::Get.Visuals.Mode)
				{
					ImGui::SliderInt("length", &Menu::Get.Visuals.Hitmarker.Length, 0, 20, "%d");

					ImGui::SliderInt("gap", &Menu::Get.Visuals.Hitmarker.Gap, 0, 20, "%d");
				}
				else
				{
					ImGui::Combo("Overlay", &Menu::Get.Visuals.Overlay, HitmarkerOverlays);
				}

				ImGui::SliderInt("lifetime", &Menu::Get.Visuals.Hitmarker.Time, 1, 1500, "%d ms");

				ImGui::PopItemWidth();

				ImGui::Text("Show Damage");
				{
					ImGui::SameLine();

					Push();

					if (ImGui::Button("...", ImVec2(50, 20)))
					{
						ImGui::OpenPopup("DmgIndicator");
					}

					if (ImGui::BeginPopup("DmgIndicator"))
					{
						if (ImGui::Checkbox("Active", &Menu::Get.Visuals.DamageIndicator.DI_Enabled))
						{
							if (auto NetChannel = ClientState->m_NetChannel; NetChannel) {

								CLC_ListenEvents msg; *(void**)&msg = CLC_ListenEvents_Table;

								Hooked_WriteListenEventList(&msg);

								ClientState->m_NetChannel->SendNetMsg((void*&)msg);
							}
						}

						ImGui::SameLine();

						CreateColorFunction(DmgIndicator);

						DmgIndicator(Menu::Get.Colors.General.DamageIndicator, "");

						//ImGui::PushItemWidth(91.f);

						//ImGui::SliderFloat("Lifetime", &Menu::Get.Visuals.DamageIndicator.Lifetime, 1.f, 5.f,"%.3f sec");

						//ImGui::SliderFloat("Speed", &Menu::Get.Visuals.DamageIndicator.Speed, 0, 2.5);

						//ImGui::PopItemWidth();

						ImGui::EndPopup();
					}

					Pop();
				}

				ImGui::EndPopup();
			}

			Pop();
		}

		ImGui::Text("ViewModel");
		{
			ImGui::SameLine(107.f);

			Push();

			if (ImGui::Button("...", ImVec2(50, 20)))
			{
				ImGui::OpenPopup("ViewModel");
			}

			if (ImGui::BeginPopup("ViewModel"))
			{
				PushW(150.f);

				ImGui::TextUnformatted("Origin");

				Push(); ImGui::SliderFloat("", &Menu::Get.Visuals.ViewModel.Origin.x, -5.f, 5.f, "Forward %.2f"); Pop();
				Push(); ImGui::SliderFloat("", &Menu::Get.Visuals.ViewModel.Origin.y, -5.f, 5.f, "Right %.2f"); Pop();
				Push(); ImGui::SliderFloat("", &Menu::Get.Visuals.ViewModel.Origin.z, -5.f, 5.f, "Up %.2f"); Pop();

				ImGui::TextUnformatted("Angles");

				Push(); ImGui::SliderFloat("", &Menu::Get.Visuals.ViewModel.Angles.x, -90.f, 90.f, "Pitch %.2f"); Pop();
				Push(); ImGui::SliderFloat("", &Menu::Get.Visuals.ViewModel.Angles.y, -90.f, 90.f, "Yaw %.2f"); Pop();
				Push(); ImGui::SliderFloat("", &Menu::Get.Visuals.ViewModel.Angles.z, -90.f, 90.f, "Roll %.2f"); Pop();

				PopW();

				ImGui::EndPopup();
			}

			Pop();
		}

		ImGui::Text("Third Person");
		{
			ImGui::SameLine(107.f);

			Push();

			if (ImGui::Button("...", ImVec2(50, 20)))
			{
				ImGui::OpenPopup("Third Person");
			}

			if (ImGui::BeginPopup("Third Person"))
			{
				PushW(150.f);

				if (ImGui::SliderInt("Distance", &Menu::Get.Misc.m_vecCameraOffset, 0, 300, "%d"))
				{
					void UpdateThirdPerson();
					UpdateThirdPerson();
				}

				PopW();

				ImGui::EndPopup();
			}

			Pop();
		}

		ImGui::Columns(1);

		ImGui::Separator();

		PushW(206.5f);

		ImGui::TextColored(ImColor{ 20,255,20 }, "MuzzleFlash");

		ImGui::SameLine();

		Push();

		ImGui::Combo("", &Menu::Get.Visuals.MuzzleFlash, EffectMaterials.data(), EffectMaterials.size());

		Pop();

		ImGui::SameLine();

		Push();

		ImGui::InputText("", CustomMaterialName, IM_ARRAYSIZE(CustomMaterialName));

		Pop();

		PopW();
	}
	else
	{

		char DirectoryPath[MAX_PATH];

		unsigned short Iteration = 0;

		[[maybe_unused]] static auto Once = GetCurrentDirectory(MAX_PATH, DirectoryPath);

		PushW(152.f);

		if (ImGui::Button("Force Full Update", ImVec2(152.f,0)))
		{
			ForceFullUpdate();
		}

		ImGui::BeginChild("Valid-Models", ImVec2{ 494.f , 284.3f });
		{
			PushW(152.f);

			for (auto&& GetModel : GameModels)
			{
				if (GetModel.models.size() > 1)
				{
					ImGui::Combo(GetModel.weapon.c_str(), &GetModel.INDEX, GetModel.models);
				}
			}

			PopW();
		}ImGui::EndChild();

		ImGui::Separator();

		ImGui::BeginChild("Non-Valid-Models", ImVec2{ 494.f , 210 });
		{
			PushW(152.f);

			for (auto&& GetModel : GameModels)
			{
				if (GetModel.models.size() == 1)
				{
					ImGui::Combo(GetModel.weapon.c_str(), &GetModel.INDEX, GetModel.models);
				}
			}

			PopW();
		}ImGui::EndChild();


		PopW();
	}
}

char DirectoryPath[MAX_PATH];

std::vector<SoundPad> SoundList;

extern std::vector<std::string> Killmessages;

void Menu::Miscellaneous() noexcept
{
	ImGui::Separator();

	ImGui::Columns(2, nullptr, false);

	ImGui::SetColumnOffset(1, 200);

	ImGui::SetColumnWidth(0, 331.5f);
	
	ImGui::Checkbox("Fast Run", &Menu::Get.Misc.Fastrun);

	ImGui::Checkbox("Fast Stop", &Menu::Get.Misc.Faststop);

	ImGui::Checkbox("Auto Pisol", &Menu::Get.Misc.AutoPisol);

	ImGui::Checkbox("Pure Bypass", &Menu::Get.Misc.Sv_Pure_Bypass);

	ImGui::Checkbox("Anti AFK Kick", &Menu::Get.Misc.AntiAFKkick);

	ImGui::Checkbox("Disable Radio", &Menu::Get.Misc.MuteRadio);

	ImGui::Checkbox("Disable MOTD", &Menu::Get.Misc.NoMotd);

	ImGui::Checkbox("Anti Bullet Recoil", &Menu::Get.Misc.NoRecoil);

	ImGui::Checkbox("Anti Bullet Spread", &Menu::Get.Misc.NoSpread);

	ImGui::Checkbox("Fast Ladder Climb", &Menu::Get.Misc.FastLadder);

	ImGui::Checkbox("Disable Server Drug", &Menu::Get.Misc.NoDrug);

	ImGui::Checkbox("Disable Screen Shake", &Menu::Get.Misc.NoShake);

	ImGui::Checkbox("Disable Game Console Warnings", &Menu::Get.Misc.Disable_Game_Console_Warnings);

	ImGui::NextColumn();

	PushW(110.f);

	PopW();

	ImGui::Checkbox("Auto Strafe", &Menu::Get.Misc.Autostrafe);

	ImGui::SameLine(122.f);

	HotKey(Menu::Get.Keys.Autostrafe);

	ImGui::Checkbox("Circle Strafe", &Menu::Get.Misc.CircleStrafe);

	ImGui::SameLine(122.f);

	HotKey(Menu::Get.Keys.CircleStrafe);

	ImGui::Checkbox("Edge Jump", &Menu::Get.Misc.Edgejump);

	ImGui::SameLine(122.f);

	HotKey(Menu::Get.Keys.Edgejump);

	ImGui::Text("\n");

	ImGui::Text("KnifeBot");
	{
		ImGui::SameLine(107.f);

		Push();

		if (ImGui::Button("...", ImVec2(50, 20)))
		{
			ImGui::OpenPopup("KnifeBot");
		}

		if (ImGui::BeginPopup("KnifeBot"))
		{
			ImGui::Checkbox("Enabled", &Menu::Get.Misc.KnifeBot.KB_Enabled); 

			ImGui::SameLine();

			ImGui::TextColored(ImColor(20, 255, 20), "Key");

			ImGui::SameLine();

			HotKey(Menu::Get.Keys.KnifeBot);

			ImGui::PushItemWidth(150.f);

			ImGui::Combo("Method", &Menu::Get.Misc.KnifeBot.Method, "Left\0Right\0Smart\0");

			ImGui::Combo("Usage", &Menu::Get.Misc.KnifeBot.KB_Usage, "On Key\0Always\0");

			ImGui::PopItemWidth();

			ImGui::EndPopup();
		}

		Pop();
	}

	ImGui::Text("Bunnyhop");
	{
		ImGui::SameLine(107.f);

		Push();

		if (ImGui::Button("...", ImVec2(50, 20)))
		{
			ImGui::OpenPopup("Bhop");
		}

		if (ImGui::BeginPopup("Bhop"))
		{
			ImGui::PushItemWidth(300);

			ImGui::Checkbox("Enabled", &Menu::Get.Misc.Bunnyhop);

			Push();

			ImGui::SliderInt("", &Menu::Get.Misc.Bunnyhop_Perfect_Rate, 1, 100, "Perfection %d%%");

			Pop();

			HotKey(Menu::Get.Keys.Bunnyhop);

			std::string KeyName = InputSystem->VirtualKeyToString(Menu::Get.Keys.Bunnyhop);

			std::transform(KeyName.begin(), KeyName.begin() + 1, KeyName.begin(), ::toupper);

			if (KeyName.size() > 2)
			{
				std::transform(KeyName.begin() + 1, KeyName.end(), KeyName.begin() + 1, ::tolower);
			}

			ImGui::SameLine();

			ImGui::TextColored(ImColor{ 20,255,20 }, "Make sure that %s is unbounded in Game!", KeyName.c_str());

			ImGui::PopItemWidth();

			ImGui::EndPopup();
		}

		Pop();
	}

	ImGui::Text("Strafe Optimizer");
	{
		ImGui::SameLine(107.f);

		Push();

		if (ImGui::Button("...", ImVec2(50, 20)))
		{
			ImGui::OpenPopup("Optimizer");
		}

		if (ImGui::BeginPopup("Optimizer"))
		{
			ImGui::Checkbox("Enabled", (bool*)&Menu::Get.Misc.StrafeOptimizer.Enabled);

			ImGui::SameLine();

			ImGui::TextColored(ImColor{ 20,255,20 }, "We appreciate TraitCore for this <3");

			PushW(250);

			ImGui::SliderFloat("Desired Gain", &Menu::Get.Misc.StrafeOptimizer.Desired_Gain, 0, 100, "%.2f");

			ImGui::SliderFloat("Required Speed", &Menu::Get.Misc.StrafeOptimizer.Required_Speed, 0, 4000, "%.0f");

			ImGui::SliderFloat("Greatest Possible Strafe Angle", &Menu::Get.Misc.StrafeOptimizer.Greatest_Possible_Strafe_Angle, 0, 180, "%.2f");

			PopW();

			ImGui::EndPopup();
		}

		Pop();
	}

	/*ImGui::Text("Backtrack");
	{
		ImGui::SameLine(107.f);

		Push();

		if (ImGui::Button("...", ImVec2(50, 20)))
		{
			ImGui::OpenPopup("Backtrack");
		}

		if (ImGui::BeginPopup("Backtrack"))
		{
			ImGui::Checkbox("Enabled", &Menu::Get.Misc.Backtrack.BT_Enabled);

			ImGui::PushItemWidth(150.f);

			ImGui::Checkbox("Trigger bot", &Menu::Get.Misc.Backtrack.Triggerbot);

			ImGui::PopItemWidth();

			ImGui::EndPopup();
		}

		Pop();
	}*/

	PushW(156.f);

	ImGui::Text("\n");

	ImGui::Text(""); ImGui::SameLine(67.3f); ImGui::Text("Sounds\n");

	Push();

	if (ImGui::SliderInt("", &Menu::Get.Misc.Sounds.MusicVolume, 0, 100, "Music Volume: %d%%"))
	{
		if (snd_musicvolume)
		{
			snd_musicvolume->SetMemValue(Menu::Get.Misc.Sounds.MusicVolume * 0.01f);
		}
	}

	Pop();

	Push();

	ImGui::SliderInt("", &Menu::Get.Misc.Sounds.WeaponsAudio, 0, 100, "Weapons Volume: %d%%");

	Pop();

	Push();

	ImGui::SliderInt("", &Menu::Get.Misc.Sounds.FootStepsVolume, 0, 100, "Footsteps Volume: %d%%");

	Pop();

	ImGui::Text("\n");

	ImGui::Text(""); ImGui::SameLine(67.3f); ImGui::Text("Other\n");

	Push();

	ImGui::SliderInt("", &Menu::Get.Misc.Ragdoll_Force, 1, 200, "Ragdoll Force: Mult by %d");

	Pop();


	PopW();

	ImGui::Columns(1);

	ImGui::Separator();

	ImGui::TextUnformatted("Kill Message");

	ImGui::SameLine();

	if (ImGui::Button("...", ImVec2(50, 20)))
	{
		ImGui::OpenPopup("Killmsg");
	}

	if (ImGui::BeginPopup("Killmsg"))
	{
		ImGui::Checkbox("Enabled", &Menu::Get.Misc.KillMessage);

		ImGui::SameLine();

		ImGui::TextUnformatted("\t\tRandom Msg will be chosen when you kill someone");

		static char KillMessage[64]{ 0 };

		PushW(500.F);

		Push();

		ImGui::InputText("", KillMessage, IM_ARRAYSIZE(KillMessage));

		Pop();

		if (ImGui::Button("Insert", ImVec2(500.F, 0)))
		{
			if (KillMessage && KillMessage[0])
			{
				Killmessages.push_back(std::string(KillMessage));

				memset(KillMessage, 0, IM_ARRAYSIZE(KillMessage));
			}
		}

		for (size_t ID = 0; ID < Killmessages.size(); ID++)
		{
			ImGui::TextUnformatted(Killmessages[ID].c_str());

			ImGui::SameLine(408.f);

			ImGui::PushID(__LINE__ + ID);

			if (ImGui::Button("Delete", ImVec2(100, 0)))
			{
				Killmessages.erase(Killmessages.begin() + ID);
			}

			ImGui::PopID();
		}

		PopW();

		ImGui::EndPopup();
	}

	ImGui::SameLine();

	ImGui::Text("Player Info");

	ImGui::SameLine();

	Push();

	if (ImGui::Button("...", ImVec2(50, 20)))
	{
		ImGui::OpenPopup("Info");
	}

	if (ImGui::BeginPopup("Info"))
	{
		ImGui::PushItemWidth(100);

		static char Name[100]{ "" }, ClanTag[100]{ "" }, DisConnectMsg[100]{ "" };

		ImGui::BeginChild("PlayerName", ImVec2{ 200 , 210 });
		{
			ImGui::Text("Name");

			Push();

			ImGui::InputTextMultiline("", Name, IM_ARRAYSIZE(Name), ImVec2(100, 85));

			Pop();

			Push();

			if (ImGui::Button("Clear Input", ImVec2(100, 20)))
			{
				for (int i{ 0 }; i < sizeof(Name); i++)
				{
					Name[i] = '\0';
				}
			}

			if (ImGui::Button("Horizontal Tab", ImVec2(100, 20)))
			{
				Name[strlen(Name)] = '\t';
			}

			if (ImGui::Button("New Line", ImVec2(100, 20)))
			{
				Name[strlen(Name)] = '\n';
			}

			if (ImGui::Button("Apply", ImVec2(100, 0)))
			{
				if (static auto NameConVar = Cvar->FindVar("name"); NameConVar)
				{
					NameConVar->m_fnChangeCallback = NULL;

					NameConVar->SetValue(Name);
				}

				if (Engine->IsInGame())
				{
					ChangeName(Name);
				}
			}

			ImGui::PopID();

		}ImGui::EndChild();


		ImGui::SameLine();

		ImGui::BeginChild("ClanTag", ImVec2{ 200 , 210 });
		{
			ImGui::Text("ClanTag");

			Push();

			ImGui::InputTextMultiline("", ClanTag, IM_ARRAYSIZE(ClanTag), ImVec2(100, 60));

			Pop();

			Push();

			if (ImGui::Button("Clear Input", ImVec2(100, 20)))
			{
				for (int i{ 0 }; i < sizeof(ClanTag); i++)
				{
					ClanTag[i] = '\0';
				}
			}

			if (ImGui::Button("Horizontal Tab", ImVec2(100, 20)))
			{
				ClanTag[strlen(ClanTag)] = '\t';
			}

			if (ImGui::Button("New Line", ImVec2(100, 20)))
			{
				ClanTag[strlen(ClanTag)] = '\n';
			}

			if (ImGui::Button("Apply", ImVec2(100, 0)))
			{
				SetClanTag(ClanTag);

				Since_Last_Used = Globals->curtime;
			}

			if (ImGui::Button("Spammer", ImVec2(100, 0)))
			{
				ImGui::OpenPopup("Spammer");
			}

			if (ImGui::BeginPopup("Spammer"))
			{
				if (ImGui::Button("Changer", ImVec2(135, 0)))
				{
					ImGui::OpenPopup("Changer");
				}

				if (ImGui::BeginPopup("Changer"))
				{
					static char CustomClanTag[100]{};

					ImGui::Checkbox("Enabled", &Menu::Get.Misc.ClanTag.Changer);

					Push();

					ImGui::InputTextMultiline("", CustomClanTag, IM_ARRAYSIZE(CustomClanTag), ImVec2(120, 60));

					Pop();

					if (ImGui::Button("Insert", ImVec2(115, 0)))
					{
						if (CustomClanTag && CustomClanTag[0])
						{
							ClanTagInfo.push_back(CustomClanTag);

							memset(CustomClanTag, 0, IM_ARRAYSIZE(CustomClanTag));
						}

						if (ClanTagInfo.size() > 100)
						{
							ClanTagInfo.pop_back();
						}
					}

					for (size_t ID = 0; ID < ClanTagInfo.size(); ID++)
					{
						ImGui::Text("%s\n", ClanTagInfo[ID]);

						ImGui::SameLine(140.f);

						ImGui::PushID(__LINE__ + ID);

						if (ImGui::Button("Delete", ImVec2(100, 0)))
						{
							ClanTagInfo.erase(ClanTagInfo.begin() + ID);

							memset(CustomClanTag, 0, IM_ARRAYSIZE(CustomClanTag));
						}

						ImGui::PopID();
					}

					ImGui::EndPopup();
				}

				if (ImGui::Button("Stealer", ImVec2(135, 0)))
				{
					ImGui::OpenPopup("Stealer");
				}

				if (ImGui::BeginPopup("Stealer"))
				{
					ImGui::Checkbox("Enabled", &Menu::Get.Misc.ClanTag.Stealer);

					ImGui::EndPopup();
				}

				ImGui::PushItemWidth(140);

				ImGui::SliderFloat("Delay", &Menu::Get.Misc.ClanTag.Delay, 1.f, 10.f, "%.2f Seconds");

				ImGui::PopItemWidth();

				ImGui::EndPopup();
			}

			ImGui::PopID();

		}ImGui::EndChild();

		ImGui::SameLine();

		ImGui::BeginChild("Disconnect", ImVec2{ 200 , 210 });
		{
			ImGui::Text("Disconnect Msg");

			Push();

			ImGui::InputTextMultiline("", DisConnectMsg, IM_ARRAYSIZE(DisConnectMsg), ImVec2(120, 60));

			Pop();

			Push();

			if (ImGui::Button("Clear Input", ImVec2(120, 20)))
			{
				for (int i{ 0 }; i < sizeof(DisConnectMsg); i++)
				{
					DisConnectMsg[i] = '\0';
				}
			}

			if (ImGui::Button("Horizontal Tab", ImVec2(120, 20)))
			{
				DisConnectMsg[strlen(DisConnectMsg)] = '\t';
			}

			if (ImGui::Button("New Line", ImVec2(120, 20)))
			{
				DisConnectMsg[strlen(DisConnectMsg)] = '\n';
			}

			if (ImGui::Button("Apply", ImVec2(120, 0)))
			{
				Set_DisConnection_Msg(DisConnectMsg);
			}

			if (ImGui::Button("Default Msg", ImVec2(120, 0)))
			{
				Set_DisConnection_Msg("Disconnect by user.");
			}

			ImGui::PopID();

		}ImGui::EndChild();

		ImGui::SameLine();

		ImGui::BeginChild("SteamID Spoofer", ImVec2{ 200 , 210 });
		{
			ImGui::Text("SteamID Spoofer");

			ImGui::PushItemWidth(130);

			ImGui::Checkbox("Enabled", &Menu::Get.Misc.SteamIDSpoofer.SIP_Enabled);

			ImGui::Checkbox("Random", &Menu::Get.Misc.SteamIDSpoofer.RandomSteamID);

			Push();

			ImGui::InputInt("", &Menu::Get.Misc.SteamIDSpoofer.SteamID);

			ImGui::PopID();

			ImGui::TextUnformatted(SteamID(Menu::Get.Misc.SteamIDSpoofer.SteamID).Format(SteamID::Formats::STEAMID32).c_str());

			ImGui::PopItemWidth();

		}ImGui::EndChild();

		ImGui::PopItemWidth();

		ImGui::EndPopup();
	}

	Pop();

	/*ImGui::SameLine();

	ImGui::Text("Fake Latency");

	ImGui::SameLine();

	Push();

	if (ImGui::Button("...", ImVec2(50, 20)))
	{
		ImGui::OpenPopup("FakeLatency");
	}

	Pop();*/

	ImGui::SameLine();

	ImGui::Text("Voice Chat");

	ImGui::SameLine();

	Push();

	if (ImGui::Button("...", ImVec2(50, 20)))
	{
		ImGui::OpenPopup("Voice Chat");
	}

	if (ImGui::BeginPopup("Voice Chat"))
	{
		static auto Voice_RecordStart = reinterpret_cast<bool(__cdecl*)(const char*, const char*, const char*)>(
			CallableFromRelative(Tools::FindPattern("engine.dll", "E8 ? ? ? ? 83 C4 0C 5F 5E C3")));

		static auto Dirctory = std::string(DirectoryPath).append("\\cstrike\\sound\\ZE-Elite\\");

		int ID = 0;

		for (auto& SoundInfo : SoundList)
		{
			ImGui::TextUnformatted(SoundInfo.GetName()); ImGui::SameLine(300.F); HotKey(SoundInfo.GetKey()); ImGui::SameLine(350.f);

			ImGui::PushID(__LINE__ + ID++);

			if (ImGui::Button("Play"))
			{
				Engine->ClientCmd_Unrestricted("voice_loopback 1");

				SoundInfo.EndTime = Globals->curtime + SoundInfo.SoundDuration;

				Voice_RecordStart(NULL, NULL, std::string(Dirctory + SoundInfo.GetName() + ".wav").c_str());
			}

			Pop();
		}

		ImGui::EndPopup();
	}

	Pop();


	ImGui::Separator();

	//Push();

	//PushW(234.f);

	//ImGui::TextUnformatted("AutoStrafe");

	//ImGui::SameLine();

	//ImGui::Combo("", &Menu::Get.Misc.Autostrafe, "Off\0Basic\0Rage\0");

	//Pop();

	//PopW();

	PushW(188.5f);

	ImGui::TextColored(ImColor{ 20,255,20 }, "Download Manager");

	ImGui::SameLine();

	Push();

	ImGui::Combo("", &Menu::Get.Misc.DownloadManagerFilter, "All\0No Sounds\0Maps Only\0None\0Custom File Extension\0");

	Pop();

	ImGui::SameLine();

	Push();

	ImGui::InputText("", Menu::Get.Misc.CustomizeableFiles, IM_ARRAYSIZE(Menu::Get.Misc.CustomizeableFiles));

	Pop();

	PopW();
}

void Menu::ConVars() noexcept
{
	static float Value{ 0 };

	static int Flags{ 0 };

	static char name[64]{};

	ImGui::TextColored((ImVec4)ImColor(20, 255, 20), "Enforce ConVars");

	ImGui::TextColored((ImVec4)ImColor(20, 20, 255), "Name");

	ImGui::SameLine();

	ImGui::TextColored((ImVec4)ImColor(255, 20, 20), "\t\t\t\t\t\t\tValue");

	ImGui::SameLine();

	ImGui::TextColored((ImVec4)ImColor(255, 255, 20), "\t\t\t\t\t\tFlags");

	PushW(121.1f);

	Push();

	ImGui::InputText("", name, IM_ARRAYSIZE(name));

	Pop();

	ImGui::SameLine();

	Push();

	ImGui::InputFloat("", &Value);

	Pop();

	ImGui::SameLine();

	Push();

	ImGui::InputInt("", &Flags);

	Pop();

	ImGui::SameLine();

	Push();

	if (ImGui::Button("Apply", ImVec2(100, 22)))
	{
		if (name && name[0])
		{
			if (auto CV(Cvar->FindVar(name)); CV)
				CV->SetMemValue(Value, Flags);
			else
				ColoredConMsg(
					Color(20, 255, 20, 255),
					"ConVar %s is not found.\n",
					name);
		}
	}

	Pop();

	ImGui::PopItemWidth();

	ImGui::Separator();

	ImGui::Separator();

	PushW(186.5f);

	static int Command_Flag{ 0 };

	static char Command_name[64]{};

	ImGui::TextColored((ImVec4)ImColor(20, 50, 50), "Set Command Flags");

	ImGui::TextColored((ImVec4)ImColor(20, 255, 20), "Name");



	ImGui::SameLine(207.f);

	ImGui::TextColored((ImVec4)ImColor(255, 20, 255), "Flags");


	Push();

	ImGui::InputText("", Command_name, IM_ARRAYSIZE(Command_name));

	Pop();

	ImGui::SameLine();

	Push();

	ImGui::InputInt("", &Command_Flag);

	Pop();

	ImGui::SameLine();

	Push();

	if (ImGui::Button("Apply", ImVec2(100, 22)))
	{
		if (Command_name && Command_name[0])
		{
			if (auto Command_Ptr = Cvar->FindCommand(Command_name); Command_Ptr)
				Command_Ptr->SetFlags(Command_Flag);
			else
				ColoredConMsg(
					Color(20, 255, 20, 255),
					"Command %s is not found.\n",
					Command_name);

		}
	}

	Pop();

	PopW();

	ImGui::Separator();

	ImGui::Separator();

	ImGui::TextColored(ImColor(20, 255, 20, 255), "Desired Console Variable Data Response");
	{

		static char Variable_Name[64], Value[64]; static int Status = 0;

		ImGui::TextColored((ImVec4)ImColor(20, 20, 255), "Name");

		ImGui::SameLine(71.5f);

		ImGui::TextColored((ImVec4)ImColor(255, 20, 20), "\t\t\t\t\t\t\tValue");


		ImGui::SameLine(224.f);

		ImGui::TextColored((ImVec4)ImColor(255, 255, 20), "\t\t\t\t\t\tExistence");

		ImGui::PushItemWidth(138);
		Push();
		ImGui::InputText("", Variable_Name, IM_ARRAYSIZE(Variable_Name));
		Pop();
		ImGui::SameLine();
		Push();
		ImGui::InputText("", Value, IM_ARRAYSIZE(Value));
		Pop();
		ImGui::SameLine();
		Push();
		ImGui::Combo("", &Status, "Yes\0No\0");
		Pop();
		ImGui::SameLine();

		if (ImGui::Button("Add", ImVec2(40, 22)))
		{
			if (Cvar->FindVar(Variable_Name) || Cvar->FindCommand(Variable_Name))
			{
				for (int i = 0; i < DesiredConVarsValue.size(); i++)
				{
					if (strcmp(DesiredConVarsValue.at(i).GetName(), Variable_Name) == 0)
					{
						DesiredConVarsValue.at(i).Set(Value, Status);

						memset(Variable_Name, 0, IM_ARRAYSIZE(Variable_Name));

						memset(Value, 0, IM_ARRAYSIZE(Value));

						Status = 0;

						goto Label;
					}
				}

				DesiredConVarsValue.push_back({});

				DesiredConVarsValue.at(DesiredConVarsValue.size() - 1).Set(Variable_Name, Value, Status);

				memset(Variable_Name, 0, IM_ARRAYSIZE(Variable_Name));

				memset(Value, 0, IM_ARRAYSIZE(Value));

				Status = 0;
			}
			else
			{
				ColoredConMsg(Color(RandomInt(10, 200), RandomInt(10, 200), 255), "Cvar %s was not found\n", Variable_Name);
			}
		}

	Label:

		if (DesiredConVarsValue.size())
		{
			ImGui::TextColored(ImColor(20, 20, 255), "Added ConVars:");

			for (int i = 0; i < DesiredConVarsValue.size(); i++)
			{
				auto&& pCvar = DesiredConVarsValue[i];

				//ConMsg("%s %s %s\n", pCvar.Name, pCvar.Value, pCvar.Status);

				ImGui::Text("%s -> Prescribed Value: %s Existance: %s\n", pCvar.GetName(), pCvar.GetValue(), pCvar.GetStatus() ? "No" : "Yes");

				ImGui::SameLine(438);

				ImGui::PushID(__LINE__ + i);

				if (ImGui::Button("Remove", ImVec2(60, 22)))
				{
					DesiredConVarsValue.erase(DesiredConVarsValue.begin() + i);
				}

				Pop();
			}
		}

		ImGui::PopItemWidth();
	}
}

int FontID = 0; 

extern const char* MenuFont[12];

void Menu::Menu() noexcept
{
	ImGui::PushItemWidth(200.f);

	ImGui::Combo("Font", &FontID, MenuFont, IM_ARRAYSIZE(MenuFont));

	ImGui::SameLine(244.f);

	if (auto Font = ImGui::GetIO().Fonts->Fonts[FontID]; Font)
	{
		ImGui::SliderFloat("Font Size", &Font->FontSize, 1, 20.f);
	}

	ImGui::PopItemWidth();

	ImGuiStyle& style = ImGui::GetStyle();
	for (int i = 0; i < ImGuiCol_COUNT; i++) {
		if (i && i % 2) ImGui::SameLine(244.f);

		ColorPopup(ImGui::GetStyleColorName(i), (std::array<float, 4>&)style.Colors[i], __LINE__ + i);
	}
}


void Menu::RenderTabs() noexcept
{
	static float flRainbow;

	float flSpeed = 0.0003f;

	int curWidth = 1, y, size;

	ImVec2 curPos = ImGui::GetCursorPos();

	ImVec2 curWindowPos = ImGui::GetWindowPos();

	curPos.x += curWindowPos.x;

	curPos.y += curWindowPos.y;

	Engine->GetScreenSize(y, size);

	DrawRectRainbow(curPos.x - 10, curPos.y - 5, ImGui::GetWindowSize().x + size, curPos.y + -4, flSpeed, flRainbow);

	ImGui::PushFont(ImGui::GetIO().Fonts->Fonts[0]);

#define PushF() ImGui::PushFont(ImGui::GetIO().Fonts->Fonts[FontID])

#define PopF()  ImGui::PopFont();

	if (ImGui::BeginTabBar("TabBar", ImGuiTabBarFlags_Reorderable | ImGuiTabBarFlags_FittingPolicyScroll))
	{
		if (ImGui::BeginTabItem("Zombie Escape"))
		{
			PushF(); ZE(); PopF();

			ImGui::EndTabItem();
		}
		if (ImGui::BeginTabItem("Aimbot"))
		{
			PushF(); Aimbot(); PopF();

			ImGui::EndTabItem();
		}
		if (ImGui::BeginTabItem("Triggerbot"))
		{
			PushF(); TriggerBot(); PopF();

			ImGui::EndTabItem();
		}
		if (ImGui::BeginTabItem("Chams"))
		{
			PushF(); RenderChamsTab(); PopF();

			ImGui::EndTabItem();
		}
		if (ImGui::BeginTabItem("Visuals"))
		{
			PushF(); Visuals(); PopF();

			ImGui::EndTabItem();
		}
		if (ImGui::BeginTabItem("Miscellaneous"))
		{
			PushF(); Miscellaneous(); PopF();

			ImGui::EndTabItem();
		}
		if (ImGui::BeginTabItem("ConVars"))
		{
			PushF(); ConVars(); PopF();

			ImGui::EndTabItem();
		}
		if (ImGui::BeginTabItem("Menu"))
		{
			PushF(); Menu(); PopF();

			ImGui::EndTabItem();
		}

		ImGui::EndTabBar();
	}

	ImGui::PopFont();
}

void Menu::Render() noexcept
{
	if (Menu::Get.Menu.On)
	{
		ImGui_ImplDX9_NewFrame();

		ImGui_ImplWin32_NewFrame();

		ImGui::NewFrame();

		//ImGui::SetNextWindowSize({ 510.f , 0.0f });

		ImGui::Begin("ZE Elite", nullptr, windowFlags);
		{
			RenderTabs();
		}

		ImGui::End();

		ImGui::EndFrame();

		ImGui::Render();

		ImGui_ImplDX9_RenderDrawData(ImGui::GetDrawData());
	}
}
