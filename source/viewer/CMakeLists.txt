set(srcs
    ViewerHandler.cpp
    ViewerData.cpp
    FrameConvertor.cpp
    gui/ViewerWidget.cpp
    gui/FrameScene.cpp
    gui/ViewerWindow.cpp
)

set(hdrs
    ViewerPointerTypes.hpp
)

qt5_wrap_ui(uis_hdrs gui/ViewerWidget.ui gui/ViewerWindow.ui)
qt5_add_resources(resources icons.qrc)

include_directories(${CMAKE_SOURCE_DIR}/source/common)
add_library(viewer SHARED ${srcs} ${hdrs} ${uis_hdrs} ${resources})
target_link_libraries(viewer common Qt5::Core Qt5::Widgets ${OpenCV_LIBS})


