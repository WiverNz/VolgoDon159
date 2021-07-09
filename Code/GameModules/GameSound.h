#pragma once

#include <CrySerialization/Decorators/Resources.h>
//#include <CryAudio/IAudioSystem.h>

//! \namespace GameModules
//! \brief The game modules namespace. Here only dependence with the Common is possible.
namespace GameModules
{
//! \struct IGameSound
//! \brief Interface for {CGameSound}
struct IGameSound : public ICryUnknown
{
	CRYINTERFACE_DECLARE_GUID(IGameSound, "5CC14F8B-58EC-45CA-AF4D-5C21056E9C80"_cry_guid);
public:
	//! \brief Play a sound trigger
	//! \param soundTrigger {string} sound trigger to play
	//! \return {bool} sound started successfully
	virtual bool PlaySoundTrigger(const string& soundTrigger) const = 0;
	//! \brief Play a sound file
	//! \param soundFile {string} sound file to play
	//! \return {bool} sound started successfully
	virtual bool PlaySoundFile(const string& soundFile) const = 0;
};

DECLARE_SHARED_POINTERS(IGameSound);

//! \class CGameSound
//! \brief The class that controls the audio in the game
class CGameSound : public IGameSound
{
	CRYINTERFACE_SIMPLE(IGameSound)
	CRYGENERATE_SINGLETONCLASS_GUID(CGameSound, "CGameSound", "E2B8928A-EE1B-46E1-9178-47C79C130514"_cry_guid)

	CGameSound();
	virtual ~CGameSound() = default;
public:
	bool PlaySoundTrigger(const string& soundTrigger) const override;
	bool PlaySoundFile(const string& soundFile) const override;
};

DECLARE_SHARED_POINTERS(CGameSound);

//! \brief Get game sound interface pointer
static IGameSoundPtr GetIGameSoundPtr()
{
	IGameSoundPtr pGameSound;
	CryCreateClassInstanceForInterface(cryiidof<IGameSound>(), pGameSound);

	return pGameSound;
}

} // namespace GameModules