set(CMAKE_PREFIX_PATH "C:/Qt/6.11.2/mingw_64")
set(CMAKE_C_COMPILER "C:/Qt/Tools/mingw1310_64/bin/gcc.exe")
set(CMAKE_CXX_COMPILER "C:/Qt/Tools/mingw1310_64/bin/g++.exe")

cmake_minimum_required(VERSION 3.22)
project(WordSprintBackend LANGUAGES CXX)

find_package(Qt6 REQUIRED COMPONENTS Core Gui Qml Quick Sql Test)
find_package(Qt6 COMPONENTS WebEngineCore WebEngineQuick WebChannel QUIET)

if (Qt6WebEngineCore_FOUND)
    add_definitions(-DHAVE_WEBENGINECORE)
endif()
if (Qt6WebEngineQuick_FOUND)
    add_definitions(-DHAVE_WEBENGINEQUICK)
endif()
if (Qt6WebChannel_FOUND)
    add_definitions(-DHAVE_WEBCHANNEL)
endif()

qt_standard_project_setup()
qt_policy(SET QTP0001 NEW)
qt_policy(SET QTP0004 NEW)

find_program(PNPM_EXECUTABLE NAMES pnpm pnpm.cmd npm npm.cmd)
set(FRONTEND_DIR "${CMAKE_CURRENT_SOURCE_DIR}/../frontend")
set(FRONTEND_DIST_DIR "${FRONTEND_DIR}/dist")

if(PNPM_EXECUTABLE)
    if(NOT EXISTS "${FRONTEND_DIST_DIR}/index.html")
        execute_process(COMMAND ${PNPM_EXECUTABLE} run build WORKING_DIRECTORY "${FRONTEND_DIR}")
    endif()
    add_custom_target(build_frontend COMMAND ${PNPM_EXECUTABLE} run build WORKING_DIRECTORY "${FRONTEND_DIR}")
endif()

file(GLOB_RECURSE FRONTEND_DIST_FILES CONFIGURE_DEPENDS "${FRONTEND_DIST_DIR}/*")

qt_add_executable(WordSprint
    src/main.cpp
    src/model/Topic.h
    src/model/Word.h
    src/model/Word.cpp
    src/repository/WordRepository.h
    src/repository/WordRepository.cpp
)

set_target_properties(WordSprint PROPERTIES WIN32_EXECUTABLE TRUE MACOSX_BUNDLE TRUE)

if(FRONTEND_DIST_FILES)
    qt_add_resources(WordSprint "frontend_resources" PREFIX "/dist" BASE "${FRONTEND_DIST_DIR}" FILES ${FRONTEND_DIST_FILES})
endif()

qt_add_qml_module(WordSprint URI WordSprint VERSION 1.0 QML_FILES qml/Main.qml)

target_link_libraries(WordSprint PRIVATE Qt6::Core Qt6::Gui Qt6::Qml Qt6::Quick Qt6::Sql)
if (Qt6WebEngineCore_FOUND)
    target_link_libraries(WordSprint PRIVATE Qt6::WebEngineCore)
endif()
if (Qt6WebEngineQuick_FOUND)
    target_link_libraries(WordSprint PRIVATE Qt6::WebEngineQuick)
endif()
if (Qt6WebChannel_FOUND)
    target_link_libraries(WordSprint PRIVATE Qt6::WebChannel)
endif()

if(WIN32)
    add_custom_command(TARGET WordSprint POST_BUILD
        COMMAND "${QT6_INSTALL_PREFIX}/bin/windeployqt.exe" --qmldir "${CMAKE_CURRENT_SOURCE_DIR}/qml" $<TARGET_FILE:WordSprint>
    )
endif()

add_custom_command(TARGET WordSprint POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different ${CMAKE_CURRENT_SOURCE_DIR}/database/schema.sql $<TARGET_FILE_DIR:WordSprint>/schema.sql
    COMMAND ${CMAKE_COMMAND} -E copy_if_different ${CMAKE_CURRENT_SOURCE_DIR}/database/seed.sql $<TARGET_FILE_DIR:WordSprint>/seed.sql
)

enable_testing()

add_executable(test_sqlite ${CMAKE_CURRENT_SOURCE_DIR}/database/test_sqlite.cpp)
target_link_libraries(test_sqlite PRIVATE Qt6::Core Qt6::Sql Qt6::Test)
add_custom_command(TARGET test_sqlite POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different ${CMAKE_CURRENT_SOURCE_DIR}/database/schema.sql $<TARGET_FILE_DIR:test_sqlite>/schema.sql
    COMMAND ${CMAKE_COMMAND} -E copy_if_different ${CMAKE_CURRENT_SOURCE_DIR}/database/seed.sql $<TARGET_FILE_DIR:test_sqlite>/seed.sql
)
add_test(NAME SQLiteSchemaTest COMMAND test_sqlite)

add_executable(RepositoryTest
    src/repository_test/main.cpp
    src/model/Word.cpp
    src/repository/WordRepository.cpp
)
target_include_directories(RepositoryTest PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/src/model ${CMAKE_CURRENT_SOURCE_DIR}/src/repository)
target_link_libraries(RepositoryTest PRIVATE Qt6::Core Qt6::Sql)
add_custom_command(TARGET RepositoryTest POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different ${CMAKE_CURRENT_SOURCE_DIR}/database/schema.sql $<TARGET_FILE_DIR:RepositoryTest>/schema.sql
    COMMAND ${CMAKE_COMMAND} -E copy_if_different ${CMAKE_CURRENT_SOURCE_DIR}/database/seed.sql $<TARGET_FILE_DIR:RepositoryTest>/seed.sql
)
add_test(NAME RepositoryDaoTest COMMAND RepositoryTest)
