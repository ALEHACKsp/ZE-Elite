#include <cstdint>

#define TEXTURE_GROUP_WORLD							"World textures"
#define TEXTURE_GROUP_MODEL							"Model textures"
#define TEXTURE_GROUP_PARTICLE						"Particle textures"
#define TEXTURE_GROUP_SKYBOX						"SkyBox textures"
#define TEXTURE_GROUP_CLIENT_EFFECTS				"ClientEffect textures"
#define TEXTURE_GROUP_OTHER							"Other textures"

class IMaterialVar;

struct studiohwdata_t;

struct StudioDecalHandle_t;


enum MaterialVarFlags_t
{
	MATERIAL_VAR_NO_DRAW = (1 << 2),
	MATERIAL_VAR_FLAT = (1 << 12),
	MATERIAL_VAR_IGNOREZ = (1 << 15),
	MATERIAL_VAR_TRANSLUCENT = (1 << 21),
	MATERIAL_VAR_WIREFRAME = (1 << 28),
};

enum PreviewImageRetVal_t
{
	MATERIAL_PREVIEW_IMAGE_BAD = 0,
	MATERIAL_PREVIEW_IMAGE_OK,
	MATERIAL_NO_PREVIEW_IMAGE,
};

enum MaterialThreadMode_t
{
	MATERIAL_SINGLE_THREADED,
	MATERIAL_QUEUED_SINGLE_THREADED,
	MATERIAL_QUEUED_THREADED
};

enum MaterialPropertyTypes_t
{
	MATERIAL_PROPERTY_NEEDS_LIGHTMAP = 0, // bool
	MATERIAL_PROPERTY_OPACITY, // int (enum MaterialPropertyOpacityTypes_t)
	MATERIAL_PROPERTY_REFLECTIVITY, // vec3_t
	MATERIAL_PROPERTY_NEEDS_BUMPED_LIGHTMAPS // bool
};

struct DrawModelState_t
{
	studiohdr_t* m_pStudioHdr;
	studiohwdata_t* m_pStudioHWData;
	void* m_pRenderable;
	const matrix3x4* m_pModelToWorld;
	StudioDecalHandle_t* m_decals;
	int						m_drawFlags;
	int						m_lod;
};

struct ModelRenderInfo_t
{
	Vector origin;
	Vector angles;
	PVOID* pRenderable;
	const model_t* pModel;
	const matrix3x4* pModelToWorld;
	const matrix3x4* pLightingOffset;
	const Vector* pLightingOrigin;
	int flags;
	int entity_index;
	int skin;
	int body;
	int hitboxset;
	int instance;
	ModelRenderInfo_t()
	{
		pModelToWorld = NULL;
		pLightingOffset = NULL;
		pLightingOrigin = NULL;
	}
};

enum OverrideType_t
{
	OVERRIDE_NORMAL = 0,
	OVERRIDE_BUILD_SHADOWS,
	OVERRIDE_DEPTH_WRITE,
	OVERRIDE_SSAO_DEPTH_WRITE,
};

class CModelRender
{
public:
	void ForcedMaterialOverride(IMaterial* mat, OverrideType_t type = OVERRIDE_NORMAL)
	{
		typedef void(__thiscall* OriginalFn)(PVOID, IMaterial*, OverrideType_t);
		return Get_vFunction<OriginalFn>(this, 1)(this, mat, type);
	}

	void DrawModelExecute(void* state, ModelRenderInfo_t& pInfo, matrix3x4* pCustomBoneToWorld)
	{
		typedef void(__thiscall* OriginalFn)(PVOID, void*, ModelRenderInfo_t&, matrix3x4*);
		return Get_vFunction<OriginalFn>(this, 19)(this, state, pInfo, pCustomBoneToWorld);
	}

	void SuppressEngineLighting(bool t)
	{
		typedef void(__thiscall* OriginalFn)(PVOID, bool);
		return Get_vFunction<OriginalFn>(this, 22)(this, t);
	}
};

class IMaterial
{
public:

	const char* GetName()
	{
		return Get_vFunction<const char* (__thiscall*)(void*)>(this, 0)(this);
	}

	const char* GetTextureGroupName()
	{
		return Get_vFunction<const char* (__thiscall*)(void*)>(this, 1)(this);
	}

	void IncrementReferenceCount()
	{
		return Get_vFunction<void(__thiscall*)(void*)>(this, 12)(this);
	}

	void DecrementReferenceCount()
	{
		return Get_vFunction<void(__thiscall*)(void*)>(this, 13)(this);
	}

	inline void AddRef()
	{
		IncrementReferenceCount();
	}

	inline void Release()
	{
		DecrementReferenceCount();
	}

	void AlphaModulate(float alpha)
	{
		return Get_vFunction<void(__thiscall*)(void*, float)>(this, 27)(this, alpha);
	}

	void ColorModulate(float r, float g, float b)
	{
		return Get_vFunction<void(__thiscall*)(void*, float, float, float)>(this, 28)(this, r, g, b);
	}

	void SetMaterialVarFlag(MaterialVarFlags_t flag, bool on)
	{
		return Get_vFunction<void(__thiscall*)(void*, MaterialVarFlags_t, bool)>(this, 29)(this, flag, on);
	}

	bool GetMaterialVarFlag(MaterialVarFlags_t flag)
	{
		return Get_vFunction<bool(__thiscall*)(void*, MaterialVarFlags_t)>(this, 30)(this, flag);
	}

	const bool IsPrecached()
	{
		return Get_vFunction<const bool(__thiscall*)(void*)>(this, 56)(this);
	}
};

class IMaterialSystem
{
public:
	IMaterial* CreateMaterial(const char* pMaterialName, KeyValues* pVMTKeyValues)
	{
		return Get_vFunction<IMaterial* (__thiscall*)(void*, const char*, KeyValues*)>(this, 70)(this, pMaterialName, pVMTKeyValues);
	}

	IMaterial* FindMaterial(char const* pMaterialName, const char* pTextureGroupName, bool complain = true, const char* pComplainPrefix = NULL)
	{
		return Get_vFunction<IMaterial* (__thiscall*)(void*, char const*, const char*, bool complain, const char*)>(this, 71)(this, pMaterialName, pTextureGroupName, complain, pComplainPrefix);
	}

	unsigned short FirstMaterial()
	{
		return Get_vFunction<unsigned short(__thiscall*)(void*)>(this, 73)(this);
	}

	unsigned short NextMaterial(unsigned short h)
	{
		return Get_vFunction<unsigned short(__thiscall*)(void*, unsigned short)>(this, 74)(this, h);
	}

	unsigned short InvalidMaterial()
	{
		return Get_vFunction<unsigned short(__thiscall*)(void*)>(this, 75)(this);
	}

	IMaterial* GetMaterial(unsigned short h)
	{
		return Get_vFunction<IMaterial* (__thiscall*)(void*, unsigned short)>(this, 76)(this, h);
	};
};