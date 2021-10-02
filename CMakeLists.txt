cmake_minimum_required(VERSION 3.1)

project(wpewebkit-qt VERSION 0.1.0)

set(CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake ${CMAKE_MODULE_PATH})

include(FeatureSummary)

set(QT_MIN_VERSION "5.15.0")
find_package(Qt5 ${QT_MIN_VERSION} REQUIRED COMPONENTS Core Gui Quick)

find_package(PkgConfig)
pkg_check_modules(EGL egl IMPORTED_TARGET)
pkg_check_modules(EPOXY epoxy IMPORTED_TARGET)
pkg_check_modules(WPE wpe-1.0 IMPORTED_TARGET)
pkg_check_modules(WPE_FDO wpebackend-fdo-1.0 IMPORTED_TARGET)
pkg_check_modules(WPE_WEBKIT wpe-webkit-1.0 IMPORTED_TARGET)

add_subdirectory(src)

feature_summary(WHAT ALL FATAL_ON_MISSING_REQUIRED_PACKAGES)
