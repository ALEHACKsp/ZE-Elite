#ifndef HealthSprite_H
#define HealthSprite_H

#include <array>
#include "SDK.h"
#include "Assorted.h"

namespace HealthSprite
{
	class MyCSprite
	{
	public:
		//MyCSprite(CSprite* pEntity) : MaxHealth(0), Entity(pEntity) {}

		int MaxHealth = 0;

		CSprite* Entity = nullptr;
	};

	extern std::array<MyCSprite, 65> SpriteEntities;

	inline void UpdateSpritesScale()
	{
		for (auto&& sprite : SpriteEntities)
		{
			if (sprite.Entity)
			{
				sprite.Entity->GetSpriteScale() = Menu::Get.Visuals.HP_SpriteScale;
			}
		}
	}

	inline bool IsFoundInSpriteList(CSprite* Entity)
	{
		for (auto&& sprite : SpriteEntities)
		{
			if (sprite.Entity == Entity)
			{
				return true;
			}
		}

		return false;
	}

	inline void ClearSprites()
	{
		for (auto&& sprite : SpriteEntities)
		{
			if (sprite.Entity)
			{
				sprite.Entity->Remove();

				sprite.Entity = nullptr;
			}
		}
	}

	inline void OnPlayerHurt(MyCSprite& sprite, int Health)
	{
		auto& pEntity = sprite.Entity;

		if (!pEntity) return;

		if (Health < 1)
		{
			pEntity->Remove();

			pEntity = nullptr;

			return;
		}

		float MaxHealth = sprite.MaxHealth;

		auto Total{ std::round((Health / MaxHealth) * 100.f) };

		int modelIndex = 6;

		if (Total >= 100) {

			modelIndex = 1;

		}
		else if (Total >= 80) {

			modelIndex = 2;
		}
		else if (Total >= 60) {

			modelIndex = 3;
		}
		else if (Total >= 40) {

			modelIndex = 4;
		}
		else if (Total > 20) {

			modelIndex = 5;
		}

		std::string Sprite("sprites/franug/hp_bar_2/hp_bar");

		Sprite.append(std::to_string(modelIndex));

		Sprite.append(".vmt");

		auto ModelIndex = ModelInfo->GetModelIndex(Sprite.c_str());

		if (ModelIndex == -1)
		{
			if (PrecacheModel(Sprite.c_str()))
			{
				ModelIndex = ModelInfo->GetModelIndex(Sprite.c_str());
			}
		}

		assert(ModelIndex != -1);

		if (ModelIndex != -1 && ModelIndex != pEntity->GetModelIndex())
		{
			pEntity->SetModelByIndex(ModelIndex);
		}
	}

	inline void CreateSpriteForEntity(CBaseEntity* pEntity) {

		auto Index = pEntity->GetIndex();

		auto& pSprite = SpriteEntities[Index];

		static auto CreateEntityByName = reinterpret_cast<CSpriteTrail * (*)(const char* className)>(Tools::FindPattern("client.dll", "8B EC E8 ? ? ? ? FF 75 08 8B C8") - 1);

		if (auto Entity = CreateEntityByName("env_sprite"); Entity)
		{
			pSprite.Entity = Entity;

			Entity->FollowEntity(pEntity, 1);

			Entity->PreDataUpdate(0);

			Entity->OnPreDataChanged(0);

			Entity->GetSpriteScale() = Menu::Get.Visuals.HP_SpriteScale;

			Entity->GetRenderColor() = Color(255, 255, 255);

			Entity->GetRenderMode() = kRenderTransColor;

			Entity->OnDataChanged(0);

			OnPlayerHurt(pSprite, pEntity->GetHealth());
		}
	}

	inline void ValidateSprites(CBaseEntity* pLocal)
	{
		for (auto i = 0; i < SpriteEntities.size(); i++)
		{
			auto& sprite = SpriteEntities[i];

			if (auto Entity = GetEntity(i); Entity && !Entity->IsDormant() && Entity->IsPlayer() && Entity->IsAlive() && Entity->IsEnemy(pLocal) && Menu::Get.Visuals.Health && Menu::Get.Visuals.HealthType == 2)
			{
				if (auto SpriteEntity = sprite.Entity; !SpriteEntity)
				{
					CreateSpriteForEntity(Entity);
				}

				continue;
			}

			if (SpriteEntities[i].MaxHealth != 100)
			{
				SpriteEntities[i].MaxHealth = 100;
			}

			if (auto& Entity = sprite.Entity; Entity)
			{
				Entity->Remove();

				Entity = nullptr;
			}
		}

	}
}

#endif
