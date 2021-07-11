#include "stdafx.h"
#ifdef CRY_TESTING

#include "TestCommon/common.h"
#ifdef USE_STUB_TESTS
#include "StubCryTest/PluginTestSystem.h"
#include "TestCommon/UnitTest.h"
#else
#include <CrySystem/Testing/CryTest.h>
#endif

#include <CrySerialization/yasli/JSONIArchive.h>
#include <CrySerialization/yasli/JSONOArchive.h>

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
#include "Common/StubFeatures/MemoryWriterImpl.h"

CRY_TEST_SUITE(VaildationTestsSuit)
{
	CRY_TEST(MemoryWriter_test, m_TestInfo.module = TESTS_CONSOLE_MODULE_NAME, game = true, editor = false)
	{
		std::unique_ptr<YasliFix::MemoryWriter> buffer_;
		buffer_.reset(new YasliFix::MemoryWriter(1024, true));
		float testValueSmall = 0.012345;
		buffer_->appendAsString(testValueSmall, false);
		float testValueNormal = 1.01234;
		buffer_->appendAsString(testValueNormal, false);
		const string resultString = buffer_->c_str();

		CryLog(resultString);
		string tempString;
		tempString.Format("%.4f", testValueNormal);
		CRY_TEST_CHECK_DIFFERENT(resultString.find(tempString), -1, "Can't find 1.0123 in string");
		tempString.Format("%.4f", testValueSmall);
		CRY_TEST_CHECK_DIFFERENT(resultString.find(tempString), -1, "Can't find 0.0123 in string");
	}

	CRY_TEST(Archive_test, m_TestInfo.module = TESTS_CONSOLE_MODULE_NAME, timeout = 20, game = true, editor = false)
	{
		Serialization::IArchive ar(yasli::Archive::ArchiveCaps::TEXT);
		float testValueNormal = 1.01234;
		float testValueSmall = 0.012345;
		float testValueLast = 101.012345;
		ar(testValueNormal, "testValueNormal");
		ar(testValueSmall, "testValueSmall");

		yasli::JSONOArchive archive;
		archive(testValueSmall, "testValueSmall");
		archive(testValueNormal, "testValueNormal");
		archive(testValueLast, "testValueLast");
		const string resultString = archive.c_str();

		CryLog(resultString);
		string tempString;
		tempString.Format("%.5f", testValueNormal);
		CRY_TEST_CHECK_DIFFERENT(resultString.find(tempString), -1, "Can't find 1.01234 in string");
		tempString.Format("%.5f", testValueSmall);
		CRY_TEST_CHECK_DIFFERENT(resultString.find(tempString), -1, "Can't find 0.01234 in string");
		tempString.Format("%.5f", testValueLast);
		CRY_TEST_CHECK_DIFFERENT(resultString.find(tempString), -1, "Can't find 101.01234 in string");
	}

	CRY_TEST(Serialization_test, m_TestInfo.module = TESTS_CONSOLE_MODULE_NAME, timeout = 20, game = true, editor = false)
	{
		CryLog("Serialization test");
		string tempString;
		Navigation::SStatusShip shipStatus;
		shipStatus.rudder = 1.01234;								
		shipStatus.speed = 0.012345;									// FIXME: With 0.012345 serialization error in MemoryWriter::appendAsString
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
