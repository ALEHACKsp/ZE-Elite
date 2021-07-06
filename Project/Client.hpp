#ifndef CHLClient_H
#define CHLClient_H

#include "include.h"
#include "SDK.h"
#include <optional>
#include "Trails.h"
#include "ClientEntityListener.h"
#include "SkinChanger.h"
#include "Backtrack.h"
#include "Soundpad.h"

extern std::vector < dlight_t* > dLights;

extern float Global_Jumps, Perfect_Jumps;

extern CSpriteTrail* SpriteEntity;

extern CBaseEntity* RainEntity;

extern void* RainNetworkable;

bool Was_On_Ground_Backup{ 0 };

extern bool Toggled_On;

/*static float GetLerpTime()
{
	static const ConVar* cl_updaterate = Cvar->FindVar("cl_updaterate");

	static const ConVar* pMinUpdateRate = Cvar->FindVar("sv_minupdaterate");

	static const ConVar* pMaxUpdateRate = Cvar->FindVar("sv_maxupdaterate");

	static const ConVar* cl_interpolate = Cvar->FindVar("cl_interpolate");

	static const ConVar* cl_interp_ratio = Cvar->FindVar("cl_interp_ratio");

	static const ConVar* cl_interp = Cvar->FindVar("cl_interp");

	auto m_nUpdateRate = cl_updaterate->GetFloat();

	if (pMinUpdateRate && pMaxUpdateRate)
		m_nUpdateRate = (int)std::clamp(m_nUpdateRate, pMinUpdateRate->GetFloat(), pMaxUpdateRate->GetFloat());

	bool useInterpolation = cl_interpolate->GetFloat() != 0;

	if (useInterpolation)
	{
		float flLerpRatio = cl_interp_ratio->GetFloat();

		if (flLerpRatio == 0)
			flLerpRatio = 1.0f;

		float flLerpAmount = cl_interp->GetFloat();

		static const ConVar* pMin = Cvar->FindVar("sv_client_min_interp_ratio");

		static const ConVar* pMax = Cvar->FindVar("sv_client_max_interp_ratio");

		if (pMin && pMax && pMin->GetFloat() != -1)
		{
			flLerpRatio = std::clamp(flLerpRatio, pMin->GetFloat(), pMax->GetFloat());
		}
		else
		{
			if (flLerpRatio == 0)
				flLerpRatio = 1.0f;
		}
		return max(flLerpAmount, flLerpRatio / m_nUpdateRate);
	}
	else
	{
		return 0.f;
	}
}*/

bool AntiSmac(CUserCmd* cmd) noexcept
{
	static auto Last_Angles{ cmd->viewangles };

	bool ReturnValue{ false };

	if (Menu::Get.Aimbot.AntiSmac && Menu::Get.Aimbot.Ragebot.Silent != 2)
	{
		auto snap{ cmd->viewangles - Last_Angles };

		Math::NormalizeAngle(snap);

		if (snap.Normalized() >= 15.f)
		{
			cmd->viewangles = Last_Angles + snap * 15.f;

			Math::NormalizeAngle(cmd->viewangles);

			ReturnValue = true;
		}
	}

	Last_Angles = cmd->viewangles;

	return ReturnValue;
}

namespace CHLClient
{
	// Original_CreateMove

	using Init_Type = void(__thiscall*)(void*);

	Init_Type Original_Init;

	// Original_CreateMove

	using Shutdown_Type = void(__thiscall*)(void*);

	Shutdown_Type Original_Shutdown;

	// Original_CreateMove

	using CreateMove_Type = void(__stdcall*)(int, float, bool);

	CreateMove_Type Original_CreateMove;

	// Original_FrameStageNotify

	using FrameStageNotify_Type = void(__thiscall*)(void*, int);

	FrameStageNotify_Type Original_FrameStageNotify;

	// Original_DispatchUserMessage

	using DispatchUserMessage_Type = bool(__thiscall*)(void*, int, void*&);

	DispatchUserMessage_Type Original_DispatchUserMessage;

	void __stdcall LevelInitPostEntity()
	{
		//dLights.clear();

		EntitiesListener.clear();

		Toggled_On = _flHurtTime = 0.f;

		void OnLevelInit(); OnLevelInit();

		for (auto& SoundInfo : SoundList) SoundInfo.Reset();

		for (auto&& sprite : HealthSprite::SpriteEntities)
		{
			if (sprite.Entity)
			{
				sprite.Entity = nullptr;
			}
		}SpriteEntity = nullptr;

		DamageIndicator::data.flEraseTime = 0.f;  return Original_Init(BaseClientDLL);
	}

	void __stdcall LevelShutdown(void*, void*)
	{
		//dLights.clear();

		RainEntity = nullptr;

		RainNetworkable = nullptr;

		return Original_Shutdown(BaseClientDLL);
	}

	void __stdcall CreateMove(int sequence_number, float input_sample_frametime, bool active, bool& bSendPacket) noexcept
	{
		CBaseEntity* pLocal{ CBaseEntity::GetLocalPlayer() };

		CUserCmd* cmd = GlobalVars::cmd = { &Input->m_pCommands[sequence_number % MULTIPLAYER_BACKUP] };

		CVerifiedUserCmd* verified{ &Input->m_pVerifiedCommands[sequence_number % MULTIPLAYER_BACKUP] };

		if (!cmd || !verified || !pLocal || Menu::Get.General.Panic)
		{
			return;
		}

		QAngle ViewAngles_Pre_CreateMove{ cmd->viewangles }; Original_CreateMove(sequence_number, input_sample_frametime, active);

		if (Menu::Get.Aimbot.Ragebot.Silent && Aimbot::LoopCount >= 1 && ClientState->chokedcommands < 14)
		{
			QAngle ViewAngles_Post_CreateMove{ cmd->viewangles }; cmd->viewangles = ViewAngles_Pre_CreateMove;

			Math::CorrectMovement(ViewAngles_Post_CreateMove, cmd, cmd->forwardmove, cmd->sidemove); Aimbot::LoopCount--;
		}

		if (ClientState->chokedcommands < 14)
		{
			Aimbot::Run(pLocal, cmd, bSendPacket);

			Was_On_Ground_Backup = pLocal->OnGround();

			Miscellaneous::Run(pLocal, cmd, bSendPacket);

			AutoLaser::Run(pLocal, cmd, bSendPacket);

			Miscellaneous::EnginePrediction(pLocal, cmd);

			void BunnyHop(CBaseEntity*, CUserCmd*) noexcept; BunnyHop(pLocal, cmd);

			void EdgeJump(CBaseEntity*, CUserCmd*) noexcept; EdgeJump(pLocal, cmd);

			void StrafeOptimizer(CBaseEntity*, CUserCmd*); StrafeOptimizer(pLocal, cmd);

			void Bullet_Accuracy(CBaseEntity*, CUserCmd*, bool&) noexcept; Bullet_Accuracy(pLocal, cmd, bSendPacket);

			AntiSmac(cmd);
		}

		verified->m_cmd = *cmd;

		verified->m_crc = cmd->GetChecksum();
	}

	inline void UpdateSlowly(CBaseEntity* pLocal)
	{
		void ExpensiveTrail(CSpriteTrail*&, CBaseEntity*); 

		if (Menu::Get.Visuals.Trails.TR_Enabled) ExpensiveTrail(SpriteEntity, pLocal);
		else 
		if (SpriteEntity) { SpriteEntity->Remove(); SpriteEntity = nullptr; }
	}

	void __stdcall FrameStageNotify(int curStage) noexcept
	{
		if (CBaseEntity* MyPlayer{ CBaseEntity::GetLocalPlayer() }; MyPlayer) {

			if (curStage == FRAME_NET_UPDATE_POSTDATAUPDATE_START)
			{
				RunSoundpad();
				
				if (MyPlayer->IsAlive())
				{
					SkinChanger(MyPlayer);

					HealthSprite::ValidateSprites(MyPlayer);

					auto CurTickCount = TickCount;

					if (static auto LastUsedTick = CurTickCount; CurTickCount > LastUsedTick + 1500)
					{
						UpdateSlowly(MyPlayer); LastUsedTick = CurTickCount;
					}

					void Auto_Kevlar(CBaseEntity * MyPlayer); Auto_Kevlar(MyPlayer);
				}
				
			}

			if (curStage == FRAME_RENDER_START)
			{
				void Precipitation(); Precipitation();

				if (MyPlayer->IsAlive())
				{
					SkinChanger(MyPlayer);

					MyPlayer->GetFlashMaxAlpha() = 255.0f - Menu::Get.Visuals.FlashPercentage * 2.55f;

					if (Menu::Get.Visuals.NoVisualRecoil)
					{
						auto Backup_AimPunch = MyPlayer->GetPunchAngle();

						MyPlayer->GetPunchAngle().Init();

						Original_FrameStageNotify(BaseClientDLL, curStage);

						MyPlayer->GetPunchAngle() = Backup_AimPunch;

						return;
					}
				}
			}
		}

		Original_FrameStageNotify(BaseClientDLL, curStage);
	}

	static bool __stdcall DispatchUserMessage(int messageType, void*& msg_data) noexcept
	{
		if (Menu::Get.Misc.NoMotd && messageType == CS_UM_VGUIMenu)
		{
			const char* VGUIMenu_Name = *(const char**)(&msg_data);

			if (!strcmp(VGUIMenu_Name, "info"))
			{
				Engine->ClientCmd_Unrestricted("joingame");

				return true;
			}
		}

		if (!messageType
			|| (messageType == CS_UM_Shake && Menu::Get.Misc.NoShake)
			|| (messageType == CS_UM_Fade && Menu::Get.Misc.NoDrug)
			|| ((messageType == CS_UM_RadioText || messageType == CS_UM_SendAudio) && Menu::Get.Misc.MuteRadio))
			return true;

		return Original_DispatchUserMessage(BaseClientDLL, messageType, msg_data);
	}


	void __declspec(naked) __stdcall Hooked_CreateMove(int sequence_number, float input_sample_frametime, bool active) noexcept
	{
		__asm
		{
			LEA  EAX, [EBP - 01]
			PUSH EAX
			PUSH[ESP + 0x10]
			PUSH[ESP + 0x10]
			PUSH[ESP + 0x10]
			CALL CreateMove
			RET 0x0C
		}
	}
}

#endif
