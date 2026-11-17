cmake_minimum_required(VERSION 3.22)

project(WordSprintBackend LANGUAGES CXX)

find_package(Qt6 REQUIRED COMPONENTS
    Core
    Gui
    Qml
    Quick
)

# Optional Qt components (WebEngine, WebChannel, etc.)
find_package(Qt6 QUIET COMPONENTS EntryPoint)
find_package(Qt6 QUIET COMPONENTS WebEngineCore)
find_package(Qt6 QUIET COMPONENTS WebEngineWidgets)
find_package(Qt6 QUIET COMPONENTS WebChannel)

# Set flags based on found components
if (Qt6WebEngineQuick_FOUND)
    set(HAVE_WEBENGINEQUICK ON)
else()
    set(HAVE_WEBENGINEQUICK OFF)
endif()

if (Qt6WebEngineCore_FOUND)
    set(HAVE_WEBENGINECORE ON)
else()
    set(HAVE_WEBENGINECORE OFF)
endif()

if (Qt6WebEngineWidgets_FOUND)
    set(HAVE_WEBENGINEWIDGETS ON)
else()
    set(HAVE_WEBENGINEWIDGETS OFF)
endif()

if (Qt6WebChannel_FOUND)
    set(HAVE_WEBCHANNEL ON)
else()
    set(HAVE_WEBCHANNEL OFF)
endif()



qt_standard_project_setup()
qt_policy(SET QTP0001 NEW)
qt_policy(SET QTP0004 NEW)

find_program(PNPM_EXECUTABLE NAMES pnpm pnpm.cmd npm npm.cmd)
set(FRONTEND_DIR "${CMAKE_CURRENT_SOURCE_DIR}/../frontend")
set(FRONTEND_DIST_DIR "${FRONTEND_DIR}/dist")

if(PNPM_EXECUTABLE)
    if(NOT EXISTS "${FRONTEND_DIST_DIR}/index.html")
        message(STATUS "Frontend dist not found. Building with ${PNPM_EXECUTABLE}...")
        execute_process(
            COMMAND ${PNPM_EXECUTABLE} run build
            WORKING_DIRECTORY "${FRONTEND_DIR}"
            RESULT_VARIABLE FRONTEND_BUILD_RESULT
        )
    endif()

    add_custom_target(build_frontend
        COMMAND ${PNPM_EXECUTABLE} run build
        WORKING_DIRECTORY "${FRONTEND_DIR}"
        COMMENT "Building React frontend with Vite..."
    )
endif()

file(GLOB_RECURSE FRONTEND_DIST_FILES
    CONFIGURE_DEPENDS
    "${FRONTEND_DIST_DIR}/*"
)

qt_add_executable(WordSprint
    src/main.cpp
    src/viewmodel/AppViewModel.cpp
    src/viewmodel/GameplayViewModel.cpp
    src/model/MockVocabularyRepository.cpp
    src/model/IVocabularyRepository.cpp
)

#set_target_properties(WordSprint PROPERTIES
#    WIN32_EXECUTABLE TRUE
#    MACOSX_BUNDLE TRUE
#)

if(FRONTEND_DIST_FILES)
    qt_add_resources(WordSprint "frontend_resources"
        PREFIX "/dist"
        BASE "${FRONTEND_DIST_DIR}"
        FILES ${FRONTEND_DIST_FILES}
    )
endif()

qt_add_qml_module(WordSprint
    URI WordSprint
    VERSION 1.0
    QML_FILES
        qml/Main.qml
)

target_link_libraries(WordSprint PRIVATE
    mingw32
    Qt6::Core
    Qt6::Gui
    Qt6::Qml
    Qt6::Quick
)

enable_testing()
find_package(Qt6 REQUIRED COMPONENTS Test)

add_executable(test_gameplay
    tests/test_gameplay.cpp
    src/viewmodel/GameplayViewModel.cpp
    src/model/MockVocabularyRepository.cpp
    src/model/IVocabularyRepository.cpp
)

target_link_libraries(test_gameplay PRIVATE
    Qt6::Test
    Qt6::Core
    Qt6::Gui
    Qt6::Qml
    Qt6::Quick
)

add_test(NAME GameplayTests COMMAND ${CMAKE_CURRENT_BINARY_DIR}/test_gameplay)

if(HAVE_WEBCHANNEL)
    target_link_libraries(WordSprint PRIVATE Qt6::WebChannel)
    target_compile_definitions(WordSprint PRIVATE HAVE_WEBCHANNEL)
endif()

if(HAVE_WEBENGINECORE)
    target_link_libraries(WordSprint PRIVATE Qt6::WebEngineCore)
    target_compile_definitions(WordSprint PRIVATE HAVE_WEBENGINECORE)
endif()

if(HAVE_WEBENGINEQUICK)
    target_link_libraries(WordSprint PRIVATE Qt6::WebEngineQuick)
    target_compile_definitions(WordSprint PRIVATE HAVE_WEBENGINEQUICK)
endif()





if(WIN32)
    add_custom_command(TARGET WordSprint POST_BUILD
        COMMAND "${QT6_INSTALL_PREFIX}/bin/windeployqt.exe"
                --qmldir "${CMAKE_CURRENT_SOURCE_DIR}/qml"
                "$<TARGET_FILE:WordSprint>"
        COMMENT "Deploying Qt runtime DLLs with windeployqt..."
    )
endif()
