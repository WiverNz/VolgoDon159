#pragma once

#include <CryExtension/ICryFactory.h>
#include <CryExtension/ClassWeaver.h>
#include <CryExtension/CryCreateClassInstance.h>

#include <CrySystem/ConsoleRegistration.h>

#include "GameSettings.h"

//! \namespace GameModules
//! \brief The game modules namespace. Here only dependence with the Common is possible.
namespace GameModules
{

//! \brief Events that are sent by the game manager to subscribers
enum class EGameManagerEvent
{
	GM_EVENT_EXTEND_DEBUG_UPDATE		// Sent when value ga_extendDebug is updated
};

//! \struct IGameManagerListener
//! \brief Interface for subscribers of events from the game manager
struct IGameManagerListener
{
	virtual ~IGameManagerListener() {}
	virtual void OnGameManagerEvent(EGameManagerEvent event) = 0;
};

//! \struct IGameManager
//! \brief Interface for {CGameManager}
struct IGameManager : public Cry::IDefaultModule
{
	CRYINTERFACE_DECLARE_GUID(IGameManager, "A0CDBAAE-50B2-4663-91D3-59A05FDBA953"_cry_guid);
public:
	//! \brief generating next controlling id for the player or for the autopilot
	//! \return {int} generated id
	virtual int GenNextControllingId() = 0;
	//! \brief checking if flash elements can be used
	//! \return {bool} true if we need to skip flash integration
	virtual bool IsSkipFlash() const = 0;
	//! \brief setting if flash elements can be used
	//! \param isSkipFlash {bool} skip flash integration
	virtual void SetSkipFlash(bool isSkipFlash) = 0;
	//! \brief get game settings
	//! \return {CGameSettings} game settings
	virtual CGameSettings& GetGameSettings() = 0;
	//! \brief checking if extended debugging is needed
	//! \return {bool} value for CVar ga_extendDebug (used for flash debugging) 
	virtual bool IsExtendDebug() const = 0;
	//! \brief manager initialization
	//! Called from initialization for CGamePlugin
	//! \param env {SSystemGlobalEnvironment} Global environment. Contains pointers to all global often needed interfaces
	//! \param initParams {SSystemInitParams} pointer to a structure, containing system initialization setup
	//! \return {bool} true if initialized correctly
	virtual bool Initialize(SSystemGlobalEnvironment& env, const SSystemInitParams& initParams) = 0;
	//! \brief add a game manager listener
	//! \param pListener {IGameManagerListener*} a listener to add
	virtual void RegisterListener(IGameManagerListener* pListener) = 0;
	//! \brief remove a game manager listener
	//! \param pListener {IGameManagerListener*} a listener to remove
	virtual void UnregisterListner(IGameManagerListener* pListener) = 0;
	//! \brief stop sending events
	//! \param isSupress {bool} if true events will not be sent
	virtual void SetSupressEvents(bool isSupress) = 0;
	//! \brief sending an event
	//! \param event {EGameManagerEvent} an event to send
	virtual void SendEvent(EGameManagerEvent event) = 0;
};

DECLARE_SHARED_POINTERS(IGameManager);

//! \brief Get game manager interface pointer
static IGameManagerPtr GetIGameManagerPtr()
{
	IGameManagerPtr pGameManager;
	CryCreateClassInstanceForInterface(cryiidof<IGameManager>(), pGameManager);

	return pGameManager;
}

//! \class CGameManager
//! \brief Game manager
class CGameManager : public IGameManager
{
	CRYINTERFACE_BEGIN()
	CRYINTERFACE_ADD(Cry::IDefaultModule)
	CRYINTERFACE_ADD(IGameManager)
	CRYINTERFACE_END()

	CRYGENERATE_SINGLETONCLASS_GUID(CGameManager, "CGameManager", "9D13DCDD-5F9E-4C85-9903-48C1973F182E"_cry_guid)

	CGameManager() = default;
	virtual ~CGameManager() = default;
public:
	inline int GenNextControllingId() override;
	inline bool IsSkipFlash() const override;
	inline void SetSkipFlash(bool isSkipFlash) override;
	inline CGameSettings& GetGameSettings() override;
	virtual bool IsExtendDebug() const override;

	// Inherited via IGameManager
	const char* GetName() const override;
	const char* GetCategory() const override;
	bool Initialize(SSystemGlobalEnvironment& env, const SSystemInitParams& initParams) override;
	void RegisterListener(IGameManagerListener* pListener) override;
	void UnregisterListner(IGameManagerListener* pListener) override;
	void SetSupressEvents(bool isSupress) override;
	void SendEvent(EGameManagerEvent event) override;

protected:
	static void OnExtendDebugChanged(ICVar* pCVar = nullptr);

protected:
	int m_lastShipControllingId = -1;				//!< last ship controlling id
	bool m_isSkipFlash = false;						//!< do not use flash interface
	CGameSettings m_gameSettings;					//!< current game settings
	ICVar* m_pExtendDebugCVar = nullptr;			//!< this CVar is used to debug the game UI, flash menu
	int ga_extendDebug = 0;							//!< CVar id for the m_pExtendDebugCVar
	std::vector<IGameManagerListener*> m_listeners;	//!< event listeners
	bool m_isSupressEvents = false;					//!< flag to stop sending events
};

DECLARE_SHARED_POINTERS(CGameManager);

//! \brief Get game manager pointer
ILINE CGameManager* GetPGameManager()
{
	static IGameManagerPtr pGameManager(GetIGameManagerPtr());

	return static_cast<CGameManager*>(pGameManager.get());
};

} // namespace GameModules
