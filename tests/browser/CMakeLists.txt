set(browser_SRCS
    main.cpp
)

qt_add_resources(browser_SRCS
    main.qrc
)

add_executable(browser ${browser_SRCS})
target_link_libraries(browser Qt::Quick)
