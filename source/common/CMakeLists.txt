set(srcs
    TimestampedFrame.cpp
)

add_library(common SHARED ${srcs})
target_link_libraries(common Qt5::Core Qt5::Widgets ${OpenCV_LIBS})

