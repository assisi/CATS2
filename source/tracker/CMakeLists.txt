set(srcs
    routines/TrackingRoutine.cpp
    routines/BlobDetector.cpp
    routines/ColorDetector.cpp
    routines/FishBotLedsTracking.cpp
    routines/TwoColorsTagTracking.cpp
    settings/TrackingSettings.cpp
    settings/TrackingRoutineSettings.cpp
    settings/BlobDetectorSettings.cpp
    settings/FishBotLedsTrackingSettings.cpp
    settings/TrackingSetupSettings.cpp
    settings/ColorDetectorSettings.cpp
    settings/TwoColorsTagTrackingSettings.cpp
    TrackingData.cpp
    TrackingDataManager.cpp
    TrackingSetup.cpp
    TrackingHandler.cpp
    TrajectoryWriter.cpp
    gui/TrackingRoutineWidget.cpp
    gui/BlobDetectorWidget.cpp
    gui/FishBotLedsTrackingWidget.cpp
    gui/ColorDetectorWidget.cpp
    gui/TrackingUiFactory.cpp
    gui/TwoColorsTagsTrackingWidget.cpp
    gui/FishbotLedColorWidget.cpp
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
            gui/FishBotLedsTrackingWidget.ui
            gui/ColorDetectorWidget.ui
            gui/TwoColorsTagsTrackingWidget.ui
            gui/FishbotLedColorWidget.ui)

include_directories(${CMAKE_SOURCE_DIR}/source/common)
include_directories(${CMAKE_SOURCE_DIR}/source/grabber)
include_directories(${CMAKE_SOURCE_DIR}/source/viewer)
include_directories(${CMAKE_SOURCE_DIR}/source/hub)
add_library(tracker SHARED ${srcs} ${hdrs} ${uis_hdrs} ${resources})
target_link_libraries(tracker grabber common viewer hub
                      Qt5::Core Qt5::Widgets ${OpenCV_LIBS} -fsanitize=address) # ${LIBTSAI_LIBRARY})

install(TARGETS tracker DESTINATION .)
install(FILES
        TrackerPointerTypes.hpp TrackingSetup.hpp TrackingDataManager.hpp
        #TrackingHandler.hpp
        DESTINATION include/tracker)
install(FILES settings/TrackingSettings.hpp settings/TrackingSetupSettings.hpp
        DESTINATION include/tracker/settings)
#install(FILES gui/TrackingRoutineWidget.hpp
#        DESTINATION include/tracker/gui)

