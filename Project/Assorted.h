#ifndef Assorted_H
#define Assorted_H

#include "SDK.h"
#include "Defintions.h"

void Hit_Marker_On_Paint() noexcept;

void SetClanTag(const char* tag) noexcept;

void Precache_Custom_Stuff(HMODULE DLL) noexcept;

bool PrecacheModel(const char* szModelName) noexcept;

void DrawBeamd(Vector src, Vector end, Color color) noexcept;

bool FindStringCS(std::string data, std::string toSearch) noexcept;

void Set_DisConnection_Msg(const char* Message, bool Reset = false) noexcept;

std::string ReplaceString(std::string subject, const std::string& search, const std::string& replace) noexcept;

void UTIL_TraceLine(const Vector& vecAbsStart, const Vector& vecAbsEnd, unsigned int mask, const CBaseEntity* ignore, int collisionGroup, trace_t* ptr) noexcept;

bool IntersectRayWithOBB(const Vector& vecRayStart, const Vector& vecRayDelta, const matrix3x4& matOBBToWorld, const Vector& vecOBBMins, const Vector& vecOBBMaxs) noexcept;

void UTIL_ClipTraceToPlayers
(
	const Vector& vecAbsStart,
	const Vector& vecAbsEnd,
	unsigned int mask,
	ITraceFilter* filter,
	trace_t* tr
) noexcept;

extern float _flHurtTime;

extern bool Overlay_Triggered;

extern std::vector<std::string> HitSounds;

extern CLC_ListenEvents* CLC_ListenEvents_Table;

inline __declspec(naked) void __cdecl Invoke_NET_SetConVar(void* pfn, const char* cvar, const char* value) noexcept
{
	__asm
	{
		push    ebp
		mov     ebp, esp
		sub     esp, 2Ch
		push    esi
		push    edi
		mov     edi, cvar
		mov     esi, value
		jmp     pfn
	}
}

inline void NET_SetConVar(const char* cvar, const char* value) noexcept
{
	static void* pvSetConVar = (void*)(Tools::FindPattern("engine.dll", "56 57 8D 4D D4"));

	Invoke_NET_SetConVar(pvSetConVar, cvar, value);
}

#endif
