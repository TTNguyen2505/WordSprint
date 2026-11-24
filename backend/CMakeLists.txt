cmake_minimum_required(VERSION 3.22)

project(WordSprintBackend LANGUAGES CXX)

find_package(Qt6 REQUIRED COMPONENTS
    Core
    Gui
    Qml
    Quick
    WebChannel
    Sql
)

find_package(Qt6 OPTIONAL_COMPONENTS
    WebEngineQuick
    WebEngineCore
)

if(NOT WIN32)
    find_package(SQLite3 REQUIRED)
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
    src/controller/BridgeController.h
    src/controller/BridgeController.cpp
    src/models/CEFRLevel.h
    src/models/Stage.h
    src/models/StageStatusDTO.h
    src/models/StageRecord.h
    src/models/StageRecordDTO.h
    src/models/Topic.h
    src/models/User.h
    src/models/WordItem.h
    src/models/OverallRankDTO.h
    src/core/ApplicationSetup.h
    src/core/ApplicationSetup.cpp
    src/core/DatabaseManager.h
    src/core/DatabaseManager.cpp
    src/repositories/RecordRepository.h
    src/repositories/RecordRepository.cpp
    src/repositories/StageRepository.h
    src/repositories/StageRepository.cpp
    src/repositories/UserRepository.h
    src/repositories/UserRepository.cpp
    src/repositories/SqliteRepositoryUtils.h
)

set(DATABASE_SCHEMA_FILE "${CMAKE_CURRENT_SOURCE_DIR}/sql/schema.sql")
set(DATABASE_APP_DATA_SEED_FILE "${CMAKE_CURRENT_SOURCE_DIR}/sql/seed_app_data.sql")
set(DATABASE_MOCK_USERS_SEED_FILE "${CMAKE_CURRENT_SOURCE_DIR}/sql/seed_mock_users.sql")
if(EXISTS "${DATABASE_SCHEMA_FILE}" AND EXISTS "${DATABASE_APP_DATA_SEED_FILE}" AND
   EXISTS "${DATABASE_MOCK_USERS_SEED_FILE}")
    add_custom_command(TARGET WordSprint POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E make_directory "$<TARGET_FILE_DIR:WordSprint>/database"
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
                "${DATABASE_SCHEMA_FILE}"
                "$<TARGET_FILE_DIR:WordSprint>/database/schema.sql"
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
                "${DATABASE_APP_DATA_SEED_FILE}"
                "$<TARGET_FILE_DIR:WordSprint>/database/seed_app_data.sql"
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
                "${DATABASE_MOCK_USERS_SEED_FILE}"
                "$<TARGET_FILE_DIR:WordSprint>/database/seed_mock_users.sql"
        COMMENT "Copying database schema and seed files..."
        VERBATIM
    )
endif()

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

target_include_directories(WordSprint PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/src")

qt_add_qml_module(WordSprint
    URI WordSprint
    VERSION 1.0
    QML_FILES
        src/Main.qml
)

set(OPTIONAL_WEBENGINE_LIBS "")
if(TARGET Qt6::WebEngineQuick)
    list(APPEND OPTIONAL_WEBENGINE_LIBS Qt6::WebEngineQuick)
endif()
if(TARGET Qt6::WebEngineCore)
    list(APPEND OPTIONAL_WEBENGINE_LIBS Qt6::WebEngineCore)
endif()

target_link_libraries(WordSprint PRIVATE
    Qt6::Core
    Qt6::Gui
    Qt6::Qml
    Qt6::Quick
    Qt6::WebChannel
    Qt6::Sql
    ${OPTIONAL_WEBENGINE_LIBS}
)

if(WIN32)
    target_link_libraries(WordSprint PRIVATE winsqlite3)
else()
    target_link_libraries(WordSprint PRIVATE SQLite::SQLite3)
endif()

if(BUILD_TESTING)
    add_executable(BridgeViewModelTest
        tests/BridgeViewModelTest.cpp
        src/controller/BridgeController.h
        src/controller/BridgeController.cpp
        src/core/DatabaseManager.cpp
        src/repositories/UserRepository.cpp
        src/repositories/StageRepository.cpp
        src/repositories/RecordRepository.cpp
    )
    target_include_directories(BridgeViewModelTest PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/src")
    target_compile_definitions(BridgeViewModelTest PRIVATE WORDSPRINT_SQL_DIR="${CMAKE_CURRENT_SOURCE_DIR}/sql")
    target_link_libraries(BridgeViewModelTest PRIVATE Qt6::Core Qt6::Sql)
    if(WIN32)
        target_link_libraries(BridgeViewModelTest PRIVATE winsqlite3)
    else()
        target_link_libraries(BridgeViewModelTest PRIVATE SQLite::SQLite3)
    endif()
    add_test(NAME BridgeViewModel COMMAND BridgeViewModelTest)
    set_tests_properties(BridgeViewModel PROPERTIES
        ENVIRONMENT "QT_PLUGIN_PATH=${QT6_INSTALL_PREFIX}/plugins"
        ENVIRONMENT_MODIFICATION "PATH=path_list_prepend:$<TARGET_FILE_DIR:Qt6::Core>"
    )

    add_executable(DatabaseInitializationTest
        tests/DatabaseInitializationTest.cpp
        src/core/DatabaseManager.cpp
    )
    target_include_directories(DatabaseInitializationTest PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/src")
    target_compile_definitions(DatabaseInitializationTest PRIVATE WORDSPRINT_SQL_DIR="${CMAKE_CURRENT_SOURCE_DIR}/sql")
    target_link_libraries(DatabaseInitializationTest PRIVATE Qt6::Core Qt6::Sql)
    if(WIN32)
        target_link_libraries(DatabaseInitializationTest PRIVATE winsqlite3)
    else()
        target_link_libraries(DatabaseInitializationTest PRIVATE SQLite::SQLite3)
    endif()
    add_test(NAME DatabaseInitialization COMMAND DatabaseInitializationTest)
    set_tests_properties(DatabaseInitialization PROPERTIES
        ENVIRONMENT "QT_PLUGIN_PATH=${QT6_INSTALL_PREFIX}/plugins"
        ENVIRONMENT_MODIFICATION "PATH=path_list_prepend:$<TARGET_FILE_DIR:Qt6::Core>"
    )
endif()

if(WIN32)
    add_custom_command(TARGET WordSprint POST_BUILD
        COMMAND "${QT6_INSTALL_PREFIX}/bin/windeployqt.exe"
                --qmldir "${CMAKE_CURRENT_SOURCE_DIR}/src"
                --include-plugins qsqlite
                "$<TARGET_FILE:WordSprint>"
        COMMENT "Deploying Qt runtime DLLs with windeployqt..."
    )
endif()
