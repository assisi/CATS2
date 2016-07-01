set(srcs
   main.cpp
)

include_directories(${CMAKE_SOURCE_DIR}/source/video-grabber)
add_executable(video-grabber-example ${srcs})
target_link_libraries(video-grabber-example video-grabber Qt5::Core Qt5::Widgets
                        ${QTGSTREAMER_LIBRARY} ${QTGSTREAMER_LIBRARIES}
                        ${QTGSTREAMER_UTILS_LIBRARY} ${QTGSTREAMER_UTILS_LIBRARIES}
                        ${QTGSTREAMER_UTILS_LIBRARY} ${QTGSTREAMER_UTILS_LIBRARIES}
                        ${QTGLIB_LIBRARIES} ${QTGSTREAMER_UI_LIBRARY} ${QTGSTREAMER_UI_LIBRARIES}
                        ${GSTREAMER_LIBRARY} ${GSTREAMER_VIDEO_LIBRARY})
