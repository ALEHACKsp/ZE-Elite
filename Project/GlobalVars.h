#pragma once

class CGlobalVarsBase
{
public:
#include "Inconstructible.h"
	INCONSTRUCTIBLE(CGlobalVarsBase);

	float			realtime;
	int				framecount;
	float			absoluteframetime;
	float			curtime;
	float			frametime;
	int				maxClients;
	int				tickcount;
	float			interval_per_tick;
	float			interpolation_amount;
	int				simTicksThisFrame;
	int				network_protocol;
};