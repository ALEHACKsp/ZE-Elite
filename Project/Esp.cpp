#pragma once

#include "SDK.h"
#include "include.h"
#include "Assorted.h"
#include "CNadeTracer.h"
#include "ImGui/imgui.h"
#include "ImGui/ImGui_internal.h"
#include "ImGui/imgui_impl_dx9.h"

#include "ClientEntityListener.h"
#include "SkinChanger.h"

#define IM_FLOOR(_VAL) ((float)(int)(_VAL))    

HFont Esp::Others[5]{ 0,0,0,0,0 }, Esp::courier_new{ 0 }, Esp::Counter_Strike_Logo{ 0 };

bool Esp::Fonts_Were_Initialized{ false };

struct BoundingBox
{
	float x0, y0;

	float x1, y1;

	BoundingBox(CBaseEntity* Entity) noexcept
	{
		int width, height;

		Engine->GetScreenSize(width, height);

		x0 = static_cast<float>(width * 2);

		y0 = static_cast<float>(height * 2);

		x1 = -x0;

		y1 = -y0;

		Vector mins = Entity->GetMins(), maxs = Entity->GetMaxs();

		for (int i = 0; i < 8; ++i)
		{
			const Vector point{ i & 1 ? maxs.x : mins.x,
								i & 2 ? maxs.y : mins.y,
								i & 4 ? maxs.z : mins.z };

			Vector Screen;

			if (!WorldToScreen(point.Transform(Entity->GetRgflCoordinateFrame()), Screen))
			{
				valid = false;

				return;
			}

			x0 = min(x0, Screen.x);
			y0 = min(y0, Screen.y);
			x1 = max(x1, Screen.x);
			y1 = max(y1, Screen.y);
		}

		valid = true;
	}

	operator bool() noexcept
	{
		return valid;
	}
private:
	bool valid;
};

const char* WeaponIDToAlias(int id) noexcept
{
	static auto Location = CallableFromRelative((void*)Tools::FindPattern("client.dll", "E8 ? ? ? ? 50 FF 75 94"));

	using WeaponIDToAlias_Fn = const char* (*)(int);

	return WeaponIDToAlias_Fn((unsigned int)Location)(id);
}

const wchar_t* GetWC(const char* c) noexcept
{
	const size_t cSize = strlen(c) + 1;
	wchar_t* wc = new wchar_t[cSize];
	mbstowcs(wc, c, cSize);

	return wc;
}

void RenderText(int x, int y, Color color, HFont font, const wchar_t* text) noexcept
{
	Surface->DrawSetTextPos(x, y);
	Surface->DrawSetTextFont(font);
	Surface->DrawSetTextColor(color);
	Surface->DrawPrintText(text, wcslen(text));
}


bool ScreenTransform(const Vector& point, Vector& screen) noexcept
{
	const matrix3x4& w2sMatrix = Engine->WorldToScreenMatrix();
	screen.x = w2sMatrix[0][0] * point.x + w2sMatrix[0][1] * point.y + w2sMatrix[0][2] * point.z + w2sMatrix[0][3];
	screen.y = w2sMatrix[1][0] * point.x + w2sMatrix[1][1] * point.y + w2sMatrix[1][2] * point.z + w2sMatrix[1][3];
	screen.z = 0.0f;

	float w = w2sMatrix[3][0] * point.x + w2sMatrix[3][1] * point.y + w2sMatrix[3][2] * point.z + w2sMatrix[3][3];

	if (w < 0.001f) 
	{
		screen.x *= 100000;
		screen.y *= 100000;

		return true;(0xEE8 - 0xd8d);
	}

	float invw = 1.0f / w;
	screen.x *= invw;
	screen.y *= invw;

	return false;
}


bool WorldToScreen(const Vector& origin, Vector& screen) noexcept
{
	if (!ScreenTransform(origin, screen))
	{
		int iScreenWidth, iScreenHeight;

		Engine->GetScreenSize(iScreenWidth, iScreenHeight);

		screen.x = (iScreenWidth / 2.0f) + (screen.x * iScreenWidth) / 2;

		screen.y = (iScreenHeight / 2.0f) - (screen.y * iScreenHeight) / 2;

		return true;
	}
	return false;
}

void Esp::Initialize() noexcept
{
	Counter_Strike_Logo = Surface->CreateFont();

	courier_new = Surface->CreateFont();

	for(auto& Font : Others) Font = Surface->CreateFont();

	Surface->SetFontGlyphSet(Others[0], "Tahoma", 14, 0, 0, 0, ISurface::FONTFLAG_ANTIALIAS);

	Surface->SetFontGlyphSet(Others[1], "Tahoma", 25, 15,1, 0, ISurface::FONTFLAG_DROPSHADOW | ISurface::FONTFLAG_ANTIALIAS);

	Surface->SetFontGlyphSet(Others[2], "Arial", 16, 0, 0, 0, ISurface::FONTFLAG_ANTIALIAS);

	Surface->SetFontGlyphSet(Others[3], "Verdana", 12, 0, 0, 0, ISurface::FONTFLAG_OUTLINE);

	Surface->SetFontGlyphSet(courier_new, "Courier New", 40, 20, 1, 0, ISurface::FONTFLAG_ANTIALIAS | ISurface::FONTFLAG_OUTLINE);

	Surface->SetFontGlyphSet(Counter_Strike_Logo, "Counter-Strike", 40, 0, 0, 0, ISurface::FONTFLAG_ANTIALIAS);

	Fonts_Were_Initialized = true;
}

extern bool EntityIsLaser(CBaseEntity* Entity);

void Esp::Debug(CBaseEntity* pLocal) noexcept
{
	static char Buffer[64]{ 0 };

	static Vector vScreenPos{};

	const bool Can_Complete = Menu::Get.Visuals.Debug.INDEX
		|| Menu::Get.Visuals.Debug.Classname
		|| Menu::Get.Visuals.Debug.ModelName
		|| Menu::Get.Visuals.Debug.ModelIndex
		|| Menu::Get.Visuals.Debug.Origin
		|| Menu::Get.Visuals.Debug.Velocity;

	if (Can_Complete)
	{
		LoopTroughEntities()
		{
			auto Entity = GetEntity(i);

			if (Entity == NULL || Entity->IsDormant())
				continue;

			auto ModelName = GetModelName(Entity->GetModel());

			bool Filter0 = Menu::Get.Visuals.Debug.Filter[0] && Entity->IsPlayer();

			bool Filter1 = Menu::Get.Visuals.Debug.Filter[1] && (strstr(ModelName, "weapon") || strstr(ModelName, "Weapon"));

			bool Filter2 = Menu::Get.Visuals.Debug.Filter[2] && strstr(ModelName, "sprite");

			bool Filter3 = Menu::Get.Visuals.Debug.Filter[3] && !Entity->EstimateAbsVelocity().Length();

			if (Filter0 || Filter1 || Filter2 || Filter3)
				continue;

			Vector vWorldPos{ Entity->WorldSpaceCenter() };

			if (pLocal->GetAbsOrigin().DistTo(Entity->GetAbsOrigin()) <= Menu::Get.Visuals.Debug.MaxDistance)
			{
				if (WorldToScreen(vWorldPos, vScreenPos))
				{
					int8_t Value{ 0 };

					if (Menu::Get.Visuals.Debug.INDEX)
					{
						snprintf(Buffer, sizeof Buffer, "Index: %i", Entity->GetIndex());

						RenderText(vScreenPos.x, vScreenPos.y, Color(255, 255, 255, 255), Esp::Others[2], GetWC(Buffer));

						Value += 15;
					}

					if (Menu::Get.Visuals.Debug.Classname)
					{
						snprintf(Buffer, sizeof Buffer, "%s", Entity->GetDebugName());

						RenderText(vScreenPos.x, vScreenPos.y + Value, Color(255, 255, 255, 255), Esp::Others[2], GetWC(Buffer));

						Value += 15;
					}

					if (Menu::Get.Visuals.Debug.ModelName)
					{
						snprintf(Buffer, sizeof Buffer, "Modelname: %s", GetModelName(Entity->GetModel()));

						RenderText(vScreenPos.x, vScreenPos.y + Value, Color(255, 255, 255, 255), Esp::Others[2], GetWC(Buffer));

						Value += 15;
					}

					if (Menu::Get.Visuals.Debug.ModelIndex)
					{
						snprintf(Buffer, sizeof Buffer, "Model Index: %i", Entity->GetModelIndex());

						RenderText(vScreenPos.x, vScreenPos.y + Value, Color(255, 255, 255, 255), Esp::Others[2], GetWC(Buffer));

						Value += 15;
					}

					if (Menu::Get.Visuals.Debug.Origin)
					{
						snprintf(Buffer, sizeof Buffer, "Origin: %.2f %.2f %.2f", Entity->GetAbsOrigin().x, Entity->GetAbsOrigin().y, Entity->GetAbsOrigin().z);

						RenderText(vScreenPos.x, vScreenPos.y + Value, Color(255, 255, 255, 255), Esp::Others[2], GetWC(Buffer));

						Value += 15;
					}

					if (Menu::Get.Visuals.Debug.Velocity)
					{
						snprintf(Buffer, sizeof Buffer, "Velocity: %i", static_cast<int>(Entity->EstimateAbsVelocity().Length()));

						RenderText(vScreenPos.x, vScreenPos.y + Value, Color(255, 255, 255, 255), Esp::Others[2], GetWC(Buffer));
					}
				}
			}
		}
	}
}

void RenderDynamiclights(Vector LocalOrigin)
{
	extern std::vector < dlight_t* > dLights;

	for (auto& dLight : dLights)
	{
		if (LocalOrigin.DistTo(dLight->origin) < 1000)
		{
			//dLight->color.exponent = true;
			//dLight->flags = 0;
			dLight->radius = 500;
			dLight->die = Globals->curtime + 0.1f;
			//dLight->decay = 20.0f;
		}
	}
}

void RenderCrosshair(CBaseEntity* pLocal)
{
	static int Width, Height;

	constexpr auto& Crosshair = Menu::Get.Visuals.Crosshair;

	constexpr auto& Circle_Shape = Crosshair.Circle_Shape;

	constexpr auto& Plus_Shape = Crosshair.Plus_Shape;

	constexpr auto& X_Shape = Crosshair.X_Shape;

	Surface->GetScreenSize(Width, Height); int screenWide = Width / 2; int screenTall = Height / 2;

#define DrawLine Surface->DrawLine

	constexpr auto& CrosshairColor = Menu::Get.Colors.General.Crosshair;

	if (Crosshair.Recoil) {

		screenWide = (Width / 2) - ((Width / 90) * pLocal->GetPunchAngle().y);

		screenTall = (Height / 2) + ((Height / 90) * pLocal->GetPunchAngle().x);
	}

	if (Plus_Shape.Enabled)
	{
		constexpr auto& gap = Plus_Shape.Gap;

		constexpr auto& length = Plus_Shape.Length;

		Surface->DrawSetColor(CrosshairColor.Plus_Shape);

		DrawLine(screenWide - (length + gap), screenTall, screenWide - gap, screenTall);

		DrawLine(screenWide + (length + gap), screenTall, screenWide + gap, screenTall);

		DrawLine(screenWide, screenTall - (length + gap), screenWide, screenTall - gap);

		DrawLine(screenWide, screenTall + (length + gap), screenWide, screenTall + gap);
	}

	if (X_Shape.Enabled)
	{
		constexpr auto& gap = X_Shape.Gap;

		constexpr auto& length = X_Shape.Length;

		Surface->DrawSetColor(CrosshairColor.X_Shape);

		DrawLine(screenWide - (length + gap), screenTall - (length + gap), screenWide - gap, screenTall - gap);

		DrawLine(screenWide - (length + gap), screenTall + (length + gap), screenWide - gap, screenTall + gap);

		DrawLine(screenWide + (length + gap), screenTall + (length + gap), screenWide + gap, screenTall + gap);

		DrawLine(screenWide + (length + gap), screenTall - (length + gap), screenWide + gap, screenTall - gap);
	}

	if (Circle_Shape.Enabled)
	{
		Surface->DrawSetColor(CrosshairColor.Circle_Shape);

		Surface->DrawOutlinedCircle(screenWide, screenTall, Circle_Shape.Radius, Circle_Shape.Segments);
	}


#undef DrawLine
}

void RunGrenadeTracer()
{
	if (Menu::Get.Colors.General.NadeTracer.a())
	{
		LoopTroughEntities()
		{
			if (static auto m_hThrower = GetNetVarOffset("DT_BaseGrenade", "m_hThrower", BaseClientDLL->GetAllClasses()); m_hThrower)
			{
				if (auto Entity = GetEntity(i); Entity && Entity->GetVelocity().Length() > 1.f && !strcmp(Entity->GetDebugName(), "grenade"))
				{
					if (auto Thrower = *reinterpret_cast<BYTE*>((unsigned int)Entity + m_hThrower); Thrower == Engine->GetLocalPlayer())
					{
						pNadeTracer.AddTracer(Entity, 60);

						pNadeTracer.Draw();
					}
				}
			}
		}

		pNadeTracer.Clear();
	}
}

struct DynamicBoundingBox
{
	float x0, y0;

	float x1, y1;

	DynamicBoundingBox(CBaseEntity* Entity) noexcept
	{
		valid = false;

		int width, height;

		Engine->GetScreenSize(width, height);

		x0 = static_cast<float>(width * 2);

		y0 = static_cast<float>(height * 2);

		x1 = -x0;

		y1 = -y0;

		const auto model = Entity->GetModel();
		if (!model) return;

		const auto studio = ModelInfo->GetStudiomodel(model);
		if (!studio) return;

		auto pStudioHdr = ModelInfo->GetStudiomodel(model);
		if (!pStudioHdr)
			return;

		auto pHitboxSet = pStudioHdr->GetHitboxSet(Entity->GetHitboxSet());
		if (!pHitboxSet)
			return;

		matrix3x4 matrix[128];

		if (!Entity->SetupBones(matrix, 128, 0x00000100, Globals->curtime))
			return;

		for (auto i = pHitboxSet->numhitboxes; i--; ) {

			if (auto HitBox = pHitboxSet->GetHitbox(i); HitBox)
			{
				auto maxs = HitBox->bbmax;

				auto mins = HitBox->bbmin;

				for (int i = 0; i < 8; ++i)
				{
					const Vector point{ i & 1 ? maxs.x : mins.x,
										i & 2 ? maxs.y : mins.y,
										i & 4 ? maxs.z : mins.z };

					Vector Screen;

					if (!WorldToScreen(point.Transform(matrix[HitBox->bone]), Screen)) return;

					x0 = min(x0, Screen.x);
					x1 = max(x1, Screen.x);

					y0 = min(y0, Screen.y);
					y1 = max(y1, Screen.y);
				}
			}
		}

		valid = true;
	}

	operator bool() noexcept
	{
		return valid;
	}
private:
	bool valid;
};

void Esp::Render() noexcept
{
	if (const auto pLocal{ CBaseEntity::GetLocalPlayer() }; pLocal)
	{
		const bool IsAlive(pLocal->IsAlive());

		//const auto Team_Number(pLocal->GetTeamNum());

		Debug(pLocal);

		if (IsAlive)
		{
			RunGrenadeTracer();

			RenderCrosshair(pLocal);

			if (Menu::Get.Visuals.DamageIndicator.DI_Enabled)
			{
				DamageIndicator::Paint(pLocal);
			}

			Hit_Marker_On_Paint();

			//RenderDynamiclights(pLocal->GetVecOrigin());

			const unsigned char DLights(Menu::Get.Visuals.DLights);

			bool IGnoreDllLights(DLights == 1);

			if (IGnoreDllLights)
			{
				const unsigned short INDEX(Engine->GetLocalPlayer());

				dlight_t* dLight = Effects->CL_AllocDlight(INDEX);
				dLight->die = Globals->curtime + 0.05f;
				dLight->radius = 200.f;

				dLight->color.r = Menu::Get.Colors.PlayerEsp.DLights[0];
				dLight->color.g = Menu::Get.Colors.PlayerEsp.DLights[1];
				dLight->color.b = Menu::Get.Colors.PlayerEsp.DLights[2];

				dLight->color.exponent = 5;
				dLight->key = INDEX;
				dLight->decay = dLight->radius / 5.0f;
				dLight->origin = pLocal->GetAbsOrigin() + Vector(0, 0, 2);
			}

			LoopTroughPlayers()
			{
				if (auto Entity = GetEntity(i); Entity && !Entity->IsDormant() && Entity->IsPlayer() && Entity->IsAlive()) {

					const auto IsEnemy(Entity->IsEnemy(pLocal));

					//LocalRadarHack(i);

					if (!IGnoreDllLights)
					{
						if ((IsEnemy && DLights == 2) || (!IsEnemy && DLights == 3) || DLights == 4)
						{
							dlight_t* dLight = Effects->CL_AllocDlight(i);
							dLight->die = Globals->curtime + 0.05f;
							dLight->radius = 200.f;

							dLight->color.r = Menu::Get.Colors.PlayerEsp.DLights[0];
							dLight->color.g = Menu::Get.Colors.PlayerEsp.DLights[1];
							dLight->color.b = Menu::Get.Colors.PlayerEsp.DLights[2];

							dLight->color.exponent = 5;
							dLight->key = i;
							dLight->decay = dLight->radius / 5.0f;
							dLight->origin = Entity->GetAbsOrigin() + Vector(0, 0, 2);
						}
					}

					if (IsEnemy)
					{
						DynamicBoundingBox DynamicBBox{ Entity };

						BoundingBox BBox{ Entity };

						if (!BBox)
							continue;

						Box(DynamicBBox);
						Skeleton(Entity);
						Name(BBox, Entity);
						Health(BBox, Entity);
						SnapLine(BBox, Entity);
						Weapon(Entity->GetAbsOrigin(), Entity);
					}
				}
			}
		}
	}
}

void Esp::Box(DynamicBoundingBox bbox) noexcept
{
	if (Menu::Get.Visuals.Box)
	{
		Surface->DrawSetColor(Menu::Get.Colors.PlayerEsp.Box);

		switch (Menu::Get.Visuals.BoxType)
		{
		case 0:
			Surface->DrawOutlinedRect(bbox.x0, bbox.y0, bbox.x1, bbox.y1);

			if (!true)
			{
				Surface->DrawOutlinedRect(bbox.x0 + 1, bbox.y0 + 1, bbox.x1 - 1, bbox.y1 - 1);

				Surface->DrawOutlinedRect(bbox.x0 - 1, bbox.y0 - 1, bbox.x1 + 1, bbox.y1 + 1);
			}
			break;
		case 1:

			Surface->DrawLine(bbox.x0, bbox.y0, bbox.x0, IM_FLOOR(bbox.y0 * 0.75f + bbox.y1 * 0.25f));

			Surface->DrawLine(bbox.x0, bbox.y0, IM_FLOOR(bbox.x0 * 0.75f + bbox.x1 * 0.25f), bbox.y0);

			Surface->DrawLine(bbox.x1, bbox.y0, IM_FLOOR(bbox.x1 * 0.75f + bbox.x0 * 0.25f), bbox.y0);

			Surface->DrawLine(bbox.x1 - 1.0f, bbox.y0, bbox.x1 - 1.0f, IM_FLOOR(bbox.y0 * 0.75f + bbox.y1 * 0.25f));

			Surface->DrawLine(bbox.x0, bbox.y1, bbox.x0, IM_FLOOR(bbox.y1 * 0.75f + bbox.y0 * 0.25f));

			Surface->DrawLine(bbox.x0, bbox.y1 - 1.0f, IM_FLOOR(bbox.x0 * 0.75f + bbox.x1 * 0.25f), bbox.y1 - 1.0f);

			Surface->DrawLine(bbox.x1 - 0.5f, bbox.y1 - 1.0f, IM_FLOOR(bbox.x1 * 0.75f + bbox.x0 * 0.25f), bbox.y1 - 1.0f);

			Surface->DrawLine(bbox.x1 - 1.0f, bbox.y1, bbox.x1 - 1.0f, IM_FLOOR(bbox.y1 * 0.75f + bbox.y0 * 0.25f));

			break;
		}
	}
}

void Esp::Health(BoundingBox bbox, CBaseEntity* Entity) noexcept
{
	if (Menu::Get.Visuals.Health)
	{
		Surface->DrawSetColor(Menu::Get.Colors.PlayerEsp.Health);

		if (!Menu::Get.Visuals.HealthType)
		{
			Vector ScreenPos;

			static int width, height;

			auto Str = std::to_wstring(Entity->GetHealth()) + L" HP";

			Surface->GetTextSize(Others[Menu::Get.Visuals.HealthFont], Str.c_str(), width, height);

			auto Origin = Entity->GetAbsOrigin(); Origin.z += Entity->GetMaxs().z + 50.f;

			if (WorldToScreen(Origin, ScreenPos))
				RenderText(ScreenPos.x - width / 2, ScreenPos.y,
					Menu::Get.Colors.PlayerEsp.Health, Others[Menu::Get.Visuals.HealthFont], Str.c_str());
		}
		else if (Menu::Get.Visuals.HealthType == 1)
		{
			if (auto EntityIndex = Entity->GetIndex(); EntityIndex < 65)
			{
				auto MaxHealth = HealthSprite::SpriteEntities[EntityIndex].MaxHealth;

				MaxHealth = max(MaxHealth, Entity->GetMaxHealth());

				auto  hp = Entity->GetHealth();

				float box_h = (float)fabs(bbox.y0 - bbox.y1);

				constexpr float off = 8;

				int height = (box_h * hp) / MaxHealth;

				int x = bbox.x0 - off;

				int y = bbox.y0;

				constexpr int w = 4;

				int h = box_h;

				Surface->DrawSetColor(Color::Black());

				Surface->DrawFilledRect(x, y, x + w, y + h);

				Surface->DrawSetColor(Menu::Get.Colors.PlayerEsp.Health);

				Surface->DrawFilledRect(x + 1, bbox.y1 - (height + 2), x + w - 1, bbox.y1 - 2);
			}
		}
	}
}

class CHudTexture
{
public:
	char pad[134];
	char		cCharacterInFont;
	HFont hFont;

	// vgui texture Id assigned to this item
	int			textureId;
	// s0, t0, s1, t1
	float		texCoords[4];
};

void Esp::Weapon(const Vector& EntityOrigin, CBaseEntity* Entity) noexcept
{
	if (Menu::Get.Visuals.Weapon)
	{
		if (const auto ActiveWeapon{ Entity->GetActiveWeapon() }; ActiveWeapon)
		{
			auto Origin = EntityOrigin; Origin.z -= 10.f;

			Vector ScreenPos;

			static int width, height;

			static char Buffer[64]{ "" };

			if (!Menu::Get.Visuals.WeaponRenderingMode)
			{
				snprintf(Buffer, sizeof Buffer, "%s", WeaponIDToAlias(ActiveWeapon->GetWeaponID()));

				Buffer[0] = std::toupper(Buffer[0]);

				Surface->GetTextSize(Others[Menu::Get.Visuals.WeaponFont], GetWC(Buffer), width, height);

				if (WorldToScreen(Origin, ScreenPos))
					RenderText(ScreenPos.x - width / 2, ScreenPos.y,
						Menu::Get.Colors.PlayerEsp.Weapon, Others[Menu::Get.Visuals.WeaponFont], GetWC(Buffer));
			}
			else
			{
				if (auto WeaponData = ActiveWeapon->GetWpnData(); WeaponData)
				{
					if (auto WpnIcon = WeaponData->iconActive; WpnIcon)
					{
						snprintf(Buffer, sizeof Buffer, "%c", WpnIcon->cCharacterInFont);

						Surface->GetTextSize(Counter_Strike_Logo, GetWC(Buffer), width, height);

						if (WorldToScreen(Origin, ScreenPos))
							RenderText(ScreenPos.x - width / 2, ScreenPos.y,
								Menu::Get.Colors.PlayerEsp.Weapon, Counter_Strike_Logo, GetWC(Buffer));
					}
				}
			}
		}
	}
}

void Esp::SnapLine(BoundingBox bbox, CBaseEntity* Entity) noexcept
{
	if (Menu::Get.Visuals.SnapLine)
	{
		static Vector position;

		if (!WorldToScreen(Entity->GetAbsOrigin(), position))
			return;

		Surface->DrawSetColor(Menu::Get.Colors.PlayerEsp.SnapLine);

		static int width, height;

		Engine->GetScreenSize(width, height);

		Surface->DrawLine(width / 2, height, static_cast<int>(position.x), static_cast<int>(position.y));

	}
}

void Esp::Name(BoundingBox bbox, CBaseEntity* Entity) noexcept
{
	if (Menu::Get.Visuals.Name)
	{
		if (wchar_t name[128]; MultiByteToWideChar(CP_UTF8, 0, Entity->GetName(), -1, name, 128))
		{
			static int width, height;

			Surface->GetTextSize(Others[Menu::Get.Visuals.NameFont], name, width, height);

			RenderText(bbox.x0 - width * 2, bbox.y0 - 5 - height, Menu::Get.Colors.PlayerEsp.Name, Others[Menu::Get.Visuals.NameFont], name);
		}
	}
}

void Esp::Skeleton(CBaseEntity* pEntity) noexcept
{
	if(!Menu::Get.Visuals.Skeleton)
		return;

	studiohdr_t* pStudioHdr = ModelInfo->GetStudiomodel(pEntity->GetModel());

	if (!pStudioHdr)
		return;

	Vector vParent, vChild, sParent, sChild;

	for (int j = 0; j < pStudioHdr->numbones; j++)
	{
		mstudiobone_t* pBone = pStudioHdr->GetBone(j);

		if (pBone && (pBone->flags & BONE_USED_BY_HITBOX) && (pBone->parent != -1))
		{
			vChild = pEntity->GetBonePos(j);

			vParent = pEntity->GetBonePos(pBone->parent);

			if (WorldToScreen(vParent, sParent) && WorldToScreen(vChild, sChild))
			{
				Surface->DrawSetColor(Menu::Get.Colors.PlayerEsp.Skeleton);

				Surface->DrawLine(sParent[0], sParent[1], sChild[0], sChild[1]);
			}
		}
	}
}


void Nightmod()
{
	for (auto i = MatSystemOther->FirstMaterial(); i != MatSystemOther->InvalidMaterial(); i = MatSystemOther->NextMaterial(i)) {

		auto pMaterial = MatSystemOther->GetMaterial(i);

		if (!pMaterial || !pMaterial->IsPrecached())
			continue;

		if (strstr(pMaterial->GetTextureGroupName(), TEXTURE_GROUP_WORLD)) {

			float NightMod_Value = (100.f - float(Menu::Get.Visuals.NightMode)) / 100.f;

			pMaterial->ColorModulate(NightMod_Value, NightMod_Value, NightMod_Value);
		}
	}
}

CBaseEntity* RainEntity = nullptr;

void* RainNetworkable = nullptr;

void Precipitation() {

	static ClientClass* Precipitation_ClientClass = nullptr;

	static auto& g_Precipitations_Count = **(int**)(Tools::FindPattern("client.dll", "39 05 ? ? ? ? 0F 8E ? ? ? ? 53") + 2);

	if (!Menu::Get.Visuals.Weather.Active || ClientState->GetDeltaTick() == -1 || g_Precipitations_Count > 1)
	{
		if (RainEntity)
		{
			if (auto Entity = EntityList->GetClientEntity(MAX_EDICTS - 1); Entity)
				Entity->Remove();

			RainNetworkable = RainEntity = nullptr;
		}

		return;
	}

	if (!Precipitation_ClientClass)
		for (auto pClass = BaseClientDLL->GetAllClasses(); pClass && !Precipitation_ClientClass; pClass = pClass->m_pNext)
			if (pClass->m_ClassID == 87)
				Precipitation_ClientClass = pClass;


	if (!EntityList->GetClientEntity(MAX_EDICTS - 1) && Precipitation_ClientClass && Precipitation_ClientClass->m_pCreateFn) {

		RainNetworkable = Precipitation_ClientClass->m_pCreateFn(MAX_EDICTS - 1, 0);

		if (RainNetworkable) {

			RainEntity = EntityList->GetClientEntity(MAX_EDICTS - 1);

			RainEntity->GetPrecipType() = Menu::Get.Visuals.Weather.Type;

			RainEntity->PreDataUpdate(0);

			RainEntity->OnPreDataChanged(0);

			RainEntity->GetMins() = Vector(-MAX_POSITION_FLT, -MAX_POSITION_FLT, -MAX_POSITION_FLT);

			RainEntity->GetMaxs() = Vector(MAX_POSITION_FLT, MAX_POSITION_FLT, MAX_POSITION_FLT);

			RainEntity->OnDataChanged(0);

			RainEntity->PostDataUpdate(0);
		}
	}
}

void UpdateThirdPerson()
{
	static auto& m_vecCameraOffset = **(Vector**)(Tools::FindPattern("client.dll", "D8 05 ? ? ? ? D9 5F 04") + 2);

	Input->m_fCameraInThirdPerson = Menu::Get.Misc.m_vecCameraOffset;

	m_vecCameraOffset.z = static_cast<float>(Menu::Get.Misc.m_vecCameraOffset);
}

static auto GameResources() -> C_PlayerResource*
{
	static auto GameResources = reinterpret_cast<C_PlayerResource * (__cdecl*)()>(
		CallableFromRelative(Tools::FindPattern("client.dll", "E8 ? ? ? ? 8B D8 85 DB 75 07 32 C0")));

	return GameResources();
}

class INetworkStringTable;

std::vector < dlight_t* > dLights;

extern INetworkStringTable* m_pModelPrecacheTable;

char cl_downloadfilter_backup_value[64];

const char* Default_Weapons_ModelName[]
{
	"v_c4",
	"v_eq_flashbang",
	"v_eq_fraggrenade",
	"v_eq_smokegrenade",
	"v_knife_t",
	"v_mach_m249para",
	"v_pist_deagle",
	"v_pist_elite",
	"v_pist_fiveseven",
	"v_pist_glock18",
	"v_pist_p228",
	"v_pist_usp",
	"v_rif_ak47",
	"v_rif_aug",
	"v_rif_famas",
	"v_rif_galil",
	"v_rif_m4a1",
	"v_rif_sg552",
	"v_shot_m3super90",
	"v_shot_xm1014",
	"v_smg_mac10",
	"v_smg_mp5",
	"v_smg_p90",
	"v_smg_tmp",
	"v_smg_ump45",
	"v_snip_awp",
	"v_snip_g3sg1",
	"v_snip_scout",
	"v_snip_sg550"
};

inline bool IsValidID(int size, int& ID, bool reset_if_not_valid)
{
	auto RetV = ID > -1 && ID < size;

	if (!RetV && reset_if_not_valid) ID = 0;
	
	return RetV;
}

void OnLevelInit()
{
	/*if (static auto host_state_worldbrush = (worldbrushdata_t**)(*(void**)(Tools::FindPattern("engine.dll", "8B 0D ? ? ? ? 03 F6") + 0x2)); *host_state_worldbrush)
	{
		for (int i = 0; i < (*host_state_worldbrush)->numworldlights; ++i)
		{
			auto pLight = &(*host_state_worldbrush)->worldlights[i];

			auto Position = *(Vector*)((i * 88) + *(int*)((UINT)(*host_state_worldbrush) + 160));

			Color color = Menu::Get.Colors.PlayerEsp.DLights;

			dlight_t* dLight = Effects->CL_AllocDlight(i);
			dLight->key = i;
			dLight->color.r = (unsigned char)color.r();
			dLight->color.g = (unsigned char)color.g();
			dLight->color.b = (unsigned char)color.b();
			dLight->color.exponent = true;
			dLight->flags = 0;
			dLight->m_Direction = Position;
			dLight->origin = Position;
			dLight->radius = 500;
			dLight->die = Globals->curtime + 50.f;
			dLight->decay = 20.0f;

			dLights.push_back(dLight);
		}
	}*/

	void mmcopy
	(
		void* address,
		const void* value,
		size_t bytes
	);

	extern char SkyBoxName[64]; extern int SkyBoxIndex;

	if (static auto Fog_Enable(Cvar->FindVar("Fog_Enable")); Fog_Enable)
		Fog_Enable->SetMemValue(!Menu::Get.Visuals.NoFog);

	if (static auto Fog_Override(Cvar->FindVar("NewFogOverride")); Fog_Override)
		Fog_Override->SetMemValue(Menu::Get.Visuals.NoFog);

	if (static auto fog_color = Cvar->FindVar("fog_color"); fog_color)
	{
		char Buffer[64];

		snprintf(Buffer, sizeof Buffer, "%i %i %i", Menu::Get.Colors.General.Fog[0], Menu::Get.Colors.General.Fog[1], Menu::Get.Colors.General.Fog[2]);

		fog_color->SetValue(Buffer);
	}

	if (static auto fog_maxdensity = Cvar->FindVar("fog_maxdensity"); fog_maxdensity)
	{
		fog_maxdensity->SetMemValue(Menu::Get.Visuals.FOG.maxdensity);
	}

	if (r_rainwidth)
	{
		r_rainwidth->SetMemValue(Menu::Get.Visuals.Weather.Width);
	}

	if (r_rainspeed)
	{
		r_rainspeed->SetMemValue(Menu::Get.Visuals.Weather.Speed);
	}

	if (r_RainRadius)
	{
		r_RainRadius->SetMemValue(Menu::Get.Visuals.Weather.Radius);
	}

	if (r_rainlength)
	{
		r_rainlength->SetMemValue(Menu::Get.Visuals.Weather.Length);
	}

	if (snd_musicvolume)
	{
		snd_musicvolume->SetMemValue(Menu::Get.Misc.Sounds.MusicVolume * 0.01f);
	}

	[[maybe_unused]] static bool Once = [&]() {
		
		if (auto cl_downloadfilter(Cvar->FindVar("cl_downloadfilter")); cl_downloadfilter)
		{
			snprintf(cl_downloadfilter_backup_value, sizeof cl_downloadfilter_backup_value, "%s", cl_downloadfilter->GetString());

			cl_downloadfilter->SetValue("all");
		}

		if (!IsValidID(SkyBoxes.size(), SkyBoxIndex, 1)) assert(0 && "SkyBoxIndex Got Invalid Index");
		if (!IsValidID(ScopeOverlays.size(), Menu::Get.Visuals.ScopeLen, 1)) assert(0 && "ScopeLen Got Invalid Index");
		if (!IsValidID(HitSounds.size(), Menu::Get.Visuals.HitSound, 1)) assert(0 && "HitSound Got Invalid Index");
		if (!IsValidID(HitmarkerOverlays.size(), Menu::Get.Visuals.Overlay, 1)) assert(0 && "Overlay Got Invalid Index");
		if (!IsValidID(TrailMaterials.size(), Menu::Get.Visuals.Trails.SpriteTexture, 1)) assert(0 && "SpriteTexture Got Invalid Index");

		return 1;
	}();

	if (auto pLocal = GetLocalPlayerBase; pLocal)
	{
		//RefreshSprites();

		UpdateThirdPerson();

		pLocal->GetNightVisionAlpha() = pLocal->IsNightVisionOn() = Menu::Get.Colors.General.Nightvision.a();

		void UpdateScopeLens(); UpdateScopeLens();
		
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

		static std::array<float, 4> SkyBoxCurrentColor{};

		Menu::Get.Colors.General.SkyBox.GetColor(SkyBoxCurrentColor, 255.f);

		static std::array<float, 4> WorldCurrentColor{};

		Menu::Get.Colors.General.World.GetColor(WorldCurrentColor, 255.f);

		for (auto i = MatSystemOther->FirstMaterial(); i != MatSystemOther->InvalidMaterial(); i = MatSystemOther->NextMaterial(i)) {

			auto pMaterial = MatSystemOther->GetMaterial(i);

			if (!pMaterial || !pMaterial->IsPrecached())
				continue;

			if (strstr(pMaterial->GetTextureGroupName(), TEXTURE_GROUP_WORLD)) {

				pMaterial->ColorModulate(WorldCurrentColor[0], WorldCurrentColor[1], WorldCurrentColor[2]); pMaterial->AlphaModulate(WorldCurrentColor[3]);
			}

			if (strstr(pMaterial->GetTextureGroupName(), TEXTURE_GROUP_SKYBOX)) {

				pMaterial->ColorModulate(SkyBoxCurrentColor[0], SkyBoxCurrentColor[1], SkyBoxCurrentColor[2]); pMaterial->AlphaModulate(SkyBoxCurrentColor[3]);
			}
		}Nightmod();


		for (auto&& GetModelInfo : GameModels)
		{
			GetModelInfo.Modelindex.clear();

			for (auto i = 0; i < GetModelInfo.models.size(); i++)
			{
				std::string ModelPath = std::string("models/weapons/");

				if (i == 0)
				{
					for (auto Temp : Default_Weapons_ModelName)
					{
						if (strstr(Temp, GetModelInfo.weapon.c_str()))
						{
							ModelPath.append(Temp); ModelPath.append(".mdl"); break;
						}
					}
				}
				else
				{
					auto Model = GetModelInfo.models[i];

					ModelPath.append(GetModelInfo.weapon); ModelPath.append("/"); ModelPath.append(Model); ModelPath.append(".mdl");
				}

				PrecacheModel(ModelPath.c_str());

				GetModelInfo.Modelindex.push_back(ModelInfo->GetModelIndex(ModelPath.c_str()));
			}
		}
	}

	PlayerResource = GameResources();
}

void OnUnload()
{
	if (static auto HandsMat(MatSystemOther->FindMaterial("models/weapons/v_models/hands/v_hands.vtf", NULL)); HandsMat) {

		HandsMat->SetMaterialVarFlag(MATERIAL_VAR_WIREFRAME, false);

		HandsMat->SetMaterialVarFlag(MATERIAL_VAR_NO_DRAW, false);
	}

	if (sci_fi_lvl2_28) {

		sci_fi_lvl2_28->SetMaterialVarFlag(MATERIAL_VAR_WIREFRAME, false);
	}

	if (blueblacklargebeam) {

		blueblacklargebeam->SetMaterialVarFlag(MATERIAL_VAR_WIREFRAME, false);
	}

	if (static auto ItemsMat(MatSystemOther->FindMaterial("particle/particle_glow_05_add_15ob_minsize", NULL)); ItemsMat)
	{
		ItemsMat->SetMaterialVarFlag(MATERIAL_VAR_IGNOREZ, false);
	}

	if (static auto mat_fullbright(Cvar->FindVar("mat_fullbright")); mat_fullbright)
		mat_fullbright->SetMemValue(0);

	if (static auto Fog_Enable(Cvar->FindVar("Fog_Enable")); Fog_Enable)
		Fog_Enable->SetMemValue(1);

	if (static auto sv_skyname(Cvar->FindVar("sv_skyname")); sv_skyname)
		LoadSkyBox(sv_skyname->GetString());

	if (auto cl_downloadfilter(Cvar->FindVar("cl_downloadfilter")); cl_downloadfilter)
	{
		cl_downloadfilter->SetValue(cl_downloadfilter_backup_value);
	}

	if (auto CCCrosshair{ Cvar->FindVar("crosshair") }; CCCrosshair)
	{
		CCCrosshair->SetValue(1);
	}

	auto sv_cheats_command_base = Cvar->FindCommandBase("sv_cheats"); assert(sv_cheats_command_base);

	auto fog_override_command_base = Cvar->FindCommandBase("fog_override"); assert(fog_override_command_base);

	if (auto NewSvCheats = Cvar->FindVar("NewSvCheats"); NewSvCheats)
	{
		NewSvCheats->SetValue(0);
	}

	if (auto NewFogOverride = Cvar->FindVar("NewFogOverride"); NewFogOverride)
	{
		NewFogOverride->SetValue(0);
	}

	SpoofConVarName("NewSvCheats", "sv_cheats");

	if(sv_cheats_command_base) Cvar->UnregisterConCommand(sv_cheats_command_base);

	if (auto sv_cheats = Cvar->FindVar("sv_cheats"); sv_cheats)
	{
		sv_cheats->SetValue(0);

		sv_cheats->m_nFlags = (FCVAR_NOTIFY | FCVAR_REPLICATED);

		sv_cheats->SetMemValue(1.f);
	}

	SpoofConVarName("NewFogOverride", "fog_override");

	if (fog_override_command_base)Cvar->UnregisterConCommand(fog_override_command_base);

	if (auto fog_override = Cvar->FindVar("fog_override"); fog_override)
	{
		fog_override->m_nFlags = FCVAR_CHEAT | FCVAR_CLIENTDLL;
	}
	
	extern void* sv_cheats; free(sv_cheats); extern void* fog_override; free(fog_override);

	if (auto pLocal = GetLocalPlayerBase; pLocal)
	{
		pLocal->IsNightVisionOn() = false;

		pLocal->GetNightVisionAlpha() = 0.f;

		if (auto Entity = EntityList->GetClientEntity(MAX_EDICTS - 1); Entity)
			Entity->Remove();

		for (auto i = MatSystemOther->FirstMaterial(); i != MatSystemOther->InvalidMaterial(); i = MatSystemOther->NextMaterial(i)) {

			auto pMaterial = MatSystemOther->GetMaterial(i);

			if (!pMaterial || !pMaterial->IsPrecached())
				continue;

			if (strstr(pMaterial->GetTextureGroupName(), TEXTURE_GROUP_SKYBOX) || strstr(pMaterial->GetTextureGroupName(), TEXTURE_GROUP_WORLD)) {

				pMaterial->ColorModulate(1.f, 1.f, 1.f); pMaterial->AlphaModulate(1.f);
			}
		}

		if (extern CSpriteTrail* SpriteEntity; pLocal->IsAlive()) {

			for (auto& sprite : HealthSprite::SpriteEntities)
			{
				if (sprite.Entity)
				{
					sprite.Entity->Remove();
				}
			}
			
			if(SpriteEntity) SpriteEntity->Remove();
		}
	}


	extern void mmcopy
	(
		void* address,
		const void* value,
		size_t bytes
	);

	if (AddFifteen) {

		auto Protection_Backup{ TickCount };

		VirtualProtect(AddFifteen, 4, PAGE_EXECUTE_READWRITE, &Protection_Backup);

		*reinterpret_cast<std::float_t*>(AddFifteen) = 15.0f;

		VirtualProtect(AddFifteen, 4, Protection_Backup, nullptr);
	}

	char Original_Bytes[] = { 0, 255, 0, 255 };

	if(overlaycolor) mmcopy(overlaycolor, Original_Bytes, 4);

	std::remove("cstrike\\materials\\Water.vmt");
	std::remove("cstrike\\materials\\glass0.vmt");
	std::remove("cstrike\\materials\\glass1.vmt");
	std::remove("cstrike\\materials\\glass2.vmt");
	std::remove("cstrike\\materials\\concrete.vmt");

	void ForceFullUpdate(); ForceFullUpdate();

	Set_DisConnection_Msg(NULL, true);
}

DamageIndicator_t DamageIndicator::data;

void DamageIndicator::Paint(CBaseEntity* pLocal)
{
	float current_time = pLocal->GetTickBase() * Globals->interval_per_tick;

	if (data.flEraseTime < current_time) {
		return;
	}

	static int Width, Height;

	Surface->GetScreenSize(Width, Height); Width /= 2; Height /= 2;

	auto Text = GetWC((std::string("- ") + std::to_string(data.iDamage) + std::string(" HP")).c_str());

	RenderText(Width - 40 , Height - 155, Menu::Get.Colors.General.DamageIndicator, Esp::Others[1], Text);
}
