set(srcs
    ViewerHandler.cpp
    ViewerData.cpp
    FrameConvertor.cpp
    gui/ViewerWidget.cpp
)

qt5_wrap_ui(uis_hdrs gui/ViewerWidget.ui)

include_directories(${CMAKE_SOURCE_DIR}/source/common)
add_library(viewer SHARED ${srcs} ${uis_hdrs})
target_link_libraries(viewer common Qt5::Core Qt5::Widgets ${OpenCV_LIBS})


