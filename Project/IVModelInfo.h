#pragma once

enum modtype_t
{
	mod_bad = 0,
	mod_brush,
	mod_sprite,
	mod_studio
};

class IVModelInfo
{
public:
	model_t* GetModel(int index)
	{
		typedef model_t* (__thiscall* GetModelFn)(void*, int);
		return Get_vFunction<GetModelFn>(this, 1)(this, index);
	}

	int	GetModelIndex(const char* name)
	{
		typedef int(__thiscall* GetModelIndexFn)(void*, const char*);
		return Get_vFunction< GetModelIndexFn >(this, 2)(this, name);
	}

	const char* GetMdlName(const model_t* model)
	{
		typedef const char* (__thiscall* GetModelNameFn)(void*, const model_t*);
		return Get_vFunction< GetModelNameFn >(this, 3)(this, model);
	}

	int GetModelType(const model_t* model)
	{
		typedef int(__thiscall* test)(void*, const model_t*);
		return Get_vFunction< test >(this, 9)(this, model);
	}

	studiohdr_t* GetStudiomodel(const void* model)
	{
		typedef studiohdr_t* (__thiscall* GetStudiomodel_t)(void*, const void*);
		return Get_vFunction<GetStudiomodel_t>(this, 28)(this, model);
	}

	const model_t* FindOrLoadModel(const char* name)
	{
		return Get_vFunction<model_t*(__thiscall*)(void*, const char*)>(this, 39)(this, name);
	}

	void GetModelMaterials(const model_t* model, int count, IMaterial** ppMaterial)//16
	{
		Get_vFunction<void(__thiscall*)(void*, const model_t* model, int count, IMaterial** ppMaterial)>(this, 16)(this, model, count, ppMaterial);
	}
};