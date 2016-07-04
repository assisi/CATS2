set(srcs
    VideoGrabber.cpp
    StreamReceiver.cpp
    QueueingApplicationSink.cpp
    VideoGrabberSettings.cpp
)

include_directories(${CMAKE_SOURCE_DIR}/source/common)
add_library(video-grabber SHARED ${srcs})
target_link_libraries(video-grabber common Qt5::Core Qt5::Widgets ${OpenCV_LIBS}
                        ${QTGSTREAMER_LIBRARY} ${QTGSTREAMER_LIBRARIES}
                        ${QTGSTREAMER_UTILS_LIBRARY} ${QTGSTREAMER_UTILS_LIBRARIES}
                        ${QTGLIB_LIBRARIES} ${QTGSTREAMER_UI_LIBRARY} ${QTGSTREAMER_UI_LIBRARIES}
                        ${GSTREAMER_LIBRARY} ${GSTREAMER_VIDEO_LIBRARY})
