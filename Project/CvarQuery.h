#ifndef CvarQuery_H
#define CvarQuery_H

#include "SDK.h"

typedef enum
{
	eQueryCvarValueStatus_ValueIntact = 0,	// It got the value fine.
	eQueryCvarValueStatus_CvarNotFound = 1,
	eQueryCvarValueStatus_NotACvar = 2,		// There's a ConCommand, but it's not a ConVar.
	eQueryCvarValueStatus_CvarProtected = 3	// The cvar was marked with FCVAR_SERVER_CAN_NOT_QUERY, so the server is not allowed to have its value.
} EQueryCvarValueStatus;

typedef int QueryCvarCookie_t;

class CLC_RespondCvarValue
{
	char pad[16];
public:
	QueryCvarCookie_t		m_iCookie;

	const char* m_szCvarName;
	const char* m_szCvarValue;	// The sender sets this, and it automatically points it at m_szCvarNameBuffer when receiving.

	EQueryCvarValueStatus	m_eStatusCode;

private:
	char		m_szCvarNameBuffer[256];
	char		m_szCvarValueBuffer[256];
};

class SVC_GetCvarValue
{

	char pad[16];
public:

	QueryCvarCookie_t	m_iCookie;
	const char* m_szCvarName;	// The sender sets this, and it automatically points it at m_szCvarNameBuffer when receiving.

private:
	char		m_szCvarNameBuffer[256];
};

class NET_StringCmd
{

	char pad[16];
public:
	const char* m_szCommand;	// execute this command

private:
	char		m_szCommandBuffer[1024];	// buffer for received messages
};

#include <string>

class DesiredConVarsValueInfo
{
	std::string Name, Value; int Status;
public:
	inline void Set(const char* name, const char* value, int status)
	{
		Name = name, Value = value, Status = status;
	}

	inline void Set(const char* value, int status)
	{
		Value = value, Status = status;
	}

	inline const char* GetName()
	{
		return Name.c_str();
	}

	inline const char* GetValue()
	{
		return Value.c_str();
	}

	inline int GetStatus()
	{
		return Status;
	}
};

#endif