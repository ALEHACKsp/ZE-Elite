#pragma once
#include "UtlVector.h"
#include "Virtuals.h"

class CBaseEntity;

class IClientEntityListener
{
public:
	virtual void OnEntityCreated(CBaseEntity* pEntity) {};
	//virtual void OnEntitySpawned(C_BaseEntity* pEntity) {};
	virtual void OnEntityDeleted(CBaseEntity* pEntity) {};
};

class CClientEntityList
{
public:
	virtual ~CClientEntityList() {}

	CBaseEntity* GetClientEntity(int num)
	{
		return Get_vFunction<CBaseEntity* (__thiscall*)(void*, int)>(this, 3)(this, num);
	}

	CBaseEntity* GetClientEntityFromHandle(void* num)
	{
		return Get_vFunction<CBaseEntity* (__thiscall*)(void*, void*)>(this, 4)(this, num);
	}

	int GetHighestEntityIndex()
	{
		return Get_vFunction<int(__thiscall*)(void*)>(this, 6)(this);
	}

	void AddEntityListner(IClientEntityListener* pListener)
	{
		m_entityListeners.AddToTail(pListener);
	}

	void RemoveEntityListner(IClientEntityListener* pListener)
	{
		m_entityListeners.FindAndRemove(pListener);
	}

private:
	CUtlVector<IClientEntityListener*> m_entityListeners;
};