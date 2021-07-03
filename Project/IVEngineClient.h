#pragma once


#include "Inconstructible.h"
#include "Virtuals.h"
#include "Vector.h"

typedef unsigned int UINT;

typedef struct player_info_s
{
	char			name[32];
	int				userID;
	char			guid[33];
	unsigned long	friendsID;
	char			friendsName[32];
	bool			fakeplayer;
	bool			ishltv;
	unsigned long	customFiles[4];
	unsigned char	filesDownloaded;
} player_info_t;

class CNetChan
{
public:
	INCONSTRUCTIBLE(CNetChan);

	float GetLatency(int flow)
	{
		return Get_vFunction<float(__thiscall*)(void*, int)>(this, 9)(this, flow);
	}

	bool SendNetMsg(void*& msg, bool bForceReliable = false, bool bVoice = false)
	{
		return Get_vFunction<bool(__thiscall*)(void*, void*&, bool, bool)>(this, 40)(this, msg, bForceReliable, bVoice);
	}

public:
	bool m_bProcessingMessages;
	bool m_bShouldDelete;

	// last send outgoing sequence number
	int m_nOutSequenceNr;
	// last received incoming sequnec number
	int m_nInSequenceNr;
	// last received acknowledge outgoing sequnce number
	int m_nOutSequenceNrAck;

	// state of outgoing reliable data (0/1) flip flop used for loss detection
	int m_nOutReliableState;
	// state of incoming reliable data
	int m_nInReliableState;

	int m_nChokedPackets; //number of choked packets
};

class Color;

class IVEngineClient
{
public:
	INCONSTRUCTIBLE(IVEngineClient);

	void GetScreenSize(int& width, int& height)
	{
		Get_vFunction<void(__thiscall*)(void*, int&, int&)>(this, 5)(this, width, height);
	}
	bool GetPlayerInfo(int ent_num, player_info_t* pinfo)
	{
		return Get_vFunction<bool(__thiscall*)(void*, int, player_info_t*)>(this, 8)(this, ent_num, pinfo);
	}
	int GetPlayerForUserID(int userID)
	{
		return Get_vFunction<int(__thiscall*)(void*, int)>(this, 9)(this, userID);
	}
	bool Con_IsVisible(void)
	{
		return Get_vFunction<bool(__thiscall*)(void*)>(this, 11)(this);
	}
	int GetLocalPlayer(void)
	{
		return Get_vFunction<int(__thiscall*)(void*)>(this, 12)(this);
	}
	void GetViewAngles(Vector& va)
	{
		Get_vFunction<void(__thiscall*)(void*, Vector&)>(this, 19)(this, va);
	}
	void SetViewAngles(Vector& va)
	{
		Get_vFunction<void(__thiscall*)(void*, Vector&)>(this, 20)(this, va);
	}
	int GetMaxClients(void)
	{
		return Get_vFunction<int(__thiscall*)(void*)>(this, 21)(this);
	}
	bool IsInGame(void)
	{
		return Get_vFunction<bool(__thiscall*)(void*)>(this, 26)(this);
	}
	bool IsConnected(void)
	{
		return Get_vFunction<bool(__thiscall*)(void*)>(this, 27)(this);
	}

	const matrix3x4& WorldToScreenMatrix()
	{
		return Get_vFunction<const matrix3x4& (__thiscall*)(void*)>(this, 36)(this);
	}

	const matrix3x4& WorldToViewMatrix()
	{
		return Get_vFunction<const matrix3x4& (__thiscall*)(void*)>(this, 37)(this);
	}

	void ClientCmd_Unrestricted(const char* szCmdString)
	{
		Get_vFunction<void(__thiscall*)(void*, const char*)>(this, 106)(this, szCmdString);
	}

	void ClientCmd_Unrestricted_Formatted(const char* fmt, ...)
	{
		va_list va_alist;
		char buf[256];
		va_start(va_alist, fmt);
		_vsnprintf(buf, sizeof(buf), fmt, va_alist);
		va_end(va_alist);

		Get_vFunction<void(__thiscall*)(void*, const char*)>(this, 106)(this, buf);
	}
};