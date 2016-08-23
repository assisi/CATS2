set(srcs
    main.cpp
    MainWindow.cpp
)

qt5_wrap_ui(uis_hdrs MainWindow.ui)
#qt5_add_resources(resources icons.qrc)

include_directories(${CMAKE_SOURCE_DIR}/source/grabber)
include_directories(${CMAKE_SOURCE_DIR}/source/viewer)
include_directories(${CMAKE_SOURCE_DIR}/source/common)
include_directories(${CMAKE_SOURCE_DIR}/source/hub)
include_directories(${CMAKE_SOURCE_DIR}/source/tracker)
add_executable(simple-tracker ${srcs} ${uis_hdrs})
target_link_libraries(simple-tracker common grabber tracker viewer hub Qt5::Core Qt5::Widgets Qt5::Gui
                        ${QTGSTREAMER_LIBRARY} ${QTGSTREAMER_LIBRARIES}
                        ${QTGSTREAMER_UTILS_LIBRARY} ${QTGSTREAMER_UTILS_LIBRARIES}
                        ${QTGSTREAMER_UTILS_LIBRARY} ${QTGSTREAMER_UTILS_LIBRARIES}
                        ${QTGLIB_LIBRARIES} ${QTGSTREAMER_UI_LIBRARY} ${QTGSTREAMER_UI_LIBRARIES}
                        ${GSTREAMER_LIBRARY} ${GSTREAMER_VIDEO_LIBRARY})
