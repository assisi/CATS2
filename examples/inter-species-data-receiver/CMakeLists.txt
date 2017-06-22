# get zmq
find_package(PkgConfig)
if (PKG_CONFIG_FOUND)
    pkg_check_modules(ZeroMQ_PKGCONF libzmq)
endif (PKG_CONFIG_FOUND)
find_path(ZeroMQ_INCLUDE_DIR NAMES zmq.hpp PATHS ${ZEROMQ_ROOT}/include ${ZeroMQ_PKGCONF_INCLUDE_DIRS})
find_library(ZeroMQ_LIBRARY NAMES zmq PATHS ${ZEROMQ_ROOT}/lib ${ZeroMQ_PKGCONF_LIBRARY_DIRS})
if (NOT ZeroMQ_INCLUDE_DIR)
    message(FATAL_ERROR "ZMQ is not found")
endif()
include_directories(${ZeroMQ_INCLUDE_DIR})

set(srcs
    main.cpp
    MainWindow.cpp
)

qt5_wrap_ui(uis_hdrs MainWindow.ui)
#qt5_add_resources(resources icons.qrc)

include_directories(${CMAKE_SOURCE_DIR}/source/common)
include_directories(${CMAKE_SOURCE_DIR}/source/inter-species)
include_directories(${CMAKE_SOURCE_DIR}/source/messages)

add_executable(inter-species-data-receiver ${srcs} ${uis_hdrs})
target_link_libraries(inter-species-data-receiver common inter-species messages
                      Qt5::DBus Qt5::Core Qt5::Widgets Qt5::Gui
                      ${ZeroMQ_LIBRARY})
