set(srcs
    main.cpp
    MainWindow.cpp
)

qt5_wrap_ui(uis_hdrs MainWindow.ui)
#qt5_add_resources(resources icons.qrc)

include_directories(${CMAKE_SOURCE_DIR}/source/common)
include_directories(${CMAKE_SOURCE_DIR}/source/robot-control)

add_executable(simple-robot-controller ${srcs} ${uis_hdrs})
target_link_libraries(simple-robot-controller common robot-control Qt5::DBus Qt5::Core Qt5::Widgets Qt5::Gui)
