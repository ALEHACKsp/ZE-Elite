#include "Offsets.h"
#include "SDK.h"


void Offsets(ClientClass* ClientClass)
{
	assert(ClientClass);

	m_flNextAttack = GetNetVarOffset("DT_BaseCombatCharacter", "m_flNextAttack", ClientClass); assert(m_flNextAttack);

	m_hMyWeapons = GetNetVarOffset("DT_BaseCombatCharacter", "m_hMyWeapons", ClientClass); assert(m_hMyWeapons);

	m_nHitboxSet = GetNetVarOffset("DT_BaseAnimating", "m_nHitboxSet", ClientClass); assert(m_nHitboxSet);

	m_hGroundEntity = GetNetVarOffset("DT_BasePlayer", "m_hGroundEntity", ClientClass); assert(m_hGroundEntity);

	m_nPrecipType = GetNetVarOffset("DT_Precipitation", "m_nPrecipType", ClientClass); assert(m_nPrecipType);

	m_nRenderMode = GetNetVarOffset("DT_BaseEntity", "m_nRenderMode", ClientClass); assert(m_nRenderMode);

	m_CollisionGroup = GetNetVarOffset("DT_BaseEntity", "m_CollisionGroup", ClientClass); assert(m_CollisionGroup);

	m_hViewModel = GetNetVarOffset("DT_BasePlayer", "m_hViewModel[0]", ClientClass); assert(m_hViewModel);

	m_Collision = GetNetVarOffset("DT_BaseEntity", "m_Collision", ClientClass); assert(m_Collision);

	m_ffEffects = GetNetVarOffset("DT_BaseEntity", "m_fEffects", ClientClass); assert(m_ffEffects);

	m_vecMins = GetNetVarOffset("DT_BaseEntity", "m_vecMins", ClientClass); assert(m_vecMins);

	m_vecMaxs = GetNetVarOffset("DT_BaseEntity", "m_vecMaxs", ClientClass); assert(m_vecMaxs);

	m_iAccount = GetNetVarOffset("DT_CSPlayer", "m_iAccount", ClientClass); assert(m_iAccount);

	m_ArmorValue = GetNetVarOffset("DT_CSPlayer", "m_ArmorValue", ClientClass); assert(m_ArmorValue);

	m_bHasHelmet = GetNetVarOffset("DT_CSPlayer", "m_bHasHelmet", ClientClass); assert(m_bHasHelmet);

	m_vecViewOffset = GetNetVarOffset("DT_BasePlayer", "m_vecViewOffset[0]", ClientClass); assert(m_vecViewOffset);

	m_nWaterLevel = GetNetVarOffset("DT_BasePlayer", "m_nWaterLevel", ClientClass); assert(m_nWaterLevel);

	m_lifeState = GetNetVarOffset("DT_BasePlayer", "m_lifeState", ClientClass); assert(m_lifeState);

	m_nTickBase = GetNetVarOffset("DT_BasePlayer", "m_nTickBase", ClientClass); assert(m_nTickBase);

	m_iFOV = GetNetVarOffset("DT_BasePlayer", "m_iFOV", ClientClass); assert(m_iFOV);

	m_iDefaultFOV = GetNetVarOffset("DT_BasePlayer", "m_iDefaultFOV", ClientClass); assert(m_iDefaultFOV);

	m_hWeapon = GetNetVarOffset("DT_BaseViewModel", "m_hWeapon", ClientClass); assert(m_hWeapon);

	m_vecPunchAngle = GetNetVarOffset("DT_BasePlayer", "m_vecPunchAngle", ClientClass); assert(m_vecPunchAngle);

	m_vecVelocity = GetNetVarOffset("DT_BasePlayer", "m_vecVelocity[0]", ClientClass); assert(m_vecVelocity);

	m_bDucking = GetNetVarOffset("DT_BasePlayer", "m_bDucking", ClientClass); assert(m_bDucking);

	m_fFlags = GetNetVarOffset("DT_BasePlayer", "m_fFlags", ClientClass); assert(m_fFlags);

	m_bDucked = GetNetVarOffset("DT_BasePlayer", "m_bDucked", ClientClass); assert(m_bDucked);

	m_flDucktime = GetNetVarOffset("DT_BasePlayer", "m_flDucktime", ClientClass); assert(m_flDucktime);

	m_iHealth = GetNetVarOffset("DT_BasePlayer", "m_iHealth", ClientClass); assert(m_iHealth);

	m_flFallVelocity = GetNetVarOffset("DT_BasePlayer", "m_flFallVelocity", ClientClass); assert(m_flFallVelocity);

	m_clrRender = GetNetVarOffset("DT_BaseEntity", "m_clrRender", ClientClass); assert(m_clrRender);

	m_iTeamNum = GetNetVarOffset("DT_BaseEntity", "m_iTeamNum", ClientClass); assert(m_iTeamNum);

	m_vecOrigin = GetNetVarOffset("DT_BaseEntity", "m_vecOrigin", ClientClass); assert(m_vecOrigin);

	m_nModelIndex = GetNetVarOffset("DT_BaseEntity", "m_nModelIndex", ClientClass); assert(m_nModelIndex);

	m_flSimulationTime = GetNetVarOffset("DT_BaseEntity", "m_flSimulationTime", ClientClass); assert(m_flSimulationTime);

	m_flFlashMaxAlpha = GetNetVarOffset("DT_CSPlayer", "m_flFlashMaxAlpha", ClientClass); assert(m_flFlashMaxAlpha);

	m_iShotsFired = GetNetVarOffset("DT_CSPlayer", "m_iShotsFired", ClientClass); assert(m_iShotsFired);

	m_bInBombZone = GetNetVarOffset("DT_CSPlayer", "m_bInBombZone", ClientClass); assert(m_bInBombZone);

	m_bFlipViewModel = GetNetVarOffset("DT_BaseCombatWeapon", "m_bFlipViewModel", ClientClass); assert(m_bFlipViewModel);

	m_iPrimaryAmmoType = GetNetVarOffset("DT_BaseCombatWeapon", "m_iPrimaryAmmoType", ClientClass); assert(m_iPrimaryAmmoType);

	m_flNextPrimaryAttack = GetNetVarOffset("DT_BaseCombatWeapon", "m_flNextPrimaryAttack", ClientClass); assert(m_flNextPrimaryAttack);

	m_iClip1 = GetNetVarOffset("DT_BaseCombatWeapon", "m_iClip1", ClientClass); assert(m_iClip1);

	m_hOwner = GetNetVarOffset("DT_BaseCombatWeapon", "m_hOwner", ClientClass); assert(m_hOwner);

	m_weaponMode = GetNetVarOffset("DT_WeaponCSBase", "m_weaponMode", ClientClass); assert(m_weaponMode);

	m_fAccuracyPenalty = GetNetVarOffset("DT_WeaponCSBase", "m_fAccuracyPenalty", ClientClass); assert(m_fAccuracyPenalty);

	m_angEyeAngles = GetNetVarOffset("DT_CSPlayer", "m_angEyeAngles[0]", ClientClass); assert(m_angEyeAngles);

	m_bIsDefusing = GetNetVarOffset("DT_CSPlayer", "m_bIsDefusing", ClientClass); assert(m_bIsDefusing);

	m_vecForce = GetNetVarOffset("DT_CSRagdoll", "m_vecForce", ClientClass); assert(m_vecForce);

	m_bNightVisionOn = GetNetVarOffset("DT_CSPlayer", "m_bNightVisionOn", ClientClass); assert(m_bNightVisionOn);

	m_vecRagdollVelocity = GetNetVarOffset("DT_CSRagdoll", "m_vecRagdollVelocity", ClientClass); assert(m_vecRagdollVelocity);

	// Addresses

	Original_SetOwnerEntity = reinterpret_cast<void(__thiscall*)(void*, void* pOwner)>(CallableFromRelative(Tools::FindPattern("client.dll", "E8 ? ? ? ? 8B 7D 24 8B 45 20")));

	Original_SetParent = reinterpret_cast<void(__thiscall*)(void*, void* pParent, int attachment)>(CallableFromRelative(Tools::FindPattern("client.dll", "E8 ? ? ? ? 83 66 7C FE")));

	Original_HierarchySetParent = reinterpret_cast<void(__thiscall*)(void*, void* pNewParent)>(Tools::FindPattern("client.dll", "83 C8 FF 8B 8E ? ? ? ? 3B C1") - 0x19);

	GetOwner = reinterpret_cast<CBaseEntity * (__thiscall*)(void*)>(
		CallableFromRelative(Tools::FindPattern("client.dll", "E8 ? ? ? ? 85 C0 75 EB"))); assert(GetOwner);

	GetEffectName = reinterpret_cast<const char* (__thiscall*)(void*)>(
		Tools::FindPattern("client.dll", "8B 49 40 85 C9 74 0B")); assert(GetEffectName);

	Original_CalcAbsoluteVelocity = reinterpret_cast<void(__thiscall*)(void*)>(
		CallableFromRelative((void*)Tools::FindPattern("client.dll", "E8 ? ? ? ? D9 86 ? ? ? ? 8B 45 0C"))); assert(Original_CalcAbsoluteVelocity);

	Original_CalcAbsolutePosition = reinterpret_cast<void(__thiscall*)(void*)>(
		Tools::FindPattern("client.dll", "55 8B EC 83 EC 64 80")); assert(Original_CalcAbsolutePosition);

	Original_GetLocalAngles = reinterpret_cast<QAngle & (__thiscall*)(void*)>(
		Tools::FindPattern("client.dll", "8D 81 CC 02 00 00")); assert(Original_GetLocalAngles);

	Original_GetLocalOrigin = reinterpret_cast<const Vector & (__thiscall*)(void*)>(
		CallableFromRelative(Tools::FindPattern("client.dll", "E8 ? ? ? ? 8B 48 04"))); assert(Original_GetLocalOrigin);

	Original_GetDebugName = reinterpret_cast<const char* (__thiscall*)(void*)>(
		Tools::FindPattern("client.dll", "53 8B D9 C6")); assert(Original_GetDebugName);

	Original_EstimateAbsVelocity = reinterpret_cast<Vector & (__thiscall*)(void*, Vector&)>(
		CallableFromRelative((void*)Tools::FindPattern("client.dll", "E8 ? ? ? ? D9 45 F4 DC C8"))); assert(Original_EstimateAbsVelocity);

	Original_SetAbsOrigin = reinterpret_cast<void(__thiscall*)(void*, const Vector&)>(
		CallableFromRelative(Tools::FindPattern("client.dll", "E8 ? ? ? ? 8D 45 C8 8B CE"))); assert(Original_SetAbsOrigin);

	Original_SetAbsAngles = reinterpret_cast<void(__thiscall*)(void*, const Vector&)>(
		CallableFromRelative(Tools::FindPattern("client.dll", "E8 ? ? ? ? 8B 4D 08 8B 01"))); assert(Original_SetAbsAngles);

	Original_SetLocalOrigin = reinterpret_cast<void(__thiscall*)(void*, const Vector&)>(
		CallableFromRelative(Tools::FindPattern("client.dll", "E8 ? ? ? ? 43 83 FB 14"))); assert(Original_SetLocalOrigin);

	Original_SetLocalAngles = reinterpret_cast<void(__thiscall*)(void*, const Vector&)>(
		CallableFromRelative(Tools::FindPattern("client.dll", "E8 ? ? ? ? 8D 45 90 8B CE"))); assert(Original_SetLocalAngles);

	Original_GetWpnData = reinterpret_cast<CCSWeaponInfo * (__thiscall*)(void*)>(
		Tools::FindPattern("client.dll", "0F B7 81 ? ? ? ? 50 E8 ? ? ? ? 83 C4 04 C3")); assert(Original_GetWpnData);

	LoadSkyBox = reinterpret_cast<bool(__cdecl*)(const char*)>(
		Tools::FindPattern("engine.dll", "55 8B EC 81 EC ? ? ? ? 8B 0D ? ? ? ? 53 56 57 8B 01")); assert(LoadSkyBox);

	blueblacklargebeam = MatSystemOther->FindMaterial("effects/blueblacklargebeam", NULL);

	sci_fi_lvl2_28 = MatSystemOther->FindMaterial("realworldtextures/walls/sci_fi_lvl2_28", NULL);

	m_flNightVisionAlpha = *(int*)(Tools::FindPattern("client.dll", "F3 0F 10 86 ? ? ? ? 74 11") + 4); assert(m_flNightVisionAlpha);

	m_MoveType = *(uintptr_t*)(Tools::FindPattern("client.dll", "80 BE ? ? ? ? ? 75 2D") + 2); assert(m_MoveType);

	m_nCreationTick = *(uintptr_t*)(Tools::FindPattern("client.dll", "B8 ? ? ? ? 57 56") + 0x16); assert(m_nCreationTick);

	overlaycolor = (void*)(Tools::FindPattern("client.dll", "C7 45 FC 00 FF 00 FF 8B") + 3); assert(overlaycolor);

	// Byte Patching

	AddFifteen = *(void**)(Tools::FindPattern("client.dll", "74 11 F3 0F 58 05 ? ? ? ?") + 6); assert(AddFifteen);

	if (AddFifteen) {

		auto Protection_Backup{ TickCount };

		VirtualProtect(AddFifteen, 4, PAGE_EXECUTE_READWRITE, &Protection_Backup);

		*reinterpret_cast<std::float_t*>(AddFifteen) = 0;

		VirtualProtect(AddFifteen, 4, Protection_Backup, nullptr);
	}

	snd_musicvolume = Cvar->FindVar("snd_musicvolume"); assert(snd_musicvolume);

	r_rainlength = Cvar->FindVar("r_rainlength"); assert(r_rainlength);

	r_RainRadius = Cvar->FindVar("r_RainRadius"); assert(r_RainRadius);

	r_rainspeed = Cvar->FindVar("r_rainspeed"); assert(r_rainspeed);

	r_rainwidth = Cvar->FindVar("r_rainwidth"); assert(r_rainwidth);
}


intptr_t m_flNextAttack{ 0 },

m_hGroundEntity{ 0 },

m_nModelIndex{ 0 },

m_hViewModel{ 0 },

m_clrRender,

m_ffEffects{ 0 },

m_nPrecipType{ 0 },

m_nRenderMode{ 0 },

m_CollisionGroup{ 0 },

m_Collision{ 0 },

m_vecMins{ 0 },

m_vecMaxs{ 0 },

m_iAccount{ 0 },

m_ArmorValue{ 0 },

m_bHasHelmet{ 0 },

m_vecViewOffset{ 0 },

m_nWaterLevel{ 0 },

m_hMyWeapons{ 0 },

m_nHitboxSet{ 0 },

m_lifeState{ 0 },

m_nCreationTick{ 0 },

m_nTickBase{ 0 },

m_vecPunchAngle{ 0 },

m_flFallVelocity{ 0 },

m_vecVelocity{ 0 },

m_bDucking{ 0 },

m_fFlags{ 0 },

m_iFOV{ 0 },

m_iDefaultFOV{ 0 },

m_bDucked{ 0 },

m_flDucktime{ 0 },

m_iTeamNum{ 0 },

m_vecOrigin{ 0 },

m_flSimulationTime{ 0 },

m_iHealth{ 0 },

m_flFlashMaxAlpha{ 0 },

m_iShotsFired{ 0 },

m_bInBombZone{ 0 },

m_iPrimaryAmmoType{ 0 },

m_flNextPrimaryAttack{ 0 },

m_iClip1{ 0 },

m_hOwner{ 0 },

m_weaponMode{ 0 },

m_fAccuracyPenalty{ 0 },

m_angEyeAngles{ 0 },

m_bIsDefusing{ 0 },

m_vecForce{ 0 },

m_vecRagdollVelocity{ 0 },

m_MoveType{ 0 },

m_bNightVisionOn{ 0 },

m_flNightVisionAlpha{ 0 },

m_bFlipViewModel{ 0 },

m_hWeapon{ 0 };


PVOID overlaycolor, AddFifteen;

IMaterialPTR blueblacklargebeam, sci_fi_lvl2_28;

ConVarPTR r_rainlength, r_RainRadius, r_rainspeed, r_rainwidth, snd_musicvolume;


CBaseEntity* (__thiscall* GetOwner)(void*);

const char* (__thiscall* GetEffectName)(void*);

void(__thiscall* Original_SetOwnerEntity)(void*, void* pOwner);

void(__thiscall* Original_SetParent)(void*, void* pParent, int attachment);

void(__thiscall* Original_HierarchySetParent)(void*, void* pNewParent);

void(__thiscall* Original_CalcAbsoluteVelocity)(void*);

void(__thiscall* Original_CalcAbsolutePosition)(void*);

QAngle& (__thiscall* Original_GetLocalAngles)(void*);

const Vector& (__thiscall* Original_GetLocalOrigin)(void*);

const char* (__thiscall* Original_GetDebugName)(void*);

Vector& (__thiscall* Original_EstimateAbsVelocity)(void*, Vector&);

void(__thiscall* Original_SetAbsOrigin)(void*, const Vector&);

void(__thiscall* Original_SetAbsAngles)(void*, const Vector&);

void(__thiscall* Original_SetLocalOrigin)(void*, const Vector&);

void(__thiscall* Original_SetLocalAngles)(void*, const Vector&);

CCSWeaponInfo* (__thiscall* Original_GetWpnData)(void*);

bool(__cdecl* LoadSkyBox)(const char*);

bool COffsets::GetOffsets()
{
	// Add signatures here
	gOffsets.KeyValues = Tools::FindPattern("engine.dll", "FF 15 ? ? ? ? 83 C4 08 89 06 8B C6"); assert(gOffsets.KeyValues);

	gOffsets.LoadFromBuffer = Tools::FindPattern("engine.dll", "55 8B EC 83 EC 38 53 8B 5D 0C"); assert(gOffsets.LoadFromBuffer);

	gOffsets.KeyValues -= 0x42;

	return false;
}

COffsets gOffsets;
