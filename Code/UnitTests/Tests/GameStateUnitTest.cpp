#include "stdafx.h"
#ifdef CRY_TESTING

#include "TestCommon/common.h"
#ifdef USE_STUB_TESTS
#include "StubCryTest/PluginTestSystem.h"
#include "TestCommon/UnitTest.h"
#else
#include <CrySystem/Testing/CryTest.h>
#endif

#include <CrySystem/Testing/CryTestCommands.h>
#include <CryGame/IGameFramework.h>

#include "GameModules/GameState.h"

CRY_TEST_SUITE(GameStateTestsSuit)
{
	CRY_TEST_FIXTURE(CGameStateFixture)
	{
		void Start()
		{
			CryLog("CTestStateFixture Start");
			m_gameStateContext = std::make_shared<GameModules::CGameStateContext>();
			CRY_TEST_ASSERT(m_gameStateContext);
		}

		virtual void Done() override
		{
			CryLog("CGameStateFixture Done");
			m_gameStateContext.reset();
			if (gEnv->pGameFramework->IsGamePaused())
			{
				gEnv->pGameFramework->PauseGame(false, true);
				CrySleep(100);
			}
			// CRY_TEST_ASSERT(gEnv->pGameFramework->IsGamePaused());
		}

		void CheckFromPauseToRun()
		{
			m_gameStateContext->Pause();
			CRY_TEST_ASSERT(m_gameStateContext->GetEState() == GameModules::EGameState::GamePaused);
			m_gameStateContext->Resume();
			CRY_TEST_ASSERT(m_gameStateContext->GetEState() == GameModules::EGameState::GameRunning);
		}

		void CheckFromRunToPause()
		{
			m_gameStateContext->Resume();
			CRY_TEST_ASSERT(m_gameStateContext->GetEState() == GameModules::EGameState::GameRunning);
			m_gameStateContext->Pause();
			CRY_TEST_ASSERT(m_gameStateContext->GetEState() == GameModules::EGameState::GamePaused);
			m_gameStateContext->Resume();
			CRY_TEST_ASSERT(m_gameStateContext->GetEState() == GameModules::EGameState::GameRunning);
		}

		GameModules::IGameStateContextPtr m_gameStateContext = nullptr;
	};
	
	CRY_TEST_WITH_FIXTURE(GameState_switching_test, CGameStateFixture, m_TestInfo.module = TESTS_CONSOLE_MODULE_NAME, timeout = 20, game = true, editor = false)
	{
		commands =
		{
			// CryTest::CCommandLoadLevel("default_level"),
			CryTest::CCommandFunction(this, &CGameStateFixture::Start),
			CryTest::CCommandFunction(this, &CGameStateFixture::CheckFromPauseToRun),
			CryTest::CCommandWait(1.f),
			CryTest::CCommandFunction(this, &CGameStateFixture::CheckFromRunToPause),
			CryTest::CCommandWait(1.f),
			CryTest::CCommandFunction(this, &CGameStateFixture::Done),
			CryTest::CCommandWait(1.f),
		};
	}
}

#endif // #ifdef CRY_TESTING
