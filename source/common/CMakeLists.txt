set(srcs
    TimestampedFrame.cpp
    AgentData.cpp
    CameraCalibration.cpp
    CoordinatesConversion.cpp
    RunTimer.cpp
    settings/CommandLineParameters.cpp
    settings/CalibrationSettings.cpp
)

set(hdrs
    AgentState.hpp
    SetupType.hpp
    CommonPointerTypes.hpp
    settings/ReadSettingsHelper.hpp
)

add_library(common SHARED ${srcs} ${hdrs})
target_link_libraries(common Qt5::Core Qt5::Widgets ${OpenCV_LIBS} ${LIBTSAI_LIBRARY})

