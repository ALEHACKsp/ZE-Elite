#ifndef Assorted_H
#define Assorted_H

#include "SDK.h"
#include <optional>
#include "Defintions.h"

extern void HitMarkerOnPaint() noexcept;

extern void SetClanTag(const char* tag) noexcept;

extern void Precache_Custom_Stuff(HMODULE DLL) noexcept;

extern bool PrecacheModel(const char* szModelName) noexcept;

extern void DrawBeamd(Vector src, Vector end, Color color) noexcept;

extern bool FindStringCS(std::string data, std::string toSearch) noexcept;

extern void Set_DisConnection_Msg(const char* Message, bool Reset = false) noexcept;

extern std::string ReplaceString(std::string subject, const std::string& search, const std::string& replace) noexcept;

extern void UTIL_TraceLine(const Vector& vecAbsStart, const Vector& vecAbsEnd, unsigned int mask, const CBaseEntity* ignore, int collisionGroup, trace_t* ptr) noexcept;

bool IntersectRayWithOBB(const Vector& vecRayStart, const Vector& vecRayDelta, const matrix3x4& matOBBToWorld, const Vector& vecOBBMins, const Vector& vecOBBMaxs) noexcept;

extern void UTIL_ClipTraceToPlayers
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

class CLC_ListenEvents
{
	char pad[16];
public:
	char m_EventArray[64];
};

extern void* CLC_ListenEvents_Table;

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

inline void NET_Set_ConVar(const char* cvar, const char* value) noexcept
{
	static void* pvSetConVar = (void*)(Tools::FindPattern("engine.dll", "56 57 8D 4D D4"));

	Invoke_NET_SetConVar(pvSetConVar, cvar, value);
}

#endif
