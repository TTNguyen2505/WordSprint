cmake_minimum_required(VERSION 3.22)

project(WordSprintBackend LANGUAGES CXX)

find_package(Qt6 REQUIRED COMPONENTS
    Core
    Gui
    Qml
    Quick
    WebChannel
    Sql
    Lottie
)

find_package(Qt6 OPTIONAL_COMPONENTS
    WebEngineQuick
    WebEngineCore
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
    src/BridgeController.h
    src/BridgeController.cpp
    src/models/Stage.h
    src/models/StageRecord.h
    src/models/Topic.h
    src/models/UserProgress.h
    src/models/Vocabulary.h
    src/database/DatabaseManager.h
    src/database/DatabaseManager.cpp
    src/repositories/IVocabularyRepository.h
    src/repositories/SqliteVocabularyRepository.h
    src/repositories/SqliteVocabularyRepository.cpp
)

set(DATABASE_SCHEMA_FILE "${CMAKE_CURRENT_SOURCE_DIR}/database/schema.sql")
set(DATABASE_SEED_FILE "${CMAKE_CURRENT_SOURCE_DIR}/database/seed_data.sql")
if(EXISTS "${DATABASE_SCHEMA_FILE}" AND EXISTS "${DATABASE_SEED_FILE}")
    add_custom_command(TARGET WordSprint POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E make_directory "$<TARGET_FILE_DIR:WordSprint>/database"
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
                "${DATABASE_SCHEMA_FILE}"
                "$<TARGET_FILE_DIR:WordSprint>/database/schema.sql"
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
                "${DATABASE_SEED_FILE}"
                "$<TARGET_FILE_DIR:WordSprint>/database/seed_data.sql"
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

set(LOTTIE_ASSET_DIR "${FRONTEND_DIR}/src/assets/lottie")
set(LOTTIE_ASSET_FILES
    "${LOTTIE_ASSET_DIR}/result-correct.json"
    "${LOTTIE_ASSET_DIR}/result-incorrect.json"
    "${LOTTIE_ASSET_DIR}/summary-complete.json"
)
qt_add_resources(WordSprint "lottie_resources"
    PREFIX "/lottie"
    BASE "${LOTTIE_ASSET_DIR}"
    FILES ${LOTTIE_ASSET_FILES}
)

qt_add_qml_module(WordSprint
    URI WordSprint
    VERSION 1.0
    QML_FILES
        qml/Main.qml
        qml/ResultLottieFeedback.qml
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
    Qt6::Lottie
    ${OPTIONAL_WEBENGINE_LIBS}
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
