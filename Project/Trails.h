#ifndef Trails_H
#define Trails_H

#include "SDK.h"
#include "Assorted.h"

CSpriteTrail* SpriteEntity = nullptr;

void ExpensiveTrail(CSpriteTrail*& SpriteEnt, CBaseEntity* pParent) {

    if (SpriteEnt == 0)
    {
        static auto CreateEntityByName = reinterpret_cast<CSpriteTrail * (*)(const char* className)>(Tools::FindPattern("client.dll", "8B EC E8 ? ? ? ? FF 75 08 8B C8") - 1);

        SpriteEnt = CreateEntityByName("env_spritetrail");

        if (SpriteEnt)
        {
            SpriteEnt->PreDataUpdate(0);

            SpriteEnt->OnPreDataChanged(0);

            SpriteEnt->GetStartWidth() = Menu::Get.Visuals.Trails.Expensive.Width;

            SpriteEnt->GetTextureRes() = 0.05;

            SpriteEnt->GetEndWidth() = Menu::Get.Visuals.Trails.Expensive.EndWidth;

            SpriteEnt->GetLifeTime() = Menu::Get.Visuals.Trails.Expensive.LifeTime;

            SpriteEnt->GetRenderMode() = Menu::Get.Visuals.Trails.Expensive.RenderMode;

            SpriteEnt->OnDataChanged(0);

            SpriteEnt->FollowEntity(pParent, 1);

            if (!TrailMaterials.size()) return;

            std::string Sprite("sprites/trails/");

            Sprite.append(TrailMaterials[Menu::Get.Visuals.Trails.SpriteTexture]);

            Sprite.append(".vmt");

            auto ModelIndex = ModelInfo->GetModelIndex(Sprite.c_str());

            if (ModelIndex == -1)
            {
                if (PrecacheModel(Sprite.c_str()))
                {
                    ModelIndex = ModelInfo->GetModelIndex(Sprite.c_str());
                }
            }

            if (ModelIndex != -1)
            {
                SpriteEnt->SetModelByIndex(ModelIndex);
            }
        }
    }
}

#endif
