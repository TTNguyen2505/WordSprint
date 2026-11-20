cmake_minimum_required(VERSION 3.22)

project(WordSprintBackend LANGUAGES CXX)

find_package(Qt6 REQUIRED COMPONENTS
    Core
    Gui
    Qml
    Quick
    Sql
)

qt_standard_project_setup()
qt_policy(SET QTP0001 NEW)
qt_policy(SET QTP0004 NEW)

qt_add_executable(WordSprint
    src/main.cpp
)

set_target_properties(WordSprint PROPERTIES
    WIN32_EXECUTABLE TRUE
    MACOSX_BUNDLE TRUE
)

qt_add_qml_module(WordSprint
    URI WordSprint
    VERSION 1.0
    QML_FILES
        qml/BrutalButton.qml
        qml/MenuScreen.qml
        qml/ProfileScreen.qml
        qml/CreateProfileScreen.qml
        qml/CountdownScreen.qml
        qml/GameScreen.qml
        qml/LeaderboardScreen.qml
        qml/Main.qml
)

target_link_libraries(WordSprint PRIVATE
    Qt6::Core
    Qt6::Gui
    Qt6::Qml
    Qt6::Quick
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
