#pragma once

#ifndef CAimbot_H
#define CAimbot_H

#include "Vector.h"

class CBaseEntity;
class CUserCmd;


namespace Aimbot
{
	void Run(CBaseEntity* pLocal, CUserCmd* cmd, bool& bSendPacket) noexcept;

	CBaseEntity* GetClosestEnemy(CBaseEntity* pLocal) noexcept;

	extern bool NoSpreadWasCalled, NoRecoilWasCalled;

	extern int LoopCount;

	extern CBaseEntity* BestTarget;
};

Vector Predicted_Position(CBaseEntity* Entity, bool pLocal = false) noexcept;

void GetMaterialParameters(int iMaterial, float& flPenetrationModifier, float& flDamageModifier);

void GetBulletTypeParameters(int iBulletType, float& fPenetrationPower, float& flPenetrationDistance);

float ScaleDamage(float damage, int hitgroup, int armor_value, bool has_helmet, float weapon_armor_ratio);

bool TraceToExit(Vector& start, Vector& dir, Vector& end, float flStepSize, float flMaxDistance);

bool CanPenetratePoint(CBaseEntity* pLocal, CBaseEntity* target_player, Vector shootAngles);

#endif