#include "StdAfx.h"
#include "GameSettings.h"

#include <CrySerialization/Enum.h>
//! \namespace GameModules
//! \brief The game modules namespace. Separate block for serialization to avoid function definition not found for a next method.
namespace GameModules
{
SERIALIZATION_ENUM_BEGIN_NESTED(CGameSettings, ESettingsType, "Type")
SERIALIZATION_ENUM(CGameSettings::ESettingsType::Unknown, "", "Unknown")
SERIALIZATION_ENUM(CGameSettings::ESettingsType::All, "all", "All")
SERIALIZATION_ENUM(CGameSettings::ESettingsType::Video, "video", "Video")
SERIALIZATION_ENUM(CGameSettings::ESettingsType::Audio, "audio", "Audio")
SERIALIZATION_ENUM(CGameSettings::ESettingsType::Game, "game", "Game")
SERIALIZATION_ENUM_END()

SERIALIZATION_ENUM_BEGIN_NESTED(CGameSettings, EDifficulty, "Difficulty")
SERIALIZATION_ENUM(CGameSettings::EDifficulty::Unknown, "", "Unknown")
SERIALIZATION_ENUM(CGameSettings::EDifficulty::Easy, "easy", "Easy")
SERIALIZATION_ENUM(CGameSettings::EDifficulty::Medium, "medium", "Medium")
SERIALIZATION_ENUM(CGameSettings::EDifficulty::Hard, "hard", "Hard")
SERIALIZATION_ENUM_END()
}

//! \namespace GameModules
//! \brief The game modules namespace.
namespace GameModules
{
void CGameSettings::Clear()
{
	m_type = ESettingsType::Unknown;
	m_videoSettings.Clear();
	m_audioSettings.Clear();
	m_gameSettings.Clear();
}

bool CGameSettings::Serialize(Serialization::IArchive& ar)
{
	if (ar.isInput())
	{
		Clear();
	}

	SERIALIZE_VAR(ar, m_type);
	if (m_type == ESettingsType::All || m_type == ESettingsType::Video)
	{
		if (!ar(m_videoSettings.isFullscreen, "Fullscreen"))
		{
			CryWarning(VALIDATOR_MODULE_GAME, VALIDATOR_WARNING, "Could not find 'Fullscreen' param");
		}
	}
	if (m_type == ESettingsType::All || m_type == ESettingsType::Audio)
	{
		if (!ar(m_audioSettings.enableAudio, "EnableAudio"))
		{
			CryWarning(VALIDATOR_MODULE_GAME, VALIDATOR_WARNING, "Could not find 'EnableAudio' param");
		}
	}
	if (m_type == ESettingsType::All || m_type == ESettingsType::Game)
	{
		if (!ar(m_gameSettings.shipName, "ShipName"))
		{
			CryWarning(VALIDATOR_MODULE_GAME, VALIDATOR_WARNING, "Could not find 'ShipName' param");
		}
		if (!ar(m_gameSettings.difficulty, "Difficulty"))
		{
			CryWarning(VALIDATOR_MODULE_GAME, VALIDATOR_WARNING, "Could not find 'Difficulty' param");
		}
		if (!ar(m_gameSettings.speed, "Speed"))
		{
			CryWarning(VALIDATOR_MODULE_GAME, VALIDATOR_WARNING, "Could not find 'Speed' param");
		}
	}

	return true;
}

CGameSettings::ESettingsType CGameSettings::GetType() const
{
	return m_type;
}

bool CGameSettings::SaveSettings() const
{
	CRY_ASSERT(m_settingsFile);
	return Serialization::SaveJsonFile(m_settingsFile, *this);
}

bool CGameSettings::LoadSettings()
{
	CRY_ASSERT(m_settingsFile);
	CGameSettings gameSettings;
	if (Serialization::LoadJsonFile(gameSettings, m_settingsFile))
	{
		ApplyNewSettings(gameSettings);
		return true;
	}
	
	return false;
}

void CGameSettings::SetSettingsFile(const string& settingsFile)
{
	m_settingsFile = settingsFile;
}

string CGameSettings::GetJson() const
{
	DynArray<char> outBuffer;
	if (!Serialization::SaveJsonBuffer(outBuffer, *this))
	{
		return "{}";
	}
	string result = string(outBuffer.data(), outBuffer.size());

	return result;
}

void CGameSettings::ApplyNewSettings(const CGameSettings& settings)
{
	const auto currType = settings.GetType();
	if (currType == ESettingsType::All || currType == ESettingsType::Video)
	{
		CryLog("Applying video settings");
		this->m_videoSettings = settings.m_videoSettings;
		ApplyVideoSettings();
	}
	if (currType == ESettingsType::All || currType == ESettingsType::Audio)
	{
		CryLog("Applying audio settings");
		this->m_audioSettings = settings.m_audioSettings;
		ApplyAudioSettings();
	}
	if (currType == ESettingsType::All || currType == ESettingsType::Game)
	{
		CryLog("Applying game settings");
		this->m_gameSettings = settings.m_gameSettings;
		ApplyGameSettings();
	}

	SaveSettings();
}

void CGameSettings::ApplyAllSettings() const
{
	ApplyVideoSettings();
	ApplyAudioSettings();
	ApplyGameSettings();
}

void CGameSettings::ApplyVideoSettings() const
{
	IConsole* pConsole = gEnv->pSystem->GetIConsole();
	if (m_videoSettings.isFullscreen)
	{
		pConsole->ExecuteString("r_FullscreenNativeRes 1");
		pConsole->ExecuteString("r_Fullscreen 1");
	}
	else
	{
		pConsole->ExecuteString("r_Fullscreen 0");
		pConsole->ExecuteString("r_FullscreenNativeRes 0");
	}
}

void CGameSettings::ApplyAudioSettings() const
{
	// TODO: Implement ApplyAudioSettings
	//IConsole* pConsole = gEnv->pSystem->GetIConsole();

}

void CGameSettings::ApplyGameSettings() const
{
	// TODO: Implement ApplyGameSettings
}

} // namespace GameModules

