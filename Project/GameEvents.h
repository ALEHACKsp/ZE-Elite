#pragma once
#include "SDK.h"
#include "Assorted.h"
#include "HitSound.h"
#include "Esp.h"
#pragma comment(lib, "Winmm.lib") 

inline std::vector<std::string> Killmessages = {
		"LOL, you died like a chicken",
		"$nick deserved to be killed" };

inline float PointScale = 0.03f;

class CEventListener
{
	class BulletListener : IGameEventListener2
	{
	public:

		void Start()
		{
			//EventManager->AddListener(this, "player_hurt"), false);
			//EventManager->AddListener(this, "player_death"), false);
			//EventManager->AddListener(this, "player_spawn"), false);
			//EventManager->AddListener(this, "bullet_impact"), false);
		}

		void Stop()
		{
			/*EventManager->RemoveListener(this);

			for (auto i = 0; i < EventManager->m_GameEvents.Count(); i++)
			{
				auto& Descriptor = EventManager->m_GameEvents[i];

				for (auto J = 0; J < Descriptor.listeners.Count(); J++)
				{
					auto Listener = Descriptor.listeners[J];

					if (Listener->m_pCallback == this)
					{
						Descriptor.listeners.FindAndRemove(Listener);
					}
				}
			}

			for (auto i = 0; i < EventManager->m_Listeners.Count(); i++)
			{
				auto& Listener = EventManager->m_Listeners[i];

				if (Listener->m_pCallback == this)
				{
					EventManager->m_Listeners.FindAndRemove(Listener);
				}
			}*/
		}

		void FireGameEvent(IGameEvent* event) override
		{
			if (auto pLocal = CBaseEntity::GetLocalPlayer(); pLocal && event && !Menu::Get.General.Panic) {

				auto EventName = event->GetName();

				if (!strcmp(EventName, "player_spawn"))
				{
					int Spawned_INDEX = Engine->GetPlayerForUserID(event->GetInt("userid"));

					if (auto Entity = GetEntity(Spawned_INDEX); Entity)
					{
						if (Spawned_INDEX > 0 && Spawned_INDEX < HealthSprite::SpriteEntities.size()) {

							HealthSprite::SpriteEntities[Spawned_INDEX].MaxHealth = Entity->GetHealth();
						}
					}
				}

				if (!strcmp(EventName, "bullet_impact"))
				{
					if (auto PlayerIndex = Engine->GetPlayerForUserID(event->GetInt("userid")); PlayerIndex == Engine->GetLocalPlayer()) {

						Vector Position(event->GetFloat("x"), event->GetFloat("y"), event->GetFloat("z"));

						constexpr auto& Bullet_Impact{ Menu::Get.Colors.General.BulletImpact };

						if (Bullet_Impact.a()) {

							ConMsg("HAHa\n");

							DebugOverlay->AddBoxOverlay(Position, Vector(-2, -2, -2), Vector(2, 2, 2), QAngle(0, 0, 0), Bullet_Impact.r(), Bullet_Impact.g(), Bullet_Impact.b(), Bullet_Impact.a(), 4);
						}

						if (Menu::Get.Visuals.Tracers.TR_Enabled) {

							ConMsg("HAHa\n");

							DrawBeamd(pLocal->GetEyePosition() - Vector(0, 0, PointScale), Position, Menu::Get.Colors.General.Tracers);
						}
					}
				}


				if (!strcmp(EventName, "player_hurt"))
				{
					int attacker = event->GetInt("attacker");

					int dmg_health = event->GetInt("dmg_health");

					int Hurted_INDEX = Engine->GetPlayerForUserID(event->GetInt("userid"));

					/*if (Hurted_INDEX > 0 && Hurted_INDEX < 65)
					{
						int Health = 100;

						if (auto Entity = GetEntity(Hurted_INDEX); Entity) Health = Entity->GetHealth();

						HealthSprite::SpriteEntities[Hurted_INDEX].MaxHealth = max(HealthSprite::SpriteEntities[Hurted_INDEX].MaxHealth, Health);

						HealthSprite::OnPlayerHurt(HealthSprite::SpriteEntities[Hurted_INDEX], Health - dmg_health);
					}*/

					if (Engine->GetPlayerForUserID(attacker) == Engine->GetLocalPlayer())
					{
						if (Menu::Get.Visuals.DamageIndicator.DI_Enabled)
						{
							DamageIndicator::data = DamageIndicator_t{ dmg_health, pLocal->GetTickBase() * Globals->interval_per_tick + 1.f };
						}

						if (Menu::Get.Visuals.Hitmarker.HM_Enabled)
						{
							if (Menu::Get.Visuals.HitSound)
							{
								auto Temp = HitSounds.size() - 2;

								Engine->ClientCmd_Unrestricted_Formatted("play \"HitSounds\\%s\"", HitSounds[Menu::Get.Visuals.HitSound >= Temp ? RandomInt(1, Temp) : Menu::Get.Visuals.HitSound].c_str());
							}

							if (Menu::Get.Visuals.Mode)
							{
								Engine->ClientCmd_Unrestricted_Formatted("r_screenoverlay \"overlays\\hitmarkers\\hitmarker%i\"", Menu::Get.Visuals.Overlay);

								Overlay_Triggered = true;
							}

							_flHurtTime = Globals->curtime;
						}
					}
				}

				if (!strcmp(EventName, "player_death"))
				{
					int DIED_INDEX = Engine->GetPlayerForUserID(event->GetInt("userid")), KILLER_INDEX = Engine->GetPlayerForUserID(event->GetInt("attacker"));

					CBaseEntity* Died_Player = GetEntity(DIED_INDEX), * Killer = GetEntity(KILLER_INDEX);

					if (Died_Player && Killer && Killer == pLocal) {

						if (Menu::Get.Misc.KillMessage && Killmessages.size())
						{
							std::string message = Killmessages[std::rand() % Killmessages.size()];

							if (strstr(message.c_str(), "$nick"))
							{
								std::string dead_player_name = std::string(Died_Player->GetName());

								dead_player_name.erase(std::remove(dead_player_name.begin(), dead_player_name.end(), ';'), dead_player_name.end());
								dead_player_name.erase(std::remove(dead_player_name.begin(), dead_player_name.end(), '"'), dead_player_name.end());
								dead_player_name.erase(std::remove(dead_player_name.begin(), dead_player_name.end(), '\n'), dead_player_name.end());

								message = ReplaceString(message, "$nick", dead_player_name);
							}

							Engine->ClientCmd_Unrestricted_Formatted("say \"%s\"", message.c_str());
						}

						if (Menu::Get.Visuals.KillEffect_Type > 0) {

							auto HeadHitBox(Died_Player->GetHitboxPosition(12));

							auto Angles = Died_Player->GetAbsAngles();

							static CTeslaInfo teslaInfo;

							static CEffectData data;

							[[maybe_unused]] static const auto Once = []()
							{
								data.m_fFlags = 6;
								data.m_vNormal.Init(-1.f, 1.f, -2.f); //some shit I don't care about
								data.m_flScale = 2.f;
								data.m_nColor = 3; //not actual color, but what kind of blood spray that would be (headshot, knife impact, etc)

								teslaInfo.m_flBeamWidth = 5.f;

								teslaInfo.m_flRadius = 100.f;

								teslaInfo.m_vColor.Init(1.f, 1.f, 1.f); //your value up to 255 / 255.f

								teslaInfo.m_flTimeVisible = 0.75f;

								teslaInfo.m_nBeams = 12;

								return 0;
							}();

							teslaInfo.m_pszSpriteName = "sprites/physbeam.vmt";

							data.m_nEntIndex = teslaInfo.m_nEntIndex = DIED_INDEX;

							data.m_vAngles = teslaInfo.m_vAngles = Angles;

							data.m_vOrigin = teslaInfo.m_vPos = HeadHitBox;

							static auto DispatchEffect = reinterpret_cast<void(*)(const char*, const CEffectData&)>(
								CallableFromRelative(Tools::FindPattern("client.dll", "E8 ? ? ? ? 83 C4 08 EB 15")));

							if (Menu::Get.Visuals.KillEffect_Type == 1)
							{
								FX_Tesla(teslaInfo);

								DispatchEffect("csblood", data);
							}
							else
							{
								DispatchEffect(KillEffects[Menu::Get.Visuals.KillEffect_Type], data);
							}

							Surface->Play_Sound("ambient/energy/zap3.wav");
						}
					}
				}
			}
		}
	};
public:

	static CEventListener* Singleton()
	{
		static CEventListener* instance = new CEventListener();

		return instance;
	}

	void Start()
	{
		Listener.Start();
	}
	void End()
	{
		Listener.Stop();
	}

	BulletListener Listener;
};
