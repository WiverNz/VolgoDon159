#pragma once

#ifdef USE_STUB_TESTS
#include "StubCryTest/PluginTestSystem.h"
#else
#include <CrySystem/Testing/CryTest.h>
#endif

#include <CrySystem/ICryPlugin.h>
#include <CryGame/IGameFramework.h>
#include <CryEntitySystem/IEntityClass.h>
#include <CryNetwork/INetwork.h>

class CUnitTestPlayerComponent;

// The entry-point of the application
// An instance of CUnitTestPlugin is automatically created when the library is loaded
// We then construct the local player entity and CUnitTestPlayerComponent instance when OnClientConnectionReceived is first called.
class CUnitTestPlugin 
	: public Cry::IEnginePlugin
	, public ISystemEventListener
	, public INetworkedClientListener
{
public:
	CRYINTERFACE_SIMPLE(Cry::IEnginePlugin)
	CRYGENERATE_SINGLETONCLASS_GUID(CUnitTestPlugin, "Blank", "f01244b0-a4e7-4dc6-91e1-0ed18906fe7c"_cry_guid)

	virtual ~CUnitTestPlugin();
	
	// Cry::IEnginePlugin
	virtual const char* GetCategory() const override { return "Game"; }
	virtual bool Initialize(SSystemGlobalEnvironment& env, const SSystemInitParams& initParams) override;
	// ~Cry::IEnginePlugin

	// ISystemEventListener
	virtual void OnSystemEvent(ESystemEvent event, UINT_PTR wparam, UINT_PTR lparam) override;
	// ~ISystemEventListener

	// INetworkedClientListener
	// Sent to the local client on disconnect
	virtual void OnLocalClientDisconnected(EDisconnectionCause cause, const char* description) override {}

	// Sent to the server when a new client has started connecting
	// Return false to disallow the connection
	virtual bool OnClientConnectionReceived(int channelId, bool bIsReset) override;
	// Sent to the server when a new client has finished connecting and is ready for gameplay
	// Return false to disallow the connection and kick the player
	virtual bool OnClientReadyForGameplay(int channelId, bool bIsReset) override;
	// Sent to the server when a client is disconnected
	virtual void OnClientDisconnected(int channelId, EDisconnectionCause cause, const char* description, bool bKeepClient) override;
	// Sent to the server when a client is timing out (no packets for X seconds)
	// Return true to allow disconnection, otherwise false to keep client.
	virtual bool OnClientTimingOut(int channelId, EDisconnectionCause cause, const char* description) override { return true; }
	// ~INetworkedClientListener

	// Helper function to call the specified callback for every player in the game
	void IterateOverPlayers(std::function<void(CUnitTestPlayerComponent& player)> func) const;

	// Helper function to get the CUnitTestPlugin instance
	// Note that CUnitTestPlugin is declared as a singleton, so the CreateClassInstance will always return the same pointer
	static CUnitTestPlugin* GetInstance()
	{
		return cryinterface_cast<CUnitTestPlugin>(CUnitTestPlugin::s_factory.CreateClassInstance().get());
	}
	
	//! Called after ISystem has been updated, this is the main update where most game logic is expected to occur
	//! This is the default update that should be preferred if you don't need any special behavior
	//! Called on a plug-in if it has called EnableUpdate(EUpdateStep::MainUpdate, true)
	void MainUpdate(float frameTime) override;

	// Test system initialization
	void TestSystemInitialization();

	// Run all tests for this plugin (marked as m_TestInfo.module = TESTS_MODULE_NAME)
	// e.g. CRY_TEST(MySimpleTest, m_TestInfo.module = TESTS_MODULE_NAME) { }
	// output: solutions/win64/Debug/user/testresults/crytest.xml
	void RunAllTests() const;

protected:
	// Map containing player components, key is the channel id received in OnClientConnectionReceived
	std::unordered_map<int, EntityId> m_players;

	// Test system pointer
#ifdef USE_STUB_TESTS
	std::shared_ptr<CryTest::ITestSystem> m_pUnitTestSystem = nullptr;
#else
	CryTest::ITestSystem* m_pUnitTestSystem = nullptr;
#endif
};