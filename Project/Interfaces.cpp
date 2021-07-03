#include "SDK.h"

Menu::Variables Menu::Get;

using CreateInterface_Fn = void* (__cdecl*)(const char*, int*);

void* Interfaces::GetInterface(const char* ModuleName, const char* InterfaceName)
{
	CreateInterface_Fn CreateInterface = (CreateInterface_Fn)GetProcAddress(GetModuleHandle(ModuleName), "CreateInterface"); assert(CreateInterface);

	if (CreateInterface) return CreateInterface(InterfaceName, nullptr); return nullptr;
}

void* FindHudElement(const char* HudName)
{
	static auto FindHudElementFn = reinterpret_cast<void* (__thiscall*)(void*, const char*)>(CallableFromRelative(Tools::FindPattern("client.dll", "B9 ? ? ? ? D9 5D F8") + 0x8));

	return FindHudElementFn(Hud, HudName);
}


void Interfaces::Initialize()
{
	// Create InterfaceS

	Panel = (IPanel*)GetInterface("vgui2.dll", "VGUI_Panel009");

	Cvar = (ICvar*)GetInterface("vstdlib.dll", "VEngineCvar004");

	MdlRender = (CModelRender*)GetInterface("engine.dll", "VEngineModel016");

	Engine = (IVEngineClient*)GetInterface("engine.dll", "VEngineClient014");

	BaseClientDLL = (IBaseClientDLL*)GetInterface("client.dll", "VClient017");

	Trace = (IEngineTrace*)GetInterface("engine.dll", "EngineTraceClient003");

	Surface = (ISurface*)GetInterface("vguimatsurface.dll", "VGUI_Surface030");

	ModelInfo = (IVModelInfo*)GetInterface("engine.dll", "VModelInfoClient006");

	GameMovement = (IGameMovement*)GetInterface("client.dll", "GameMovement001");

	g_pEffects = (IEffects*)Interfaces::GetInterface("client.dll", "IEffects001");

	Prediction = (IPrediction*)GetInterface("client.dll", "VClientPrediction001");

	RenderView = (IVRenderView*)GetInterface("engine.dll", "VEngineRenderView014");

	Effects = (IVEfx*)(Interfaces::GetInterface("engine.dll", "VEngineEffects001"));

	DebugOverlay = (IVDebugOverlay*)GetInterface("engine.dll", "VDebugOverlay003");

	EntityList = (CClientEntityList*)GetInterface("client.dll", "VClientEntityList003");

	InputSystem = (IInputSystem*)GetInterface("inputsystem.dll", "InputSystemVersion001");

	EventManager = (IGameEventManager2*)GetInterface("engine.dll", "GAMEEVENTSMANAGER002");

	MatSystemOther = (IMaterialSystem*)GetInterface("MaterialSystem.dll", "VMaterialSystem080");

	EngineSound = (IEngineSound*)Interfaces::GetInterface("engine.dll", "IEngineSoundClient003");

	// Signatures Scan

	Input = **(CInput***)(Tools::FindPattern("client.dll", "8B 0D ? ? ? ? 8B 01 FF 60 44") + 2);

	Globals = *(CGlobalVarsBase**)(Tools::FindPattern("engine.dll", "A1 ? ? ? ? 8B 11 68") + 8);

	ClientState = *(CClientState**)(Tools::FindPattern("engine.dll", "B9 ? ? ? ? FF 76 30") + 0x1);

	MoveHelper = **(IMoveHelper***)(Tools::FindPattern("client.dll", "FF 35 ? ? ? ? 50 8B 11 56") + 2);

	Create_Console_Variable_Location = (void*)Tools::FindPattern("engine.dll", "55 8B EC D9 EE 56 FF 75 18");

	Beams = **(IViewRenderBeams***)(Tools::FindPattern("client.dll", "8B 0D ? ? ? ? 8B 01 FF 50 0C 6A 00") + 2);

	ClientMode = **(ClientModeShared***)(Tools::FindPattern("client.dll", "8B 0D ? ? ? ? 8B 01 5D FF 60 28 CC") + 2);

	Hud = *reinterpret_cast<CHud**>(Tools::FindPattern("client.dll","B9 ? ? ? ? D9 5D F8") + 0x1); assert(Hud);

	HudScope = (CHudScope*)FindHudElement("CHudScope"); assert(HudScope);

	// Members

	if (ClientMode) if (ChatElement = (CHudChat*)(((uintptr_t*)(ClientMode))[4]); ChatElement)ChatElement->ChatPrintf(0, 0, "\x04ZE-Elite:"" \x03Have Fun.");

	// Other

	gOffsets.GetOffsets();

	// Assert

	assert(BaseClientDLL), assert(ClientMode), assert(ChatElement), assert(EngineSound), assert(EntityList), assert(Globals), assert(Surface), assert(Engine), assert(Cvar);

	assert(GameMovement), assert(Prediction), assert(ModelInfo), assert(Beams), assert(Input), assert(MoveHelper), assert(Trace), assert(MdlRender), assert(RenderView), assert(MatSystemOther),

	assert(ClientState), assert(EventManager), assert(InputSystem), assert(Effects);

	// Other


	SpoofConVarName("sv_cheats", "NewSvCheats");

	Engine->ClientCmd_Unrestricted("NewSvCheats 1");

	SpoofConVarName("fog_override", "NewFogOverride");

	//credits valve.

	ConCommandBase* var;	// Temporary Pointer to cvars

	if (Cvar) for (var = Cvar->GetCommands(); var; var = var->m_pNext)
	{
		if (!var->IsCommand() && var->IsFlagSet(FCVAR_DEVELOPMENTONLY) || var->IsFlagSet(FCVAR_HIDDEN) || var->IsFlagSet(FCVAR_NOT_CONNECTED))
		{
			var->m_nFlags &= ~(FCVAR_DEVELOPMENTONLY | FCVAR_HIDDEN | FCVAR_NOT_CONNECTED);

			ConMsg("Convar %s has been unlocked\n", var->m_pszName);
		}
	}
}

C_PlayerResource* PlayerResource{ nullptr };

IGameEventManager2* EventManager{ nullptr };

IMaterialSystem* MatSystemOther{ nullptr };

CClientEntityList* EntityList{ nullptr };

IBaseClientDLL* BaseClientDLL{ nullptr };

ClientModeShared* ClientMode{ nullptr };

IVDebugOverlay* DebugOverlay{ nullptr };

IGameMovement* GameMovement{ nullptr };

IEngineSound* EngineSound{ nullptr };

CClientState* ClientState{ nullptr };

IInputSystem* InputSystem{ nullptr };

IVRenderView* RenderView{ nullptr };

CGlobalVarsBase* Globals{ nullptr };

CModelRender* MdlRender{ nullptr };

IPrediction* Prediction{ nullptr };

IViewRenderBeams* Beams{ nullptr };

IMoveHelper* MoveHelper{ nullptr };

IVEngineClient* Engine{ nullptr };

IVModelInfo* ModelInfo{ nullptr };

CHudChat* ChatElement{ nullptr };

IEffects* g_pEffects{ nullptr };

IEngineTrace* Trace{ nullptr };

CHudScope* HudScope{ nullptr };

ISurface* Surface{ nullptr };

IVEfx* Effects{ nullptr };

CInput* Input{ nullptr };

IPanel* Panel{ nullptr };

ICvar* Cvar{ nullptr };

CHud* Hud{ nullptr };
