set(srcs
    main.cpp
    MainWindow.cpp
)

qt5_wrap_ui(uis_hdrs MainWindow.ui)
#qt5_add_resources(resources icons.qrc)

include_directories(${CMAKE_SOURCE_DIR}/source/common)
include_directories(${CMAKE_SOURCE_DIR}/source/inter-species)
include_directories(${CMAKE_SOURCE_DIR}/source/messages)

add_executable(inter-species-data-sender ${srcs} ${uis_hdrs})
target_link_libraries(inter-species-data-sender common inter-species messages Qt5::DBus Qt5::Core Qt5::Widgets Qt5::Gui)
