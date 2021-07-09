#include "StdAfx.h"
#include "GamePlugin.h"

#include <IGameObjectSystem.h>
#include <IGameObject.h>

#include <CrySchematyc/Env/IEnvRegistry.h>
#include <CrySchematyc/Env/EnvPackage.h>
#include <CrySchematyc/Utils/SharedString.h>
#include <combaseapi.h> // CoInitializeEx

// Included only once per DLL module.
#include <CryCore/Platform/platform_impl.inl>

#include "GameModules/GameState.h"
#include "GameModules/GameManager.h"
#include "UserInterface/MainMenu.h"
#include "UserInterface/ShipInterface.h"
#include "Components/Player/Player.h"

CGamePlugin::~CGamePlugin()
{
	// Remove any registered listeners before 'this' becomes invalid
	if (gEnv->pGameFramework != nullptr)
	{
		gEnv->pGameFramework->RemoveNetworkedClientListener(*this);
	}

	gEnv->pSystem->GetISystemEventDispatcher()->RemoveListener(this);

	if (gEnv->pSchematyc)
	{
		gEnv->pSchematyc->GetEnvRegistry().DeregisterPackage(CGamePlugin::GetCID());
	}

	::CoUninitialize();
}

bool CGamePlugin::Initialize(SSystemGlobalEnvironment& env, const SSystemInitParams& initParams)
{
	// Fix onecore\com\combase\objact\objact.cxx(812)\combase.dll!00007FFE23C22169: (caller: 00007FFE23C20F78) 
	// ReturnHr(12) tid(454c) 800401F0 CoInitialize has not been called.
	// https://github.com/CRYTEK/CRYENGINE/issues/335
	::CoInitializeEx(nullptr, COINIT_MULTITHREADED);

	// FIXME: Assert from gEnv->pSystem->Quit() in CD3D9Renderer::RT_EndFrame(). Disabled via:
	Cry::Assert::DisableAssertionsForModule(eCryM_Render);
	// Register for engine system events, in our case we need ESYSTEM_EVENT_GAME_POST_INIT to load the map
	gEnv->pSystem->GetISystemEventDispatcher()->RegisterListener(this, "CGamePlugin");

	GameModules::GetIGameManagerPtr()->Initialize(env, initParams);

	EnableUpdate(EUpdateStep::BeforeSystem, true);
	EnableUpdate(EUpdateStep::MainUpdate, true);

	return true;
}

void CGamePlugin::OnSystemEvent(ESystemEvent event, UINT_PTR wparam, UINT_PTR lparam)
{
	switch (event)
	{
		// Called when the game framework has initialized and we are ready for game logic to start
	case ESYSTEM_EVENT_GAME_POST_INIT:
		{
			OnGamePostInit();
		}
		break;
	case ESYSTEM_EVENT_REGISTER_SCHEMATYC_ENV:
		{
			OnRegisterSchematyc();
		}
		break;
	case ESYSTEM_EVENT_GAME_PAUSED:
		{
			if (GameModules::GetGameStateContextPtr()->GetEState() == GameModules::EGameState::GamePaused)
			{
				OnGamePaused();
			}
		}
		break;
	case ESYSTEM_EVENT_GAME_RESUMED:
		{
			if (GameModules::GetGameStateContextPtr()->GetEState() == GameModules::EGameState::GameRunning)
			{
				OnGameResumed();
			}
		}
		break;
	case ESYSTEM_EVENT_LEVEL_LOAD_END:
		{

		}
		break;
	case ESYSTEM_EVENT_LEVEL_UNLOAD:
		{
			m_players.clear();
		}
		break;
	}
}

bool CGamePlugin::OnClientConnectionReceived(int channelId, bool bIsReset)
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
		Navigation::CPlayerComponent* pPlayer = pPlayerEntity->GetOrCreateComponentClass<Navigation::CPlayerComponent>();

		if (pPlayer != nullptr)
		{
			// Push the component into our map, with the channel id as the key
			m_players.emplace(std::make_pair(channelId, pPlayerEntity->GetId()));
		}
	}

	return true;
}

bool CGamePlugin::OnClientReadyForGameplay(int channelId, bool bIsReset)
{
	// Revive players when the network reports that the client is connected and ready for gameplay
	auto it = m_players.find(channelId);
	if (it != m_players.end())
	{
		if (IEntity* pPlayerEntity = gEnv->pEntitySystem->GetEntity(it->second))
		{
			if (Navigation::CPlayerComponent* pPlayer = pPlayerEntity->GetComponent<Navigation::CPlayerComponent>())
			{
				pPlayer->OnReadyForGameplayOnServer();
			}
		}
	}

	return true;
}

void CGamePlugin::OnClientDisconnected(int channelId, EDisconnectionCause cause, const char* description, bool bKeepClient)
{
	// Client disconnected, remove the entity and from map
	auto it = m_players.find(channelId);
	if (it != m_players.end())
	{
		gEnv->pEntitySystem->RemoveEntity(it->second);

		m_players.erase(it);
	}
}

void CGamePlugin::IterateOverPlayers(std::function<void(Navigation::CPlayerComponent& player)> func) const
{
	for (const std::pair<int, EntityId>& playerPair : m_players)
	{
		if (IEntity* pPlayerEntity = gEnv->pEntitySystem->GetEntity(playerPair.second))
		{
			if (Navigation::CPlayerComponent* pPlayer = pPlayerEntity->GetComponent<Navigation::CPlayerComponent>())
			{
				func(*pPlayer);
			}
		}
	}
}

void CGamePlugin::OnGamePostInit()
{
	// Listen for client connection events, in order to create the local player
	gEnv->pGameFramework->AddNetworkedClientListener(*this);

	// Don't need to load the map in editor
	if (!gEnv->IsEditor())
	{
		// Load the example map in client server mode
		gEnv->pConsole->ExecuteString("map default_level", false, true);
	}
	GameModules::GetIGameManagerPtr()->GetGameSettings().LoadSettings();
}

void CGamePlugin::OnRegisterSchematyc()
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
				CGamePlugin::GetCID(),
				"EntityComponents",
				"Crytek GmbH",
				"Components",
				staticAutoRegisterLambda
				)
		);
	}

	GameModules::GetIGameManagerPtr()->SetSkipFlash(gEnv->IsEditor());

	m_pMainMenu = UserInterface::GetMainManuPtr();
	m_pMainMenu->Initialize();

	m_pShipInterface = UserInterface::GetShipInterfacePtr();
	m_pShipInterface->Initialize();

	GameModules::GetGameStateContextPtr()->Pause();
}

void CGamePlugin::UpdateBeforeSystem()
{

}

void CGamePlugin::MainUpdate(float frameTime)
{
	for (const std::pair<int, EntityId>& playerPair : m_players)
	{
		if (IEntity* pPlayerEntity = gEnv->pEntitySystem->GetEntity(playerPair.second))
		{
			if (Navigation::CPlayerComponent* pPlayer = pPlayerEntity->GetComponent<Navigation::CPlayerComponent>())
			{

			}
		}
	}

	if (m_pMainMenu && m_pMainMenu->IsVisible())
	{
		m_pMainMenu->MainUpdate(frameTime);
	}

	if (m_pShipInterface && m_pShipInterface->IsVisible())
	{
		m_pShipInterface->MainUpdate(frameTime);
	}
}

void CGamePlugin::OnGamePaused()
{
	CryLogAlways("OnGamePause");

	if (m_pShipInterface)
	{
		m_pShipInterface->SetVisible(false);
	}

	if (m_pMainMenu)
	{
		m_pMainMenu->SetVisible(true);
	}
}

void CGamePlugin::OnGameResumed()
{
	CryLogAlways("OnGameResume");
	CGamePlugin::GetInstance()->IterateOverPlayers([this](Navigation::CPlayerComponent& player)
	{
		player.ResetMouseMovement();
	});
	if (m_pMainMenu)
	{
		m_pMainMenu->SetVisible(false);
		m_pMainMenu->SetResumeGameState(true);
	}

	if (m_pShipInterface)
	{
		m_pShipInterface->SetVisible(true);
	}
}

CRYREGISTER_SINGLETON_CLASS(CGamePlugin)