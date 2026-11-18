cmake_minimum_required(VERSION 3.22)

project(WordSprintBackend LANGUAGES CXX)

find_package(Qt6 REQUIRED COMPONENTS
    Core
    Gui
    Qml
    Quick
    WebEngineQuick
    WebEngineCore
    WebChannel
    Sql
)

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
)

set_target_properties(WordSprint PROPERTIES
    WIN32_EXECUTABLE TRUE
    MACOSX_BUNDLE TRUE
)

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
    Qt6::Core
    Qt6::Gui
    Qt6::Qml
    Qt6::Quick
    Qt6::WebEngineQuick
    Qt6::WebEngineCore
    Qt6::WebChannel
    Qt6::Sql
)

if(WIN32)
    add_custom_command(TARGET WordSprint POST_BUILD
        COMMAND "${QT6_INSTALL_PREFIX}/bin/windeployqt.exe"
                --qmldir "${CMAKE_CURRENT_SOURCE_DIR}/qml"
                --include-plugins qsqlite
                "$<TARGET_FILE:WordSprint>"
        COMMENT "Deploying Qt runtime DLLs with windeployqt..."
    )
endif()
