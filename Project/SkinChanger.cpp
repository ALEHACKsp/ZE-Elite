#include "SDK.h"
#include "Assorted.h"
#include "SkinChanger.h"

void SkinChanger(CBaseEntity* pLocal)
{
	//static auto SetWeaponModel = reinterpret_cast<void(__thiscall*)(void*, const char*, void*)>(Tools::FindPattern("client.dll"),
		//"55 8B EC 56 8B F1 8B 4D 0C 85 C9 74 09"));

	//static auto SetModel = reinterpret_cast<void(__thiscall*)(void*, const char*)>(
		//UINT(CallableFromRelative(Tools::FindPattern("client.dll", "E8 ? ? ? ? 8B 06 8B CE FF 50 7C")));

	//static auto GetViewModel = reinterpret_cast<void* (__thiscall*)(void*, int, int)>(ClientModule + 0xA2630);

	auto ViewModel = EntityList->GetClientEntityFromHandle(pLocal->GetViewModel());
	if (!ViewModel) return;

	auto ActiveWeapon = pLocal->GetActiveWeapon();
	if (!ActiveWeapon) return;

	auto ActiveWeaponModelPath = GetModelName(ActiveWeapon->GetModel());

	for (auto&& Model : GameModels) {

		auto SizeOfModels = Model.models.size();

		if (SizeOfModels > 1 && strstr(ActiveWeaponModelPath, Model.weapon.c_str())) {

			if (auto ID = Model.INDEX; ID > -1 && ID < Model.Modelindex.size())
			{
				auto ModelIndex = Model.Modelindex[ID];

				if (ModelIndex != -1 && ViewModel->GetModel() != ModelInfo->GetModel(ModelIndex))
				{
					ViewModel->SetModelByIndex(ModelIndex);
				}
			}

			break;
		}
	}
}
