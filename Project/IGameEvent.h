#pragma once

#include <winnt.h>
#include "Inconstructible.h"

class KeyValues;

class CGameEventCallback
{
public:
	INCONSTRUCTIBLE(CGameEventCallback);

	void* m_pCallback;		// callback pointer
	int					m_nListenerType;	// client or server side ?
};

class CGameEventDescriptor
{
public:

	CGameEventDescriptor()
	{
		name[0] = 0;
		eventid = -1;
		keys = NULL;
		local = false;
		reliable = true;
	}

public:
	char		name[32];	// name of this event
	int			eventid;	// network index number, -1 = not networked
	KeyValues* keys;		// KeyValue describing data types, if NULL only name 
	bool		local;		// local event, never tell clients about that
	bool		reliable;	// send this event as reliable message
	CUtlVector<CGameEventCallback*>	listeners;	// registered listeners
};


class IGameEvent
{
public:
	virtual ~IGameEvent()
	{
	};
	virtual const char* GetName() const = 0; // get event name

	virtual bool IsReliable() const = 0;				  // if event handled reliable
	virtual bool IsLocal() const = 0;					  // if event is never networked
	virtual bool IsEmpty(const char* keyName = NULL) = 0; // check if data field exists

														  // Data access
	virtual bool GetBool(const char* keyName = NULL, bool defaultValue = false) = 0;
	virtual int GetInt(const char* keyName = NULL, int defaultValue = 0) = 0;
	virtual float GetFloat(const char* keyName = NULL, float defaultValue = 0.0f) = 0;
	virtual const char* GetString(const char* keyName = NULL, const char* defaultValue = "") = 0;

	virtual void SetBool(const char* keyName, bool value) = 0;
	virtual void SetInt(const char* keyName, int value) = 0;
	virtual void SetFloat(const char* keyName, float value) = 0;
	virtual void SetString(const char* keyName, const char* value) = 0;
};


class IGameEventListener2
{
public:
	virtual ~IGameEventListener2(void)
	{
	};

	// FireEvent is called by EventManager if event just occured
	// KeyValue memory will be freed by manager if not needed anymore
	virtual void FireGameEvent(IGameEvent* event) = 0;
};

struct bf_write;
class IGameEventManager2
{
public:
	virtual ~IGameEventManager2() {};
	virtual int LoadEventsFromFile(const char* filename) = 0;
	virtual void Reset() = 0;
	virtual bool AddListener(IGameEventListener2* listener, const char* name, bool serverside) = 0;
	virtual bool FindListener(IGameEventListener2* listener, const char* name) = 0;
	virtual void RemoveListener(IGameEventListener2* listener) = 0;
	virtual void AddListenerGlobal(IGameEventListener2* listener, bool serverside) = 0;
	virtual IGameEvent* CreateEvent(const char* name, bool force = false, int* cookie = nullptr) = 0;
	virtual bool FireEvent(IGameEvent* event, bool bDontBroadcast = false) = 0;
	virtual bool FireEventClientSide(IGameEvent* event) = 0;
	virtual IGameEvent* DuplicateEvent(IGameEvent* event) = 0;
	virtual void FreeEvent(IGameEvent* event) = 0;
	virtual bool SerializeEvent(IGameEvent* event, bf_write* buffer) = 0;
	virtual IGameEvent* UnserializeEvent(void* buffer) = 0;
	virtual KeyValues* GetEventDataTypes(IGameEvent* event) = 0;

	CUtlVector<CGameEventDescriptor>	m_GameEvents;	// list of all known events
	CUtlVector<CGameEventCallback*>		m_Listeners;	// list of all registered listeners
};
