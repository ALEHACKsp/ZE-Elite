#ifndef Interface
#define Interface

#pragma once

#include <windows.h>

class IGameEventManager2;
class CClientEntityList;
class IViewRenderBeams;
class C_PlayerResource;
class ClientModeShared;
class CGlobalVarsBase;
class IMaterialSystem;
class IVDebugOverlay;
class IBaseClientDLL;
class IVEngineClient;
class IGameMovement;
class IInputSystem;
class IViewEffects;
class IEngineTrace;
class CModelRender;
class IVRenderView;
class CClientState;
class IEngineSound;
class IVModelInfo;
class IMoveHelper;
class IPrediction;
class CHudScope;
class ISurface;
class CHudChat;
class IEffects;
class CInput;
class IPanel;
class ICvar;
class IVEfx;
class CHud;


extern CHud* Hud;
extern ICvar* Cvar;
extern IPanel* Panel;
extern CInput* Input;
extern IVEfx* Effects;
extern ISurface* Surface;
extern CHudScope* HudScope;
extern IEngineTrace* Trace;
extern IEffects* g_pEffects;
extern CHudChat* ChatElement;
extern IVEngineClient* Engine;
extern IVModelInfo* ModelInfo;
extern IMoveHelper* MoveHelper;
extern IViewRenderBeams* Beams;
extern CModelRender* MdlRender;
extern IPrediction* Prediction;
extern CGlobalVarsBase* Globals;
extern IVRenderView* RenderView;
extern IInputSystem* InputSystem;
extern CClientState* ClientState;
extern IEngineSound* EngineSound;
extern IGameMovement* GameMovement;
extern ClientModeShared* ClientMode;
extern IVDebugOverlay* DebugOverlay;
extern CClientEntityList* EntityList;
extern IBaseClientDLL* BaseClientDLL;
extern IMaterialSystem* MatSystemOther;
extern IGameEventManager2* EventManager;
extern C_PlayerResource* PlayerResource;

namespace Interfaces
{
	void* GetInterface(const char* ModuleName, const char* InterfaceName);

	void Initialize();
}

#endif