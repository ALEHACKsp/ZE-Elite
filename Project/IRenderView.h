#pragma once

#include "Vector.h"
#include "Color.h"

class IVRenderView
{
public:
#include "Inconstructible.h"
	INCONSTRUCTIBLE(IVRenderView);

	void SetBlend(float blend)
	{
		Get_vFunction<void(__thiscall*)(void*, float blend)>(this, 4)(
			this, blend);
	}

	float GetBlend()
	{
		return Get_vFunction<float(__thiscall*)(void*)>(this, 5)(this);
	}

	void SetColorModulation(float const* blend)
	{
		Get_vFunction<void(__thiscall*)(void*, float const* blend)>(this, 6)(
			this, blend);
	}

	void GetColorModulation(float* blend)
	{
		Get_vFunction<void(__thiscall*)(void*, float* blend)>(this, 7)(
			this, blend);
	}

	inline void                 SetColorModulation(float r, float g, float b, float a = 255)
	{
		float clr[4] = { r, g, b,a };
		SetColorModulation(clr);
	}
};