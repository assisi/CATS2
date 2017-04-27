set(srcs
    main.cpp
    MainWindow.cpp
    FrameScene.cpp
)

qt5_wrap_ui(uis_hdrs MainWindow.ui)
qt5_add_resources(resources icons.qrc)

add_executable(pixel-locator ${srcs} ${uis_hdrs} ${resources})

target_link_libraries(pixel-locator Qt5::Core Qt5::Widgets Qt5::Gui)
