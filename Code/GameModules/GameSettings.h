#pragma once
#include <CrySerialization/Decorators/Resources.h>

//! \namespace GameModules
//! \brief The game modules namespace. Here only dependence with the Common is possible.
namespace GameModules
{
//! \class CGameSettings
//! Class for storing settings.
//! Can be serialized and deserialized.
//! It is also used to send/receive from a user menu
class CGameSettings : public Serialization::SStruct
{
public:
	//! Enum for settings type
	enum class ESettingsType
	{
		Unknown,
		All,
		Video,
		Audio,
		Game
	};

	//! Enum for game difficulty
	enum class EDifficulty
	{
		Unknown,
		Easy,
		Medium,
		Hard
	};

	//! \struct SVideoSettings
	//! \brief The structure with video settings
	struct SVideoSettings
	{
		bool isFullscreen = false;
		void Clear()
		{
			isFullscreen = false;
		}
	};

	//! \struct SVideoSettings
	//! \brief The structure with audio settings
	struct SAudioSettings
	{
		bool enableAudio = true;
		void Clear()
		{
			enableAudio = true;
		}
	};

	//! \struct SVideoSettings
	//! \brief The structure with game settings
	struct SGameSettings
	{
		string shipName = "My ship";
		EDifficulty difficulty = EDifficulty::Unknown;
		int speed = 1;
		void Clear()
		{
			shipName.clear();
			difficulty = EDifficulty::Unknown;
			speed = 1;
		}
	};
public:
	//! \brief Clearing settings to default
	void Clear();

	//! \brief Serialization of the class
	//! \param {Serialization::IArchive} archive for serialization/deserialization
	//! \return {bool} true if successful
	bool Serialize(Serialization::IArchive& ar);

	//! \brief Get type of current settings
	//! \return {ESettingsType} type of current settings
	ESettingsType GetType() const;

	//! \brief Save settings to a file with the path {m_settingsFile}
	//! \return {bool} true if saved successful
	bool SaveSettings() const;

	//! \brief Load settings from a file with the path {m_settingsFile}
	//! \return {bool} true if loaded successful
	bool LoadSettings();

	//! \brief Set settings file path {m_settingsFile}
	//! \param settingsFile {string} a path to the file
	void SetSettingsFile(const string &settingsFile);

	//! \brief Get current settings in the Json format
	//! \return {string} Json settings string
	string GetJson() const;

	//! \brief Applies the new settings and saves to a file
	//! Can be used for apply settings from flash
	//! \param settings {CGameSettings} new settings to apply
	void ApplyNewSettings(const CGameSettings& settings);

	//! \brief Apply all settings (can be used to load from a file)
	void ApplyAllSettings() const;

	//! \brief apply video settings
	void ApplyVideoSettings() const;

	//! \brief Apply audio settings
	void ApplyAudioSettings() const;

	//! \brief Apply game settings
	void ApplyGameSettings() const;

	//! \brief Get ship name from settings
	//! \return {string} a ship name
	inline string GetShipName() const { return m_gameSettings.shipName; }

protected:
	ESettingsType m_type = ESettingsType::All;	//!< type of the current settings
	SVideoSettings m_videoSettings;				//!< current video settings
	SAudioSettings m_audioSettings;				//!< current audio settings
	SGameSettings m_gameSettings;				//!< current game settings
	string m_settingsFile = "settings.json";	//!< path to save file
};

} // namespace Common
