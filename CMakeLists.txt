cmake_minimum_required(VERSION 3.16)

project(Df64Tester VERSION 0.1 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Qt6 REQUIRED COMPONENTS Quick)
find_package(Qt6 REQUIRED COMPONENTS SerialPort)

qt_standard_project_setup(REQUIRES 6.8)

qt_add_executable(appDf64Tester

)

qt_add_qml_module(appDf64Tester
    URI Df64Tester
    QML_FILES

    SOURCES src/main.cpp
    QML_FILES qml/App.qml
    SOURCES
    SOURCES src/fn_log.h
    QML_FILES
    QML_FILES qml/App.qml
    QML_FILES qml/Constants.qml
    SOURCES src/Globals.cpp
    SOURCES src/Globals.h
    SOURCES src/tqmllayer.h src/tqmllayer.cpp
    SOURCES src/df64iface.cpp src/df64iface.h
)

# Qt for iOS sets MACOSX_BUNDLE_GUI_IDENTIFIER automatically since Qt 6.1.
# If you are developing for iOS or macOS you should consider setting an
# explicit, fixed bundle identifier manually though.
set_target_properties(appDf64Tester PROPERTIES
#    MACOSX_BUNDLE_GUI_IDENTIFIER com.example.appDf64Tester
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
    MACOSX_BUNDLE TRUE
    WIN32_EXECUTABLE TRUE
)

target_link_libraries(appDf64Tester
    PRIVATE Qt6::Quick
    PRIVATE Qt6::SerialPort
    shlwapi
)

include(GNUInstallDirs)
install(TARGETS appDf64Tester
    BUNDLE DESTINATION .
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)
