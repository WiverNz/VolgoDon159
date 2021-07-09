function(printenv)
	get_cmake_property(_variableNames VARIABLES)
	list (SORT _variableNames)
	foreach (_variableName ${_variableNames})
		 message(STATUS "${_variableName}=${${_variableName}}")
	endforeach()
endfunction()

function(include_google_test)
    find_package(GTest)
	if (GTEST_FOUND)
	    message("Using default Google test")
		include_directories(${GTEST_INCLUDE_DIR})
		target_link_libraries(${THIS_PROJECT} ${GTEST_LIBRARY_DEBUG} ${GTEST_MAIN_LIBRARY_DEBUG})
	elseif(EXISTS "${CRYENGINE_DIR}/Code/SDKs/googletest_CE_Support/googletest/include")
	    message("Using googletest_CE_Support from code/SDKs")
		target_include_directories(
			${THIS_PROJECT} PUBLIC 
			"${CRYENGINE_DIR}/Code/SDKs/googletest_CE_Support/googletest/include"
			"${CRYENGINE_DIR}/Code/SDKs/googletest_CE_Support/googlemock/include"
			"${CRYENGINE_DIR}/Code/CryEngine/UnitTests/Common"
		)
	elseif(EXISTS "${CMAKE_SOURCE_DIR}/../ThirdParty/googletest-release-1.10.0" OR EXISTS "${CMAKE_SOURCE_DIR}/../ThirdParty/googletest-release-1.10.0.zip")
	    message("Using googletest-release-1.10.0 from the source ThirdParty directory")
	    if(NOT EXISTS "${CMAKE_SOURCE_DIR}/../ThirdParty/googletest-release-1.10.0")
		    message("Unpacking unittest")
			execute_process(
			    COMMAND ${CMAKE_COMMAND} -E
			    tar xf "googletest-release-1.10.0.zip" 
				WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}/../ThirdParty/"
				RESULT_VARIABLE STATUS
			    OUTPUT_VARIABLE OUTPUT1 
			    ERROR_QUIET
			)
			if(STATUS AND NOT STATUS EQUAL 0)
			  message(FATAL_ERROR "Can't unzip unittest. Please unzip it manually in '${CMAKE_SOURCE_DIR}/../ThirdParty/googletest-release-1.10.0' and restart build.")
			else()
			  message(STATUS "SUCCESS: ${OUTPUT1}")
			endif()
		endif()
	    set(GTEST_ROOT "${CMAKE_SOURCE_DIR}/../ThirdParty/googletest-release-1.10.0")
		set(ENV{GTEST_ROOT} "${GTEST_ROOT}")
	    add_subdirectory("${GTEST_ROOT}" ${CMAKE_CURRENT_BINARY_DIR}/googletest EXCLUDE_FROM_ALL)
		enable_testing()
		include_directories("${gtest_SOURCE_DIR}/include" "${gmock_SOURCE_DIR}/include")
		target_include_directories(
			${THIS_PROJECT} PUBLIC 
			"${CRYENGINE_DIR}/Code/CryEngine/UnitTests/Common"
		)
	elseif(EXISTS "${CRYENGINE_DIR}/Code/SDKs/VulkanSDK/shaderc/third_party/googletest/googletest/include")
	    message("Using googletest from VulkanSDK/shaderc directory")
	    set(GTEST_ROOT "${CRYENGINE_DIR}/Code/SDKs/VulkanSDK/shaderc/third_party/googletest")
		set(ENV{GTEST_ROOT} "${GTEST_ROOT}")
		add_subdirectory("${GTEST_ROOT}" ${CMAKE_CURRENT_BINARY_DIR}/googletest EXCLUDE_FROM_ALL)
		enable_testing()
		target_include_directories(
			${THIS_PROJECT} PUBLIC 
			"${CRYENGINE_DIR}/Code/SDKs/VulkanSDK/shaderc/third_party/googletest/googletest/include"
			"${CRYENGINE_DIR}/Code/SDKs/VulkanSDK/shaderc/third_party/googletest/googlemock/include"
			"${CRYENGINE_DIR}/Code/CryEngine/UnitTests/Common"
		)
	else()
	    message(FATAL_ERROR "Can't find google test/mock folder")
	endif()
endfunction()

function(CryPluginUnitTestSuite target)
	set(TEST_MODULES ${TEST_MODULES} ${target} CACHE INTERNAL "List of test modules being built" FORCE)	

	add_sources("NoUberFile" SOURCE_GROUP "Imported" 
		"common/UnitTest.h"
		"common/Printer.cpp"
	)

	if (WINDOWS OR LINUX)
		add_sources("NoUberFile" SOURCE_GROUP "Imported" 
			"common/Main.cpp"
		)
	endif()

	prepare_project(${ARGN})
	add_executable(${THIS_PROJECT} ${${THIS_PROJECT}_SOURCES})
	if(WINDOWS)
		set_property(TARGET ${THIS_PROJECT} APPEND_STRING PROPERTY LINK_FLAGS " /SUBSYSTEM:CONSOLE")
	endif()
	apply_compile_settings()	

	if ((OPTION_STATIC_LINKING OR MODULE_FORCE_STATIC) AND NOT MODULE_FORCE_SHARED)
		target_compile_definitions(${THIS_PROJECT} PRIVATE _LIB -DCRY_IS_MONOLITHIC_BUILD)
	endif()

	if (LINUX)
		target_link_libraries( ${THIS_PROJECT} PRIVATE dl pthread)
	endif()

	include_google_test()

	target_link_libraries(${THIS_PROJECT} PUBLIC gtest gmock)
endfunction()

function(LocalEditorPluginUnitTestSuite CryEditorPluginUnitTestSuite target)
	add_sources("NoUberFile" SOURCE_GROUP "Imported" 
		"common/UnitTest.h"
		"common/Main.cpp"
		"common/Printer.cpp"
	)

	prepare_project(${ARGN})
	add_executable(${THIS_PROJECT} ${${THIS_PROJECT}_SOURCES})
	if(WIN32)
		set_property(TARGET ${THIS_PROJECT} APPEND_STRING PROPERTY LINK_FLAGS " /SUBSYSTEM:CONSOLE")
	endif()
	apply_compile_settings()

	include_google_test()
	target_link_libraries(${THIS_PROJECT} PUBLIC gtest gmock)

	set_editor_module_flags()
	target_compile_definitions(${THIS_PROJECT} PRIVATE _LAUNCHER -DEDITOR_PLUGIN_UNIT_TEST -DSANDBOX_IMPORTS -DEDITOR_COMMON_IMPORTS -DNOT_USE_CRY_MEMORY_MANAGER)
	target_link_libraries(${THIS_PROJECT} PRIVATE EditorCommon)
endfunction()

function(CryGameModule target)
	prepare_project(${ARGN})
	
	if (USE_STUB_TESTS)
		add_sources("NoUberFile" SOURCE_GROUP "Imported" 
			"common/UnitTest.h"
			"common/Printer.cpp"
			"${CRYENGINE_DIR}/Code/CryEngine/CryCommon/CryCore/Assert/CryAssert_impl.h"
		)
	endif()
	
	if (OPTION_STATIC_LINKING AND NOT OPTION_STATIC_LINKING_WITH_GAME_AS_DLL)
		add_library(${THIS_PROJECT} ${${THIS_PROJECT}_SOURCES})
	else()
		add_library(${THIS_PROJECT} SHARED ${${THIS_PROJECT}_SOURCES})
	endif()
	apply_compile_settings()
	set(game_folder ${CMAKE_CURRENT_SOURCE_DIR} CACHE INTERNAL "Game folder used for resource files on Windows" FORCE)

	if (OPTION_STATIC_LINKING AND NOT OPTION_STATIC_LINKING_WITH_GAME_AS_DLL)
		target_compile_definitions(${THIS_PROJECT} PRIVATE _LIB -DCRY_IS_MONOLITHIC_BUILD)
	elseif(ANDROID)
		set(SHARED_MODULES ${SHARED_MODULES} ${THIS_PROJECT} CACHE INTERNAL "Shared modules for APK creation" FORCE)
		target_link_libraries(${THIS_PROJECT} PRIVATE m log c android)
	endif()

	add_metadata()
	
	if (USE_STUB_TESTS)
		include_google_test()
		target_link_libraries(${THIS_PROJECT} PUBLIC gtest gmock)
	endif()

	if(EXISTS "${game_folder}/EngineDefineOverrides.h" AND OPTION_RUNTIME_CVAR_OVERRIDES)
		MESSAGE(WARNING "Disabling project engine define overrides because OPTION_RUNTIME_CVAR_OVERRIDES was enabled!")
	endif()
endfunction()
