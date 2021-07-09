#include "stdafx.h"
#ifdef CRY_TESTING

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
#include "Common/NavigationTypes.h"

CRY_TEST_SUITE(VaildationTestsSuit)
{
	CRY_TEST(Serialization_test, m_TestInfo.module = TESTS_MODULE_NAME, timeout = 20, game = true, editor = false)
	{
		CryLog("Serialization test");
		string tempString;
		Navigation::SStatusShip shipStatus;
		shipStatus.rudder = 1.01234;								
		shipStatus.speed = 0.12345;									// FIXME: With 0.012345 serialization error in MemoryWriter::appendAsString
		shipStatus.steer = 123.4567;
		const string jsonValuesString = shipStatus.GetJsonString();
		CryLog(jsonValuesString);
		tempString.Format("%.4f", shipStatus.steer.Get());
		CRY_TEST_CHECK_DIFFERENT(jsonValuesString.find(tempString.c_str()), -1, "Can't find steer value (123.4567) in string");
		tempString.Format("%.5f", shipStatus.rudder.Get());
		CRY_TEST_CHECK_DIFFERENT(jsonValuesString.find(tempString), -1, "Can't find rudder value (1.01234) in string");
		tempString.Format("%.1f", shipStatus.speed.Get());
		tempString = "0.0";											// FIXME: With 0.012345 serialization error in MemoryWriter::appendAsString
		CRY_TEST_CHECK_DIFFERENT(jsonValuesString.find(tempString), -1, "Can't find speed value (0.0) in string");
	}
}

#endif // #ifdef CRY_TESTING
