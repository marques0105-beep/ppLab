cmake_minimum_required(VERSION 3.16)

project(ParkOut VERSION 0.1 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Qt6 REQUIRED COMPONENTS Core Quick Concurrent)

qt_standard_project_setup(REQUIRES 6.5)

qt_add_executable(appParkOut
    main.cpp
)

qt_add_qml_module(appParkOut
    URI ParkOut
    QML_FILES
        Main.qml
    SOURCES
        bus.h bus.cpp
        board.h board.cpp
        gamecontroller.h gamecontroller.cpp
        gameanalytics.h
        persistence.h
        passenger.h
        gametests.h
)

qt_add_resources(appParkOut "gamedata"
    PREFIX "/"
    FILES levels.json
)

set_target_properties(appParkOut PROPERTIES
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
    MACOSX_BUNDLE TRUE
    WIN32_EXECUTABLE TRUE
)

target_link_libraries(appParkOut PRIVATE
    Qt6::Quick
    Qt6::Core
    Qt6::Concurrent
)

configure_file(
    ${CMAKE_CURRENT_SOURCE_DIR}/levels.json
    ${CMAKE_CURRENT_BINARY_DIR}/levels.json
    COPYONLY
)

install(TARGETS appParkOut
    BUNDLE DESTINATION .
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)