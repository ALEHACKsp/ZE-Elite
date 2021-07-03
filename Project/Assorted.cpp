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

extern std::vector<std::string> TrailMaterials;

void mmcopy
(
	void* address,
	const void* value,
	size_t bytes
);

void UTIL_TraceLine(const Vector& vecAbsStart, const Vector& vecAbsEnd, unsigned int mask, const CBaseEntity* ignore, int collisionGroup, trace_t* ptr)
{
	static auto Original_UTIL_TraceLine = reinterpret_cast<decltype(&UTIL_TraceLine)>(
		CallableFromRelative(Tools::FindPattern("client.dll", "E8 ? ? ? ? 83 C4 18 8B 06")));

	Original_UTIL_TraceLine(vecAbsStart, vecAbsEnd, mask, ignore, collisionGroup, ptr);
}

void FX_Tesla(const CTeslaInfo& teslaInfo)
{
	CBaseEntity* pEntity = GetEntity(teslaInfo.m_nEntIndex);

	// Send out beams around us
	int iNumBeamsAround = (teslaInfo.m_nBeams * 2) / 3; // (2/3 of the beams are placed around in a circle)
	int iNumRandomBeams = teslaInfo.m_nBeams - iNumBeamsAround;
	int iTotalBeams = iNumBeamsAround + iNumRandomBeams;
	float flYawOffset = RandomFloat(0, 360);
	for (int i = 0; i < iTotalBeams; i++)
	{
		// Make a couple of tries at it
		int iTries = -1;
		Vector vecForward;
		trace_t tr;
		do
		{
			iTries++;

			// Some beams are deliberatly aimed around the point, the rest are random.
			if (i < iNumBeamsAround)
			{
				QAngle vecTemp = teslaInfo.m_vAngles;
				vecTemp[YAW] += anglemod(flYawOffset + ((360 / iTotalBeams) * i));
				Math::AngleVectors(vecTemp, vecForward);

				// Randomly angle it up or down
				vecForward.z = RandomFloat(-1, 1);
			}
			else
			{
				vecForward = RandomVector(-1, 1);
			}
			VectorNormalize(vecForward);

			UTIL_TraceLine(teslaInfo.m_vPos, teslaInfo.m_vPos + (vecForward * teslaInfo.m_flRadius), MASK_SHOT, pEntity, 0, &tr);
		} while (tr.fraction >= 1.0 && iTries < 3);

		Vector vecEnd = tr.endpos - (vecForward * 8);


		static const Vector vec3_origin(0, 0, 0);
		// Only spark & glow if we hit something
		if (tr.fraction < 1.0)
		{
			static auto EffectOccluded(reinterpret_cast<bool(__cdecl*)(const Vector & pos, void*)>(
				Tools::FindPattern("client.dll", "55 8B EC 8B 4D 0C 83 EC 70")));

			if (!EffectOccluded(tr.endpos, 0))
			{
				// Move it towards the camera
				Vector vecFlash = tr.endpos;
				Vector vecForward;

				static auto MainViewAngles(reinterpret_cast<Vector(__cdecl*)()>(
					CallableFromRelative(Tools::FindPattern("client.dll", "E8 ? ? ? ? 8B 08 89 4D C8"))));

				Math::AngleVectors(MainViewAngles(), vecForward);
				vecFlash -= (vecForward * 8);

				static auto EnergySplash(reinterpret_cast<void(__thiscall*)(void*, Vector&, const Vector&, bool)>(
					(*((void***)(g_pEffects)))[7]));

				EnergySplash(g_pEffects, vecFlash, -vecForward, false);

				static auto CSimpleEmitter_Create(reinterpret_cast<void* (__cdecl*)(void*, const char*)>(
					CallableFromRelative(Tools::FindPattern("client.dll", "E8 ? ? ? ? 83 C4 40 8B 38"))));

				void* pSimple; CSimpleEmitter_Create(&pSimple, "dust");

				static auto GetPMaterial(reinterpret_cast<PVOID(__thiscall*)(void*, const char*)>(
					CallableFromRelative(Tools::FindPattern("client.dll", "E8 ? ? ? ? 8D 5F F8"))));

				static auto SetSortOrigin(reinterpret_cast<void(__thiscall*)(void*, const Vector&)>(
					CallableFromRelative(Tools::FindPattern("client.dll", "E8 ? ? ? ? 8B 07 8D 4D A0"))));

				static auto AddParticle(reinterpret_cast<void* (__thiscall*)(void*, int, PVOID, const Vector&)>(
					CallableFromRelative(Tools::FindPattern("client.dll", "E8 ? ? ? ? 5E 85 C0 74 06"))));

				SetSortOrigin(pSimple, vecFlash);

				auto pMaterial(GetPMaterial(pSimple, "effects/tesla_glow_noz"));

				SimpleParticle* pParticle = (SimpleParticle*)AddParticle(pSimple, 60, pMaterial, vecFlash);

				if (pParticle != NULL)
				{
					pParticle->m_flLifetime = 0.0f;
					pParticle->m_flDieTime = RandomFloat(0.5, 1);
					pParticle->m_vecVelocity = vec3_origin;
					Vector color(1, 1, 1);
					float  colorRamp = RandomFloat(0.75f, 1.25f);
					pParticle->m_uchColor[0] = min(1.0f, color[0] * colorRamp) * 255.0f;
					pParticle->m_uchColor[1] = min(1.0f, color[1] * colorRamp) * 255.0f;
					pParticle->m_uchColor[2] = min(1.0f, color[2] * colorRamp) * 255.0f;
					pParticle->m_uchStartSize = RandomFloat(6, 13);
					pParticle->m_uchEndSize = pParticle->m_uchStartSize - 2;
					pParticle->m_uchStartAlpha = 255;
					pParticle->m_uchEndAlpha = 10;
					pParticle->m_flRoll = RandomFloat(0, 360);
					pParticle->m_flRollDelta = 0;
				}
			}
		}

		static auto FX_BuildTesla = reinterpret_cast<void(*)(CBaseEntity * pEntity,
			const Vector & vecOrigin,
			const Vector & vecEnd,
			const char* pModelName,
			float flBeamWidth,
			const Vector & vColor,
			int nFlags,
			float flTimeVisible)>(CallableFromRelative(Tools::FindPattern("client.dll", "E8 ? ? ? ? 8B 4D CC 83 C4 20")));

		// Build the tesla
		FX_BuildTesla(
			pEntity,
			teslaInfo.m_vPos,
			tr.endpos,
			teslaInfo.m_pszSpriteName,
			teslaInfo.m_flBeamWidth,
			teslaInfo.m_vColor,
			FBEAM_ONLYNOISEONCE,
			teslaInfo.m_flTimeVisible
		);
	}
}

void DrawBeamd(Vector src, Vector end, Color color)
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

void HitMarkerOnPaint()
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

class INetworkStringTable
{
public:

	virtual                 ~INetworkStringTable(void) {};

	// Table Info
	virtual const char* GetTableName(void) const = 0;
	virtual int          GetTableId(void) const = 0;
	virtual int             GetNumStrings(void) const = 0;
	virtual int             GetMaxStrings(void) const = 0;
	virtual int             GetEntryBits(void) const = 0;

	// Networking
	virtual void            SetTick(int tick) = 0;
	virtual bool            ChangedSinceTick(int tick) const = 0;

	// Accessors (length -1 means don't change user data if string already exits)
	virtual int             AddString(bool bIsServer, const char* value, int length = -1, const void* userdata = 0) = 0;

	virtual const char* GetString(int stringNumber) = 0;
	virtual void            SetStringUserData(int stringNumber, int length, const void* userdata) = 0;
	virtual const void* GetStringUserData(int stringNumber, int* length) = 0;
	virtual int             FindStringIndex(char const* string) = 0; // returns INVALID_STRING_INDEX if not found
};

INetworkStringTable* m_pModelPrecacheTable = nullptr;

const unsigned short INVALID_STRING_INDEX = (unsigned short)-1;

bool PrecacheModel(const char* szModelName)
{
	class CNetworkStringTableContainer
	{
	public:
		INetworkStringTable* FindTable(const char* tableName)
		{
			typedef INetworkStringTable* (__thiscall* oFindTable)(PVOID, const char*);
			return Get_vFunction< oFindTable >(this, 3)(this, tableName);
		}
	};

	static auto ClientStringTableContainer = (CNetworkStringTableContainer*)Interfaces::GetInterface("engine.dll", "VEngineClientStringTable001"); assert(ClientStringTableContainer);

	if (m_pModelPrecacheTable = ClientStringTableContainer->FindTable("modelprecache"); m_pModelPrecacheTable)
	{
		m_pModelPrecacheTable->AddString(false, szModelName);
	}

	return true;
}

template<class T>
T remove_extension(T const& filename)
{
	typename T::size_type const p(filename.find_last_of('.'));
	return p > 0 && p != T::npos ? filename.substr(0, p) : filename;
}

const char* WeaponNames[]
{
	"ak47",
	"aug",
	"awp",
	"deagle",
	"elite",
	"famas",
	"fiveseven",
	"flashbang",
	"g3sg1",
	"galil",
	"glock",
	"hegrenade",
	"knife",
	"m249",
	"m3",
	"m4a1",
	"mac10",
	"mp5navy",
	"p228",
	"p90",
	"scout",
	"sg550",
	"sg552",
	"smokegrenade",
	"tmp",
	"ump45",
	"usp",
	"xm1014"
};

int FindAndReturnV(std::vector<std::string>& vector, const char* item)
{
	for (auto i = 0; i < vector.size(); i++)
	{
		if (!strcmp(vector[i].c_str(), item))
			return i;
	}

	return 0;
}

void Precache_materials_and_models() {

	HMODULE hm = NULL;

	char ThisDll[MAX_PATH];

	GetCurrentDirectory(MAX_PATH, DirectoryPath);

	if (GetModuleHandleEx(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS | GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT, (LPCSTR)&Precache_materials_and_models, &hm))
	{
		if (GetModuleFileName(hm, ThisDll, sizeof(ThisDll)))
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

					std::filesystem::create_directories(TargetParent); // Recursively create target directory if not existing.

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

								TrailMaterials.push_back(remove_extension(std::string(ThisFile)));
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
	}

	Menu::Get.Visuals.Tracers.SpriteTexture = 8;

	HitSounds.push_back(std::string("Random"));

	Menu::Get.Visuals.Trails.SpriteTexture = FindAndReturnV(TrailMaterials, "seprainbow");

	Menu::Get.Visuals.Overlay = FindAndReturnV(HitmarkerOverlays, "hitmarker3");

	Menu::Get.Visuals.HitSound = FindAndReturnV(HitSounds, "Random");
}

char NEW_Disconnect_Msg[100];

void Set_DisConnection_Msg(const char* Message, bool Reset)
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

void SetClanTag(const char* tag)
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


std::string ReplaceString(std::string subject, const std::string& search, const std::string& replace)
{
	size_t pos = 0;

	while ((pos = subject.find(search, pos)) != std::string::npos)
	{
		subject.replace(pos, search.length(), replace);
		pos += replace.length();
	}

	return subject;
}

bool IntersectRayWithOBB
(
	const Vector& vecRayStart, 
	const Vector& vecRayDelta,
	const matrix3x4& matOBBToWorld,
	const Vector& vecOBBMins,
	const Vector& vecOBBMaxs
) noexcept
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

FORCEINLINE vec_t DotProduct(const Vector& a, const Vector& b)
{
	CHECK_VALID(a);
	CHECK_VALID(b);
	return(a.x * b.x + a.y * b.y + a.z * b.z);
}

std::optional<Vector> GetIntersectionPoint(const Vector& start, const Vector& end, const Vector& mins, const Vector& maxs, float radius)
{
	auto sphereRayIntersection = [start, end, radius](auto&& center) -> std::optional<Vector>
	{
		auto direction = (end - start).Normalize();

		auto q = center - start;
		auto v = DotProduct(q, direction);
		auto d = radius * radius - (q.LengthSqr() - v * v);

		if (d < FLT_EPSILON)
			return {};

		return start + direction * (v - std::sqrt(d));
	};

	auto delta = (maxs - mins).Normalize();
	for (size_t i{}; i < std::floor(mins.DistTo(maxs)); ++i)
	{
		if (auto intersection = sphereRayIntersection(mins + delta * float(i)); intersection)
			return intersection;
	}

	if (auto intersection = sphereRayIntersection(maxs); intersection)
		return intersection;

	return {};
}

bool FindStringCS(std::string data, std::string toSearch)
{
	std::transform(data.begin(), data.end(), data.begin(), ::tolower);

	std::transform(toSearch.begin(), toSearch.end(), toSearch.begin(), ::tolower);

	return strstr(data.data(), toSearch.data());
}