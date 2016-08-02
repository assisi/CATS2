set(srcs
    GrabberData.cpp
    StreamReceiver.cpp
    QueueingApplicationSink.cpp
)

set(hdrs
    GrabberPointerTypes.hpp
)

include_directories(${CMAKE_SOURCE_DIR}/source/common)
add_library(grabber SHARED ${srcs} ${hdrs})
target_link_libraries(grabber common Qt5::Core Qt5::Widgets ${OpenCV_LIBS}
                        ${QTGSTREAMER_LIBRARY} ${QTGSTREAMER_LIBRARIES}
                        ${QTGSTREAMER_UTILS_LIBRARY} ${QTGSTREAMER_UTILS_LIBRARIES}
                        ${QTGLIB_LIBRARIES} ${QTGSTREAMER_UI_LIBRARY} ${QTGSTREAMER_UI_LIBRARIES}
                        ${GSTREAMER_LIBRARIES} ${GSTREAMER_VIDEO_LIBRARIES})
