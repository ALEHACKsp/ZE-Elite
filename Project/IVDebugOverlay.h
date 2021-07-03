#pragma once

#include "SDK.h"
class Color;

#define NDEBUG_PERSIST_TILL_NEXT_SERVER (0.01023f)

class OverlayText_t;

class IVDebugOverlay
{
public:
#include "Inconstructible.h"
	INCONSTRUCTIBLE(IVDebugOverlay);

	void AddBoxOverlay(const Vector& origin, const Vector& mins, const Vector& max, QAngle const& orientation, int r, int g, int b, int a, float duration)
	{
		Get_vFunction<void(__thiscall*)(void*, const Vector& origin, const Vector& mins, const Vector& max, QAngle const& orientation, int r, int g, int b, int a, float duration)>(this, 1)(
			this, origin, mins, max, orientation, r, g, b, a, duration);
	}
};