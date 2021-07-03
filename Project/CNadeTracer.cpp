#include "CNadeTracer.h"
#include "Esp.h"

CNadeTracer pNadeTracer;

void CNadeTracer::AddTracer(CBaseEntity* pGrenade, unsigned int iMaxLength)
{
	m_setGrenadeExists.insert(pGrenade);
	if (m_mapGrenades.find(pGrenade) == m_mapGrenades.end())
		m_mapGrenades[pGrenade] = CTraceObj(iMaxLength);

	if (m_mapGrenades[pGrenade].m_timerPointLock.diff() > 0.025f) //25 ms
	{
		m_mapGrenades[pGrenade].m_vecTracePoints.push_back(pGrenade->GetAbsOrigin());
		if (m_mapGrenades[pGrenade].m_vecTracePoints.size() > m_mapGrenades[pGrenade].m_iMaxLength)
			m_mapGrenades[pGrenade].m_vecTracePoints.erase(m_mapGrenades[pGrenade].m_vecTracePoints.begin());

		m_mapGrenades[pGrenade].m_timerPointLock.init();
	}
}

void CNadeTracer::Draw()
{
	Vector vecLastScreenPos;
	bool bInit = false;

	for (auto& traceObj : m_mapGrenades)
	{
		for (auto& vecPos : traceObj.second.m_vecTracePoints)
		{
			Vector vecScreenPos;

			if (WorldToScreen(vecPos, vecScreenPos))
			{
				if (bInit)
				{
					Surface->DrawSetColor(Menu::Get.Colors.General.NadeTracer);

					Surface->DrawLine(vecLastScreenPos.x, vecLastScreenPos.y, vecScreenPos.x, vecScreenPos.y);
				}

				vecLastScreenPos = vecScreenPos;
				bInit = true;
			}
		}

		bInit = false;
	}
}

void CNadeTracer::Clear()
{
	for (auto it = m_mapGrenades.begin(); it != m_mapGrenades.end(); ++it)
	{
		if (m_setGrenadeExists.find((*it).first) == m_setGrenadeExists.end())
		{
			it = m_mapGrenades.erase(it);
			if (it == m_mapGrenades.end())
				break;
		}
	}

	m_setGrenadeExists.clear();
}

CTraceObj::CTraceObj(unsigned int iMaxLength)
{
	m_iMaxLength = iMaxLength;
	m_timerPointLock.init();
}