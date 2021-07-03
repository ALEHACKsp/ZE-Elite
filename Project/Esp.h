#pragma once

#ifndef Esp_H
#define Esp_H

#include "Vector.h"
#include <vector>

struct DynamicBoundingBox;
struct BoundingBox;
struct ImDrawList;
class CBaseEntity;

#ifndef max
#define max(a,b)            (((a) > (b)) ? (a) : (b))
#endif

#ifndef min
#define min(a,b)            (((a) < (b)) ? (a) : (b))
#endif

typedef unsigned long HFont;

extern bool ScreenTransform(const Vector& point, Vector& screen) noexcept;

extern bool WorldToScreen(const Vector& origin, Vector& screen) noexcept;

namespace Esp
{
	void Render() noexcept;

	void Initialize() noexcept;

	void Debug(CBaseEntity* pLocal) noexcept;

	void Box(DynamicBoundingBox BBox) noexcept;

	void Skeleton(CBaseEntity* pEntity) noexcept;

	void Name(BoundingBox bbox, CBaseEntity* Entity) noexcept;

	void Health(BoundingBox bbox, CBaseEntity* Entity) noexcept;

	void Weapon(const Vector& EntityOrigin, CBaseEntity* Entity) noexcept;

	void SnapLine(BoundingBox bbox, CBaseEntity* Entity) noexcept;

	extern HFont courier_new, Counter_Strike_Logo, Others[5];



	extern bool Fonts_Were_Initialized;
}

struct DamageIndicator_t {
	int iDamage;
	float flEraseTime;
};

namespace DamageIndicator
{
	void Paint(CBaseEntity* pLocal);

	extern DamageIndicator_t data;
}

#endif