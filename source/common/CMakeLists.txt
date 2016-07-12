set(srcs
    TimestampedFrame.cpp
    AgentData.cpp
)

set(hdrs
    AgentState.hpp
)

add_library(common SHARED ${srcs} ${hdrs})
target_link_libraries(common Qt5::Core Qt5::Widgets ${OpenCV_LIBS} )

