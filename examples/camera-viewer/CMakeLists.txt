set(srcs
    main.cpp
    MainWindow.cpp
)

qt5_wrap_ui(uis_hdrs MainWindow.ui)
qt5_add_resources(resources icons.qrc)

include_directories(${CMAKE_SOURCE_DIR}/source/video-grabber)
include_directories(${CMAKE_SOURCE_DIR}/source/viewer)
include_directories(${CMAKE_SOURCE_DIR}/source/common)
add_executable(camera-viewer ${srcs} ${uis_hdrs} ${resources})
target_link_libraries(camera-viewer common video-grabber viewer Qt5::Core Qt5::Widgets Qt5::Gui
                        ${QTGSTREAMER_LIBRARY} ${QTGSTREAMER_LIBRARIES}
                        ${QTGSTREAMER_UTILS_LIBRARY} ${QTGSTREAMER_UTILS_LIBRARIES}
                        ${QTGSTREAMER_UTILS_LIBRARY} ${QTGSTREAMER_UTILS_LIBRARIES}
                        ${QTGLIB_LIBRARIES} ${QTGSTREAMER_UI_LIBRARY} ${QTGSTREAMER_UI_LIBRARIES}
                        ${GSTREAMER_LIBRARIES} ${GSTREAMER_VIDEO_LIBRARIES})
