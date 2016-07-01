set(srcs
    VideoGrabber.cpp
    StreamReceiver.cpp
    QueueingApplicationSink.cpp
    TimestampedFrame.cpp
    VideoGrabberSettings.cpp
)

add_library(video-grabber SHARED ${srcs})
target_link_libraries(video-grabber Qt5::Core Qt5::Widgets ${OpenCV_LIBS}
                        ${QTGSTREAMER_LIBRARY} ${QTGSTREAMER_LIBRARIES}
                        ${QTGSTREAMER_UTILS_LIBRARY} ${QTGSTREAMER_UTILS_LIBRARIES}
                        ${QTGLIB_LIBRARIES} ${QTGSTREAMER_UI_LIBRARY} ${QTGSTREAMER_UI_LIBRARIES}
                        ${GSTREAMER_LIBRARY} ${GSTREAMER_VIDEO_LIBRARY})

#  QTGSTREAMER_DEFINITIONS - definitions recommended for using QtGStreamer
#  QTGSTREAMER_FLAGS - extra compiler switches recommended for using QtGStreamer
