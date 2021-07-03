#ifndef Config_H
#define Config_H

#include <string>
#include <vector>
#include "Json/Single_include/Nlohmann/Json.hpp"

namespace Config
{
	void SaveConfig();

	void LoadConfig();
}

#endif