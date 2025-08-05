cmake_minimum_required(VERSION 3.16)

project(cantracer VERSION 0.1 LANGUAGES CXX)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Widgets Charts)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Widgets Charts)


set(PROJECT_SOURCES
        main.cpp


        mainwindow.ui
        dbc_json_parser.cpp
        dbc_json_parser.h
        asc_reader.cpp
        asc_reader.h
        signal_selection_dialog.cpp
        signal_selection_dialog.h



)

if(${QT_VERSION_MAJOR} GREATER_EQUAL 6)
    qt_add_executable(cantracer
        MANUAL_FINALIZATION
        ${PROJECT_SOURCES}
        can_frame.h
        can_decoder.cpp
        signal_model.h
        can_decoder.h
        signal_selection_dialog.h
        signal_selection_dialog.cpp

        mainwindow.cpp
        mainwindow.h
        cursormanager.h
        cursormanager.cpp
        movablelineitem.h
        customchartview.h
        customchartview.cpp



    )
# Define target properties for Android with Qt 6 as:
#    set_property(TARGET cantracer APPEND PROPERTY QT_ANDROID_PACKAGE_SOURCE_DIR
#                 ${CMAKE_CURRENT_SOURCE_DIR}/android)
# For more information, see https://doc.qt.io/qt-6/qt-add-executable.html#target-creation
else()
    if(ANDROID)
        add_library(cantracer SHARED
            ${PROJECT_SOURCES}
        )
# Define properties for Android with Qt 5 after find_package() calls as:
#    set(ANDROID_PACKAGE_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/android")
    else()
        add_executable(cantracer
            ${PROJECT_SOURCES}
        )
    endif()
endif()

target_link_libraries(cantracer PRIVATE Qt${QT_VERSION_MAJOR}::Widgets Qt${QT_VERSION_MAJOR}::Charts)

# Qt for iOS sets MACOSX_BUNDLE_GUI_IDENTIFIER automatically since Qt 6.1.
# If you are developing for iOS or macOS you should consider setting an
# explicit, fixed bundle identifier manually though.
if(${QT_VERSION} VERSION_LESS 6.1.0)
  set(BUNDLE_ID_OPTION MACOSX_BUNDLE_GUI_IDENTIFIER com.example.cantracer)
endif()
set_target_properties(cantracer PROPERTIES
    ${BUNDLE_ID_OPTION}
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
    MACOSX_BUNDLE TRUE
    WIN32_EXECUTABLE TRUE
)

include(GNUInstallDirs)
install(TARGETS cantracer
    BUNDLE DESTINATION .
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

if(QT_VERSION_MAJOR EQUAL 6)
    qt_finalize_executable(cantracer)
endif()
