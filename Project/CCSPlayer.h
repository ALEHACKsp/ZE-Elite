#pragma once
#include "SDK.h"
#include <math.h>
#include "Vector.h"
#include "FindPattern.h"
#include "Offsets.h"

class CBaseCombatWeapon;

class CCSWeaponInfo;

class ICollideable;

class CBaseEntity
{
public:
#include "Inconstructible.h"

	INCONSTRUCTIBLE(CBaseEntity);

	DECLARE_OFFSET_FUNCTION(GetFlags, m_fFlags, uintptr_t);
	DECLARE_OFFSET_FUNCTION(GetMyWeapons, m_hMyWeapons, uintptr_t[MAX_WEAPONS]);
	DECLARE_OFFSET_FUNCTION(GetPrecipType, m_nPrecipType, uintptr_t);
	DECLARE_OFFSET_FUNCTION(GetRenderMode, m_nRenderMode, std::uint8_t);
	DECLARE_OFFSET_FUNCTION(GetShotsFired, m_iShotsFired, uintptr_t);
	DECLARE_OFFSET_FUNCTION(GetPunchAngle, m_vecPunchAngle, Vector);
	DECLARE_OFFSET_FUNCTION(GetWaterLevel, m_nWaterLevel, char);
	DECLARE_OFFSET_FUNCTION(GetCreationTick, m_nCreationTick, std::int32_t);
	DECLARE_OFFSET_FUNCTION(GetTickBase, m_nTickBase, uintptr_t);
	DECLARE_OFFSET_FUNCTION(GetViewModel, m_hViewModel, void*);
	DECLARE_OFFSET_FUNCTION(GetMoveType, m_MoveType, char);
	DECLARE_OFFSET_FUNCTION(GetVelocity, m_vecVelocity, Vector);
	DECLARE_OFFSET_FUNCTION(GetHitboxSet, m_nHitboxSet, int);
	DECLARE_OFFSET_FUNCTION(GetModelIndex, m_nModelIndex, int);
	DECLARE_OFFSET_FUNCTION(GetTeamNum, m_iTeamNum, uintptr_t);
	DECLARE_OFFSET_FUNCTION(GetNetworkMoveParent, 0x190, int);
	DECLARE_OFFSET_FUNCTION(GetAccount, m_iAccount, int);
	DECLARE_OFFSET_FUNCTION(GetArmorValue, m_ArmorValue, int);
	DECLARE_OFFSET_FUNCTION(HasHelmet, m_bHasHelmet, bool);
	DECLARE_OFFSET_FUNCTION(GetFlashMaxAlpha, m_flFlashMaxAlpha, float);
	DECLARE_OFFSET_FUNCTION(GetVecViewOffset, m_vecViewOffset, Vector);
	DECLARE_OFFSET_FUNCTION(GetNextAttack, m_flNextAttack, float);
	DECLARE_OFFSET_FUNCTION(GetVecOrigin, m_vecOrigin, Vector);
	DECLARE_OFFSET_FUNCTION(GetEyeAngles, m_angEyeAngles, Vector);
	DECLARE_OFFSET_FUNCTION(GetRenderColor, m_clrRender, Color);
	DECLARE_OFFSET_FUNCTION(GetOwner, m_hOwner, uint8_t);
	DECLARE_OFFSET_FUNCTION(GetMins, m_vecMins, Vector&);
	DECLARE_OFFSET_FUNCTION(GetMaxs, m_vecMaxs, Vector&);
	DECLARE_OFFSET_FUNCTION(GetEffects, m_ffEffects, uint32_t);
	DECLARE_OFFSET_FUNCTION(GetWeapon, m_hWeapon, void*);
	DECLARE_OFFSET_FUNCTION(IsNightVisionOn, m_bNightVisionOn, uint32_t);
	DECLARE_OFFSET_FUNCTION(GetNightVisionAlpha, m_flNightVisionAlpha, float);

	const char* GetName()
	{
		static player_info_t Info;

		static const char* Unknown{ "" };

		if (Engine->GetPlayerInfo(GetIndex(), &Info))
		{
			return Info.name;
		}

		return Unknown;
	}

	void FollowEntity(void* pEntity, int attachment = 1)
	{
		SetParent(pEntity, attachment);

		GetEffects() |= EF_BONEMERGE;

		GetMoveType() == MOVETYPE_NONE;
	}

	void SetModelByIndex(int index)
	{
		static auto SetModelByIndex =  reinterpret_cast<void(__thiscall*)(void*, int)>(
			CallableFromRelative(Tools::FindPattern("client.dll", "E8 ? ? ? ? 83 7D 08 00 5B"))); 

		assert(SetModelByIndex); if(SetModelByIndex) SetModelByIndex(this, index);
	}

	const Vector GetEyePosition()
	{
		return GetVecViewOffset() + GetVecOrigin();
	}

	const bool OnGround()
	{
		return GetFlags() & FL_ONGROUND;
	}

	const bool InAir()
	{
		return !OnGround();
	}

	const bool IsDead()
	{
		return !IsAlive();
	}

	const bool IsEnemy(CBaseEntity* Entity)
	{
		return GetTeamNum() NotEquals Entity->GetTeamNum();
	}

	int GetHealth()
	{
		return  (Get_vFunction<int(__thiscall*)(void*)>(this, 106)(this));
	}

	int GetMaxHealth()
	{
		return  (Get_vFunction<int(__thiscall*)(void*)>(this, 107)(this));
	}

	void GetRenderBounds(Vector& mins, Vector& maxs)
	{
		void* pThis = (void*)(this + IClientRenderable);

		return  Get_vFunction<void(__thiscall*)(void*, Vector&, Vector&)>(pThis, 20)(pThis, mins, maxs);
	}

	const Vector& WorldSpaceCenter()
	{
		return Get_vFunction<Vector& (__thiscall*)(void*)>(this, 66)(this);
	}

	const Vector& GetRenderOrigin()
	{
		void* pThis = (void*)(this + IClientRenderable);

		return  Get_vFunction<const Vector& (__thiscall*)(void*)>(pThis, 1)(pThis);
	}

	matrix3x4& GetRgflCoordinateFrame()
	{
		void* pThis = (void*)(this + IClientRenderable);

		return Get_vFunction<matrix3x4& (__thiscall*)(void*)>(pThis, 34)(pThis);
	}

	bool SetupBones(matrix3x4* pBoneToWorldOut, int nMaxBones, int boneMask, float currentTime)
	{
		void* pThis = (void*)(this + IClientRenderable);

		return  Get_vFunction<bool(__thiscall*)(void*, matrix3x4*, int, int, float)>(pThis, 16)
			(pThis, pBoneToWorldOut, nMaxBones, boneMask, currentTime);
	}


	CBaseEntity* GetRenderedWeaponModel()
	{
		return  Get_vFunction<CBaseEntity* (__thiscall*)(void*)>(this, 243)(this);
	}

	const class model_t* GetModel()
	{
		void* pThis = (void*)(this + IClientRenderable);

		return  Get_vFunction<const class model_t* (__thiscall*)(void*)>(pThis, 9)(pThis);
	}

	const int GetFxBlend()
	{
		void* pThis = (void*)(this + IClientRenderable);

		return  Get_vFunction<int(__thiscall*)(void*)>(pThis, 13)(pThis);
	}

	void GetColorModulation(float* color)
	{
		void* pThis = (void*)(this + IClientRenderable);

		return  Get_vFunction<void(__thiscall*)(void*, float*)>(pThis, 14)(pThis, color);
	}

	int GetIndex()
	{
		void* pThis = (void*)(this + IClientNetworkable);

		return  Get_vFunction<int(__thiscall*)(void*)>(pThis, 9)(pThis);
	}

	ClientClass* GetClientClass()
	{
		void* pThis = (void*)(this + IClientNetworkable);

		return  Get_vFunction<ClientClass* (__thiscall*)(void*)>(pThis, 2)(pThis);
	}

	bool IsDormant()
	{
		void* pThis = (void*)(this + IClientNetworkable);

		return  Get_vFunction<bool(__thiscall*)(void*)>(pThis, 8)(pThis);
	}

	void OnPreDataChanged(int updateType)
	{
		void* pThis = (void*)(this + IClientNetworkable);

		return  Get_vFunction<void(__thiscall*)(void*, int)>(pThis, 4)(pThis, updateType);
	}

	void OnDataChanged(int updateType)
	{
		void* pThis = (void*)(this + IClientNetworkable);

		return  Get_vFunction<void(__thiscall*)(void*, int)>(pThis, 5)(pThis, updateType);
	}

	void PreDataUpdate(int updateType)
	{
		void* pThis = (void*)(this + IClientNetworkable);

		return  Get_vFunction<void(__thiscall*)(void*, int)>(pThis, 6)(pThis, updateType);
	}

	void PostDataUpdate(int updateType)
	{
		void* pThis = (void*)(this + IClientNetworkable);

		return  Get_vFunction<void(__thiscall*)(void*, int)>(pThis, 7)(pThis, updateType);
	}

	int	LookupAttachment(const char* pAttachmentName)
	{
		void* pThis = (void*)(this + IClientRenderable);

		return  Get_vFunction<int(__thiscall*)(void*, const char*)>(pThis, 35)(pThis, pAttachmentName);
	}

	bool GetAttachment(int number, Vector& origin, QAngle& angles)
	{
		void* pThis = (void*)(this + IClientRenderable);

		return  Get_vFunction<bool(__thiscall*)(void*, int, Vector&, Vector&)>(pThis, 37)(pThis, number, origin, angles);
	}

	void ClientThink()
	{
		auto pThis = (void*)(this + IClientThinkable);

		Get_vFunction<void(__thiscall*)(void*)>(pThis, 1)(pThis);
	}

	void Release()
	{
		auto pThis = (void*)(this + IClientThinkable);

		Get_vFunction<void(__thiscall*)(void*)>(pThis, 4)(pThis);
	}

	bool IsPlayer()
	{
		return  Get_vFunction<bool(__thiscall*)(void*)>(this, 131)(this);
	}

	bool IsAlive()
	{
		return  Get_vFunction<bool(__thiscall*)(void*)>(this, 130)(this);
	}

	ICollideable* GetCollideable()
	{
		return Get_vFunction<ICollideable* (__thiscall*)(void*)>(this, 3)(this);
	}

	const Vector& GetAbsAngles()
	{
		return Get_vFunction<const Vector& (__thiscall*)(void*)>(this, 10)(this);
	}

	const Vector& GetAbsOrigin()
	{
		return  Get_vFunction<const Vector& (__thiscall*)(void*)>(this, 9)(this);
	}

	CBaseCombatWeapon* GetActiveWeapon()
	{
		return Get_vFunction<CBaseCombatWeapon* (__thiscall*)(void*)>(this, 222)(this);
	}

	void SetLocalViewAngles(const QAngle& viewAngles)
	{
		Get_vFunction<void(__thiscall*)(void*, const QAngle&)>(this, 293)(this, viewAngles);
	}

	void CalcAbsoluteVelocity()
	{
		Original_CalcAbsoluteVelocity(this);
	}

	void SetParent(void* pParent,int attachment)
	{
		Original_SetParent(this, pParent, attachment);
	}

	void HierarchySetParent(void* pParent)
	{
		Original_HierarchySetParent(this, pParent);
	}

	void SetOwnerEntity(void* pOwner)
	{
		Original_SetOwnerEntity(this, pOwner);
	}

	void CalcAbsolutePosition()
	{
		Original_CalcAbsolutePosition(this);
	}

	QAngle& GetLocalAngles()
	{
		return Original_GetLocalAngles(this);
	}

	const Vector& GetLocalOrigin()
	{
		return Original_GetLocalOrigin(this);
	}

	const char* GetDebugName()
	{
		return Original_GetDebugName(this);
	}

	Vector& EstimateAbsVelocity()
	{
		static auto Veloctiy{ Vector(0,0,0) };

		Original_EstimateAbsVelocity(this, Veloctiy);

		return Veloctiy;
	}

	void SetAbsOrigin(const Vector& vecOrigin)
	{
		Original_SetAbsOrigin(this, vecOrigin);
	}

	void SetAbsAngles(const Vector& vecAngles)
	{
		Original_SetAbsAngles(this, vecAngles);
	}

	void SetLocalOrigin(const Vector& vecOrigin)
	{
		Original_SetLocalOrigin(this, vecOrigin);
	}

	void SetLocalAngles(const Vector& vecAngles)
	{
		Original_SetLocalAngles(this, vecAngles);
	}

	void UpdateButtonState(int nUserCmdButtonMask)
	{
		static auto UpdateButtonState = reinterpret_cast<void(__thiscall*)(void*, int)>(
			CallableFromRelative(Tools::FindPattern("client.dll", "E8 ? ? ? ? 8B 17 8D 43 0C")));

		UpdateButtonState(this, nUserCmdButtonMask);
	}

	void Remove()
	{
		static auto Remove = reinterpret_cast<void(__thiscall*)(void*)>(
			Tools::FindPattern("client.dll", "56 8B F1 83 BE ? ? ? ? ? 7E 1A"));

		Remove(this);
	}

	Vector GetBonePos(int i)
	{
		matrix3x4 boneMatrix[128];

		if (this->SetupBones(boneMatrix, 128, 0x00000100, GetTickCount64()))
		{
			return Vector(boneMatrix[i][0][3], boneMatrix[i][1][3], boneMatrix[i][2][3]);
		}
		return Vector(0, 0, 0);
	}

	bool CanSeePlayer(CBaseEntity* player, const Vector& pos);

	const Vector GetHitboxPosition(int iHitbox);

	/*fogparams_t* GetFogParams()
	{ 
		return reinterpret_cast<fogparams_t*>(UINT32(this) + 0xD90);
	}*/

	static CBaseEntity* GetLocalPlayer()
	{
		static auto& LocalPlayer = **reinterpret_cast<CBaseEntity***>(Tools::FindPattern("client.dll", "39 05 ? ? ? ? 75 0B") + 2);

		return LocalPlayer;
	}
};


class CBaseCombatWeapon : public CBaseEntity
{
public:
	DECLARE_OFFSET_FUNCTION(GetNextThinkTick, 0x88, uintptr_t);
	DECLARE_OFFSET_FUNCTION(GetOwner, 0x870, int);
	DECLARE_OFFSET_FUNCTION(GetState, 0x8AC, uintptr_t);
	DECLARE_OFFSET_FUNCTION(GetViewModelIndex, 0x888, uintptr_t);
	DECLARE_OFFSET_FUNCTION(GetWorldModelIndex, 0x88C, uintptr_t);
	DECLARE_OFFSET_FUNCTION(GetNextPrimaryAttack, 0x878, float);
	DECLARE_OFFSET_FUNCTION(GetNextSecondaryAttack, 0x87C, float);
	DECLARE_OFFSET_FUNCTION(GetTimeWeaponIdle, 0x880, float);
	DECLARE_OFFSET_FUNCTION(GetPrimaryAmmoType, 0x8B4, uintptr_t);
	DECLARE_OFFSET_FUNCTION(GetSecondaryAmmoType, 0x8B8, uintptr_t);
	DECLARE_OFFSET_FUNCTION(GetClip1, 0x8BC, uintptr_t);
	DECLARE_OFFSET_FUNCTION(GetClip2, 0x8C0, uintptr_t);
	DECLARE_OFFSET_FUNCTION(IsInReload, 0x884, bool);
	DECLARE_OFFSET_FUNCTION(IsFireOnEmpty, 0x885, bool);
	DECLARE_OFFSET_FUNCTION(IsFiringWholeClip, 0x886, bool);
	DECLARE_OFFSET_FUNCTION(GetNextEmptySoundTime, 0x890, float);
	DECLARE_OFFSET_FUNCTION(GetActivity, 0x894, uintptr_t);
	DECLARE_OFFSET_FUNCTION(GetFireDuration, 0x8DC, float);
	DECLARE_OFFSET_FUNCTION(GetszName, 0x8B0, uintptr_t);
	DECLARE_OFFSET_FUNCTION(IsFiresUnderwater, 0x8C4, bool);
	DECLARE_OFFSET_FUNCTION(IsAltFiresUnderwater, 0x8C5, bool);
	DECLARE_OFFSET_FUNCTION(GetMinRange1, 0x8C8, float);
	DECLARE_OFFSET_FUNCTION(GetMinRange2, 0x8CC, float);
	DECLARE_OFFSET_FUNCTION(GetMaxRange1, 0x8D0, float);
	DECLARE_OFFSET_FUNCTION(GetMaxRange2, 0x8D4, float);
	DECLARE_OFFSET_FUNCTION(IsReloadsSingly, 0x8D8, bool);
	DECLARE_OFFSET_FUNCTION(IsRemoveable, 0x8A0, bool);
	DECLARE_OFFSET_FUNCTION(GetPrimaryAmmoCount, 0x8A4, uintptr_t);
	DECLARE_OFFSET_FUNCTION(GetSecondaryAmmoCount, 0x8A8, uintptr_t);
	DECLARE_OFFSET_FUNCTION(ShouldFlipViewModel, m_bFlipViewModel, bool);
	DECLARE_OFFSET_FUNCTION(GetWeaponMode, m_weaponMode, int32_t);
	DECLARE_OFFSET_FUNCTION(GetAccuracyPenalty, m_fAccuracyPenalty, float);

	bool IsC4()
	{
		return GetWeaponID() == WEAPON_C4;
	}

	bool IsKnife()
	{
		return GetWeaponID() == WEAPON_KNIFE;
	}

	bool IsGrenade()
	{
		int ID = GetWeaponID();

		return (ID == WEAPON_HEGRENADE || ID == WEAPON_SMOKEGRENADE || ID == WEAPON_FLASHBANG);
	}

	bool IsFullAuto()
	{
		return Get_vFunction<bool(__thiscall*)(void*)>(this, 363)(this);
	}

	int GetWeaponID()
	{
		return Get_vFunction<int(__thiscall*)(void*)>(this, 365)(this);
	}

	float GetInaccuracy()
	{
		return Get_vFunction<float(__thiscall*)(void*)>(this, 376)(this);
	}

	float GetSpread()
	{
		return Get_vFunction<float(__thiscall*)(void*)>(this, 377)(this);
	}

	void UpdateAccuracyPenalty()
	{
		return Get_vFunction<void(__thiscall*)(void*)>(this, 378)(this);
	}

	CCSWeaponInfo* GetWpnData()
	{
		return Original_GetWpnData(this);
	}
};

class CSprite : public CBaseEntity
{
public:
	DECLARE_OFFSET_FUNCTION(GetAttachedToEntity, 0x528, CBaseEntity*);
	DECLARE_OFFSET_FUNCTION(GetSpriteScale, 0x544, float);
	DECLARE_OFFSET_FUNCTION(GetAttachment, 0x52C, int);
};

class CSpriteTrail : public CSprite
{
public:

	DECLARE_OFFSET_FUNCTION(GetLifeTime, 0x1DAC, float);
	DECLARE_OFFSET_FUNCTION(GetStartWidth, 0x1DB0, float);
	DECLARE_OFFSET_FUNCTION(GetEndWidth, 0x1DB4, float);
	DECLARE_OFFSET_FUNCTION(GetStartWidthVariance, 0x1DB8, float);
	DECLARE_OFFSET_FUNCTION(GetTextureRes, 0x1DBC, float);
	DECLARE_OFFSET_FUNCTION(GetMinFadeLength, 0x1DC0, float);
	DECLARE_OFFSET_FUNCTION(GetSkyboxOrigin, 0x1DC4, Vector);
	DECLARE_OFFSET_FUNCTION(GetSkyboxScale, 0x1DD0, float);
};


class C_PlayerResource
{
public:
	const char* GetPlayerName(size_t INDEX)
	{
		static auto GetPlayerName = reinterpret_cast<const char* (__thiscall*)(void*, int)>(
			Tools::FindPattern("client.dll", "55 8B EC 56 8B 75 08 57 8B F9 8D 46 FF 83 F8 40 77 47"));

		return GetPlayerName(this, INDEX);
	}

	bool IsConnected(size_t INDEX)
	{
		static auto IsConnected = reinterpret_cast<bool(__thiscall*)(void*, int)>(
			Tools::FindPattern("client.dll", "55 8B EC 8B 45 08 8D 50 FF 83 FA 40 77 0B 8A 84 08 ? ? ? ?"));

		return IsConnected(this, INDEX);
	}

	bool& IsBombSpooted()
	{
		static auto m_bBombSpotted = GetNetVarOffset("DT_CSPlayerResource", "m_bBombSpotted", BaseClientDLL->GetAllClasses());

		return *(bool*)(this - 0x518 + m_bBombSpotted);
	}

	bool& IsPlayerSpooted(size_t INDEX)
	{
		static auto m_bPlayerSpotted = GetNetVarOffset("DT_CSPlayerResource", "m_bPlayerSpotted", BaseClientDLL->GetAllClasses());

		return *reinterpret_cast<bool*>(this - 0x518 + m_bPlayerSpotted + INDEX * 1);
	}

	const char* GetClanTag(size_t INDEX)
	{
		static auto m_szClan = GetNetVarOffset("DT_CSPlayerResource", "m_szClan", BaseClientDLL->GetAllClasses());

		return (const char*)(this - 0x518 + m_szClan + INDEX * 0X10);
	}
};

class CCSWeaponInfo
{
public:
	virtual void Parse(KeyValues*, const char*) = 0;

	bool bParsedScript; //0x0004
	bool bLoadedHudElements; //0x0005
	char szClassName[MAX_WEAPON_STRING]; //0x0006
	char szPrintName[MAX_WEAPON_STRING]; //0x0056
	char szViewModel[MAX_WEAPON_STRING]; //0x00A6
	char szWorldModel[MAX_WEAPON_STRING]; //0x00F6
	char szAnimationPrefix[MAX_WEAPON_PREFIX]; //0x0146
	char pad_0156[2]; //0x0156
	int32_t iSlot; //0x0158
	int32_t iPosition; //0x015C
	int32_t iMaxClip1; //0x0160
	int32_t iMaxClip2; //0x0164
	int32_t iDefaultClip1; //0x0168
	int32_t iDefaultClip2; //0x016C
	int32_t iWeight; //0x0170
	int32_t iRumbleEffect; //0x0174
	bool bAutoSwitchTo; //0x0178
	bool bAutoSwitchFrom; //0x0179
	char pad_017A[2]; //0x017A
	int32_t iFlags; //0x017C
	char szAmmo1[MAX_WEAPON_AMMO_NAME]; //0x0180
	char szAmmo2[MAX_WEAPON_AMMO_NAME]; //0x01A0
	char aShootSounds[NUM_SHOOT_SOUND_TYPES][MAX_WEAPON_STRING]; //0x01C0
	int32_t iAmmoType; //0x06C0
	int32_t iAmmo2Type; //0x06C4
	bool m_bMeleeWeapon; //0x06C8
	bool m_bBuiltRightHanded; //0x06C9
	bool m_bAllowFlipping; //0x06CA
	char pad_06CB[1]; //0x06CB
	int32_t iSpriteCount; //0x06CC
	class CHudTexture* iconActive; //0x06D0
	class CHudTexture* iconInactive; //0x06D4
	class CHudTexture* iconAmmo; //0x06D8
	class CHudTexture* iconAmmo2; //0x06DC
	class CHudTexture* iconCrosshair; //0x06E0
	class CHudTexture* iconAutoaim; //0x06E4
	class CHudTexture* iconZoomedCrosshair; //0x06E8
	class CHudTexture* iconZoomedAutoaim; //0x06EC
	class CHudTexture* iconSmall; //0x06F0
	bool bShowUsageHint; //0x06F4
	char pad_06F5[3]; //0x06F5
	float m_flMaxSpeed; //0x06F8
	int32_t m_WeaponType; //0x06FC
	bool bFullAuto; //0x0700
	char pad_0701[3]; //0x0701
	int32_t m_iTeam; //0x0704
	float m_flBotAudibleRange; //0x0708
	float m_flArmorRatio; //0x070C
	int32_t m_iCrosshairMinDistance; //0x0710
	int32_t m_iCrosshairDeltaDistance; //0x0714
	bool m_bCanUseWithShield; //0x0718
	char m_WrongTeamMsg[32]; //0x0719
	char m_szAnimExtension[16]; //0x0739
	char m_szShieldViewModel[64]; //0x0749
	char m_szAddonModel[MAX_WEAPON_STRING]; //0x0789
	char m_szDroppedModel[MAX_WEAPON_STRING]; //0x07D9
	char m_szSilencerModel[MAX_WEAPON_STRING]; //0x0829
	char pad_0879[3]; //0x0879
	int32_t m_iMuzzleFlashStyle; //0x087C
	float m_flMuzzleScale; //0x0880
	int32_t m_iPenetration; //0x0884
	int32_t m_iDamage; //0x0888
	float m_flRange; //0x088C
	float m_flRangeModifier; //0x0890
	int32_t m_iBullets; //0x0894
	float m_flCycleTime; //0x0898
	bool m_bAccuracyQuadratic; //0x089C
	char pad_089D[3]; //0x089D
	float m_flAccuracyDivisor; //0x08A0
	float m_flAccuracyOffset; //0x08A4
	float m_flMaxInaccuracy; //0x08A8
	float m_flSpread; //0x08AC
	float m_flSpreadAlt; //0x08B0
	float m_flInaccuracyCrouch; //0x08B4
	float m_flInaccuracyCrouchAlt; //0x08B8
	float m_flInaccuracyStand; //0x08BC
	float m_flInaccuracyStandAlt; //0x08C0
	float m_flInaccuracyJump; //0x08C4
	float m_flInaccuracyJumpAlt; //0x08C8
	float m_flInaccuracyLand; //0x08CC
	float m_flInaccuracyLandAlt; //0x08D0
	float m_flInaccuracyLadder; //0x08D4
	float m_flInaccuracyLadderAlt; //0x08D8
	float m_flInaccuracyFire; //0x08DC
	float m_flInaccuracyFireAlt; //0x08E0
	float m_flInaccuracyMove; //0x08E4
	float m_flInaccuracyMoveAlt; //0x08E8
	float m_flRecoveryTimeStand; //0x08EC
	float m_flRecoveryTimeCrouch; //0x08F0
	float m_flInaccuracyReload; //0x08F4
	float m_flInaccuracyAltSwitch; //0x08F8
	float m_flTimeToIdleAfterFire; //0x08FC
	float m_flIdleInterval; //0x0900
	int32_t m_iWeaponPrice; //0x0904
	int32_t m_iDefaultPrice; //0x0908
	int32_t m_iPreviousPrice; //0x090C
}; //Size: 0x0910
