#pragma once

#ifndef CAutoLaser_H
#define CAutoLaser_H

#include "Vector.h"

class CBaseEntity;
class CUserCmd;
class Entities;

namespace AutoLaser
{
	void Refresh(CBaseEntity* pLocal, CUserCmd* cmd) noexcept, Run(CBaseEntity* pLocal, CUserCmd* cmd, bool& bSendPacket) noexcept;
}

void SetReleaseTick(Vector LocalOrigin, Vector EntityOrigin, Vector EntitySpeed, Entities& EntityInfo, CUserCmd* cmd);

#endif