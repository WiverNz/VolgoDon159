#pragma once

#include <CrySystem/ICryPlugin.h>
#include <CryGame/IGameFramework.h>
#include <CryEntitySystem/IEntityClass.h>
#include <CryNetwork/INetwork.h>
#include <CryAction/IActionMapManager.h>

// Forward declaration of game classes
namespace Navigation
{
	class CPlayerComponent;
}
namespace UserInterface
{
	class IMainMenu;
	class IShipInterface;
}


// The entry-point of the application
// An instance of CGamePlugin is automatically created when the library is loaded
// We then construct the local player entity and CPlayerComponent instance when OnClientConnectionReceived is first called.
class CGamePlugin 
	: public Cry::IEnginePlugin
	, public ISystemEventListener
	, public INetworkedClientListener
{
public:
	CRYINTERFACE_SIMPLE(Cry::IEnginePlugin)
	CRYGENERATE_SINGLETONCLASS_GUID(CGamePlugin, "Blank", "f01244b0-a4e7-4dc6-91e1-0ed18906fe7c"_cry_guid)

	virtual ~CGamePlugin();
	
	// Cry::IEnginePlugin
	virtual const char* GetCategory() const override { return "Game"; }
	virtual bool Initialize(SSystemGlobalEnvironment& env, const SSystemInitParams& initParams) override;
	// ~Cry::IEnginePlugin

	// ISystemEventListener
	virtual void OnSystemEvent(ESystemEvent event, UINT_PTR wparam, UINT_PTR lparam) override;
	// ~ISystemEventListener

	// INetworkedClientListener
	//! Sent to the local client on disconnect
	virtual void OnLocalClientDisconnected(EDisconnectionCause cause, const char* description) override {}

	//! Sent to the server when a new client has started connecting
	//! Return false to disallow the connection
	virtual bool OnClientConnectionReceived(int channelId, bool bIsReset) override;
	//! Sent to the server when a new client has finished connecting and is ready for gameplay
	//! Return false to disallow the connection and kick the player
	virtual bool OnClientReadyForGameplay(int channelId, bool bIsReset) override;
	//! Sent to the server when a client is disconnected
	virtual void OnClientDisconnected(int channelId, EDisconnectionCause cause, const char* description, bool bKeepClient) override;
	//! Sent to the server when a client is timing out (no packets for X seconds)
	//! Return true to allow disconnection, otherwise false to keep client.
	virtual bool OnClientTimingOut(int channelId, EDisconnectionCause cause, const char* description) override { return true; }
	// ~INetworkedClientListener

	//! Helper function to call the specified callback for every player in the game
	void IterateOverPlayers(std::function<void(Navigation::CPlayerComponent& player)> func) const;

	//! Helper function to get the CGamePlugin instance
	//! Note that CGamePlugin is declared as a singleton, so the CreateClassInstance will always return the same pointer
	static CGamePlugin* GetInstance()
	{
		return cryinterface_cast<CGamePlugin>(CGamePlugin::s_factory.CreateClassInstance().get());
	}

	//! Called for the ESYSTEM_EVENT_GAME_POST_INIT event
	void OnGamePostInit();

	//! Called for the ESYSTEM_EVENT_REGISTER_SCHEMATYC_ENV event
	void OnRegisterSchematyc();

	//! Earliest point of update in a frame, before ISystem itself is updated
	//! Called on a plug-in if it has called EnableUpdate(EUpdateStep::BeforeSystem, true)
	void UpdateBeforeSystem() override;

	//! Called after ISystem has been updated, this is the main update where most game logic is expected to occur
	//! This is the default update that should be preferred if you don't need any special behavior
	//! Called on a plug-in if it has called EnableUpdate(EUpdateStep::MainUpdate, true)
	void MainUpdate(float frameTime) override;

	//! Called for the ESYSTEM_EVENT_GAME_PAUSED event
	void OnGamePaused();

	//! Called for the ESYSTEM_EVENT_GAME_RESUMED event
	void OnGameResumed();

protected:
	//! Map containing player components, key is the channel id received in OnClientConnectionReceived
	std::unordered_map<int, EntityId> m_players;

	//! Map containing ship-autopilot
	std::unordered_map<int, EntityId> m_shipAutoPilots;

	//! Main menu pointer
	std::shared_ptr<UserInterface::IMainMenu> m_pMainMenu = nullptr;

	//! Ship interface pointer
	std::shared_ptr<UserInterface::IShipInterface> m_pShipInterface = nullptr;
};