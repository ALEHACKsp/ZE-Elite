#pragma once

class INetChannel;

class CClientState
{
public:
#include "Inconstructible.h"

	INCONSTRUCTIBLE(CClientState);

	INT32& GetDeltaTick()
	{
		return ((INT32*)(this))[104];
	}

	bool& GetClientListenersChanged()
	{
		return ((bool*)(this))[120];
	}

	const char* GetLevelNameShort()
	{
		return (const char*)(std::uint32_t(ClientState) + 0x230);
	}

	char pad_0x0000[0xC]; //0x0000
	int m_Socket;
	CNetChan* m_NetChannel; // Our sequenced channel to the remote server.
	char pad_0x0014[0x11C]; //0x0014
	__int32 m_nSignonState; //0x0130 
	char pad_0x0134[0x60]; //0x0134
	__int32 m_nCurrentSequence; //0x0194 
	__int32 unk1; //0x0198 
	__int32 unk2; //0x019C 
	__int32 unk3; //0x01A0 
	char pad_0x01A4[0x10C]; //0x01A4
	__int32 m_nMaxClients; //0x02B0 
	void* N00000583; //0x02B4 
	void* N00000582; //0x02B8 
	char pad_0x02BC[0x4854]; //0x02BC
	float m_flLastServerTickTime; //0x4B10 
	__int32 N0000196E; //0x4B14 
	__int32 oldtickcount; //0x4B18 
	float m_tickRemainder; //0x4B1C 
	float m_frameTime; //0x4B20 
	__int32 lastoutgoingcommand; //0x4B24 
	__int32 chokedcommands; //0x4B28 
	__int32 last_command_ack; //0x4B2C 
	__int32 command_ack; //0x4B30 
	char pad_0x4B34[0x50]; //0x4B34
	Vector viewangles; //0x4B84 
	char pad_0x4B90[0xCB0]; //0x4B90*/

}; //Size=0x5840