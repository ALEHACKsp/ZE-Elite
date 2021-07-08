#include "SDK.h"
#include <Windows.h>
#include <fstream>
#include <iomanip>
#include "Config.h"

nlohmann::json j;

extern int SkyBoxIndex;

#define Version "1.2"

#define ConfigA(arg1,arg2) {#arg1,arg2}

#define ConfigArray(Var,item)

#define LoadVar(varname,Var,group,type) Var = j.at(#group).at(#varname).get<decltype(Var)>();

#define LoadItemVar(varname,group,Var,item,type) Var[item] = j.at(#group).at(#varname).get<type>();

#define LoadItemVarFromSecondGroup(varname,group1,group2,Var,item,type) Var[item] = j.at(#group1).at(#group2).at(#varname).get<type>();

#define LoadItemVarFromThirdGroup(varname,group1,group2,group3,Var,item,type) Var[item] = j.at(#group1).at(#group2).at(#group3).at(#varname).get<type>();

#define LoadVarFromSecondGroup(varname,Var,group1,group2) Var = j.at(#group1).at(#group2).at(#varname).get<decltype(Var)>();

#define LoadVarFromThirdGroup(varname,Var,group1,group2,group3) Var = j.at(#group1).at(#group2).at(#group3).at(#varname).get<decltype(Var)>();

#define ColorConfigA(varnamr,Var) \
{(std::string(#varnamr) + "[0]"),Var._color[0]}, \
{(std::string(#varnamr) + "[1]"),Var._color[1]}, \
{(std::string(#varnamr) + "[2]"),Var._color[2]}, \
{(std::string(#varnamr) + "[3]"),Var._color[3]} \

#define LoadColorVar(varname,Var,group) \
Var._color[0] = j.at(#group).at(std::string(#varname) + "[0]").get<uint8_t>();\
Var._color[1] = j.at(#group).at(std::string(#varname) + "[1]").get<uint8_t>();\
Var._color[2] = j.at(#group).at(std::string(#varname) + "[2]").get<uint8_t>();\
Var._color[3] = j.at(#group).at(std::string(#varname) + "[3]").get<uint8_t>();\


BOOL DirectoryExists(LPCTSTR szPath)
{
	DWORD dwAttrib = GetFileAttributes(szPath);

	return (dwAttrib != INVALID_FILE_ATTRIBUTES &&
		(dwAttrib & FILE_ATTRIBUTE_DIRECTORY));
}

void Config::SaveConfig()
{
	if (!DirectoryExists("ZE"))
		CreateDirectory("ZE", nullptr);

	auto j = nlohmann::json
	{
		{ ("General"), {

			ConfigA(Filter, Menu::Get.General.Filter),

			ConfigA(AutoLaser, Menu::Get.General.AutoLaser),

			ConfigA(WireFrame_Lasers, Menu::Get.General.WireFrame_Lasers),
		} },
		{ ("Aimbot"), {

			ConfigA(AntiSmac, Menu::Get.Aimbot.AntiSmac),

			{ ("Ragebot"), {

			   ConfigA(AutoCrouch, Menu::Get.Aimbot.Ragebot.AutoCrouch),
     
			   ConfigA(AutoShoot, Menu::Get.Aimbot.Ragebot.AutoShoot),

			   ConfigA(AutoStop, Menu::Get.Aimbot.Ragebot.AutoStop),

			   ConfigA(Enabled, Menu::Get.Aimbot.Ragebot.Enabled),

			   ConfigA(FOV, Menu::Get.Aimbot.Ragebot.FOV),

			   ConfigA(FriendlyFire, Menu::Get.Aimbot.Ragebot.FriendlyFire),

			   ConfigA(HitBox, Menu::Get.Aimbot.Ragebot.HitBox),

			   ConfigA(MaxAimStep, Menu::Get.Aimbot.Ragebot.MaxAimStep),

			   ConfigA(Silent, Menu::Get.Aimbot.Ragebot.Silent),

			   ConfigA(SmoothVal, Menu::Get.Aimbot.Ragebot.SmoothVal),

			   ConfigA(Predicted, Menu::Get.Aimbot.Ragebot.Predicted),

			   ConfigA(TargetSelection, Menu::Get.Aimbot.Ragebot.TargetSelection),

			   ConfigA(Timeout, Menu::Get.Aimbot.Ragebot.Timeout),

			   ConfigA(Usage, Menu::Get.Aimbot.Ragebot.Usage),

		    } }

		} },
		{ ("Triggerbot"), {

			ConfigA(TB_Enabled, Menu::Get.Misc.Triggerbot.TB_Enabled),

			ConfigA(Filter0,Menu::Get.Misc.Triggerbot.Filter[0]),

			ConfigA(Filter1, Menu::Get.Misc.Triggerbot.Filter[1]),

			ConfigA(Filter2, Menu::Get.Misc.Triggerbot.Filter[2]),

			ConfigA(Filter3, Menu::Get.Misc.Triggerbot.Filter[3]),

			ConfigA(Filter4, Menu::Get.Misc.Triggerbot.Filter[4]),

			ConfigA(Filter5, Menu::Get.Misc.Triggerbot.Filter[5]),

			ConfigA(TB_Usage, Menu::Get.Misc.Triggerbot.TB_Usage),
		} },
		{ ("Chams"), {

			ConfigA(CMB_Enabled, Menu::Get.Visuals.Chams.Backtrack.CMB_Enabled),
			ConfigA(CMB_Ignorez, Menu::Get.Visuals.Chams.Backtrack.CMB_Ignorez),
			ConfigA(CMB_Type, Menu::Get.Visuals.Chams.Backtrack.CMB_Type),
			ConfigA(CMB_Wireframe, Menu::Get.Visuals.Chams.Backtrack.CMB_Wireframe),

			ConfigA(CMP_Enabled, Menu::Get.Visuals.Chams.Player.CMP_Enabled),
			ConfigA(CMP_Ignorez, Menu::Get.Visuals.Chams.Player.CMP_Ignorez),
			ConfigA(CMP_Type, Menu::Get.Visuals.Chams.Player.CMP_Type),
			ConfigA(CMP_Wireframe, Menu::Get.Visuals.Chams.Player.CMP_Wireframe),

			ConfigA(CMW_Enabled, Menu::Get.Visuals.Chams.Weapon.CMW_Enabled),
			ConfigA(CMW_Ignorez, Menu::Get.Visuals.Chams.Weapon.CMW_Ignorez),
			ConfigA(CMW_Type, Menu::Get.Visuals.Chams.Weapon.CMW_Type),
			ConfigA(CMW_Wireframe, Menu::Get.Visuals.Chams.Weapon.CMW_Wireframe),
		} },
		{ ("Visuals"), {

			ConfigA(Ammo, Menu::Get.Visuals.Ammo),

			ConfigA(SkyBoxIndex, SkyBoxIndex),
			ConfigA(NightMode, Menu::Get.Visuals.NightMode),
			ConfigA(MuzzleFlash, Menu::Get.Visuals.MuzzleFlash),
			ConfigA(NightVision, Menu::Get.Visuals.NightVision),
			ConfigA(NightVisionAlpha, Menu::Get.Visuals.NightVisionAlpha),
			ConfigA(Mode, Menu::Get.Visuals.Mode),
			ConfigA(Overlay, Menu::Get.Visuals.Overlay),
			ConfigA(Hands, Menu::Get.Visuals.Hands),
			ConfigA(Disable_Fire_Particles, Menu::Get.Visuals.Disable_Fire_Particles),
			ConfigA(Disable_Players_MuzzleFlash, Menu::Get.Visuals.Disable_Players_MuzzleFlash),
			ConfigA(NadeTracer, Menu::Get.Visuals.NadeTracer),


			ConfigA(HP_SpriteScale, Menu::Get.Visuals.HP_SpriteScale),
			ConfigA(WeaponRenderingMode, Menu::Get.Visuals.WeaponRenderingMode),
			ConfigA(Skeleton, Menu::Get.Visuals.Skeleton),
			ConfigA(KillEffect, Menu::Get.Visuals.KillEffect),
			ConfigA(KillEffect_Type, Menu::Get.Visuals.KillEffect_Type),
			ConfigA(Recoil, Menu::Get.Visuals.Crosshair.Recoil),

			ConfigA(Backtrack, Menu::Get.Visuals.Backtrack),

			ConfigA(Box, Menu::Get.Visuals.Box),

			ConfigA(BoxType, Menu::Get.Visuals.BoxType),

			ConfigA(ScopeLen, Menu::Get.Visuals.ScopeLen),

			{ ("Weather"), {
			ConfigA(Length, Menu::Get.Visuals.Weather.Length),
			ConfigA(Radius, Menu::Get.Visuals.Weather.Radius),
			ConfigA(Speed, Menu::Get.Visuals.Weather.Speed),
			ConfigA(Width, Menu::Get.Visuals.Weather.Width),
			ConfigA(Type, Menu::Get.Visuals.Weather.Type),
			ConfigA(Active, Menu::Get.Visuals.Weather.Active),
			} },

			{ ("Trails"), {
			ConfigA(TR_Enabled, Menu::Get.Visuals.Trails.TR_Enabled),
			ConfigA(SpriteTexture, Menu::Get.Visuals.Trails.SpriteTexture),
			ConfigA(m_flHaloScale, Menu::Get.Visuals.Trails.Price),

			{ ("Expensive"), {
			ConfigA(Width, Menu::Get.Visuals.Trails.Expensive.Width),
			ConfigA(EndWidth, Menu::Get.Visuals.Trails.Expensive.EndWidth),
			ConfigA(LifeTime, Menu::Get.Visuals.Trails.Expensive.LifeTime),
			ConfigA(RenderMode, Menu::Get.Visuals.Trails.Expensive.RenderMode),
			} },

			{ ("Cheap"), {
			ConfigA(m_nFlags, Menu::Get.Visuals.Trails.Cheap.m_nFlags),
			ConfigA(m_nSegments, Menu::Get.Visuals.Trails.Cheap.m_nSegments),
			ConfigA(m_flSpeed, Menu::Get.Visuals.Trails.Cheap.m_flSpeed),
			ConfigA(m_flAmplitude, Menu::Get.Visuals.Trails.Cheap.m_flAmplitude),
			ConfigA(m_flFadeLength, Menu::Get.Visuals.Trails.Cheap.m_flFadeLength),
			ConfigA(m_flEndWidth, Menu::Get.Visuals.Trails.Cheap.m_flEndWidth),
			ConfigA(m_flWidth, Menu::Get.Visuals.Trails.Cheap.m_flWidth),
			ConfigA(m_flLife, Menu::Get.Visuals.Trails.Cheap.m_flLife),
			ConfigA(m_flHaloScale, Menu::Get.Visuals.Trails.Cheap.m_flHaloScale),
			} },
			} },
			

			{ ("Tracers"), {
			ConfigA(TR_Enabled, Menu::Get.Visuals.Tracers.TR_Enabled),
			ConfigA(SpriteTexture, Menu::Get.Visuals.Tracers.SpriteTexture),
			ConfigA(m_flHaloScale, Menu::Get.Visuals.Tracers.m_flHaloScale),
			ConfigA(m_flLife, Menu::Get.Visuals.Tracers.m_flLife),
			ConfigA(m_flWidth, Menu::Get.Visuals.Tracers.m_flWidth),
			ConfigA(m_flEndWidth, Menu::Get.Visuals.Tracers.m_flEndWidth),
			ConfigA(m_flFadeLength, Menu::Get.Visuals.Tracers.m_flFadeLength),
			ConfigA(m_flAmplitude, Menu::Get.Visuals.Tracers.m_flAmplitude),
			ConfigA(m_flSpeed, Menu::Get.Visuals.Tracers.m_flSpeed),
			ConfigA(m_nSegments, Menu::Get.Visuals.Tracers.m_nSegments),
			ConfigA(m_nFlags, Menu::Get.Visuals.Tracers.m_nFlags),
			} },

			{ ("FOG"), {
			ConfigA(Enabled, Menu::Get.Visuals.FOG.Enabled),
			ConfigA(start, Menu::Get.Visuals.FOG.start),
			ConfigA(end, Menu::Get.Visuals.FOG.end),
			ConfigA(maxdensity, Menu::Get.Visuals.FOG.maxdensity),
			} },

			{ ("DamageIndicator"), {
			ConfigA(DI_Enabled, Menu::Get.Visuals.DamageIndicator.DI_Enabled),
			ConfigA(Lifetime, Menu::Get.Visuals.DamageIndicator.Lifetime),
			ConfigA(Speed, Menu::Get.Visuals.DamageIndicator.Speed),
			} },

			{ ("Circle_Shape"), {
			ConfigA(Enabled, Menu::Get.Visuals.Crosshair.Circle_Shape.Enabled),
			ConfigA(Radius, Menu::Get.Visuals.Crosshair.Circle_Shape.Radius),
			ConfigA(Segments, Menu::Get.Visuals.Crosshair.Circle_Shape.Segments),
			} },

			{ ("X_Shape"), {
			ConfigA(Enabled, Menu::Get.Visuals.Crosshair.X_Shape.Enabled),
			ConfigA(Gap, Menu::Get.Visuals.Crosshair.X_Shape.Gap),
			ConfigA(Length, Menu::Get.Visuals.Crosshair.X_Shape.Length),
			} },

			{ ("Plus_Shape"), {
			ConfigA(Enabled, Menu::Get.Visuals.Crosshair.Plus_Shape.Enabled),
			ConfigA(Gap, Menu::Get.Visuals.Crosshair.Plus_Shape.Gap),
			ConfigA(Length, Menu::Get.Visuals.Crosshair.Plus_Shape.Length),
			} },

			ConfigA(FieldOfView, Menu::Get.Visuals.FieldOfView),

			ConfigA(FlashPercentage, Menu::Get.Visuals.FlashPercentage),

			ConfigA(FullBright, Menu::Get.Visuals.FullBright),

			ConfigA(Health, Menu::Get.Visuals.Health),

			ConfigA(HealthType, Menu::Get.Visuals.HealthType),

			ConfigA(HitSound, Menu::Get.Visuals.HitSound),

			ConfigA(Name, Menu::Get.Visuals.Name),

			ConfigA(Nodecals, Menu::Get.Visuals.Nodecals),

			ConfigA(NoFlashLight, Menu::Get.Visuals.NoFlashLight),

			ConfigA(NoFog, Menu::Get.Visuals.NoFog),

			ConfigA(NoHands, Menu::Get.Visuals.Hands),

			ConfigA(NoSky, Menu::Get.Visuals.NoSky),

			ConfigA(NoSmoke, Menu::Get.Visuals.NoSmoke),

			ConfigA(NoTeammates, Menu::Get.Visuals.NoTeammates),

			ConfigA(NoVisualRecoil, Menu::Get.Visuals.NoVisualRecoil),

			ConfigA(PlayerSpeed, Menu::Get.Visuals.PlayerSpeed),

			ConfigA(SnapLine, Menu::Get.Visuals.SnapLine),

			ConfigA(Special_Item, Menu::Get.Visuals.Special_Item),

			ConfigA(ViewModelFov, Menu::Get.Visuals.ViewModelFov),

			ConfigA(Weapon, Menu::Get.Visuals.Weapon),

			// HitMarker

			ConfigA(HM_Enabled, Menu::Get.Visuals.Hitmarker.HM_Enabled),

			ConfigA(Length, Menu::Get.Visuals.Hitmarker.Length),

			ConfigA(Time, Menu::Get.Visuals.Hitmarker.Time),

			ConfigA(Gap, Menu::Get.Visuals.Hitmarker.Gap),

            // Radar

			ConfigA(RD_Enabled, Menu::Get.Visuals.Radar.RD_Enabled),

			ConfigA(InGame_Bypass, Menu::Get.Visuals.Radar.InGame_Bypass),

			ConfigA(Bomb, Menu::Get.Visuals.Radar.Bomb),

            // ViewModel

			ConfigA(VM_Enabled, Menu::Get.Visuals.ViewModel.VM_Enabled),

			ConfigA(Origin0, Menu::Get.Visuals.ViewModel.Origin[0]),

			ConfigA(Origin1, Menu::Get.Visuals.ViewModel.Origin[1]),

			ConfigA(Origin2, Menu::Get.Visuals.ViewModel.Origin[2]),

			ConfigA(Angles0, Menu::Get.Visuals.ViewModel.Angles[0]),

			ConfigA(Angles1, Menu::Get.Visuals.ViewModel.Angles[1]),

			ConfigA(Angles2, Menu::Get.Visuals.ViewModel.Angles[2]),

            // Debug

			{ ("Debug"), {
				ConfigA(Classname, Menu::Get.Visuals.Debug.Classname),

			ConfigA(Enabled, Menu::Get.Visuals.Debug.Enabled),

			ConfigA(Filter0, Menu::Get.Visuals.Debug.Filter[0]),

			ConfigA(Filter1, Menu::Get.Visuals.Debug.Filter[1]),

			ConfigA(Filter2, Menu::Get.Visuals.Debug.Filter[2]),

			ConfigA(Filter3, Menu::Get.Visuals.Debug.Filter[3]),

			ConfigA(Filter4, Menu::Get.Visuals.Debug.Filter[4]),

			ConfigA(Filter5, Menu::Get.Visuals.Debug.Filter[5]),

			ConfigA(INDEX, Menu::Get.Visuals.Debug.INDEX),

			ConfigA(MaxDistance, Menu::Get.Visuals.Debug.MaxDistance),

			ConfigA(ModelName, Menu::Get.Visuals.Debug.ModelName),

			ConfigA(Origin, Menu::Get.Visuals.Debug.Origin),

			ConfigA(Velocity, Menu::Get.Visuals.Debug.Velocity),
				} },
		} },
        { ("Miscellaneous"), {

			ConfigA(NoMotd, Menu::Get.Misc.NoMotd),
			ConfigA(Ragdoll_Force, Menu::Get.Misc.Ragdoll_Force),
			ConfigA(MuteRadio, Menu::Get.Misc.MuteRadio),
			ConfigA(Disable_Game_Console_Warnings, Menu::Get.Misc.Disable_Game_Console_Warnings),
			ConfigA(KillMessage, Menu::Get.Misc.KillMessage),
			ConfigA(MusicVolume, Menu::Get.Misc.Sounds.MusicVolume),
			ConfigA(FootStepsVolume, Menu::Get.Misc.Sounds.FootStepsVolume),
			ConfigA(WeaponsAudio, Menu::Get.Misc.Sounds.WeaponsAudio),

			{ ("StrafeOptimizer"), {
			ConfigA(Enabled, Menu::Get.Misc.StrafeOptimizer.Enabled),
			ConfigA(Desired_Gain, Menu::Get.Misc.StrafeOptimizer.Desired_Gain),
			ConfigA(Required_Speed, Menu::Get.Misc.StrafeOptimizer.Required_Speed),
			ConfigA(Greatest_Possible_Strafe_Angle, Menu::Get.Misc.StrafeOptimizer.Greatest_Possible_Strafe_Angle),
			} },

			ConfigA(AntiAFKkick, Menu::Get.Misc.AntiAFKkick),

			ConfigA(AutoPisol, Menu::Get.Misc.AutoPisol),

			ConfigA(Autostrafe, Menu::Get.Misc.Autostrafe),

			ConfigA(Bunnyhop, Menu::Get.Misc.Bunnyhop),

			ConfigA(BlockBot, Menu::Get.Misc.BlockBot),

			ConfigA(Bunnyhop_Perfect_Rate, Menu::Get.Misc.Bunnyhop_Perfect_Rate),

			ConfigA(CircleStrafe, Menu::Get.Misc.CircleStrafe),

			ConfigA(CustomizeableFiles, Menu::Get.Misc.CustomizeableFiles),

			ConfigA(DownloadManagerFilter, Menu::Get.Misc.DownloadManagerFilter),

			ConfigA(Edgejump, Menu::Get.Misc.Edgejump),

			ConfigA(FastReload, Menu::Get.Misc.FastReload),

			ConfigA(LagExploit, Menu::Get.Misc.LagExploit),

			ConfigA(InstantDefuse, Menu::Get.Misc.InstantDefuse),

			ConfigA(InstantExplosion, Menu::Get.Misc.InstantExplosion),

			ConfigA(Fastrun, Menu::Get.Misc.Fastrun),

			ConfigA(Faststop, Menu::Get.Misc.Faststop),

			ConfigA(FastSwitch, Menu::Get.Misc.FastSwitch),

			ConfigA(Legit_Bunnyhop_Booster, Menu::Get.Misc.Legit_Bunnyhop_Booster),

			ConfigA(AutoKevlar, Menu::Get.Misc.AutoKevlar),

			ConfigA(NoDrug, Menu::Get.Misc.NoDrug),

			ConfigA(NoShake, Menu::Get.Misc.NoShake),

			ConfigA(NoRecoil, Menu::Get.Misc.NoRecoil),

			ConfigA(NoSpread, Menu::Get.Misc.NoSpread),

			ConfigA(Sv_Pure_Bypass, Menu::Get.Misc.Sv_Pure_Bypass),

			ConfigA(FastLadder, Menu::Get.Misc.FastLadder),

			ConfigA(ThirdPerson, Menu::Get.Misc.ThirdPerson),

			ConfigA(m_vecCameraOffset, Menu::Get.Misc.m_vecCameraOffset),

			// SteamIDSpoofer

			ConfigA(SIP_Enabled, Menu::Get.Misc.SteamIDSpoofer.SIP_Enabled),

			ConfigA(SteamID, Menu::Get.Misc.SteamIDSpoofer.SteamID),

			ConfigA(RandomSteamID, Menu::Get.Misc.SteamIDSpoofer.RandomSteamID),

			// ClanTag

			ConfigA(Changer, Menu::Get.Misc.ClanTag.Changer),

			ConfigA(Stealer, Menu::Get.Misc.ClanTag.Stealer),

			ConfigA(Delay, Menu::Get.Misc.ClanTag.Delay),

			// CL_interp

			ConfigA(Custom, Menu::Get.Misc.cl_interp.Custom),

			ConfigA(ClientSideValue, Menu::Get.Misc.cl_interp.ClientSideValue),

			ConfigA(ServerSideValue, Menu::Get.Misc.cl_interp.ServerSideValue),

			// Backtrack

			ConfigA(BT_Enabled, Menu::Get.Misc.Backtrack.BT_Enabled),

			ConfigA(DisableInterpolation, Menu::Get.Misc.Backtrack.DisableInterpolation),

			ConfigA(FakeLatency, Menu::Get.Misc.Backtrack.FakeLatency),

			ConfigA(FakeLatencyValue, Menu::Get.Misc.Backtrack.FakeLatencyValue),

			// Air Stuck

			ConfigA(AS_Enabled, Menu::Get.Misc.AirStuck.AS_Enabled),

			ConfigA(AS_Usage, Menu::Get.Misc.AirStuck.AS_Usage),

			// KnifeBot

			ConfigA(KB_Enabled, Menu::Get.Misc.KnifeBot.KB_Enabled),

			ConfigA(Method, Menu::Get.Misc.KnifeBot.Method),

			ConfigA(KB_Usage, Menu::Get.Misc.KnifeBot.KB_Usage),

		} },
		{ ("Keys"), {

			ConfigA(AirStuck, Menu::Get.Keys.AirStuck),

			ConfigA(Bunnyhop, Menu::Get.Keys.Bunnyhop),

			ConfigA(CircleStrafe, Menu::Get.Keys.CircleStrafe),

			ConfigA(Edgejump, Menu::Get.Keys.Edgejump),

			ConfigA(PanicKey, Menu::Get.Keys.PanicKey),

			ConfigA(RageBot, Menu::Get.Keys.RageBot),

			ConfigA(Triggerbot, Menu::Get.Keys.Triggerbot),

			ConfigA(Unload, Menu::Get.Keys.Unload),

			ConfigA(BlockBot, Menu::Get.Keys.BlockBot),

			ConfigA(KnifeBot, Menu::Get.Keys.KnifeBot),

			ConfigA(Autostrafe, Menu::Get.Keys.Autostrafe),
		} },
		{ ("Colors"), {


			ColorConfigA(SkyBox, Menu::Get.Colors.General.SkyBox),
			ColorConfigA(World, Menu::Get.Colors.General.World),
			ColorConfigA(DamageIndicator, Menu::Get.Colors.General.DamageIndicator),
			ColorConfigA(Nightvision, Menu::Get.Colors.General.Nightvision),
			ColorConfigA(NadeTracer, Menu::Get.Colors.General.NadeTracer),
			ColorConfigA(BulletImpact, Menu::Get.Colors.General.BulletImpact),
			ColorConfigA(X_Shape, Menu::Get.Colors.General.Crosshair.X_Shape),
			ColorConfigA(Plus_Shape, Menu::Get.Colors.General.Crosshair.Plus_Shape),
			ColorConfigA(Circle_Shape, Menu::Get.Colors.General.Crosshair.Circle_Shape),
			ColorConfigA(Fog, Menu::Get.Colors.General.Fog),
			ColorConfigA(HudColor, Menu::Get.Colors.General.HudColor),

			ColorConfigA(Hitmarker, Menu::Get.Colors.General.Hitmarker),

			ColorConfigA(Tracers, Menu::Get.Colors.General.Tracers),

			ColorConfigA(Trails, Menu::Get.Colors.General.Trails),

			ColorConfigA(Player, Menu::Get.Colors.Chams.Player),

			ColorConfigA(CWeapon, Menu::Get.Colors.Chams.CWeapon),

			ColorConfigA(Backtrack, Menu::Get.Colors.Chams.Backtrack),

			ColorConfigA(Box, Menu::Get.Colors.PlayerEsp.Box),

			ColorConfigA(Health, Menu::Get.Colors.PlayerEsp.Health),

			ColorConfigA(Name, Menu::Get.Colors.PlayerEsp.Name),

			ColorConfigA(SnapLine, Menu::Get.Colors.PlayerEsp.SnapLine),

			ColorConfigA(Skeleton, Menu::Get.Colors.PlayerEsp.Skeleton),

			ColorConfigA(Weapon, Menu::Get.Colors.PlayerEsp.Weapon),

		} },
	};

	std::ofstream o(std::string("ZE\\ZE.v") + Version + ".json");

	o << std::setw(4) << j << std::endl;
}

void Config::LoadConfig()
{
	std::ifstream input_file = std::ifstream(std::string("ZE\\ZE.v") + Version + ".json");

	if (!input_file.good())
		return;

	try
	{
		input_file >> j;
	}
	catch (...)
	{
		input_file.close();
		return;
	}

	static auto Load_General_Variables = []()
	{
		LoadVar(AutoLaser,Menu::Get.General.AutoLaser, General);

		LoadVar(Filter, Menu::Get.General.Filter, General);

		LoadVar(WireFrame_Lasers, Menu::Get.General.WireFrame_Lasers, General);
	};

	static auto Load_Aimbot_Variables = []()
	{
		LoadVar(AntiSmac, Menu::Get.Aimbot.AntiSmac, Aimbot);

		LoadVarFromSecondGroup(AutoCrouch, Menu::Get.Aimbot.Ragebot.AutoCrouch, Aimbot, Ragebot);

		LoadVarFromSecondGroup(AutoShoot, Menu::Get.Aimbot.Ragebot.AutoShoot, Aimbot, Ragebot);

		LoadVarFromSecondGroup(AutoStop, Menu::Get.Aimbot.Ragebot.AutoStop, Aimbot, Ragebot);

		LoadVarFromSecondGroup(Enabled, Menu::Get.Aimbot.Ragebot.Enabled, Aimbot, Ragebot);

		LoadVarFromSecondGroup(FOV, Menu::Get.Aimbot.Ragebot.FOV, Aimbot, Ragebot);

		LoadVarFromSecondGroup(FriendlyFire, Menu::Get.Aimbot.Ragebot.FriendlyFire, Aimbot, Ragebot);

		LoadVarFromSecondGroup(Predicted, Menu::Get.Aimbot.Ragebot.Predicted, Aimbot, Ragebot);

		LoadVarFromSecondGroup(HitBox, Menu::Get.Aimbot.Ragebot.HitBox, Aimbot, Ragebot);

		LoadVarFromSecondGroup(MaxAimStep, Menu::Get.Aimbot.Ragebot.MaxAimStep, Aimbot, Ragebot);

		LoadVarFromSecondGroup(Silent, Menu::Get.Aimbot.Ragebot.Silent, Aimbot, Ragebot);

		LoadVarFromSecondGroup(SmoothVal, Menu::Get.Aimbot.Ragebot.SmoothVal, Aimbot, Ragebot);

		LoadVarFromSecondGroup(TargetSelection, Menu::Get.Aimbot.Ragebot.TargetSelection, Aimbot, Ragebot);

		LoadVarFromSecondGroup(Timeout, Menu::Get.Aimbot.Ragebot.Timeout, Aimbot, Ragebot);

		LoadVarFromSecondGroup(Usage, Menu::Get.Aimbot.Ragebot.Usage, Aimbot, Ragebot);
	};

	static auto Load_Triggerbot_Variables = []()
	{
		LoadVar(TB_Enabled, Menu::Get.Misc.Triggerbot.TB_Enabled, Triggerbot);

		LoadItemVar(Filter0, Triggerbot, Menu::Get.Misc.Triggerbot.Filter, 0, bool);
		LoadItemVar(Filter1, Triggerbot, Menu::Get.Misc.Triggerbot.Filter, 1, bool);
		LoadItemVar(Filter2, Triggerbot, Menu::Get.Misc.Triggerbot.Filter, 2, bool);
		LoadItemVar(Filter3, Triggerbot, Menu::Get.Misc.Triggerbot.Filter, 3, bool);
		LoadItemVar(Filter4, Triggerbot, Menu::Get.Misc.Triggerbot.Filter, 4, bool);
		LoadItemVar(Filter5, Triggerbot, Menu::Get.Misc.Triggerbot.Filter, 5, bool);

		LoadVar(TB_Usage, Menu::Get.Misc.Triggerbot.TB_Usage, Triggerbot);
	};

	static auto Load_Chams_Variables = []()
	{
		LoadVar(CMB_Enabled, Menu::Get.Visuals.Chams.Backtrack.CMB_Enabled, Chams);
		LoadVar(CMB_Ignorez, Menu::Get.Visuals.Chams.Backtrack.CMB_Ignorez, Chams);
		LoadVar(CMB_Type, Menu::Get.Visuals.Chams.Backtrack.CMB_Type, Chams);
		LoadVar(CMB_Wireframe, Menu::Get.Visuals.Chams.Backtrack.CMB_Wireframe, Chams);

		LoadVar(CMP_Enabled, Menu::Get.Visuals.Chams.Player.CMP_Enabled, Chams);
		LoadVar(CMP_Ignorez, Menu::Get.Visuals.Chams.Player.CMP_Ignorez, Chams);
		LoadVar(CMP_Type, Menu::Get.Visuals.Chams.Player.CMP_Type, Chams);
		LoadVar(CMP_Wireframe, Menu::Get.Visuals.Chams.Player.CMP_Wireframe, Chams);

		LoadVar(CMW_Enabled, Menu::Get.Visuals.Chams.Weapon.CMW_Enabled, Chams);
		LoadVar(CMP_Ignorez, Menu::Get.Visuals.Chams.Player.CMP_Ignorez, Chams);
		LoadVar(CMP_Type, Menu::Get.Visuals.Chams.Player.CMP_Type, Chams);
		LoadVar(CMP_Wireframe, Menu::Get.Visuals.Chams.Player.CMP_Wireframe, Chams);
	};

	static auto Load_Visuals_Variables = []()
	{
		LoadVar(Box, Menu::Get.Visuals.Box, Visuals);

		LoadVar(Ammo, Menu::Get.Visuals.Ammo, Visuals);

		LoadVar(Backtrack, Menu::Get.Visuals.Backtrack, Visuals);

		LoadVar(BoxType, Menu::Get.Visuals.BoxType, Visuals);

		LoadVar(FieldOfView, Menu::Get.Visuals.FieldOfView, Visuals);

		LoadVar(FlashPercentage, Menu::Get.Visuals.FlashPercentage, Visuals);

		LoadVar(FullBright, Menu::Get.Visuals.FullBright, Visuals);

		LoadVar(Health, Menu::Get.Visuals.Health, Visuals);

		LoadVar(HealthType, Menu::Get.Visuals.HealthType, Visuals);

		LoadVar(HitSound, Menu::Get.Visuals.HitSound, Visuals);

		LoadVar(Name, Menu::Get.Visuals.Name, Visuals);

		LoadVar(Nodecals, Menu::Get.Visuals.Nodecals, Visuals);

		LoadVar(NoFlashLight, Menu::Get.Visuals.NoFlashLight, Visuals);

		LoadVar(NoFog, Menu::Get.Visuals.NoFog, Visuals);

		LoadVar(Hands, Menu::Get.Visuals.Hands, Visuals);

		LoadVar(SkyBoxIndex, SkyBoxIndex, Visuals);

		LoadVar(NoSky, Menu::Get.Visuals.NoSky, Visuals);

		LoadVar(NoSmoke, Menu::Get.Visuals.NoSmoke, Visuals);

		LoadVar(NoTeammates, Menu::Get.Visuals.NoTeammates, Visuals);

		LoadVar(NoVisualRecoil, Menu::Get.Visuals.NoVisualRecoil, Visuals);

		LoadVar(PlayerSpeed, Menu::Get.Visuals.PlayerSpeed, Visuals);

		LoadVar(SnapLine, Menu::Get.Visuals.SnapLine, Visuals);

		LoadVar(Special_Item, Menu::Get.Visuals.Special_Item, Visuals);

		LoadVar(ViewModelFov, Menu::Get.Visuals.ViewModelFov, Visuals);

		LoadVar(ScopeLen, Menu::Get.Visuals.ScopeLen, Visuals);

		LoadVar(Weapon, Menu::Get.Visuals.Weapon, Visuals);

		// Radar

		LoadVar(RD_Enabled, Menu::Get.Visuals.Radar.RD_Enabled, Visuals);

		LoadVar(InGame_Bypass, Menu::Get.Visuals.Radar.InGame_Bypass, Visuals);

		LoadVar(Bomb, Menu::Get.Visuals.Radar.Bomb, Visuals);


		// HitMarker

		LoadVar(HM_Enabled, Menu::Get.Visuals.Hitmarker.HM_Enabled, Visuals);

		LoadVar(Length, Menu::Get.Visuals.Hitmarker.Length, Visuals);

		LoadVar(Time, Menu::Get.Visuals.Hitmarker.Time, Visuals);

		LoadVar(Gap, Menu::Get.Visuals.Hitmarker.Gap, Visuals);

		// ViewModel

		LoadVar(VM_Enabled, Menu::Get.Visuals.ViewModel.VM_Enabled, Visuals);

		LoadItemVar(Origin0, Visuals, Menu::Get.Visuals.ViewModel.Origin, 0, float);

		LoadItemVar(Origin1, Visuals, Menu::Get.Visuals.ViewModel.Origin, 1, float);

		LoadItemVar(Origin2, Visuals, Menu::Get.Visuals.ViewModel.Origin, 2, float);

		LoadItemVar(Angles0, Visuals, Menu::Get.Visuals.ViewModel.Angles, 0, float);

		LoadItemVar(Angles1, Visuals, Menu::Get.Visuals.ViewModel.Angles, 1, float);

		LoadItemVar(Angles2, Visuals, Menu::Get.Visuals.ViewModel.Angles, 2, float);

		// Debug

		LoadVarFromSecondGroup(Classname, Menu::Get.Visuals.Debug.Classname, Visuals, Debug);

		LoadVarFromSecondGroup(Enabled, Menu::Get.Visuals.Debug.Enabled, Visuals, Debug);

		LoadItemVarFromSecondGroup(Filter0, Visuals, Debug, Menu::Get.Visuals.Debug.Filter, 0, bool);

		LoadItemVarFromSecondGroup(Filter1, Visuals, Debug, Menu::Get.Visuals.Debug.Filter, 1, bool);

		LoadItemVarFromSecondGroup(Filter2, Visuals, Debug, Menu::Get.Visuals.Debug.Filter, 2, bool);

		LoadItemVarFromSecondGroup(Filter3, Visuals, Debug, Menu::Get.Visuals.Debug.Filter, 3, bool);

		LoadItemVarFromSecondGroup(Filter4, Visuals, Debug, Menu::Get.Visuals.Debug.Filter, 4, bool);

		LoadItemVarFromSecondGroup(Filter5, Visuals, Debug, Menu::Get.Visuals.Debug.Filter, 5, bool);

		LoadVarFromSecondGroup(INDEX, Menu::Get.Visuals.Debug.INDEX, Visuals, Debug);

		LoadVarFromSecondGroup(MaxDistance, Menu::Get.Visuals.Debug.MaxDistance, Visuals, Debug);

		LoadVarFromSecondGroup(ModelName, Menu::Get.Visuals.Debug.ModelName, Visuals, Debug);

		LoadVarFromSecondGroup(Origin, Menu::Get.Visuals.Debug.Origin, Visuals, Debug);

		LoadVarFromSecondGroup(Velocity, Menu::Get.Visuals.Debug.Velocity, Visuals, Debug);

		LoadVar(NightMode, Menu::Get.Visuals.NightMode, Visuals);
		LoadVar(MuzzleFlash, Menu::Get.Visuals.MuzzleFlash, Visuals);
		LoadVar(NightVision, Menu::Get.Visuals.NightVision, Visuals);
		LoadVar(NightVisionAlpha, Menu::Get.Visuals.NightVisionAlpha, Visuals);
		LoadVar(Mode, Menu::Get.Visuals.Mode, Visuals);
		LoadVar(Overlay, Menu::Get.Visuals.Overlay, Visuals);
		LoadVar(Hands, Menu::Get.Visuals.Hands, Visuals);
		LoadVar(Disable_Fire_Particles, Menu::Get.Visuals.Disable_Fire_Particles, Visuals);
		LoadVar(Disable_Players_MuzzleFlash, Menu::Get.Visuals.Disable_Players_MuzzleFlash, Visuals);
		LoadVar(NadeTracer, Menu::Get.Visuals.NadeTracer, Visuals);


		LoadVar(HP_SpriteScale, Menu::Get.Visuals.HP_SpriteScale, Visuals);
		LoadVar(WeaponRenderingMode, Menu::Get.Visuals.WeaponRenderingMode, Visuals);
		LoadVar(Skeleton, Menu::Get.Visuals.Skeleton, Visuals);
		LoadVar(KillEffect, Menu::Get.Visuals.KillEffect, Visuals);
		LoadVar(KillEffect_Type, Menu::Get.Visuals.KillEffect_Type, Visuals);
		LoadVar(Recoil, Menu::Get.Visuals.Crosshair.Recoil, Visuals);


		LoadVarFromSecondGroup(Length, Menu::Get.Visuals.Weather.Length, Visuals, Weather);
		LoadVarFromSecondGroup(Radius, Menu::Get.Visuals.Weather.Radius, Visuals, Weather);
		LoadVarFromSecondGroup(Speed, Menu::Get.Visuals.Weather.Speed, Visuals, Weather);
		LoadVarFromSecondGroup(Width, Menu::Get.Visuals.Weather.Width, Visuals, Weather);
		LoadVarFromSecondGroup(Type, Menu::Get.Visuals.Weather.Type, Visuals, Weather);
		LoadVarFromSecondGroup(Active, Menu::Get.Visuals.Weather.Active, Visuals, Weather);

		LoadVarFromSecondGroup(TR_Enabled, Menu::Get.Visuals.Trails.TR_Enabled, Visuals, Trails);
		LoadVarFromSecondGroup(SpriteTexture, Menu::Get.Visuals.Trails.SpriteTexture, Visuals, Trails);
		LoadVarFromSecondGroup(m_flHaloScale, Menu::Get.Visuals.Trails.Price, Visuals, Trails);

		LoadVarFromThirdGroup(Width, Menu::Get.Visuals.Trails.Expensive.Width, Visuals, Trails, Expensive);
		LoadVarFromThirdGroup(EndWidth, Menu::Get.Visuals.Trails.Expensive.EndWidth, Visuals, Trails, Expensive);
		LoadVarFromThirdGroup(LifeTime, Menu::Get.Visuals.Trails.Expensive.LifeTime, Visuals, Trails, Expensive);
		LoadVarFromThirdGroup(RenderMode, Menu::Get.Visuals.Trails.Expensive.RenderMode, Visuals, Trails, Expensive);

		LoadVarFromThirdGroup(m_nFlags, Menu::Get.Visuals.Trails.Cheap.m_nFlags, Visuals, Trails, Cheap);
		LoadVarFromThirdGroup(m_nSegments, Menu::Get.Visuals.Trails.Cheap.m_nSegments, Visuals, Trails, Cheap);
		LoadVarFromThirdGroup(m_flSpeed, Menu::Get.Visuals.Trails.Cheap.m_flSpeed, Visuals, Trails, Cheap);
		LoadVarFromThirdGroup(m_flAmplitude, Menu::Get.Visuals.Trails.Cheap.m_flAmplitude, Visuals, Trails, Cheap);
		LoadVarFromThirdGroup(m_flFadeLength, Menu::Get.Visuals.Trails.Cheap.m_flFadeLength, Visuals, Trails, Cheap);
		LoadVarFromThirdGroup(m_flEndWidth, Menu::Get.Visuals.Trails.Cheap.m_flEndWidth, Visuals, Trails, Cheap);
		LoadVarFromThirdGroup(m_flWidth, Menu::Get.Visuals.Trails.Cheap.m_flWidth, Visuals, Trails, Cheap);
		LoadVarFromThirdGroup(m_flLife, Menu::Get.Visuals.Trails.Cheap.m_flLife, Visuals, Trails, Cheap);
		LoadVarFromThirdGroup(m_flHaloScale, Menu::Get.Visuals.Trails.Cheap.m_flHaloScale, Visuals, Trails, Cheap);


		LoadVarFromSecondGroup(TR_Enabled, Menu::Get.Visuals.Tracers.TR_Enabled, Visuals, Tracers);
		LoadVarFromSecondGroup(SpriteTexture, Menu::Get.Visuals.Tracers.SpriteTexture, Visuals, Tracers);
		LoadVarFromSecondGroup(m_flHaloScale, Menu::Get.Visuals.Tracers.m_flHaloScale, Visuals, Tracers);
		LoadVarFromSecondGroup(m_flLife, Menu::Get.Visuals.Tracers.m_flLife, Visuals, Tracers);
		LoadVarFromSecondGroup(m_flWidth, Menu::Get.Visuals.Tracers.m_flWidth, Visuals, Tracers);
		LoadVarFromSecondGroup(m_flEndWidth, Menu::Get.Visuals.Tracers.m_flEndWidth, Visuals, Tracers);
		LoadVarFromSecondGroup(m_flFadeLength, Menu::Get.Visuals.Tracers.m_flFadeLength, Visuals, Tracers);
		LoadVarFromSecondGroup(m_flAmplitude, Menu::Get.Visuals.Tracers.m_flAmplitude, Visuals, Tracers);
		LoadVarFromSecondGroup(m_flSpeed, Menu::Get.Visuals.Tracers.m_flSpeed, Visuals, Tracers);
		LoadVarFromSecondGroup(m_nSegments, Menu::Get.Visuals.Tracers.m_nSegments, Visuals, Tracers);
		LoadVarFromSecondGroup(m_nFlags, Menu::Get.Visuals.Tracers.m_nFlags, Visuals, Tracers);

		LoadVarFromSecondGroup(Enabled, Menu::Get.Visuals.FOG.Enabled, Visuals, FOG);
		LoadVarFromSecondGroup(start, Menu::Get.Visuals.FOG.start, Visuals, FOG);
		LoadVarFromSecondGroup(end, Menu::Get.Visuals.FOG.end, Visuals, FOG);
		LoadVarFromSecondGroup(maxdensity, Menu::Get.Visuals.FOG.maxdensity, Visuals, FOG);

		LoadVarFromSecondGroup(DI_Enabled, Menu::Get.Visuals.DamageIndicator.DI_Enabled, Visuals, DamageIndicator);
		LoadVarFromSecondGroup(Lifetime, Menu::Get.Visuals.DamageIndicator.Lifetime, Visuals, DamageIndicator);
		LoadVarFromSecondGroup(Speed, Menu::Get.Visuals.DamageIndicator.Speed, Visuals, DamageIndicator);

		LoadVarFromSecondGroup(Enabled, Menu::Get.Visuals.Crosshair.Circle_Shape.Enabled, Visuals, Circle_Shape);
		LoadVarFromSecondGroup(Radius, Menu::Get.Visuals.Crosshair.Circle_Shape.Radius, Visuals, Circle_Shape);
		LoadVarFromSecondGroup(Segments, Menu::Get.Visuals.Crosshair.Circle_Shape.Segments, Visuals, Circle_Shape);

		LoadVarFromSecondGroup(Enabled, Menu::Get.Visuals.Crosshair.X_Shape.Enabled, Visuals, X_Shape);
		LoadVarFromSecondGroup(Gap, Menu::Get.Visuals.Crosshair.X_Shape.Gap, Visuals, X_Shape);
		LoadVarFromSecondGroup(Length, Menu::Get.Visuals.Crosshair.X_Shape.Length, Visuals, X_Shape);

		LoadVarFromSecondGroup(Enabled, Menu::Get.Visuals.Crosshair.Plus_Shape.Enabled, Visuals, Plus_Shape);
		LoadVarFromSecondGroup(Gap, Menu::Get.Visuals.Crosshair.Plus_Shape.Gap, Visuals, Plus_Shape);
		LoadVarFromSecondGroup(Length, Menu::Get.Visuals.Crosshair.Plus_Shape.Length, Visuals, Plus_Shape);
	};

	static auto Load_Miscellaneous_Variables = []()
	{
		LoadVar(NoMotd, Menu::Get.Misc.NoMotd, Miscellaneous);
		LoadVar(Ragdoll_Force, Menu::Get.Misc.Ragdoll_Force, Miscellaneous);
		LoadVar(MuteRadio, Menu::Get.Misc.MuteRadio, Miscellaneous);
		LoadVar(Disable_Game_Console_Warnings, Menu::Get.Misc.Disable_Game_Console_Warnings, Miscellaneous);
		LoadVar(KillMessage, Menu::Get.Misc.KillMessage, Miscellaneous);
		LoadVar(MusicVolume, Menu::Get.Misc.Sounds.MusicVolume, Miscellaneous);
		LoadVar(FootStepsVolume, Menu::Get.Misc.Sounds.FootStepsVolume, Miscellaneous);
		LoadVar(WeaponsAudio, Menu::Get.Misc.Sounds.WeaponsAudio, Miscellaneous);

		LoadVarFromSecondGroup(Enabled, Menu::Get.Misc.StrafeOptimizer.Enabled, Miscellaneous, StrafeOptimizer);
		LoadVarFromSecondGroup(Desired_Gain, Menu::Get.Misc.StrafeOptimizer.Desired_Gain, Miscellaneous, StrafeOptimizer);
		LoadVarFromSecondGroup(Required_Speed, Menu::Get.Misc.StrafeOptimizer.Required_Speed, Miscellaneous, StrafeOptimizer);
		LoadVarFromSecondGroup(Greatest_Possible_Strafe_Angle, Menu::Get.Misc.StrafeOptimizer.Greatest_Possible_Strafe_Angle, Miscellaneous, StrafeOptimizer);

		LoadVar(AntiAFKkick, Menu::Get.Misc.AntiAFKkick, Miscellaneous);

		LoadVar(AutoPisol, Menu::Get.Misc.AutoPisol, Miscellaneous);

		LoadVar(Autostrafe, Menu::Get.Misc.Autostrafe, Miscellaneous);

		LoadVar(Bunnyhop, Menu::Get.Misc.Bunnyhop, Miscellaneous);

		LoadVar(BlockBot, Menu::Get.Misc.BlockBot, Miscellaneous);

		LoadVar(Bunnyhop_Perfect_Rate, Menu::Get.Misc.Bunnyhop_Perfect_Rate, Miscellaneous);

		LoadVar(CircleStrafe, Menu::Get.Misc.CircleStrafe, Miscellaneous);

		LoadVar(DownloadManagerFilter, Menu::Get.Misc.DownloadManagerFilter, Miscellaneous);

		LoadVar(Edgejump, Menu::Get.Misc.Edgejump, Miscellaneous);

		LoadVar(FastReload, Menu::Get.Misc.FastReload, Miscellaneous);

		LoadVar(Fastrun, Menu::Get.Misc.Fastrun, Miscellaneous);

		LoadVar(Faststop, Menu::Get.Misc.Faststop, Miscellaneous);

		LoadVar(FastSwitch, Menu::Get.Misc.FastSwitch, Miscellaneous);

		LoadVar(Legit_Bunnyhop_Booster, Menu::Get.Misc.Legit_Bunnyhop_Booster, Miscellaneous);

		LoadVar(AutoKevlar, Menu::Get.Misc.AutoKevlar, Miscellaneous);

		LoadVar(NoDrug, Menu::Get.Misc.NoDrug, Miscellaneous);

		LoadVar(NoShake, Menu::Get.Misc.NoShake, Miscellaneous);

		LoadVar(NoRecoil, Menu::Get.Misc.NoRecoil, Miscellaneous);

		LoadVar(NoSpread, Menu::Get.Misc.NoSpread, Miscellaneous);

		LoadVar(Sv_Pure_Bypass, Menu::Get.Misc.Sv_Pure_Bypass, Miscellaneous);

		LoadVar(FastLadder, Menu::Get.Misc.FastLadder, Miscellaneous);

		LoadVar(LagExploit, Menu::Get.Misc.LagExploit, Miscellaneous);

		LoadVar(InstantDefuse, Menu::Get.Misc.InstantDefuse, Miscellaneous);

		LoadVar(InstantExplosion, Menu::Get.Misc.InstantExplosion, Miscellaneous);

		LoadVar(ThirdPerson, Menu::Get.Misc.ThirdPerson, Miscellaneous);

		LoadVar(m_vecCameraOffset, Menu::Get.Misc.m_vecCameraOffset, Miscellaneous);

		// SteamIDSpoofer

		LoadVar(SIP_Enabled, Menu::Get.Misc.SteamIDSpoofer.SIP_Enabled, Miscellaneous);

		LoadVar(SteamID, Menu::Get.Misc.SteamIDSpoofer.SteamID, Miscellaneous);

		LoadVar(RandomSteamID, Menu::Get.Misc.SteamIDSpoofer.RandomSteamID, Miscellaneous);

		// ClanTag

		LoadVar(Changer, Menu::Get.Misc.ClanTag.Changer, Miscellaneous);

		LoadVar(Stealer, Menu::Get.Misc.ClanTag.Stealer, Miscellaneous);

		LoadVar(Delay, Menu::Get.Misc.ClanTag.Delay, Miscellaneous);

		// CL_interp

		LoadVar(Custom, Menu::Get.Misc.cl_interp.Custom, Miscellaneous);

		LoadVar(ClientSideValue, Menu::Get.Misc.cl_interp.ClientSideValue, Miscellaneous);

		LoadVar(ServerSideValue, Menu::Get.Misc.cl_interp.ServerSideValue, Miscellaneous);

		// Backtrack

		LoadVar(BT_Enabled, Menu::Get.Misc.Backtrack.BT_Enabled, Miscellaneous);

		LoadVar(DisableInterpolation, Menu::Get.Misc.Backtrack.DisableInterpolation, Miscellaneous);

		LoadVar(FakeLatency, Menu::Get.Misc.Backtrack.FakeLatency, Miscellaneous);

		LoadVar(FakeLatencyValue, Menu::Get.Misc.Backtrack.FakeLatencyValue, Miscellaneous);

		// Air Stuck

		LoadVar(AS_Enabled, Menu::Get.Misc.AirStuck.AS_Enabled, Miscellaneous);

		LoadVar(AS_Usage, Menu::Get.Misc.AirStuck.AS_Usage, Miscellaneous);

		// KnifeBot

		LoadVar(KB_Enabled, Menu::Get.Misc.KnifeBot.KB_Enabled, Miscellaneous);

		LoadVar(KB_Usage, Menu::Get.Misc.KnifeBot.KB_Usage, Miscellaneous);

		LoadVar(Method, Menu::Get.Misc.KnifeBot.Method, Miscellaneous);
	};

	static auto Load_Keys_Variables = []()
	{
		LoadVar(AirStuck, Menu::Get.Keys.AirStuck, Keys);

		LoadVar(Bunnyhop, Menu::Get.Keys.Bunnyhop, Keys);

		LoadVar(Autostrafe, Menu::Get.Keys.Autostrafe, Keys);

		LoadVar(CircleStrafe, Menu::Get.Keys.CircleStrafe, Keys);

		LoadVar(Edgejump, Menu::Get.Keys.Edgejump, Keys);

		LoadVar(PanicKey, Menu::Get.Keys.PanicKey, Keys);

		LoadVar(RageBot, Menu::Get.Keys.RageBot, Keys);

		LoadVar(Triggerbot, Menu::Get.Keys.Triggerbot, Keys);

		LoadVar(Unload, Menu::Get.Keys.Unload, Keys);

		LoadVar(BlockBot, Menu::Get.Keys.BlockBot, Keys);

		LoadVar(KnifeBot, Menu::Get.Keys.KnifeBot, Keys);
	};

	static auto Load_Colors_Variables = []()
	{

		LoadColorVar(SkyBox, Menu::Get.Colors.General.SkyBox, Colors);
		LoadColorVar(World, Menu::Get.Colors.General.World, Colors);
		LoadColorVar(DamageIndicator, Menu::Get.Colors.General.DamageIndicator, Colors);
		LoadColorVar(Nightvision, Menu::Get.Colors.General.Nightvision, Colors);
		LoadColorVar(NadeTracer, Menu::Get.Colors.General.NadeTracer, Colors);
		LoadColorVar(BulletImpact, Menu::Get.Colors.General.BulletImpact, Colors);
		LoadColorVar(Fog, Menu::Get.Colors.General.Fog, Colors);
		LoadColorVar(X_Shape, Menu::Get.Colors.General.Crosshair.X_Shape, Colors);
		LoadColorVar(Plus_Shape, Menu::Get.Colors.General.Crosshair.Plus_Shape, Colors);
		LoadColorVar(Circle_Shape, Menu::Get.Colors.General.Crosshair.Circle_Shape, Colors);

		// General

		LoadColorVar(Hitmarker,Menu::Get.Colors.General.Hitmarker, Colors);

		LoadColorVar(Tracers,Menu::Get.Colors.General.Tracers, Colors);

		LoadColorVar(Trails,Menu::Get.Colors.General.Trails, Colors);

		LoadColorVar(HudColor, Menu::Get.Colors.General.HudColor, Colors);

		// PlayerEsp

		LoadColorVar(Box, Menu::Get.Colors.PlayerEsp.Box, Colors);

		LoadColorVar(Health, Menu::Get.Colors.PlayerEsp.Health, Colors);

		LoadColorVar(Name, Menu::Get.Colors.PlayerEsp.Name, Colors);

		LoadColorVar(SnapLine, Menu::Get.Colors.PlayerEsp.SnapLine, Colors);

		LoadColorVar(Weapon, Menu::Get.Colors.PlayerEsp.Weapon, Colors);

		// Chams

		LoadColorVar(Player, Menu::Get.Colors.Chams.Player, Colors);

		LoadColorVar(CWeapon, Menu::Get.Colors.Chams.CWeapon, Colors);

		LoadColorVar(Backtrack, Menu::Get.Colors.Chams.Backtrack, Colors);
	};

	Load_General_Variables();

	Load_Aimbot_Variables();

	Load_Triggerbot_Variables();

	Load_Chams_Variables();

	Load_Visuals_Variables();

	Load_Miscellaneous_Variables();

	Load_Keys_Variables();

	Load_Colors_Variables();

	input_file.close();
}

