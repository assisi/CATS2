set(srcs
    TimestampedFrame.cpp
    AgentData.cpp
    CameraCalibration.cpp
    CoordinatesConversion.cpp
    CommandLineParameters.cpp
)

set(hdrs
    AgentState.hpp
    ReadSettingsHelper.hpp
    CommonTypes.hpp
)

add_library(common SHARED ${srcs} ${hdrs})
target_link_libraries(common Qt5::Core Qt5::Widgets ${OpenCV_LIBS} ${LIBTSAI_LIBRARY})

