#ifndef SkinChanger_H
#define SkinChanger_H

#include <string>
#include <vector>

struct SC_ModelInfo
{
	SC_ModelInfo(std::string szName)
	{
		INDEX = 0;

		weapon = szName;
	}

	int INDEX;

	std::string weapon;

	std::vector<int>Modelindex;

	std::vector<std::string>models;
};

extern std::vector<SC_ModelInfo>GameModels;

extern void SkinChanger(CBaseEntity* pLocal);

#endif