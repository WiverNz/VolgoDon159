#include "StdAfx.h"

#include "UnitTestPlugin.h"

#include <IGameObjectSystem.h>
#include <IGameObject.h>

#include <CrySchematyc/Env/IEnvRegistry.h>
#include <CrySchematyc/Env/EnvPackage.h>
#include <CrySchematyc/Utils/SharedString.h>

// Included only once per DLL module.
#include <CryCore/Platform/platform_impl.inl>

#include <combaseapi.h> // CoInitializeEx

#include "TestComponents/UnitTestPlayer/UnitTestPlayer.h"

#ifdef USE_STUB_TESTS
#include "StubCryTest/PluginTestSystem.h"
#include "TestCommon/UnitTest.h"
#else
#include <CrySystem/Testing/CryTest.h>
#include <CrySystem/Testing/CryTestCommands.h>
#endif

CUnitTestPlugin::~CUnitTestPlugin()
{
	// Remove any registered listeners before 'this' becomes invalid
	if (gEnv->pGameFramework != nullptr)
	{
		gEnv->pGameFramework->RemoveNetworkedClientListener(*this);
	}

	gEnv->pSystem->GetISystemEventDispatcher()->RemoveListener(this);

	if (gEnv->pSchematyc)
	{
		gEnv->pSchematyc->GetEnvRegistry().DeregisterPackage(CUnitTestPlugin::GetCID());
	}

	::CoUninitialize();
}

bool CUnitTestPlugin::Initialize(SSystemGlobalEnvironment& env, const SSystemInitParams& initParams)
{
	::CoInitializeEx(nullptr, COINIT_MULTITHREADED);
	Cry::Assert::DisableAssertionsForModule(eCryM_Render);
	// Register for engine system events, in our case we need ESYSTEM_EVENT_GAME_POST_INIT to load the map
	gEnv->pSystem->GetISystemEventDispatcher()->RegisterListener(this, "CUnitTestPlugin");
	
	EnableUpdate(EUpdateStep::MainUpdate, true);

	return true;
}

void CUnitTestPlugin::OnSystemEvent(ESystemEvent event, UINT_PTR wparam, UINT_PTR lparam)
{
	switch (event)
	{
		// Called when the game framework has initialized and we are ready for game logic to start
		case ESYSTEM_EVENT_GAME_POST_INIT:
		{
			// Listen for client connection events, in order to create the local player
			gEnv->pGameFramework->AddNetworkedClientListener(*this);

			// Don't need to load the map in editor
			if (!gEnv->IsEditor())
			{
				// Load the example map in client server mode
				gEnv->pConsole->ExecuteString("map example s", false, true);
			}
			TestSystemInitialization();
			RunAllTests();
		}
		break;

		case ESYSTEM_EVENT_REGISTER_SCHEMATYC_ENV:
		{
			// Register all components that belong to this plug-in
			auto staticAutoRegisterLambda = [](Schematyc::IEnvRegistrar& registrar)
			{
				// Call all static callback registered with the CRY_STATIC_AUTO_REGISTER_WITH_PARAM
				Detail::CStaticAutoRegistrar<Schematyc::IEnvRegistrar&>::InvokeStaticCallbacks(registrar);
			};

			if (gEnv->pSchematyc)
			{
				gEnv->pSchematyc->GetEnvRegistry().RegisterPackage(
					stl::make_unique<Schematyc::CEnvPackage>(
						CUnitTestPlugin::GetCID(),
						"EntityComponents",
						"Crytek GmbH",
						"Components",
						staticAutoRegisterLambda
						)
				);
			}
		}
		break;
		
		case ESYSTEM_EVENT_LEVEL_UNLOAD:
		{
			m_players.clear();
		}
		break;
	}
}

bool CUnitTestPlugin::OnClientConnectionReceived(int channelId, bool bIsReset)
{
	// Connection received from a client, create a player entity and component
	SEntitySpawnParams spawnParams;
	spawnParams.pClass = gEnv->pEntitySystem->GetClassRegistry()->GetDefaultClass();
	
	// Set a unique name for the player entity
	const string playerName = string().Format("Player%" PRISIZE_T, m_players.size());
	spawnParams.sName = playerName;
	
	// Set local player details
	if (m_players.empty() && !gEnv->IsDedicated())
	{
		spawnParams.id = LOCAL_PLAYER_ENTITY_ID;
		spawnParams.nFlags |= ENTITY_FLAG_LOCAL_PLAYER;
	}

	// Spawn the player entity
	if (IEntity* pPlayerEntity = gEnv->pEntitySystem->SpawnEntity(spawnParams))
	{
		// Set the local player entity channel id, and bind it to the network so that it can support Multiplayer contexts
		pPlayerEntity->GetNetEntity()->SetChannelId(channelId);

		// Create the player component instance
		CUnitTestPlayerComponent* pPlayer = pPlayerEntity->GetOrCreateComponentClass<CUnitTestPlayerComponent>();

		if (pPlayer != nullptr)
		{
			// Push the component into our map, with the channel id as the key
			m_players.emplace(std::make_pair(channelId, pPlayerEntity->GetId()));
		}
	}

	return true;
}

bool CUnitTestPlugin::OnClientReadyForGameplay(int channelId, bool bIsReset)
{
	// Revive players when the network reports that the client is connected and ready for gameplay
	auto it = m_players.find(channelId);
	if (it != m_players.end())
	{
		if (IEntity* pPlayerEntity = gEnv->pEntitySystem->GetEntity(it->second))
		{
			if (CUnitTestPlayerComponent* pPlayer = pPlayerEntity->GetComponent<CUnitTestPlayerComponent>())
			{
				pPlayer->OnReadyForGameplayOnServer();
			}
		}
	}

	return true;
}

void CUnitTestPlugin::OnClientDisconnected(int channelId, EDisconnectionCause cause, const char* description, bool bKeepClient)
{
	// Client disconnected, remove the entity and from map
	auto it = m_players.find(channelId);
	if (it != m_players.end())
	{
		gEnv->pEntitySystem->RemoveEntity(it->second);

		m_players.erase(it);
	}
}

void CUnitTestPlugin::IterateOverPlayers(std::function<void(CUnitTestPlayerComponent& player)> func) const
{
	for (const std::pair<int, EntityId>& playerPair : m_players)
	{
		if (IEntity* pPlayerEntity = gEnv->pEntitySystem->GetEntity(playerPair.second))
		{
			if (CUnitTestPlayerComponent* pPlayer = pPlayerEntity->GetComponent<CUnitTestPlayerComponent>())
			{
				func(*pPlayer);
			}
		}
	}
}

void CUnitTestPlugin::MainUpdate(float frameTime)
{
	if (m_pUnitTestSystem)
	{
		m_pUnitTestSystem->Update();
	}
}

void CUnitTestPlugin::TestSystemInitialization()
{
#ifdef USE_STUB_TESTS
	// testing::InitGoogleTest();
	m_pUnitTestSystem = std::make_shared<CryTest::CTestSystem>(*gEnv->pSystem, *gEnv->pSystem->GetIThreadManager(), *gEnv->pSystem->GetICmdLine(), *gEnv->pSystem->GetIConsole());
#else
	m_pUnitTestSystem = gEnv->pSystem->GetITestSystem();
#endif
}

void CUnitTestPlugin::RunAllTests() const
{
	// RUN_ALL_TESTS();
	if (!m_pUnitTestSystem)
	{
		CRY_ASSERT(false, "The test system was not initialized");
		return;
	}
	DynArray<string> testNamesForRun;
	std::map<string, std::set<CryTest::CTestFactory*>> testsByModuleName;
	DynArray<CryTest::CTestFactory*> testFactories = gEnv->pSystem->GetITestSystem()->GetFactories();
	for (CryTest::CTestFactory* pFactory : testFactories)
	{
		const string testModule = pFactory->GetTestInfo().module;
		if (testModule == TESTS_MODULE_NAME)
		{
			const string testName = pFactory->GetTestInfo().name;
			testsByModuleName[testModule].insert(pFactory);
			testNamesForRun.push_back(testName);
		}
	}
	
	// Don't check for previous errors:
	gEnv->pCryPak->RemoveFile("%USER%/TestResults/TestRecord.txt");

	CryLogAlways("Running all tests...");
	CRY_ASSERT(m_pUnitTestSystem != nullptr);
	m_pUnitTestSystem->SetQuitAfterTests(true);
	m_pUnitTestSystem->SetOpenReport(gEnv->pSystem->GetICmdLine()->FindArg(eCLAT_Pre, "crytest_open_report") != nullptr);
	m_pUnitTestSystem->RunTestsByName(testNamesForRun);
}

CRYREGISTER_SINGLETON_CLASS(CUnitTestPlugin)