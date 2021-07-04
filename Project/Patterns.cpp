#include "FindPattern.h"
#include "SDK.h"



uintptr_t Tools::FindPattern(const char* szModule, const char* szSignature)
{
	MODULEINFO modInfo;

	static auto CurrentPorcess = GetCurrentProcess();

	GetModuleInformation(CurrentPorcess, GetModuleHandle(szModule), &modInfo, sizeof(MODULEINFO));

	uintptr_t startAddress = reinterpret_cast<uintptr_t>(modInfo.lpBaseOfDll);

	uintptr_t endAddress = startAddress + modInfo.SizeOfImage;

	const char* pat = szSignature;

	uintptr_t firstMatch = 0;

	for (uintptr_t pCur = startAddress; pCur < endAddress; pCur++)
	{
		if (!*pat) return firstMatch;

		if (*(PBYTE)pat == '\?' || *(BYTE*)pCur == getByte(pat))
		{
			if (!firstMatch) firstMatch = pCur;

			if (!pat[2]) return firstMatch;

			if (*(PWORD)pat == '\?\?' || *(PBYTE)pat != '\?') pat += 3;

			else pat += 2;
		}
		else
		{
			pat = szSignature;

			firstMatch = 0;
		}
	}

	const wchar_t* GetWC(const char* c) noexcept;

	std::wstring Error = L""; Error.append(L"[").append(GetWC(szModule)).append(L"]").append(L" [").append(GetWC(szSignature)).append(L"] Pointed to nullptr address");

	_wassert(Error.c_str(), NULL, 0);
        
	return NULL;
}


bool Tools::IsCodePtr(void* ptr)
{
	constexpr const DWORD protect_flags = PAGE_EXECUTE | PAGE_EXECUTE_READ | PAGE_EXECUTE_READWRITE | PAGE_EXECUTE_WRITECOPY;

	MEMORY_BASIC_INFORMATION out;

	VirtualQuery(ptr, &out, sizeof out);

	return out.Type
		&& !(out.Protect & (PAGE_GUARD | PAGE_NOACCESS))
		&& out.Protect & protect_flags;
}
