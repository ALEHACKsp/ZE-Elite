#pragma once
#include "Vector.h"

class CUserCmd;
class CBaseEntity;
class IMoveHelper;
class CMoveData;

class IPrediction
{
public:
#include "Inconstructible.h"
	INCONSTRUCTIBLE(IPrediction);

	// Run prediction
	void Update
	(
		int startframe,				// World update ( un-modded ) most recently received
		bool validframe,			// Is frame data valid
		int incoming_acknowledged,	// Last command acknowledged to have been run by server (un-modded)
		int outgoing_command		// Last command (most recent) sent to server (un-modded)
	) 
	{
		Get_vFunction<void(__thiscall*)(void*, int, bool, int, int)>(this, 3)(this, startframe, validframe, incoming_acknowledged, outgoing_command);
	}

	void SetupMove(CBaseEntity* player, CUserCmd* cmd, IMoveHelper* pHelper, CMoveData* move)
	{
		Get_vFunction<void(__thiscall*)(void*, CBaseEntity* player, CUserCmd* cmd, IMoveHelper* pHelper, CMoveData* move)>(this, 18)(
			this, player, cmd, pHelper, move);
	}

	void FinishMove(CBaseEntity* player, CUserCmd* cmd, CMoveData* move)
	{
		Get_vFunction<void(__thiscall*)(void*, CBaseEntity* player, CUserCmd* cmd, CMoveData* move)>(this, 19)(
			this, player, cmd, move);
	}
};