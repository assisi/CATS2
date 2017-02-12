set(srcs
    TimestampedFrame.cpp
    AgentData.cpp
    CameraCalibration.cpp
    CoordinatesConversion.cpp
    RunTimer.cpp
    settings/CommandLineParameters.cpp
    settings/CalibrationSettings.cpp
    settings/CommandLineParser.cpp
    settings/StreamDescriptor.cpp
)

set(hdrs
    AgentState.hpp
    SetupType.hpp
    Timer.hpp
    CommonPointerTypes.hpp
    settings/ReadSettingsHelper.hpp
)

add_library(common SHARED ${srcs} ${hdrs})
target_link_libraries(common Qt5::Core Qt5::Widgets ${OpenCV_LIBS}) #${LIBTSAI_LIBRARY})

install(TARGETS common DESTINATION .)
install(FILES SetupType.hpp CommonPointerTypes.hpp AgentState.hpp AgentData.hpp
        RunTimer.hpp
        DESTINATION include/common)
install(FILES settings/CalibrationSettings.hpp settings/CommandLineParser.hpp
        settings/StreamDescriptor.hpp settings/ReadSettingsHelper.hpp
        DESTINATION include/common/settings)

