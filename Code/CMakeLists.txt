cmake_minimum_required (VERSION 3.14)
set(CRYENGINE_DIR "G:/Projects/Cryengine" CACHE STRING "CRYENGINE root directory.")
set(TOOLS_CMAKE_DIR "${CRYENGINE_DIR}/Tools/CMake")

set(PROJECT_BUILD 1)
set(PROJECT_DIR "${CMAKE_SOURCE_DIR}/../")

include("${TOOLS_CMAKE_DIR}/InitialSetup.cmake")
include("${TOOLS_CMAKE_DIR}/CommonOptions.cmake")

add_subdirectory("${CRYENGINE_DIR}" "${CMAKE_CURRENT_BINARY_DIR}/CRYENGINE")

include("${TOOLS_CMAKE_DIR}/Configure.cmake")
start_sources()

sources_platform(ALL)
add_sources("Code_uber.cpp"
    PROJECTS VolgoDon159
    SOURCE_GROUP "Root"
		"GamePlugin.cpp"
		"StdAfx.cpp"
		"GamePlugin.h"
		"StdAfx.h"
)
add_sources("Calculations_uber.cpp"
    PROJECTS VolgoDon159
    SOURCE_GROUP "Calculations"
		"Calculations/MovementCalculator.cpp"
		"Calculations/MovementCalculator.h"
)
add_sources("Common_uber.cpp"
    PROJECTS VolgoDon159
    SOURCE_GROUP "Common"
		"Common/ConfigLoader.cpp"
		"Common/NavigationTypes.cpp"
		"Common/ConfigLoader.h"
		"Common/NavigationTypes.h"
)
add_sources("StubFeatures_uber.cpp"
    PROJECTS VolgoDon159
    SOURCE_GROUP "Common\\\\StubFeatures"
		"Common/StubFeatures/ParamTraits.h"
		"Common/StubFeatures/ParamTraitsImpl.h"
)
add_sources("Components_uber.cpp"
    PROJECTS VolgoDon159
    SOURCE_GROUP "Components"
)
add_sources("AutoPilot_uber.cpp"
    PROJECTS VolgoDon159
    SOURCE_GROUP "Components\\\\AutoPilot"
		"Components/AutoPilot/AutoPilotShip.cpp"
		"Components/AutoPilot/AutoPilotShipSpawnPoint.cpp"
		"Components/AutoPilot/AutoPilotShip.h"
		"Components/AutoPilot/AutoPilotShipSpawnPoint.h"
)
add_sources("Player_uber.cpp"
    PROJECTS VolgoDon159
    SOURCE_GROUP "Components\\\\Player"
		"Components/Player/Player.cpp"
		"Components/Player/SpawnPoint.cpp"
		"Components/Player/Player.h"
		"Components/Player/SpawnPoint.h"
)
add_sources("ShipMechanics_uber.cpp"
    PROJECTS VolgoDon159
    SOURCE_GROUP "Components\\\\ShipMechanics"
		"Components/ShipMechanics/ShipMechanicsComponent.cpp"
		"Components/ShipMechanics/ShipMechanicsComponent.h"
)
add_sources("GameModules_uber.cpp"
    PROJECTS VolgoDon159
    SOURCE_GROUP "GameModules"
		"GameModules/GameManager.cpp"
		"GameModules/GameSettings.cpp"
		"GameModules/GameSound.cpp"
		"GameModules/GameState.cpp"
		"GameModules/GameManager.h"
		"GameModules/GameSettings.h"
		"GameModules/GameSound.h"
		"GameModules/GameState.h"
)
add_sources("ThirdParty_uber.cpp"
    PROJECTS VolgoDon159
    SOURCE_GROUP "ThirdParty"
)
add_sources("fmt_uber.cpp"
    PROJECTS VolgoDon159
    SOURCE_GROUP "ThirdParty\\\\fmt"
		"ThirdParty/fmt/format.cpp"
		"ThirdParty/fmt/chrono.h"
		"ThirdParty/fmt/color.h"
		"ThirdParty/fmt/compile.h"
		"ThirdParty/fmt/core.h"
		"ThirdParty/fmt/format-inl.h"
		"ThirdParty/fmt/format.h"
		"ThirdParty/fmt/locale.h"
		"ThirdParty/fmt/os.h"
		"ThirdParty/fmt/ostream.h"
		"ThirdParty/fmt/posix.h"
		"ThirdParty/fmt/printf.h"
		"ThirdParty/fmt/ranges.h"
)
add_sources("UserInterface_uber.cpp"
    PROJECTS VolgoDon159
    SOURCE_GROUP "UserInterface"
		"UserInterface/MainMenu.cpp"
		"UserInterface/ShipInterface.cpp"
		"UserInterface/MainMenu.h"
		"UserInterface/ShipInterface.h"
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

CryGameModule(VolgoDon159 FORCE_SHARED PCH "StdAfx.cpp" SOLUTION_FOLDER "Project")

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


if(OPTION_ENGINE)
    if(NOT EXISTS "${CRYENGINE_DIR}/Code/Sandbox/EditorQt")
        add_library(Editor STATIC "${CRYENGINE_DIR}/Code/CryEngine/CryCommon/CryCore/Platform/platform.h")
        set_target_properties(Editor PROPERTIES LINKER_LANGUAGE CXX)
        if (WIN32)
            set_visual_studio_debugger_command(Editor "${CRYENGINE_DIR}/bin/win_x64/Sandbox.exe" "-project \"${PROJECT_DIR}/VolgoDon159.cryproject\"")
        endif()
    endif()
else()
    add_library(GameLauncher STATIC "${CRYENGINE_DIR}/Code/CryEngine/CryCommon/CryCore/Platform/platform.h")
    set_target_properties(GameLauncher PROPERTIES LINKER_LANGUAGE CXX)
    if (WIN32)
        set_visual_studio_debugger_command(GameLauncher "${CRYENGINE_DIR}/bin/win_x64/GameLauncher.exe" "-project \"${PROJECT_DIR}/VolgoDon159.cryproject\"")
    endif()

    add_library(Editor STATIC "${CRYENGINE_DIR}/Code/CryEngine/CryCommon/CryCore/Platform/platform.h")
    set_target_properties(Editor PROPERTIES LINKER_LANGUAGE CXX)
    if (WIN32)
        set_visual_studio_debugger_command(Editor "${CRYENGINE_DIR}/bin/win_x64/Sandbox.exe" "-project \"${PROJECT_DIR}/VolgoDon159.cryproject\"")
    endif()

    add_library(GameServer STATIC "${CRYENGINE_DIR}/Code/CryEngine/CryCommon/CryCore/Platform/platform.h")
    set_target_properties(GameServer PROPERTIES LINKER_LANGUAGE CXX)
    if (WIN32)
        set_visual_studio_debugger_command(GameServer "${CRYENGINE_DIR}/bin/win_x64/Game_Server.exe" "-project \"${PROJECT_DIR}/VolgoDon159.cryproject\"")
    endif()
endif()

# Set StartUp project in Visual Studio
set_solution_startup_target(GameLauncher)

if (WIN32)
    set_visual_studio_debugger_command( ${THIS_PROJECT} "${CRYENGINE_DIR}/bin/win_x64/GameLauncher.exe" "-project \"${PROJECT_DIR}/VolgoDon159.cryproject\"" )
endif()

#BEGIN-CUSTOM
# Make any custom changes here, modifications outside of the block will be discarded on regeneration.
option(BUILD_UNIT_TESTS "Build Unit tests" ON)
option(USE_STUB_TESTS "Use stub tests" ON)
if(BUILD_UNIT_TESTS)
    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/${CMAKE_INSTALL_BINDIR}/${CMAKE_BUILD_TYPE_INIT}")
    include("../ThirdParty/UnitTestsMacros.cmake")
    # printenv()
    add_subdirectory("UnitTests")
else()
    message("Building without unit tests")
endif(BUILD_UNIT_TESTS)
#END-CUSTOM