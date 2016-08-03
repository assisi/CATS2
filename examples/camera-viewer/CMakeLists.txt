set(srcs
    main.cpp
)

include_directories(${CMAKE_SOURCE_DIR}/source/grabber)
include_directories(${CMAKE_SOURCE_DIR}/source/viewer)
include_directories(${CMAKE_SOURCE_DIR}/source/common)
add_executable(camera-viewer ${srcs})
target_link_libraries(camera-viewer common grabber viewer Qt5::Core Qt5::Widgets Qt5::Gui
                        ${QTGSTREAMER_LIBRARY} ${QTGSTREAMER_LIBRARIES}
                        ${QTGSTREAMER_UTILS_LIBRARY} ${QTGSTREAMER_UTILS_LIBRARIES}
                        ${QTGSTREAMER_UTILS_LIBRARY} ${QTGSTREAMER_UTILS_LIBRARIES}
                        ${QTGLIB_LIBRARIES} ${QTGSTREAMER_UI_LIBRARY} ${QTGSTREAMER_UI_LIBRARIES}
                        ${GSTREAMER_LIBRARIES} ${GSTREAMER_VIDEO_LIBRARIES})
