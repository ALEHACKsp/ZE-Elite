#ifndef _CNADETRACER_H
#define _CNADETRACER_H

#include "SDK.h"

#include <vector>
#include <set>
#include <unordered_map>

class CTimer
{
private:
	__int64 initialTS, currentTS;
	float secsPerCount;

public:
	CTimer()
	{
		__int64 countsPerSec = initialTS = currentTS = 0;
		QueryPerformanceFrequency((LARGE_INTEGER*)&countsPerSec);
		if (countsPerSec == 0)
			secsPerCount = 1.0f;
		else
			secsPerCount = 1.0f / (float)countsPerSec;
	}
	void init()
	{
		QueryPerformanceCounter((LARGE_INTEGER*)&initialTS);
	}
	float diff()
	{
		QueryPerformanceCounter((LARGE_INTEGER*)&currentTS);
		return ((float)currentTS - (float)initialTS) * secsPerCount;
	}
};

struct CTraceObj
{
	CTraceObj() = default;
	CTraceObj(unsigned int);

	unsigned int m_iMaxLength;
	std::vector<Vector> m_vecTracePoints;
	CTimer m_timerPointLock;
};

class CNadeTracer
{
private:
	std::unordered_map<CBaseEntity*, CTraceObj> m_mapGrenades;
	std::set<CBaseEntity*> m_setGrenadeExists;

public:
	void AddTracer(CBaseEntity*, unsigned int);
	void Draw();
	void Clear();
};

#endif

extern CNadeTracer pNadeTracer;