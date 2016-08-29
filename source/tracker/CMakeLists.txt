set(srcs
    routines/TrackingRoutine.cpp
    routines/BlobDetector.cpp
    routines/FishBotLedsTracking.cpp
    settings/TrackingSettings.cpp
    settings/TrackingRoutineSettings.cpp
    settings/BlobDetectorSettings.cpp
    settings/FishBotLedsTrackingSettings.cpp
    settings/TrackingSetupSettings.cpp
    TrackingData.cpp
    TrackingDataManager.cpp
    TrackingSetup.cpp
    TrackingHandler.cpp
    gui/TrackingRoutineWidget.cpp
    gui/BlobDetectorWidget.cpp
    gui/FishBotLedsTrackingWidget.cpp
    gui/TrackingUiFactory.cpp
)

set(hdrs
    TrackingFactory.hpp
    routines/TrackingRoutineType.hpp
    TrackerPointerTypes.hpp
)

qt5_add_resources(resources icons.qrc)
qt5_wrap_ui(uis_hdrs
            gui/TrackingRoutineWidget.ui
            gui/BlobDetectorWidget.ui
            gui/FishBotLedsTrackingWidget.ui)

include_directories(${CMAKE_SOURCE_DIR}/source/common)
include_directories(${CMAKE_SOURCE_DIR}/source/grabber)
include_directories(${CMAKE_SOURCE_DIR}/source/viewer)
include_directories(${CMAKE_SOURCE_DIR}/source/hub)
add_library(tracker SHARED ${srcs} ${hdrs} ${uis_hdrs} ${resources})
target_link_libraries(tracker grabber common viewer hub
                      Qt5::Core Qt5::Widgets
                      ${OpenCV_LIBS} ${LIBTSAI_LIBRARY})


