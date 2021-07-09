// Copyright 2001-2019 Crytek GmbH / Crytek Group. All rights reserved.

#pragma once

#include <CryCore/Project/CryModuleDefs.h>
#define eCryModule eCryM_EnginePlugin
#define GAME_API   DLL_EXPORT

#include <CryCore/Platform/platform.h>
#include <CrySystem/ISystem.h>
#include <Cry3DEngine/I3DEngine.h>
#include <CryNetwork/ISerialize.h>

#define TESTS_MODULE_NAME "VolgoDon"

//////////////////////////////////////////////////////////////////////////
//! Reports a Game Warning to validator with WARNING severity.
inline void GameWarning(const char* format, ...)
{
	if (!format)
		return;

	va_list args;
	va_start(args, format);
	GetISystem()->WarningV(VALIDATOR_MODULE_GAME, VALIDATOR_WARNING, 0, 0, format, args);
	va_end(args);
}
