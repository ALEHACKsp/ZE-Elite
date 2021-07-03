#pragma once

#include "Vector.h"
#include "FindPattern.h"

class CCSWeaponInfo;
class CBaseEntity;
class ClientClass;
class ConVar;

void Offsets(ClientClass* ClientClass);

extern intptr_t m_flNextAttack,

m_hGroundEntity,

m_hViewModel,

m_clrRender,

m_ffEffects,

m_nPrecipType,

m_nRenderMode,

m_CollisionGroup,

m_Collision,

m_vecMins,

m_vecMaxs,

m_iAccount,

m_ArmorValue,

m_bHasHelmet,

m_vecViewOffset,

m_nWaterLevel,

m_hMyWeapons,

m_nHitboxSet,

m_lifeState,

m_nCreationTick,

m_nTickBase,

m_vecPunchAngle,

m_nModelIndex,

m_flFallVelocity,

m_vecVelocity,

m_bDucking,

m_fFlags,

m_iFOV,

m_iDefaultFOV,

m_bDucked,

m_flDucktime,

m_iTeamNum,

m_vecOrigin,

m_flSimulationTime,

m_flFlashMaxAlpha,

m_iHealth,

m_iShotsFired,

m_bInBombZone,

m_iPrimaryAmmoType,

m_flNextPrimaryAttack,

m_iClip1,

m_hOwner,

m_weaponMode,

m_fAccuracyPenalty,

m_angEyeAngles,

m_bIsDefusing,

m_vecForce,

m_vecRagdollVelocity,

m_MoveType,

m_bNightVisionOn,

m_flNightVisionAlpha,

m_bFlipViewModel,

m_hWeapon;

class IMaterial;

typedef ConVar* ConVarPTR;

typedef IMaterial* IMaterialPTR;

extern IMaterialPTR blueblacklargebeam, sci_fi_lvl2_28;

extern ConVarPTR r_rainlength, r_RainRadius, r_rainspeed, r_rainwidth, snd_musicvolume;

/*Assorted*/

extern PVOID overlaycolor, AddFifteen;

extern CBaseEntity* (__thiscall* GetOwner)(void*);

extern const char* (__thiscall* GetEffectName)(void*);

extern void(__thiscall* Original_SetOwnerEntity)(void*, void* pOwner);

extern void(__thiscall* Original_SetParent)(void*, void* pParent, int attachment);

extern void(__thiscall* Original_HierarchySetParent)(void*, void* pNewParent);

extern void(__thiscall* Original_CalcAbsoluteVelocity)(void*);

extern void(__thiscall* Original_CalcAbsolutePosition)(void*);

extern QAngle& (__thiscall* Original_GetLocalAngles)(void*);

extern const Vector& (__thiscall* Original_GetLocalOrigin)(void*);

extern const char* (__thiscall* Original_GetDebugName)(void*);

extern Vector & (__thiscall* Original_EstimateAbsVelocity)(void*, Vector&);

extern void(__thiscall* Original_SetAbsOrigin)(void*, const Vector&);

extern void(__thiscall* Original_SetAbsAngles)(void*, const Vector&);

extern void(__thiscall* Original_SetLocalOrigin)(void*, const Vector&);

extern void(__thiscall* Original_SetLocalAngles)(void*, const Vector&);

extern CCSWeaponInfo * (__thiscall* Original_GetWpnData)(void*);

extern bool(__cdecl* LoadSkyBox)(const char*);

#include <Windows.h>

#define CHECKPTR(Pointer) (gOffsets.SafetyCheck(Pointer))

class COffsets
{
public:
	int iKeyEventOffset = 20, iCreateMoveOffset = 21, iPaintTraverseOffset = 41;

	bool GetOffsets();

	DWORD LoadFromBuffer;
	DWORD KeyValues;

	void SafetyCheck(void* Pointer)
	{
		if (Pointer)
			return;

		exit(EXIT_FAILURE);
	}
	inline void SafetyCheck(DWORD Pointer)
	{
		return SafetyCheck((void*)Pointer);
	}
};
extern COffsets gOffsets;