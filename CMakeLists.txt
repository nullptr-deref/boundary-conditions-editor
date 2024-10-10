cmake_minimum_required(VERSION 3.5)

project(BoundaryConditionsEditor VERSION 0.1 LANGUAGES CXX)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Widgets)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Widgets)
find_package(nlohmann_json REQUIRED)

set(src_dir "src/")
set(include_dir "include/")

set(sources
    "${src_dir}/EditorWindow.cpp"
    "${src_dir}/main.cpp"

    "${src_dir}/bc/Load.cpp"
    "${src_dir}/bc/Restraint.cpp"
    "${src_dir}/bc/Pressure.cpp"
    "${src_dir}/bc/BoundaryConditionsParser.cpp"
)
set(headers
    "${include_dir}/EditorWindow.hpp"
    "${include_dir}/JSONSerializable.hpp"

    "${include_dir}/bc/BoundaryConditionsParser.hpp"
    "${include_dir}/bc/Load.hpp"
    "${include_dir}/bc/Restraint.hpp"
    "${include_dir}/bc/Pressure.hpp"
)
#file(GLOB_RECURSE headers
#    LIST_DIRECTORIES FALSE
#    "${INCLUDE_DIR}/*.hpp"
#)
#file(GLOB_RECURSE sources
#    LIST_DIRECTORIES FALSE
#    "${SRC_DIR}/*.cpp"
#)

set(PROJECT_SOURCES
    ${sources}
    ${headers}
)

if(${QT_VERSION_MAJOR} GREATER_EQUAL 6)
    qt_add_executable(BoundaryConditionsEditor
        MANUAL_FINALIZATION
        ${PROJECT_SOURCES}
    )
# Define target properties for Android with Qt 6 as:
#    set_property(TARGET BoundaryConditionsEditor APPEND PROPERTY QT_ANDROID_PACKAGE_SOURCE_DIR
#                 ${CMAKE_CURRENT_SOURCE_DIR}/android)
# For more information, see https://doc.qt.io/qt-6/qt-add-executable.html#target-creation
else()
    if(ANDROID)
        add_library(BoundaryConditionsEditor SHARED
            ${PROJECT_SOURCES}
        )
# Define properties for Android with Qt 5 after find_package() calls as:
#    set(ANDROID_PACKAGE_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/android")
    else()
        add_executable(BoundaryConditionsEditor
            ${PROJECT_SOURCES}
        )
    endif()
endif()

target_link_libraries(BoundaryConditionsEditor PRIVATE
    Qt${QT_VERSION_MAJOR}::Widgets
    nlohmann_json::nlohmann_json
)

# Qt for iOS sets MACOSX_BUNDLE_GUI_IDENTIFIER automatically since Qt 6.1.
# If you are developing for iOS or macOS you should consider setting an
# explicit, fixed bundle identifier manually though.
if(${QT_VERSION} VERSION_LESS 6.1.0)
  set(BUNDLE_ID_OPTION MACOSX_BUNDLE_GUI_IDENTIFIER com.example.BoundaryConditionsEditor)
endif()
set_target_properties(BoundaryConditionsEditor PROPERTIES
    ${BUNDLE_ID_OPTION}
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
    MACOSX_BUNDLE TRUE
    WIN32_EXECUTABLE TRUE
)

include(GNUInstallDirs)
install(TARGETS BoundaryConditionsEditor
    BUNDLE DESTINATION .
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

if(QT_VERSION_MAJOR EQUAL 6)
    qt_finalize_executable(BoundaryConditionsEditor)
endif()
