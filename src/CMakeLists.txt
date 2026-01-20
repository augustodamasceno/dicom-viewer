# DICOM VIewer - Configuração do projeto CMake
# Copyright (c) 2026, Augusto Damasceno.
# All rights reserved.
#
# SPDX-License-Identifier: BSD-2-Clause
cmake_minimum_required(VERSION 3.19)

set(VCPKG_TARGET_TRIPLET "x64-mingw-dynamic" CACHE STRING "")
file(TO_CMAKE_PATH "$ENV{USERPROFILE}/vcpkg/scripts/buildsystems/vcpkg.cmake" VCPKG_TOOLCHAIN_PATH)
set(DCMTK_DIR "$ENV{USERPROFILE}/vcpkg/installed/x64-mingw-dynamic/share/dcmtk" CACHE PATH "Path to DCMTK Config" FORCE)

project(dicom-viewer LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

find_package(Qt6 6.5 REQUIRED COMPONENTS Core Widgets LinguistTools)
find_package(DCMTK REQUIRED)

qt_standard_project_setup()

qt_add_executable(dicom-viewer
    WIN32 MACOSX_BUNDLE
    main.cpp
    ui/windows/mainwindow.cpp
    ui/windows/mainwindow.h
    ui/windows/utils.cpp
    ui/windows/utils.h
    ui/forms/mainwindow.ui
    ui/resources/resources.qrc
    core/MedicalImage.cpp
    core/MedicalImage.h
)

qt_add_translations(
    TARGETS dicom-viewer
    TS_FILES translations/dicom-viewer_pt_BR.ts
)

target_link_libraries(dicom-viewer
    PRIVATE
        Qt::Core
        Qt::Widgets
        DCMTK::DCMTK
)

include(GNUInstallDirs)

install(TARGETS dicom-viewer
    BUNDLE  DESTINATION .
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
)

qt_generate_deploy_app_script(
    TARGET dicom-viewer
    OUTPUT_SCRIPT deploy_script
    NO_UNSUPPORTED_PLATFORM_ERROR
)
install(SCRIPT ${deploy_script})
