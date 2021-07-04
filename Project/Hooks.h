#ifndef Hooks_H
#define Hooks_H

#pragma once

#include "VMT_Hook.h"

namespace Hooks
{
	void Initialize(HMODULE MyDLL) noexcept;

	void Uninitialize() noexcept;

	extern VMTHook* Panel_Table;
	extern VMTHook* Engine_Table;
	extern VMTHook* Client_Table;
	extern VMTHook* Surface_Table;
	extern VMTHook* ClientMod_Table;
	extern VMTHook* D3Ddevice_Table;
	extern VMTHook* RenderView_Table;
	extern VMTHook* EngineSound_Table;
	extern VMTHook* NetChannel_Table;
	extern VMTHook* ModelRender_Table;
	extern VMTHook* GameMovement_Table;
	extern VMTHook* BSPTreeQuery_Table;
	extern VMTHook* MatSystemOther_Table;
	extern VMTHook* GameEventManager_Table;
}

#endif
