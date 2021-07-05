#include "Assorted.h"
#include <fstream>
#include "SDK.h"
#include <filesystem>
#include "SkinChanger.h"
#include "Soundpad.h"

float _flHurtTime{ 0.f };

bool Overlay_Triggered{ false };

extern char DirectoryPath[MAX_PATH];

std::vector<std::string> HitSounds;

std::vector<SC_ModelInfo>GameModels;

void mmcopy
(
	void* address,
	const void* value,
	size_t bytes
);

void UTIL_TraceLine(const Vector& vecAbsStart, const Vector& vecAbsEnd, unsigned int mask, const CBaseEntity* ignore, int collisionGroup, trace_t* ptr) noexcept
{
	static auto Original_UTIL_TraceLine = reinterpret_cast<decltype(&UTIL_TraceLine)>(
		CallableFromRelative(Tools::FindPattern("client.dll", "E8 ? ? ? ? 83 C4 18 8B 06")));

	Original_UTIL_TraceLine(vecAbsStart, vecAbsEnd, mask, ignore, collisionGroup, ptr);
}

void DrawBeamd(Vector src, Vector end, Color color) noexcept
{
	static constexpr auto& TracersVars = Menu::Get.Visuals.Tracers;

	auto ModelName = std::string("sprites/").append(TracerSprites.at(TracersVars.SpriteTexture)).append(".vmt").c_str();

	BeamInfo_t beamInfo;
	beamInfo.m_nType = TE_BEAMPOINTS;
	beamInfo.m_pszModelName = ModelName;
	beamInfo.m_flHaloScale = TracersVars.m_flHaloScale;
	beamInfo.m_flLife = TracersVars.m_flLife;
	beamInfo.m_flWidth = TracersVars.m_flWidth;
	beamInfo.m_flEndWidth = TracersVars.m_flEndWidth;
	beamInfo.m_flFadeLength = TracersVars.m_flFadeLength;
	beamInfo.m_flAmplitude = TracersVars.m_flAmplitude;
	beamInfo.m_flBrightness = color.a();
	beamInfo.m_flSpeed = TracersVars.m_flSpeed;
	beamInfo.m_nStartFrame = 0;
	beamInfo.m_flFrameRate = 1;
	beamInfo.m_flRed = color.r();
	beamInfo.m_flGreen = color.g();
	beamInfo.m_flBlue = color.b();
	beamInfo.m_nSegments = TracersVars.m_nSegments;
	beamInfo.m_bRenderable = true;
	beamInfo.m_nFlags = TracersVars.m_nFlags;
	beamInfo.m_vecStart = src;
	beamInfo.m_vecEnd = end;

	Beam_t* myBeam = Beams->CreateBeamPoints(beamInfo);
}

void Hit_Marker_On_Paint() noexcept
{
	if (Menu::Get.General.Panic || !Menu::Get.Visuals.Hitmarker.HM_Enabled)
	{
		return;
	}
	
	auto curtime = Globals->curtime;

	const auto Length = Menu::Get.Visuals.Hitmarker.Length;

	const auto Gap = Menu::Get.Visuals.Hitmarker.Gap;

	if (_flHurtTime + static_cast<float>(Menu::Get.Visuals.Hitmarker.Time / 1000.f) >= curtime)
	{
		if (!Menu::Get.Visuals.Mode)
		{
			static int screenSizeX, screenSizeY;

			Engine->GetScreenSize(screenSizeX, screenSizeY);

			screenSizeX /= 2, screenSizeY /= 2;

			Surface->DrawSetColor(Menu::Get.Colors.General.Hitmarker);

			Surface->DrawLine(screenSizeX - (Length + Gap), screenSizeY - (Length + Gap), screenSizeX - Gap, screenSizeY - Gap);

			Surface->DrawLine(screenSizeX - (Length + Gap), screenSizeY + (Length + Gap), screenSizeX - Gap, screenSizeY + Gap);

			Surface->DrawLine(screenSizeX + (Length + Gap), screenSizeY + (Length + Gap), screenSizeX + Gap, screenSizeY + Gap);

			Surface->DrawLine(screenSizeX + (Length + Gap), screenSizeY - (Length + Gap), screenSizeX + Gap, screenSizeY - Gap);
		}

	}
	else
	{
		if (Menu::Get.Visuals.Mode && Overlay_Triggered) {

			Engine->ClientCmd_Unrestricted("r_screenoverlay \"\"");

			Overlay_Triggered = !Overlay_Triggered;
		}
	}
}

bool PrecacheModel(const char* szModelName) noexcept
{
	static auto ClientStringTableContainer = Interfaces::GetInterface("engine.dll", "VEngineClientStringTable001"); assert(ClientStringTableContainer);

	static auto FindTable = Get_vFunction<void* (__thiscall*)(void*, const char*) >(ClientStringTableContainer, 3);

	if (void* m_pModelPrecacheTable = FindTable(ClientStringTableContainer, "modelprecache"); m_pModelPrecacheTable)
	{
		static auto AddString = Get_vFunction<int(__thiscall*)(void*, bool, const char*, int, const void*) >(m_pModelPrecacheTable, 8);

		AddString(m_pModelPrecacheTable, false, szModelName, -1, 0);
	}

	return true;
}

int FindAndReturnV(std::vector<std::string>& vector, const char* item) noexcept
{
	for (auto i = 0; i < vector.size(); i++)
	{
		if (!strcmp(vector[i].c_str(), item))
			return i;
	}

	return 0;
}

void Precache_Custom_Stuff(HMODULE DLL) noexcept {

	char ThisDll[MAX_PATH];

	if (GetCurrentDirectory(MAX_PATH, DirectoryPath) && GetModuleFileName(DLL, ThisDll, sizeof(ThisDll)))
	{
		char MyDll[MAX_PATH];

		for (auto i = strlen(ThisDll); i > 0; i--)
		{
			if (ThisDll[i] == '\\' || ThisDll[i] == '/')
			{
				snprintf(MyDll, i + 1, "%s", ThisDll);

				std::filesystem::path SourceFile = std::string(MyDll).append("\\cstrike");

				if (BOOL DirectoryExists(LPCTSTR szPath); !DirectoryExists(std::string(MyDll).append("\\cstrike").c_str()))
				{
					ColoredConMsg(Color::Red(), "[ERROR]: cstrike is not in same direction"); break;
				}

				std::filesystem::path TargetParent = DirectoryPath;

				auto target = TargetParent / SourceFile.filename();

				std::filesystem::create_directories(TargetParent);

				std::filesystem::copy(SourceFile, target, std::filesystem::copy_options::skip_existing | std::filesystem::copy_options::recursive);

				std::string Trials_Mat_Path = std::string(DirectoryPath).append("\\cstrike\\materials\\sprites\\trails");

				for (const auto& entry : std::filesystem::directory_iterator(Trials_Mat_Path))
				{
					auto FilePath = entry.path().string();

					if (auto FPath = FilePath.c_str(); strstr(FPath, ".vmt"))
					{
						if (auto Found = FilePath.find_last_of("\\"); Found != std::string::npos)
						{
							char ThisFile[64];

							snprintf(ThisFile, sizeof ThisFile, "%s", (const char*)&FilePath[Found + 1]);

							constexpr auto RemoveExtension = [](const char* FileName)
							{
								const std::string filename = FileName;

								const size_t p = filename.find_last_of('.');

								return p > 0 && p != std::string::npos ? filename.substr(0, p) : filename;
							};

							TrailMaterials.push_back(RemoveExtension(ThisFile));
						}
					}
				}

				std::string Skyboxs_Mat_Path = std::string(DirectoryPath).append("\\cstrike\\materials\\skybox");

				for (const auto& entry : std::filesystem::directory_iterator(Skyboxs_Mat_Path))
				{
					auto FilePath = entry.path().string();

					if (auto FPath = FilePath.c_str(); strstr(FPath, "bk.vmt"))
					{
						if (auto Found = FilePath.find_last_of("\\"); Found != std::string::npos)
						{
							char ThisFile[64];

							snprintf(ThisFile, sizeof ThisFile, "%s", (const char*)&FilePath[Found + 1]);

							if (auto Found = std::string(ThisFile).find("bk.vmt"); Found != std::string::npos)
							{
								char Skybox[64];

								snprintf(Skybox, Found + 1, "%s", ThisFile);

								SkyBoxes.push_back(Skybox);
							}
						}
					}
				}

				std::string Models_Path = std::string(DirectoryPath).append("\\cstrike\\models\\weapons");

				for (const auto& entry : std::filesystem::directory_iterator(Models_Path))
				{
					auto FilePath = entry.path().string();

					if (std::filesystem::status(FilePath).type() == std::filesystem::file_type::directory)
					{
						char WeaponName[64];

						if (auto Found = FilePath.find_last_of("\\"); Found != std::string::npos)
						{
							snprintf(WeaponName, sizeof WeaponName, "%s", (const char*)&FilePath[Found + 1]);

							GameModels.push_back(SC_ModelInfo(WeaponName));

							GameModels[GameModels.size() - 1].models.push_back("Default");
						}

						for (const auto& entry : std::filesystem::directory_iterator(FilePath))
						{
							auto FilePath = entry.path().string();

							if (auto Found = FilePath.find_last_of("\\"); Found != std::string::npos)
							{
								char ThisFile[64];

								snprintf(ThisFile, sizeof ThisFile, "%s", (const char*)&FilePath[Found + 1]);

								if (auto Found = std::string(ThisFile).find(".mdl"); Found != std::string::npos)
								{
									char model[64];

									snprintf(model, Found + 1, "%s", ThisFile);

									GameModels[GameModels.size() - 1].models.push_back(model);
								}
							}
						}
					}
				}

				std::string Scopes_Mat_Path = std::string(DirectoryPath).append("\\cstrike\\materials\\overlays\\Scopes");

				ScopeOverlays.push_back(std::string("Default"));

				for (const auto& entry : std::filesystem::directory_iterator(Scopes_Mat_Path))
				{
					auto FilePath = entry.path().string();

					if (auto FPath = FilePath.c_str(); strstr(FPath, ".vmt"))
					{
						if (auto Found = FilePath.find_last_of("\\"); Found != std::string::npos)
						{
							char ThisFile[64];

							snprintf(ThisFile, sizeof ThisFile, "%s", (const char*)&FilePath[Found + 1]);

							if (auto Found = std::string(ThisFile).find(".vmt"); Found != std::string::npos)
							{
								char Scope[64];

								snprintf(Scope, Found + 1, "%s", ThisFile);

								ScopeOverlays.push_back(Scope);
							}
						}
					}
				}

				static auto Audio_GetWaveDuration = reinterpret_cast<float(__cdecl*)(const char*)>(
					Tools::FindPattern("engine.dll", "55 8B EC 81 EC ? ? ? ? 56 8B 75 08 8D 4D D4"));

				std::string Sounds_Path = std::string(DirectoryPath).append("\\cstrike\\sound\\ZE-Elite");

				for (const auto& entry : std::filesystem::directory_iterator(Sounds_Path))
				{
					auto FilePath = entry.path().string();

					if (auto FPath = FilePath.c_str(); strstr(FPath, ".wav"))
					{
						if (auto Found = FilePath.find_last_of("\\"); Found != std::string::npos)
						{
							char ThisFile[64];

							snprintf(ThisFile, sizeof ThisFile, "%s", (const char*)&FilePath[Found + 1]);

							if (auto Found = std::string(ThisFile).find(".wav"); Found != std::string::npos)
							{
								char Sound[64];

								snprintf(Sound, Found + 1, "%s", ThisFile);

								SoundList.push_back({});

								auto Duration = Audio_GetWaveDuration(std::string("ZE-Elite\\").append(ThisFile).c_str());

								SoundList[SoundList.size() - 1].Push(Sound, 0, Duration);
							}
						}
					}
				}

				std::string Hitmarker_Overlays_Path = std::string(DirectoryPath).append("\\cstrike\\materials\\overlays\\hitmarkers");

				for (const auto& entry : std::filesystem::directory_iterator(Hitmarker_Overlays_Path))
				{
					auto FilePath = entry.path().string();

					if (auto FPath = FilePath.c_str(); strstr(FPath, ".vmt"))
					{
						if (auto Found = FilePath.find_last_of("\\"); Found != std::string::npos)
						{
							char ThisFile[64];

							snprintf(ThisFile, sizeof ThisFile, "%s", (const char*)&FilePath[Found + 1]);

							if (auto Found = std::string(ThisFile).find(".vmt"); Found != std::string::npos)
							{
								char Overlay[64];

								snprintf(Overlay, Found + 1, "%s", ThisFile);

								HitmarkerOverlays.push_back(Overlay);
							}
						}
					}
				}

				HitSounds.push_back("Off");

				std::string Hitmarker_Sounds_Path = std::string(DirectoryPath).append("\\cstrike\\sound\\HitSounds");

				for (const auto& entry : std::filesystem::directory_iterator(Hitmarker_Sounds_Path))
				{
					auto FilePath = entry.path().string();

					if (auto FPath = FilePath.c_str(); strstr(FPath, ".wav"))
					{
						if (auto Found = FilePath.find_last_of("\\"); Found != std::string::npos)
						{
							char ThisFile[64];

							snprintf(ThisFile, sizeof ThisFile, "%s", (const char*)&FilePath[Found + 1]);

							if (auto Found = std::string(ThisFile).find(".wav"); Found != std::string::npos)
							{
								char Sound[64];

								snprintf(Sound, Found + 1, "%s", ThisFile);

								HitSounds.push_back(Sound);
							}
						}
					}
				}

				break;
			}
		}
	}

	Menu::Get.Visuals.Tracers.SpriteTexture = 8;

	HitSounds.push_back(std::string("Random"));

	Menu::Get.Visuals.Trails.SpriteTexture = FindAndReturnV(TrailMaterials, "seprainbow");

	Menu::Get.Visuals.Overlay = FindAndReturnV(HitmarkerOverlays, "hitmarker3");

	Menu::Get.Visuals.HitSound = FindAndReturnV(HitSounds, "Random");
}

char NEW_Disconnect_Msg[100];

void Set_DisConnection_Msg(const char* Message, bool Reset) noexcept
{
	static auto Location = reinterpret_cast<BYTE*>(
		Tools::FindPattern("engine.dll", "B9 ? ? ? ? 8B 02") + 1);

	static BYTE Backup_Bytes[4] = { Location[0], Location[1], Location[2], Location[3] };

	[[maybe_unused]] static bool Once = []() {

		static auto Protection_Backup{ TickCount };

		VirtualProtect(Location, sizeof(void*), PAGE_EXECUTE_READWRITE, &Protection_Backup);

		*(void**)Location = &NEW_Disconnect_Msg;

		VirtualProtect(Location, sizeof(void*), Protection_Backup, nullptr);

		return 1;
	}();

	if (Reset)
	{
		mmcopy(Location, Backup_Bytes, 4);
	}
	else
	{
		snprintf(NEW_Disconnect_Msg, sizeof NEW_Disconnect_Msg, "%s", Message);
	}
}

void SetClanTag(const char* tag) noexcept
{
	static auto Kv_Init_Name = reinterpret_cast<void* (__thiscall*)(void*, const char*)>(
		CallableFromRelative(Tools::FindPattern("engine.dll", "E8 ? ? ? ? 8B F8 EB 02 33 FF 53")));

	static auto Kv_SetString = reinterpret_cast<void* (__thiscall*)(void*, const char*, const char*)>(
		CallableFromRelative(Tools::FindPattern("engine.dll", "E8 ? ? ? ? 8B 8F ? ? ? ? FF 77 0C")));

	static auto Kv_Alloc = reinterpret_cast<void* (_cdecl*)(int)>(
		CallableFromRelative(Tools::FindPattern("engine.dll", "E8 ? ? ? ? 83 C4 04 85 C0 0F 84 ? ? ? ? 56")));

	const auto v9 = Kv_Alloc(32);

	if (v9)
	{
		const auto v10 = Kv_Init_Name(v9, "ClanTagChanged");

		if (v10)
		{
			Kv_SetString(v10, "tag", tag);

			Get_vFunction<void(__thiscall*)(void*, void*)>(Engine, 127)(Engine, v10);
		}
	}
}


std::string ReplaceString(std::string subject, const std::string& search, const std::string& replace) noexcept
{
	size_t pos = 0;

	while ((pos = subject.find(search, pos)) != std::string::npos)
	{
		subject.replace(pos, search.length(), replace);
		pos += replace.length();
	}

	return subject;
}

bool IntersectRayWithOBB(const Vector& vecRayStart, const Vector& vecRayDelta, const matrix3x4& matOBBToWorld, const Vector& vecOBBMins, const Vector& vecOBBMaxs) noexcept
{


	// OPTIMIZE: Store this in the box instead of computing it here
	// compute center in local space
	Vector vecBoxExtents = (vecOBBMins + vecOBBMaxs) * 0.5;
	Vector vecBoxCenter;


	// transform to world space
	Math::VectorTransform(vecBoxExtents, matOBBToWorld, vecBoxCenter);


	// calc extents from local center
	vecBoxExtents = vecOBBMaxs - vecBoxExtents;


	// OPTIMIZE: This is optimized for world space.  If the transform is fast enough, it may make more
	// sense to just xform and call UTIL_ClipToBox() instead.  MEASURE THIS.


	// save the extents of the ray along 
	Vector extent, uextent;
	Vector segmentCenter = vecRayStart + vecRayDelta - vecBoxCenter;


	extent.x = extent.y = extent.z = 0.0f;


	// check box axes for separation
	for (int j = 0; j < 3; j++)
	{
		extent[j] = vecRayDelta.x * matOBBToWorld[0][j] + vecRayDelta.y * matOBBToWorld[1][j] + vecRayDelta.z * matOBBToWorld[2][j];
		uextent[j] = fabsf(extent[j]);
		float coord = segmentCenter.x * matOBBToWorld[0][j] + segmentCenter.y * matOBBToWorld[1][j] + segmentCenter.z * matOBBToWorld[2][j];
		coord = fabsf(coord);


		if (coord > (vecBoxExtents[j] + uextent[j]))
			return false;
	}


	// now check cross axes for separation

	float tmp, cextent;

	Vector cross;

	cross.Cross(&vecRayDelta, &segmentCenter);


	cextent = cross.x * matOBBToWorld[0][0] + cross.y * matOBBToWorld[1][0] + cross.z * matOBBToWorld[2][0];
	cextent = fabsf(cextent);
	tmp = vecBoxExtents[1] * uextent[2] + vecBoxExtents[2] * uextent[1];
	if (cextent > tmp)
		return false;


	cextent = cross.x * matOBBToWorld[0][1] + cross.y * matOBBToWorld[1][1] + cross.z * matOBBToWorld[2][1];
	cextent = fabsf(cextent);
	tmp = vecBoxExtents[0] * uextent[2] + vecBoxExtents[2] * uextent[0];
	if (cextent > tmp)
		return false;


	cextent = cross.x * matOBBToWorld[0][2] + cross.y * matOBBToWorld[1][2] + cross.z * matOBBToWorld[2][2];
	cextent = fabsf(cextent);
	tmp = vecBoxExtents[0] * uextent[1] + vecBoxExtents[1] * uextent[0];
	if (cextent > tmp)
		return false;


	return true;
}

bool FindStringCS(std::string data, std::string toSearch) noexcept
{
	std::transform(data.begin(), data.end(), data.begin(), ::tolower);

	std::transform(toSearch.begin(), toSearch.end(), toSearch.begin(), ::tolower);

	return strstr(data.data(), toSearch.data());
}

char NewScopePath[MAX_PATH]; char Backup_Of_Scope_Len_Bytes[4]; void UpdateScopeLens()
{
	static auto CHudScope_Init = reinterpret_cast<void(__thiscall*)(void*)>(
		Tools::FindPattern("client.dll", "56 8B F1 8B 0D ? ? ? ? 6A 00 8B 01"));

	static auto Scope_Len_path = reinterpret_cast<BYTE*>(
		Tools::FindPattern("client.dll", "89 46 3C 8B 0D ? ? ? ?") + 0xE);

	if (Scope_Len_path && CHudScope_Init) {

		[[maybe_unused]] static auto Once = []() -> bool
		{
			mmcopy(Backup_Of_Scope_Len_Bytes, Scope_Len_path, 4);

			return true;
		}();

		if (Menu::Get.Visuals.ScopeLen == 0)
		{
			mmcopy(Scope_Len_path, Backup_Of_Scope_Len_Bytes, 4);
		}
		else
		{
			snprintf(NewScopePath, sizeof NewScopePath, "%s", (std::string("overlays/Scopes/") + ScopeOverlays[Menu::Get.Visuals.ScopeLen]).c_str());

			static auto Protection_Backup{ TickCount };

			VirtualProtect(Scope_Len_path, sizeof(void*), PAGE_EXECUTE_READWRITE, &Protection_Backup);

			*(void**)Scope_Len_path = &NewScopePath;

			VirtualProtect(Scope_Len_path, sizeof(void*), Protection_Backup, nullptr);
		}

		if (HudScope) CHudScope_Init(HudScope);

		mmcopy(Scope_Len_path, Backup_Of_Scope_Len_Bytes, 4);
	}
}
