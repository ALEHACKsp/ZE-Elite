#include "MaterialManager.h"
#include "IKeyValues.h"
#include "Offsets.h"
#include <fstream>
#include "SDK.h"

CMat gMat;

void CMat::Initialize()
{
	CHECKPTR(Chams[0] = CreateMaterial(false, false));
	CHECKPTR(Chams[1] = CreateMaterial(false, true, false, true));

	Chams[0]->IncrementReferenceCount();
	Chams[1]->IncrementReferenceCount();

	std::ofstream("cstrike\\materials\\Water.vmt") << R"#("Refract"
		{
			"$model" 1
				"$refractamount" "2.0"
				"$refracttint" "[0 1 1 100]"
				"$dudvmap" "dev/water_dudv"
				"$normalmap" "dev/water_normal"
				"$surfaceprop" "water"
				"$bumpframe" "0"
				"Proxies"
			{
				"AnimatedTexture"
				{
					"animatedtexturevar" "$normalmap"
						"animatedtextureframenumvar" "$bumpframe"
						"animatedtextureframerate" 30.00
				}
				"TextureScroll"
				{
					"texturescrollvar" "$bumptransform"
						"texturescrollrate" .05
						"texturescrollangle" 45.00
				}
			}
		})#";

	std::ofstream("cstrike\\materials\\glass0.vmt") << R"#("UnlitGeneric"
	{
		    "$basetexture"	"detail\dt_metal1"
			"$surfaceprop" "glass"
			"$color" "[ .05 .05 .05 ]"
			"$additive" "1"
			"$envmap"		"env_cubemap"
			"$envmaptint" "[1 1 1]"
            "$model" "1"
	})#";

	std::ofstream("cstrike\\materials\\glass1.vmt") << R"#("UnlitGeneric"
	{
		    "$basetexture"	"Metal/metaldoor017a"
			"$surfaceprop" "metal"
			"$color" "[ .05 .05 .05 ]"
			"$additive" "1"
			"$envmap"		"env_cubemap"
			"$envmaptint" "[1 1 1]"
            "$model" "1"
	})#";

	std::ofstream("cstrike\\materials\\glass2.vmt") << R"#("UnlitGeneric"
	{
		    "$basetexture"	"glass/glasswindow028a"
			"$surfaceprop" "glass"
			"$color" "[ .05 .05 .05 ]"
			"$additive" "1"
			"$envmap"		"env_cubemap"
			"$envmaptint" "[1 1 1]"
            "$model" "1"
	})#";

	std::ofstream("cstrike\\materials\\concrete.vmt") << R"#("UnlitGeneric"
	{
		    "$basetexture"	"stone\infflrb"
			"$surfaceprop" "concrete"
			"$color" "[ .05 .05 .05 ]"
			"$additive" "1"
			"$envmap"		"env_cubemap"
			"$envmaptint" "[1 1 1]"
            "$model" "1"
	})#";

	Chams[2] = MatSystemOther->FindMaterial("debug/debugambientcube", NULL);

	Chams[3] = MatSystemOther->FindMaterial("Water", TEXTURE_GROUP_MODEL);

	Chams[4] = MatSystemOther->FindMaterial("glass0", TEXTURE_GROUP_MODEL);

	Chams[5] = MatSystemOther->FindMaterial("glass1", TEXTURE_GROUP_MODEL);

	Chams[6] = MatSystemOther->FindMaterial("glass2", TEXTURE_GROUP_MODEL);

	Chams[7] = MatSystemOther->FindMaterial("concrete", TEXTURE_GROUP_MODEL);

	Chams[3]->IncrementReferenceCount();
	Chams[4]->IncrementReferenceCount();
	Chams[5]->IncrementReferenceCount();
	Chams[6]->IncrementReferenceCount();
	Chams[7]->IncrementReferenceCount();

	auto pMaterial = MatSystemOther->FindMaterial("models/weapons/v_models/Erion_USP/erion_usp.vmt", NULL);

	if (pMaterial) pMaterial->AddRef();
}

Matptr CMat::CreateMaterial(bool IgnoreZ, bool Flat, bool Wireframe, bool Shiny)
{
	static int created = 0;

	static const char tmp[] =
	{
		"\"%s\"\
		\n{\
		\n\t\"$basetexture\" \"vgui/white_additive\"\
		\n\t\"$envmap\" \"%s\"\
		\n\t\"$normalmapalphaenvmapmask\" \"%i\"\
		\n\t\"$envmapcontrast\" \"%i\"\
		\n\t\"$model\" \"1\"\
		\n\t\"$flat\" \"1\"\
		\n\t\"$nocull\" \"0\"\
		\n\t\"$selfillum\" \"1\"\
		\n\t\"$halflambert\" \"1\"\
		\n\t\"$nofog\" \"0\"\
		\n\t\"$ignorez\" \"%i\"\
		\n\t\"$znearer\" \"0\"\
		\n\t\"$wireframe\" \"%i\"\
        \n}\n"
	};

	const char* baseType = (Flat ? "UnlitGeneric" : "VertexLitGeneric");
	char material[512];
	sprintf_s(material, sizeof(material), tmp, baseType, Shiny ? "env_cubemap" : "", Shiny ? 1 : 0, Shiny ? 1 : 0, (IgnoreZ) ? 1 : 0, (Wireframe) ? 1 : 0);

	char name[512];
	sprintf_s(name, sizeof(name), "#sparklyfx_mat_%i.vmt", created);
	created++;

	KeyValues* keyValues = (KeyValues*)malloc(sizeof(KeyValues));
	KeyValues::Initialize(keyValues, baseType);
	KeyValues::LoadFromBuffer(keyValues, name, material);

	IMaterial* createdMaterial = MatSystemOther->CreateMaterial(name, keyValues);
	if (!createdMaterial)
		return nullptr;

	createdMaterial->IncrementReferenceCount();

	return createdMaterial;
}

Matptr CMat::CreateMaterialOther(bool IgnoreZ, bool Flat, bool Wireframe, bool Shiny)
{
	static int created = 0;

	static const char tmp[] =
	{
		"\"%s\"\
		\n{\
		\n\t\"$basetexture\" \"vgui/white_additive\"\
		\n\t\"$envmap\" \"%s\"\
		\n\t\"$normalmapalphaenvmapmask\" \"%i\"\
		\n\t\"$envmapcontrast\" \"%i\"\
		\n\t\"$model\" \"1\"\
		\n\t\"$flat\" \"1\"\
		\n\t\"$nocull\" \"0\"\
		\n\t\"$selfillum\" \"1\"\
		\n\t\"$halflambert\" \"1\"\
		\n\t\"$nofog\" \"0\"\
		\n\t\"$ignorez\" \"%i\"\
		\n\t\"$znearer\" \"0\"\
		\n\t\"$wireframe\" \"%i\"\
        \n}\n"
	};

	const char* baseType = (Flat ? "UnlitGeneric" : "VertexLitGeneric");
	char material[512];
	sprintf_s(material, sizeof(material), tmp, baseType, Shiny ? "env_cubemap" : "", Shiny ? 1 : 0, Shiny ? 1 : 0, (IgnoreZ) ? 1 : 0, (Wireframe) ? 1 : 0);

	char name[512];
	sprintf_s(name, sizeof(name), "#sparklyfx_mat_%i.vmt", created);
	created++;

	KeyValues* keyValues = (KeyValues*)malloc(sizeof(KeyValues));
	KeyValues::Initialize(keyValues, baseType);
	KeyValues::LoadFromBuffer(keyValues, name, material);

	IMaterial* createdMaterial = MatSystemOther->CreateMaterial(name, keyValues);
	if (!createdMaterial)
		return nullptr;

	createdMaterial->IncrementReferenceCount();

	return createdMaterial;
}

void ForceMaterial(Color color, Matptr material, bool useColor, bool forceMaterial)
{
	if (useColor)
	{
		float blend[3] = { (float)color[0] / 255.f, (float)color[1] / 255.f, (float)color[2] / 255.f };
		float alpha = (float)color[3] / 255.f;

		if (RenderView->GetBlend() != alpha)
		{
			RenderView->SetBlend(alpha);
		}

		float ComparationColor[3];

		RenderView->GetColorModulation(ComparationColor);

		if (ComparationColor != blend)
		{
			RenderView->SetColorModulation(blend);
		}
	}

	if (forceMaterial)
		MdlRender->ForcedMaterialOverride(material);
}

void CMat::ResetMaterial()
{
	static const float flDefault[3] = { 1, 1, 1 };
	RenderView->SetBlend(1);
	RenderView->SetColorModulation(flDefault);
	MdlRender->ForcedMaterialOverride(nullptr);
}