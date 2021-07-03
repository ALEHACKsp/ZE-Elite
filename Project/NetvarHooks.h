#pragma once

#ifndef NetVarHooks
#define NetVarHooks

#include "Assorted.h"
#include "CNadeTracer.h"

class NetVarHookInfo
{
public:
	NetVarHookInfo(const char* Table, const  char* Var, PVOID function) : Original_Fn(nullptr), NetTableName(Table), VarName(Var), Fn(function) {

		if(function) NetVarHookInfo::HookOrUnHookNetvar(NetTableName, VarName, (PVOID&)Original_Fn, function);
	}

	~NetVarHookInfo() {

		NetVarHookInfo::HookOrUnHookNetvar(NetTableName, VarName, (PVOID&)Original_Fn, nullptr);
	}

	const char* NetTableName, * VarName;

	PVOID Fn; RecvVarProxyFn Original_Fn;

	static void HookOrUnHookNetvar(
		const char* NetTableName,
		const char* VarName,
		PVOID& Original,
		PVOID Fn) {

		ClientClass* pClass = BaseClientDLL->GetAllClasses();

		while (pClass)
		{
			const char* pszName = pClass->m_pRecvTable->m_pNetTableName;

			if (!strcmp(pszName, NetTableName))
			{
				for (int i = 0; i < pClass->m_pRecvTable->m_nProps; i++)
				{
					RecvProp* pProp = &(pClass->m_pRecvTable->m_pProps[i]);

					const char* name = pProp->m_pVarName;

					if (!strcmp(name, VarName))
					{
						if (!Original)
						{
							Original = pProp->m_ProxyFn;

							pProp->m_ProxyFn = Fn;
						}
						else
						{
							pProp->m_ProxyFn = Original;
						}
					}
				}
			}

			pClass = pClass->m_pNext;
		}
	}
};

NetVarHookInfo* m_fEffects;
NetVarHookInfo* m_iHealthProxy;
NetVarHookInfo* m_VecForceProxy; 

void m_iHealthHook(const CRecvProxyData* data, CBaseEntity* pEnt, void* out)
{
	if (auto pLocal = CBaseEntity::GetLocalPlayer(); pLocal && pLocal->IsAlive())
	{
		if (auto Index = pEnt->GetIndex(); Index > -1 && Index < HealthSprite::SpriteEntities.size())
		{
			int m_iHealth_Value = data->m_Value.m_Int;

			HealthSprite::SpriteEntities[Index].MaxHealth = max(HealthSprite::SpriteEntities[Index].MaxHealth, m_iHealth_Value);

			HealthSprite::OnPlayerHurt(HealthSprite::SpriteEntities[Index], m_iHealth_Value);
		}
	}

	if (m_iHealthProxy->Original_Fn) m_iHealthProxy->Original_Fn(data, pEnt, out);
}

void m_fEffectsHook(const CRecvProxyData* data, CBaseEntity* pEnt, void* out)
{
	CRecvProxyData* Edited_Data = const_cast<CRecvProxyData*>(data);

	int m_fEffects_Value = Edited_Data->m_Value.m_Int;

	if (auto pLocal = CBaseEntity::GetLocalPlayer(); pLocal && pEnt && pEnt != pLocal)
	{
		if (Menu::Get.Visuals.NoFlashLight && m_fEffects_Value & EF_DIMLIGHT)
		{
			Edited_Data->m_Value.m_Int &= ~EF_DIMLIGHT;
		}

		/*if (Menu::Get.Visuals.NoTeammates && pLocal->IsAlive() && !pEnt->IsEnemy(pLocal) && pEnt != pLocal)
		{
			if (!(m_fEffects_Value & EF_NODRAW))
				Edited_Data->m_Value.m_Int |= EF_NODRAW;

		}
		else
		{

			if (m_fEffects_Value & EF_NODRAW)
				Edited_Data->m_Value.m_Int &= ~EF_NODRAW;
		}*/
	}

	if(m_fEffects->Original_Fn) m_fEffects->Original_Fn(Edited_Data, pEnt, out);
}

void VecForce(const CRecvProxyData* data, CBaseEntity* pEnt, void* out)
{
	CRecvProxyData* Edited_Data = const_cast<CRecvProxyData*>(data);

	Edited_Data->m_Value.m_Vector *= Menu::Get.Misc.Ragdoll_Force;

	if (m_VecForceProxy->Original_Fn) m_VecForceProxy->Original_Fn(Edited_Data, pEnt, out);
}

#endif