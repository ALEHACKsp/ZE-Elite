#ifndef Arrays_H
#define Arrays_H

inline std::vector<std::string> HitmarkerOverlays;

inline std::vector<std::string> ScopeOverlays;

inline std::vector<std::string> TrailMaterials;

inline std::array<const char*, 28> EffectMaterials
{
	"Default",
	"spark",
	"bluemuzzle",
	"gunshipmuzzle",
	"gunshiptracer",
	"bluespark",
	"flashbang",
	"flashbang_white",
	"nightvision",
	"blood_gore",
	"blood_drop",
	"blood_puff",
	"muzzleflashX",
	"muzzleflash1",
	"muzzleflash2",
	"muzzleflash3",
	"muzzleflash4",
	"tesla_glow_noz",
	"combinemuzzle2",
	"fleck_cement1",
	"fleck_cement2",
	"fleck_antlion1",
	"fleck_antlion2",
	"fleck_wood1",
	"fleck_wood2",
	"blood",
	"blood2",
	"Custom"
};

inline std::vector<std::string> SkyBoxes
{
	"Default",
	"cx",
	"hav",
	"italy",
	"jungle",
	"office",
	"tides",
	"train_hdr",
	"assault",
	"de_cobble",
	"sky_dust_hdr",
	"sky_c17_05",
	"militia_hdr",
	"de_cobble_hdr",
	"de_piranesi",
	"sky_day02_02",
	"sky_day01_01",
	"sky_day01_04",
	"sky_day01_05",
	"sky_day01_06",
	"sky_day01_07",
	"sky_day01_08",
	"sky_day01_09",
	"sky_wasteland02"
};

inline std::array<const char*, 13>TracerSprites
{
	"zerogxplode",
	"steam1",
	"bubble",
	"laserbeam",
	"blueglow1",
	"purpleglow1",
	"purplelaser1",
	"white",
	"physbeam",
	"gunsmoke",
	"radio",
	"ledglow",
	"glow01"
};

inline std::array<const char*, 15>KillEffects
{
	"Off",
	"FX_Tesla",
	"ManhackSparks",
	"BoltImpact",
	"Explosion",
	"GlassImpact",
	"GunshipImpact",
	"gunshotsplash",
	"HelicopterMegaBomb",
	"HunterDamage",
	"ParticleEffect",
	"Smoke",
	"waterripple",
	"watersplash",
	"WaterSurfaceExplosion",
};

inline constexpr const char* LayoutsNames[]
{
	"Single",
	"Double Vertical",
	"Double Horizontal",
	"Triple Vertical",
	"Triple Horizontal"
};

inline std::vector<std::string> Killmessages = 
{
	"LOL, you died like a chicken",
	"$nick deserved to be killed" 
};

inline constexpr std::array<const char*, 6>HudList = {

		"HudLocation",
		"HudHealth",
		"HudAccount",
		"HudAmmo",
		"HudRoundTimer",
		"HudArmor"
};

inline Vector MultipointVectors[] = {
	Vector(0,0,0), Vector(0,0,1.5),Vector(0,0,3),Vector(0,0,4), Vector(0,0,-2), Vector(0,0,-4), Vector(0,0,4.8),
	Vector(0,0,5), Vector(0,0,5.4), Vector(0,3,0), Vector(3,0,0),Vector(-3,0,0),Vector(0,-3,0), Vector(0,2,4.2),
	Vector(0,-2,4.2), Vector(2,0,4.2), Vector(-2,0,4.2),  Vector(3.8,0,0), Vector(-3.8,0,0),Vector(0,3.6,0),
	Vector(0,-3.6,0), Vector(0,1.2,3.2), Vector(0,0.6,1.4), Vector(0,3.1,5.1), Vector(0,0,6.2), Vector(0,2.5,0),
	Vector(2.1,2.1,2.1) };

#endif
