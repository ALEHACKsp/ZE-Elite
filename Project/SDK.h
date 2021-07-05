#pragma once
#ifndef SDK
#define SDK

#ifdef NDEBUG
#undef NDEBUG
#endif

#pragma comment(lib, "Winmm.lib") 

#include <type_traits>
#include <windows.h>
#include <iostream>
#include <intrin.h>
#include <vector>
#include <deque>
#include <map>

#include "CRC.h"
#include "CClientEntityList.h"
#include "IVDebugOverlay.h"
#include "IVEngineClient.h"
#include "IEngineTrace.h"
#include "IMaterialVar.h"
#include "netmessages.h"
#include "FindPattern.h"
#include "IRenderView.h"
#include "InputSystem.h"
#include "IGameEvent.h"
#include "Interfaces.h"
#include "Defintions.h"
#include "GlobalVars.h"
#include "VMT_Hook.h"
#include "CUserCmd.h"
#include "ISurface.h"
#include "Netvars.h"
#include "CCSPlayer.h"
#include "ICvar.h"
#include "Hooks.h"
#include "Color.h"
#include "CHud.h"
#include "Menu.h"
#include "Math.h"

#include "IVEfx.h"
#include "studio.h"
#include "Arrays.h"
#include "ConVar.h"
#include "model_t.h"
#include "datamap.h"
#include "Offsets.h"
#include "Globals.h"
#include "JunkCode.h"
#include "IEffects.h"
#include "IMaterial.h"
#include "CViewSetup.h"
#include "CEffectData.h"
#include "IPrediction.h"
#include "IVModelInfo.h"
#include "CClientState.h"
#include "IViewRenderBeams.h"
#include "vphysics_interface.h"

#endif
