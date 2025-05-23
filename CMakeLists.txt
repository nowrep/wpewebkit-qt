cmake_minimum_required(VERSION 3.6)

project(wpewebkit-qt VERSION 0.1.0)

set(CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake ${CMAKE_MODULE_PATH})

include(FeatureSummary)

option(USE_QT6 "Use Qt6" ON)

if(USE_QT6)
    set(QT_MIN_VERSION "6.2.0")
    find_package(Qt6 ${QT_MIN_VERSION} REQUIRED COMPONENTS Core Gui Quick)
else()
    set(QT_MIN_VERSION "5.15.0")
    find_package(Qt5 ${QT_MIN_VERSION} REQUIRED COMPONENTS Core Gui Quick)
endif()

find_package(PkgConfig)
pkg_check_modules(EGL egl IMPORTED_TARGET)
pkg_check_modules(EPOXY epoxy IMPORTED_TARGET)
pkg_check_modules(WPE wpe-1.0 IMPORTED_TARGET)
pkg_check_modules(WPE_FDO wpebackend-fdo-1.0 IMPORTED_TARGET)
pkg_check_modules(WPE_WEBKIT wpe-webkit-1.0 IMPORTED_TARGET)
if(NOT WPE_WEBKIT_FOUND)
    pkg_check_modules(WPE_WEBKIT wpe-webkit-1.1 IMPORTED_TARGET)
    if(NOT WPE_WEBKIT_FOUND)
        pkg_check_modules(WPE_WEBKIT wpe-webkit-2.0 IMPORTED_TARGET)
    endif()
endif()

if(NOT WPE_WEBKIT_FOUND)
    message(FATAL_ERROR "wpe-webkit not found")
endif()

add_subdirectory(src)
add_subdirectory(tests)

feature_summary(WHAT ALL FATAL_ON_MISSING_REQUIRED_PACKAGES)
