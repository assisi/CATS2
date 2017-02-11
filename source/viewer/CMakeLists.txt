set(srcs
    ViewerHandler.cpp
    ViewerData.cpp
    FrameConvertor.cpp
    settings/ViewerSettings.cpp
    gui/ViewerWidget.cpp
    gui/FrameScene.cpp
    gui/ViewerWindow.cpp
    gui/AgentTextItem.cpp
    gui/AgentItem.cpp
    gui/AnnotatedPolygonItem.cpp
    gui/TrajectoryItem.cpp
    gui/TargetItem.cpp
)

set(hdrs
    ViewerPointerTypes.hpp
    gui/ColoredItem.hpp
)

qt5_wrap_ui(uis_hdrs gui/ViewerWidget.ui gui/ViewerWindow.ui)
qt5_add_resources(resources icons.qrc)

include_directories(${CMAKE_SOURCE_DIR}/source/common)
add_library(viewer SHARED ${srcs} ${hdrs} ${uis_hdrs} ${resources})
target_link_libraries(viewer common Qt5::Core Qt5::Widgets ${OpenCV_LIBS})

install(TARGETS viewer DESTINATION .)
install(FILES
        ViewerPointerTypes.hpp ViewerHandler.hpp ViewerData.hpp
        DESTINATION include/viewer)
install(FILES settings/ViewerSettings.hpp
        DESTINATION include/viewer/settings)
install(FILES gui/ViewerWidget.hpp
        DESTINATION include/viewer/gui)

