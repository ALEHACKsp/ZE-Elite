#pragma once

#ifndef Miscellaneous_H
#define Miscellaneous_H

#include "Vector.h"

class CBaseEntity;
class CUserCmd;

namespace Miscellaneous
{
	void Run(CBaseEntity* pLocal, CUserCmd* cmd, bool& bSendPacket) noexcept;

	void EnginePrediction(CBaseEntity* pLocal, CUserCmd* cmd) noexcept;
}

extern bool Was_On_Ground_Backup;

#endif