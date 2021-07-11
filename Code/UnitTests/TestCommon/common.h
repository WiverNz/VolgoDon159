#pragma once

constexpr char TESTS_MODULE_NAME[] = "VolgoDon";
constexpr char TESTS_CONSOLE_MODULE_NAME[] = "VolgoDonConsole";

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
