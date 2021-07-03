#pragma once

#ifndef CEntityListener_H
#define CEntityListener_H

#include "AutoLaser.h"
#include "SDK.h"

#include <vector>

struct Entities
{
    Entities(int userids)
    {
        userid = userids;

        Origin.Zero(); LocalPlayerOrigin.Zero();

        IgnoreTicks = Action_Tick = ReleaseTick = Init = Jumped = Ducked = false;
    }

    void Reset()
    {
        Origin.Zero(); LocalPlayerOrigin.Zero();

        IgnoreTicks = Action_Tick = ReleaseTick = Init = Jumped = Ducked = false;
    }

    Vector Origin, LocalPlayerOrigin;

    bool Init, Jumped, Ducked;

    unsigned short userid; int ReleaseTick, Action_Tick, IgnoreTicks;
};

extern std::vector<Entities> EntitiesListener;

class CClientEntityListener : public IClientEntityListener
{
    void OnEntityCreated(CBaseEntity* pEntity) override
    {
        if (Menu::Get.General.AutoLaser && pEntity)
        {
            if (Engine->IsConnected() && Engine->IsInGame())
            {
                if (auto Index = pEntity->GetIndex(); Index && Index != -1)
                {
                    EntitiesListener.push_back(Entities(Index));
                }
            }
        }
    }
};



class ClientEntityListner
{
    class EntityListener
        : public CClientEntityListener
    {
    public:
        void Start()
        {
            EntityList->AddEntityListner(this);
        }
        void Stop()
        {
            EntityList->RemoveEntityListner(this);
        }
    };
public:

    static ClientEntityListner* Singleton()
    {
        static ClientEntityListner* instance = new ClientEntityListner;
        return instance;
    }


    void Initialize()
    {
        _listener.Start();

    }
    void End()
    {
        _listener.Stop();
    }

private:
    EntityListener    _listener;
};

#endif