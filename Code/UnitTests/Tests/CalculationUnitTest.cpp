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
#include <CryEntitySystem/IEntitySystem.h>
#include <ILevelSystem.h>  // CryEngine/CryAction
#include <CryMath/Cry_Camera.h>
#include <DefaultComponents/Cameras/CameraComponent.h>
#include <CryRenderer/IRenderAuxGeom.h>

#include "Calculations/MovementCalculator.h"
#include "GameModules/GameState.h"
#include "TestComponents/TestGridComponent.h"
#include "TestComponents/TestTraceComponent.h"

/*
HeightMap Resolution	1024x1024
MetersPerUnit			1
TerrainSize				1024x1024 Meters
TextureDimensions		4096x4096
TerrainColorMultiplier	8
*/

CRY_TEST_SUITE(CalculationTestsSuit)
{
	CRY_TEST_FIXTURE(CCalculationFixture)
	{
		void InitialCheck()
		{
			CryLog("InitialCheck Start");
			gEnv->pGameFramework->GetILevelSystem()->Rescan("../../../Assets/levels", ILevelSystem::TAG_MAIN);
			m_pLevelInfo = gEnv->pGameFramework->GetILevelSystem()->GetLevelInfo(GetTestMapName());
			if (!m_pLevelInfo)
			{
				GameWarning("Couldn't find test map '%s'", GetTestMapName());
				gEnv->pSystem->GetISystemEventDispatcher()->OnSystemEvent(ESYSTEM_EVENT_LEVEL_LOAD_ERROR, 0, 0);
			}
			CRY_TEST_ASSERT(m_pLevelInfo, "Couldn't find test map: " + GetTestMapName());
		}

		void SetTestCamera()
		{
			IEntity* pCamera = gEnv->pEntitySystem->FindEntityByName(GetTestCameraName());
			CRY_TEST_ASSERT(pCamera, "Couldn't find test camera '%s'", GetTestCameraName());

			if (pCamera)
			{
				m_pCamera = pCamera->GetComponent<Cry::DefaultComponents::CCameraComponent>();
				gEnv->pSystem->SetViewCamera(m_pCamera->GetCamera());
			}
		}

		void CreateTestGrid()
		{
			//IEntity* pRootObject = gEnv->pEntitySystem->FindEntityByName(GetRootObjectName());
			SEntitySpawnParams spawnParams;
			spawnParams.pClass = gEnv->pEntitySystem->GetClassRegistry()->GetDefaultClass();
			IEntity* pRootObject = gEnv->pEntitySystem->SpawnEntity(spawnParams);
			CRY_ASSERT(pRootObject != nullptr, "Can't find the Root Object!");
			m_pTestGrid = pRootObject->GetOrCreateComponent<TestComponents::CTestGridComponent>();
			CRY_ASSERT(m_pTestGrid != nullptr, "Can't create test grid");
			m_pTestGrid->GetEntity()->SetPos(Vec3(0, 0, 16));

			IEntity* pTestBall = gEnv->pEntitySystem->FindEntityByName("TestBall");
			if (pTestBall)
			{
				pTestBall->SetPos(Vec3(900, 600, 18));
				//pTestBall->AddImpulse(0, Vec3(200, 0, 20), );
			}
		}

		TestComponents::CTestTraceComponent* CreateTestTrace()
		{
			TestComponents::CTestTraceComponent* traceComponent = nullptr;
			SEntitySpawnParams spawnParams;
			spawnParams.pClass = gEnv->pEntitySystem->GetClassRegistry()->GetDefaultClass();
			IEntity* pRootObject = gEnv->pEntitySystem->SpawnEntity(spawnParams);
			CRY_ASSERT(pRootObject != nullptr, "Can't find the Root Object!");
			traceComponent = pRootObject->GetOrCreateComponent<TestComponents::CTestTraceComponent>();
			CRY_ASSERT(traceComponent != nullptr, "Can't create test trace");

			return traceComponent;
		}

		void CreateTestLine()
		{
			auto pTestTrace = CreateTestTrace();
			Vec3 currPos = m_startTracePos;
			pTestTrace->GetEntity()->SetPos(currPos);
			currPos += Vec3(-10, -10, 0);
			pTestTrace->AddAbsPoint(currPos, "point 1");
			currPos += Vec3(0, -10, 0);
			pTestTrace->AddAbsPoint(currPos, "point 2");
			currPos += Vec3(-110, 0, 0);
			pTestTrace->AddAbsPoint(currPos, "point 3");
			CrySleep(1000);
			
			gEnv->pEntitySystem->RemoveEntity(pTestTrace->GetEntityId());
		}

		void LinearTest()
		{
			auto pTestTrace = CreateTestTrace();
			Vec3 currPos = m_startTracePos;
			pTestTrace->GetEntity()->SetPos(currPos);
			Navigation::SStatusShip statusShip;
			Navigation::SShipParameters shipParameters;
			statusShip.speed = 10;
			statusShip.steer = 90;

			float currFrameTime = 1;
			Calculations::CMovementCalculator calculator(currFrameTime, &shipParameters, &statusShip);
			CryLog("StartPos %f %f %f", currPos.x, currPos.y, currPos.z);
			IEntity* pTestBall = gEnv->pEntitySystem->FindEntityByName("TestBall");
			if (pTestBall)
			{
				pTestBall->SetPos(currPos);
			}
			for (int i = 0; i < 10; i++)
			{
				calculator.Calculate();
				currPos.z = 0;
				currPos = calculator.GetNextPos(currPos);
				const string currPosText = string().Format("point %d (%f, %f, %f)", i, currPos.x, currPos.y, currPos.z);
				pTestTrace->AddRelPoint(currPos, currPosText);

				if (pTestBall)
				{
					pTestBall->AddImpulse(0, pTestBall->GetPos(), currPos, true, true);
				}
			}
		}

		void Start()
		{
			CryLog("CCalculationFixture Start");
			SetTestCamera();
			CreateTestGrid();
			// CreateTestLine();
		}

		virtual void Done() override
		{
			CryLog("CCalculationFixture Done");
			// CRY_TEST_ASSERT(gEnv->pGameFramework->IsGamePaused());
		}


		inline string GetRootObjectName() const
		{
			return m_rootObjectName;
		}

		inline string GetTestMapName() const
		{
			return m_levelName;
		}

		inline string GetTestCameraName() const
		{
			return m_cameraName;
		}
	protected:
		const string m_rootObjectName = "RootObject";
		ILevelInfo* m_pLevelInfo = nullptr;
		const string m_levelName = "unittest_level";
		Cry::DefaultComponents::CCameraComponent* m_pCamera = nullptr;
		const string m_cameraName = "TestCamera";
		TestComponents::CTestGridComponent* m_pTestGrid;
		TestComponents::CTestTraceComponent* m_pTestTrace;
		const Vec3 m_startTracePos = Vec3(900, 610, 18);
	};

	CRY_TEST_WITH_FIXTURE(Calculation_initial_test, CCalculationFixture, m_TestInfo.module = TESTS_MODULE_NAME, timeout = 200, game = true, editor = false)
	{
		commands =
		{
			CryTest::CCommandFunction(this, &CCalculationFixture::InitialCheck),
			CryTest::CCommandLoadLevel(GetTestMapName()),
			CryTest::CCommandFunction(this, &CCalculationFixture::Start),
			CryTest::CCommandFunction(this, &CCalculationFixture::LinearTest),
			CryTest::CCommandFunction(this, &CCalculationFixture::Done),
			CryTest::CCommandWait(60.f),
		};
	}
}

#endif // #ifdef CRY_TESTING
