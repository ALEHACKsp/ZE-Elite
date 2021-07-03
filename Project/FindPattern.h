#pragma once

#ifndef Patterns
#define Patterns

#include <Windows.h>
#include <memory>
#include <Psapi.h>
#include <chrono>
#include <thread>

namespace Tools
{
#define INRANGE(x,a,b)  (x >= a && x <= b) 
#define getBits( x )    (INRANGE((x&(~0x20)),'A','F') ? ((x&(~0x20)) - 'A' + 0xa) : (INRANGE(x,'0','9') ? x - '0' : 0))
#define getByte( x )    (getBits(x[0]) << 4 | getBits(x[1]))

	uintptr_t FindPattern(const char* szModule, const char* szSignature);
	bool IsCodePtr(void* ptr);
}

#endif
