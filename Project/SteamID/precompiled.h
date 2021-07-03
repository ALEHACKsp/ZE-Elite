#pragma once

#define WIN32_LEAN_AND_MEAN



#include <Windows.h>
#include <time.h>
#include <stdlib.h>
#include <heapapi.h>

#include <stdio.h> // vsprintf
#include <stdarg.h> // va_start, va_end
#include <WinUser.h> // MessageBoxA

#include <exception>
#include <stdexcept>
#include <cstring>
#include <string>
#include <cassert>
#include <fstream>
#include <strstream>

#include "opcode_len_calc.h"

#include "MsgAPI.h"
#include "XMemory.h"

#pragma warning(disable: 4018)
#pragma warning(disable: 4244)

#include "Cryptography/CRijndael.h"
#include "Cryptography/DoubleBuffering.h"
#include "Cryptography/MessageDigest.h"
#include "Cryptography/SHA.h"

extern uint32_t gnSteamIDValue;