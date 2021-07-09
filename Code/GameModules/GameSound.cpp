#include "StdAfx.h"
#include "GameSound.h"

#include <CryAudio/IAudioSystem.h>

// Register the factory
// Note that this has to be done in a source file that is not included anywhere else.
CRYREGISTER_SINGLETON_CLASS(GameModules::CGameSound)

namespace GameModules
{

CGameSound::CGameSound()
{
	IConsole* pConsole = gEnv->pSystem->GetIConsole();
	if (pConsole)
	{
		pConsole->ExecuteString("s_DrawDebug 1");
	}
}

bool CGameSound::PlaySoundTrigger(const string& soundTrigger) const
{
	CryLog("Play sound trigger: %s", soundTrigger);
	// Get the internal identifier for the trigger, computed at compile-time.
	// Note that the trigger itself is created by the Audio Controls Editor (ACE).
	gEnv->pAudioSystem->ExecuteTrigger(CryAudio::StringToId("get_focus"));
	const CryAudio::ControlId audioTriggerId = CryAudio::StringToId(soundTrigger);
	if (audioTriggerId != CryAudio::InvalidControlId)
	{
		// TODO: can't play in pause state -> maybe use ADX2?
		gEnv->pAudioSystem->ExecuteTrigger(audioTriggerId);
	}
	else
	{
		CryLog("Invalid control id for the sound file");
		return false;
	}

	return true;
}

bool CGameSound::PlaySoundFile(const string& soundFile) const
{
	// #include <Mmsystem.h> #pragma comment(lib, "winmm.lib") PlaySound(soundFile.c_str(), NULL, SND_FILENAME);
	return true;
}


} // namespace GameModules
