#pragma once

#ifndef MENU
#define MENU

#include <d3d9.h>

namespace Menu
{
	struct Variables
	{
		struct
		{
			bool AntiSmac{ false };

			struct
			{
				bool Enabled{ false };

				bool FriendlyFire{ false };

				bool AutoShoot{ false };

				bool AutoStop{ false };

				bool AutoCrouch{ false };

				bool Predicted{ false };

				int TargetSelection { 0 };

				float SmoothVal{ 1.f };

				int Timeout{ 0 };

				int MaxAimStep{ 42 };

				int HitBox{ 12 };

				int Silent{ 0 };

				int Usage{ 0 };

				int FOV{ 180 };

			}Ragebot;
		}Aimbot;

		struct
		{
			bool AutoLaser{ false };

			bool WireFrame_Lasers{ false };

			int Filter{ 2 };

			bool Panic{ false };

			bool Unload{ false };

		}General;

		struct
		{
			int NameFont{ 0 };
			int HealthFont{ 0 };
			int WeaponFont{ 0 };

			bool Box{ false };

			int BoxType{ false };

			bool Health{ false };

			float HP_SpriteScale{ 0.10f };

			int HealthType{ 2 };

			bool Ammo{ false };

			bool Weapon{ false };

			int WeaponRenderingMode{ 1 };

			bool Skeleton{ false };

			bool SnapLine{ false };

			bool Name{ false };

			struct
			{
				bool Enabled{ false };

				bool INDEX{ false };

				bool Classname{ false };

				bool ModelName{ false };

				bool ModelIndex{ false };

				bool Velocity{ false };

				bool Origin{ false };

				int MaxDistance{ 1500 };

				bool Filter[6]{ false,false,false,false,false,false };

			}Debug;

			bool PlayerSpeed{ false };

			struct
			{
				bool  DI_Enabled{ false };

				float Lifetime{ 3.f };

				float Speed{ 1.f };

			}DamageIndicator;

			bool KillEffect{ false };

			int KillEffect_Type{ 0 };

			int ScopeLen{ 0 };

			struct {

				bool Enabled{ false };

				float start{ -1 }; 

				float end{ -1 };

				float maxdensity{ 0.7f };

			}FOG;

			struct {

				bool TR_Enabled{ false };

				int SpriteTexture{ 0 };

				float m_flHaloScale{ 1.f };

				float m_flLife{ 0.8f };

				float m_flWidth{ 0.7f };

				float m_flEndWidth{ 0.5f };

				float m_flFadeLength{ 55.0f };

				float m_flAmplitude{ 0.4f };

				float m_flSpeed{ 0.2f };

				int m_nSegments{ 4 };

				int m_nFlags{ 0 };

			} Tracers;

			struct {

				bool TR_Enabled{ false };

				int SpriteTexture{ 0 };

				int Price{ 1 };

				struct
				{
					float m_flHaloScale{ 0 };

					float m_flLife{ 0.8f };

					float m_flWidth{ 1.5f };

					float m_flEndWidth{ 1.5f };

					float m_flFadeLength{ 10.f };

					float m_flAmplitude{ 0.f };

					float m_flSpeed{ 0.f };

					int m_nSegments{ -1 };

					int m_nFlags{ 0 };
				}Cheap;

				struct
				{
					float Width{ 15.f }, EndWidth{ 15.f }, LifeTime{ 0.65f };

					int RenderMode{ 5 };
				}Expensive;

			} Trails;

			struct
			{

				bool HM_Enabled{ false };

				int Time{ 930 };

				int Length{ 7 };

				int Gap{ 2 };

			}Hitmarker;

			struct
			{

				bool Recoil{ false };

				struct
				{
					bool Enabled{ false };

					int Length{ 10 };

					int Gap{ 3 };
				}X_Shape;

				struct
				{
					bool Enabled{ false };

					int Length{ 10 };

					int Gap{ 3 };
				}Plus_Shape;

				struct
				{
					bool Enabled{ false };

					int Radius{ 10 };

					int Segments{ 10 };

				}Circle_Shape;

				//bool DOT_Shape{ false };

			}Crosshair;

			int NightMode{ 1 };

			int MuzzleFlash{ 0 };

			int AsusWalls{ 1 };

			bool NightVision{ false };

			int NightVisionAlpha{ 0 };

			int HitSound{ 0 };

			int Mode{ 0 };

			int Overlay{ 0 };

			bool Special_Item{ false };

			bool FullBright{ false };

			int Backtrack{ 0 };

			int FlashPercentage{ 100 };

			int ViewModelFov{ 0 };

			int FieldOfView{ 0 };

			bool NoFog{ false };

			int Hands{ 0 };

			bool Disable_Fire_Particles{ false };

			bool Disable_Players_MuzzleFlash{ false };

			bool NoTeammates{ false };

			bool Nodecals{ false };

			bool NoFlashLight{ false };

			bool NoSmoke{ false };

			bool NoShadow{ false };

			bool NoSky{ false };

			int DLights{ 0 };

			bool NadeTracer{ false };

			bool NoVisualRecoil{ false };

			struct
			{
				bool Active{ false };

				int Type{ 1 };

				float Width{ 3.f };

				float Speed{ 1000.f };

				float Radius{ 1000.f };

				float Length{ 0.1f };


			}Weather;

			struct
			{
				bool RD_Enabled{ false };

				bool InGame_Bypass{ false };

				bool Bomb{ false };

			}Radar;

			struct
			{
				struct
				{
					bool CMP_Enabled{ false };

					bool CMP_Wireframe{ false };

					bool CMP_Ignorez{ false };

					int CMP_Type{ 1 };

				}Player;

				struct
				{
					bool CMW_Enabled{ false };

					bool CMW_Wireframe{ false };

					bool CMW_Ignorez{ false };

					int CMW_Type{ 3 };

				}Weapon;

				struct
				{
					bool CMB_Enabled{ false };

					bool CMB_Wireframe{ false };

					bool CMB_Ignorez{ false };

					int CMB_Type{ 1 };

				}Backtrack;
			}Chams;

			struct
			{
				bool VM_Enabled{ false };

				Vector Origin{ 0,0,0 };

				Vector Angles{ 0,0,0 };
			}ViewModel;

		}Visuals;


		struct
		{
			bool NoMotd{ false };

			bool NoDrug{ false };

			bool NoShake{ false };

			int Ragdoll_Force{ 100 };

			int Ragdoll_Velocity{ 10 };

			bool MuteRadio{ false };

			bool NoRecoil{ false };

			bool NoSpread{ false };

			bool AutoPisol{ false };

			bool Faststop{ false };

			bool Disable_Game_Console_Warnings{ false };

			bool Fastrun{ false };

			bool AutoKevlar{ false };

			bool Bunnyhop{ false };

			bool Autostrafe{ false };

			bool CircleStrafe{ false };

			bool Edgejump{ false };

			bool BlockBot{ false };

			bool AntiAFKkick{ false };

			bool LagExploit{ false };

			bool FastReload{ false };

			bool FastLadder{ false };

			bool FastSwitch{ false };

			bool InstantDefuse{ false };

			bool InstantExplosion{ false };

			bool Legit_Bunnyhop_Booster{ false };

			bool Sv_Pure_Bypass{ false };

			bool ThirdPerson{ false };

			int m_vecCameraOffset{ 0 };

			int Bunnyhop_Perfect_Rate{ 100 };

			bool KillMessage{ false };

			struct
			{
				int MusicVolume{ 50 }, FootStepsVolume{ 50 }, WeaponsAudio{ 50 };
			}Sounds;


			struct
			{
				__int8 Enabled;

				float  Desired_Gain;

				float Required_Speed;

				float Greatest_Possible_Strafe_Angle;
			}
			StrafeOptimizer;
			

			struct
			{
				bool BT_Enabled{ false };

				float TimeLimit{ 200.f };

				bool DisableInterpolation{ false };

				bool Triggerbot{ false };

				bool FakeLatency{ false };

				int FakeLatencyValue{ 0 };
			}Backtrack;

			struct
			{
				bool TB_Enabled{ false };

				int TB_Usage{ 0 };

				bool Filter[6]{ false, false, false, false, false, false };

			}Triggerbot;

			struct
			{
				bool AS_Enabled{ false };

				int AS_Usage{ 0 };

			}AirStuck;

			struct
			{
				bool Custom{ false };

				float ClientSideValue{ 0.015f };

				float ServerSideValue{ 0.015f };

			}cl_interp;

			struct
			{
				bool SIP_Enabled{ false };

				bool RandomSteamID{ false };

				int SteamID{ 0 };

			}SteamIDSpoofer;

			struct
			{
				bool KB_Enabled{ false };

				int Method{ 0 };

				int KB_Usage{ 1 };

			}KnifeBot;

			struct
			{
				bool Changer{ false };

				bool Stealer{ false };

				float Delay{ 5.f };

			}ClanTag;

			int DownloadManagerFilter{ 1 };

			char CustomizeableFiles[100];

		}Misc;

		struct
		{
			struct
			{
				Color Box{ Color::Red() };

				Color Name{ Color::White() };

				Color Health{ Color::Yellow() };

				Color Weapon{ Color::Silver() };

				Color SnapLine{ Color::Blue() };

				Color Skeleton{ Color::Firebrick() };

				Color DLights{ 0 ,40,255 };

			}PlayerEsp;

			struct
			{
				Color Player{ Color::Green().r(),Color::Green().g(),Color::Green().b(), 200 };

				Color CWeapon{ Color::SkyBlue().r(),Color::SkyBlue().g(),Color::SkyBlue().b(), 200 };

				Color Backtrack{ Color::Gold().r(),Color::Gold().g(),Color::Gold().b(), 200 };
			}Chams;

			struct
			{
				Color SkyBox{ Color(255,255,255) };

				Color World{ Color(255,255,255) };

				Color DamageIndicator{ 255, 255, 255, 255 };

				Color Nightvision{ Color(0,123,255,0) };

				Color NadeTracer{ Color(0,123,255,0) };

				Color BulletImpact{ Color(255, 0, 0,0) };

				struct
				{
					Color X_Shape{ 50,255,50,200 };

					Color Plus_Shape{ 50,255,50,200 };

					Color Circle_Shape{ 50,255,50,200 };

				}Crosshair;

				Color Hitmarker{ Color::Yellow() };

				Color Tracers{ 135, 206, 235,255 };

				Color Fog{ 135, 206, 235,255 };

				Color Trails{ Color(255,255,255) };

				Color HudColor{ Color::SkyBlue() };

			}General;

		}Colors;

		struct
		{
			bool On{ false };

			const char* windowClassName{ "Valve001" };
		}Menu;

		struct
		{
			unsigned char RageBot{ 0 };

			unsigned char AirStuck{ 0 };

			unsigned char Edgejump{ 0 };

			unsigned char Bunnyhop{ 0 };

			unsigned char Triggerbot{ 0 };

			unsigned char Autostrafe{ 0 };

			unsigned char CircleStrafe{ 0 };

			unsigned char Unload{ VK_END };

			unsigned char PanicKey{ VK_F11 };

			unsigned char BlockBot{ 0 };

			unsigned char KnifeBot{ 0 };

			unsigned char InstantExplosion{ 0 };

		}Keys;
	};

	extern Variables Get;

	void ZE() noexcept;
	void Menu() noexcept;
	void Render() noexcept;
	void Aimbot() noexcept;
	void Colors() noexcept;
	void ConVars() noexcept;
	void Visuals() noexcept;
	void RenderTabs() noexcept;
	void TriggerBot() noexcept;
	void Miscellaneous() noexcept;
}

#endif
