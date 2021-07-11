cmake_minimum_required (VERSION 3.14)

start_sources()

sources_platform(ALL)
add_sources("UnitTests_uber.cpp"
    PROJECTS UnitTests
    SOURCE_GROUP "Root"
		"StdAfx.cpp"
		"UnitTestPlugin.cpp"
		"StdAfx.h"
		"UnitTestPlugin.h"
)
add_sources("StubCryTest_uber.cpp"
    PROJECTS UnitTests
    SOURCE_GROUP "StubCryTest"
		"StubCryTest/ExcelExport.cpp"
		"StubCryTest/Log.cpp"
		"StubCryTest/PluginTestSystem.cpp"
		"StubCryTest/UnitTestExcelReporter.cpp"
		"StubCryTest/ExcelExport.h"
		"StubCryTest/Log.h"
		"StubCryTest/PluginTestSystem.h"
		"StubCryTest/UnitTestExcelReporter.h"
)
add_sources("StubUberFiles_uber.cpp"
    PROJECTS UnitTests
    SOURCE_GROUP "StubUberFiles"
		"StubUberFiles/Common_uber.cpp"
		"StubUberFiles/GameModules_uber.cpp"
		"StubUberFiles/MovementCalculator_uber.cpp"
)
add_sources("TestCommon_uber.cpp"
    PROJECTS UnitTests
    SOURCE_GROUP "TestCommon"
		"TestCommon/Printer.cpp"
		"TestCommon/common.h"
		"TestCommon/UnitTest.h"
)
add_sources("TestComponents_uber.cpp"
    PROJECTS UnitTests
    SOURCE_GROUP "TestComponents"
		"TestComponents/TestGridComponent.cpp"
		"TestComponents/TestTraceComponent.cpp"
		"TestComponents/TestGridComponent.h"
		"TestComponents/TestTraceComponent.h"
)
add_sources("UnitTestPlayer_uber.cpp"
    PROJECTS UnitTests
    SOURCE_GROUP "TestComponents\\\\UnitTestPlayer"
		"TestComponents/UnitTestPlayer/UnitTestPlayer.cpp"
		"TestComponents/UnitTestPlayer/UnitTestPlayer.h"
)
add_sources("Tests_uber.cpp"
    PROJECTS UnitTests
    SOURCE_GROUP "Tests"
		"Tests/CalculationUnitTest.cpp"
		"Tests/GameStateUnitTest.cpp"
		"Tests/ValidationUnitTest.cpp"
)

if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/CVarOverrides.h")
    add_sources("NoUberFile"
        PROJECTS Game
        SOURCE_GROUP "Root"
            "CVarOverrides.h"
    )
endif()

if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/EngineDefineOverrides.h")
    add_sources("NoUberFile"
        PROJECTS Game
        SOURCE_GROUP "Root"
            "EngineDefineOverrides.h"
    )
endif()
end_sources()

CryGameModule(UnitTests FORCE_SHARED PCH "StdAfx.cpp" SOLUTION_FOLDER "Project")

target_include_directories(${THIS_PROJECT}
PRIVATE
    "${CRYENGINE_DIR}/Code/CryEngine/CryCommon"
    "${CRYENGINE_DIR}/Code/CryEngine/CryAction"
    "${CRYENGINE_DIR}/Code/CryEngine/CrySchematyc/Core/Interface"
    "${CRYENGINE_DIR}/Code/CryPlugins/CryDefaultEntities/Module"
)

if(CMAKE_CXX_COMPILER_ID MATCHES "[Cc]lang")
    target_compile_options(${THIS_PROJECT} PRIVATE
        -Wno-unused-variable
        -Wno-reorder
        -Wno-unknown-pragmas
        -Wno-parentheses
        -Wno-switch
        -Wno-format
        -Wno-dynamic-class-memaccess
        -Wno-unused-private-field
        -Wno-unused-value
        -Wno-invalid-offsetof
        -Wno-multichar
        -Wno-char-subscripts
        -Wno-null-conversion
        -Wno-empty-body
        -Wno-unused-lambda-capture
        -Wno-unused-function
        -Wno-tautological-constant-out-of-range-compare
    )
elseif(CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
    target_compile_options(${THIS_PROJECT} PRIVATE
        -Wno-unused-function
        -Wno-unused-value
        -Wno-unused-variable
        -Wno-aligned-new
        -Wno-int-in-bool-context
        -Wno-invalid-offsetof
        -Wno-comment
        -Wno-sign-compare
        -Wno-unused-but-set-variable
        -Wno-maybe-uninitialized
        -Wno-misleading-indentation
        -Wno-unused-result
        -Wno-unknown-pragmas
        -Wno-multichar
        -Wno-strict-aliasing
        -Wno-char-subscripts
        -Wno-conversion-null
        -Wno-reorder
        -Wno-parentheses
        -Wno-format
        -Wno-switch
    )
endif()

#BEGIN-CUSTOM
# Make any custom changes here, modifications outside of the block will be discarded on regeneration.
if (WIN32)
	set_property(TARGET UnitTests PROPERTY WIN32_EXECUTABLE true)
    set_visual_studio_debugger_command(UnitTests "${CRYENGINE_DIR}/bin/win_x64/GameLauncher.exe" "-project \"${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/UnitTests.cryproject\"" )
endif()

if (USE_STUB_TESTS)
    target_compile_definitions(UnitTests PRIVATE -DUSE_STUB_TESTS)
endif()

add_custom_command(TARGET UnitTests POST_BUILD
                   COMMAND ${CMAKE_COMMAND} -E copy
                   ${CMAKE_CURRENT_SOURCE_DIR}/UnitTests.cryproject ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/UnitTests.cryproject)
add_custom_command(TARGET UnitTests POST_BUILD
                   COMMAND ${CMAKE_COMMAND} -E copy_directory
                   ${PROJECT_DIR}/assets ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/assets)

#END-CUSTOM