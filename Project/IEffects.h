#include "SDK.h"

class IEffects
{
public:

#include "Inconstructible.h"
	INCONSTRUCTIBLE(IEffects);

	void EnergySplash(const Vector& position, const Vector& direction, bool bExplosive = false) {

		static auto EnergySplash(reinterpret_cast<void(__thiscall*)(void*, const Vector&, const Vector&, bool)>(
			(*(void***)(this))[7]));

		EnergySplash(this, position, direction, bExplosive);
	};


	void MuzzleFlash(const Vector& vecOrigin, const QAngle& vecAngles, float flScale, int iType) {

		static auto MuzzleFlash(reinterpret_cast<void(__thiscall*)(void*, const Vector & vecOrigin, const QAngle & vecAngles, float flScale, int iType)>(
			(*(void***)(this))[5]));

		MuzzleFlash(this, vecOrigin, vecAngles, flScale, iType);
	};
};