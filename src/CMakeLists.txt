set(qtwpe_SOURCES
    WPEQtViewBackend.cpp
    WPEQmlExtensionPlugin.cpp
    WPEQtView.cpp
    WPEQtViewLoadRequest.cpp
    WPEQtImContext.cpp
)

set(qtwpe_LIBRARIES
    Qt5::Core Qt5::Gui Qt5::GuiPrivate Qt5::Quick
    PkgConfig::EGL
    PkgConfig::EPOXY
    PkgConfig::WPE
    PkgConfig::WPE_FDO
    PkgConfig::WPE_WEBKIT
)

add_library(qtwpe MODULE ${qtwpe_SOURCES})
set_target_properties(qtwpe PROPERTIES
    OUTPUT_NAME qtwpe
    AUTOMOC ON
    CXX_STANDARD 14
)
target_compile_definitions(qtwpe PUBLIC QT_NO_KEYWORDS=1)
target_link_libraries(qtwpe ${qtwpe_LIBRARIES})

target_include_directories(qtwpe SYSTEM PRIVATE compat)

install(TARGETS qtwpe DESTINATION "${CMAKE_INSTALL_FULL_LIBDIR}/qml/org/wpewebkit/qtwpe/")
install(FILES qmldir DESTINATION "${CMAKE_INSTALL_FULL_LIBDIR}/qml/org/wpewebkit/qtwpe/")

file(MAKE_DIRECTORY ${CMAKE_BINARY_DIR}/qml/org/wpewebkit/qtwpe)
add_custom_command(TARGET qtwpe POST_BUILD COMMAND ${CMAKE_COMMAND} -E copy
    ${CMAKE_CURRENT_BINARY_DIR}/libqtwpe.so
    ${CMAKE_BINARY_DIR}/qml/org/wpewebkit/qtwpe)
add_custom_command(TARGET qtwpe POST_BUILD COMMAND ${CMAKE_COMMAND} -E copy
    ${CMAKE_CURRENT_SOURCE_DIR}/qmldir
    ${CMAKE_BINARY_DIR}/qml/org/wpewebkit/qtwpe)
