#include "Inconstructible.h"

struct dworldlight_t
{
	INCONSTRUCTIBLE(dworldlight_t);

	Vector		origin;
	Vector		intensity;
	Vector		normal;			// for surfaces and spotlights
	int			cluster;
};

class worldbrushdata_t
{
	byte pad[156];
public:
	INCONSTRUCTIBLE(worldbrushdata_t);

	int			numworldlights;
	dworldlight_t* worldlights;
};

struct ColorRGBExp32
{
	INCONSTRUCTIBLE(ColorRGBExp32);

	byte r, g, b;
	signed char exponent;
};

struct dlight_t
{
	INCONSTRUCTIBLE(dlight_t);

	int		flags;
	Vector	origin;
	float	radius;
	ColorRGBExp32	color;		// Light color with exponent
	float	die;				// stop lighting after this time
	float	decay;				// drop this each second
	float	minlight;			// don't add when contributing less
	int		key;
	int		style;				// lightstyle

	// For spotlights. Use m_OuterAngle == 0 for point lights
	Vector	m_Direction;		// center of the light cone
	float	m_InnerAngle;
	float	m_OuterAngle;

	// see comments above about HL2_BROKEN_MIN_LIGHTING_VALUE and MIN_LIGHTING_VALUE
	// THIS SHOULD ONLY GET CALLED FROM THE ENGINE
	float GetRadius() const
	{
		//		return FastSqrt( radius * radius * ( HL2_BROKEN_MIN_LIGHTING_VALUE / MIN_LIGHTING_VALUE ) );
		return radius;
	}

	// see comments above about HL2_BROKEN_MIN_LIGHTING_VALUE and MIN_LIGHTING_VALUE
	// THIS SHOULD ONLY GET CALLED FROM THE ENGINE
	float GetRadiusSquared() const
	{
		//		return radius * radius * ( HL2_BROKEN_MIN_LIGHTING_VALUE / MIN_LIGHTING_VALUE );
		return radius * radius;
	}

	// THIS SHOULD ONLY GET CALLED FROM THE ENGINE
	float IsRadiusGreaterThanZero() const
	{
		// don't bother calculating the new radius if you just want to know if it is greater than zero.
		return radius > 0.0f;
	}
};

class IVEfx
{
public:
	INCONSTRUCTIBLE(IVEfx);

	dlight_t* CL_AllocDlight(const int key)
	{
		return Get_vFunction<dlight_t* (__thiscall*)(void*, int)>(this, 4)(this, key);
	}

	dlight_t* CL_AllocElight(const int key)
	{
		return Get_vFunction<dlight_t* (__thiscall*)(void*, int)>(this, 5)(this, key);
	}
};