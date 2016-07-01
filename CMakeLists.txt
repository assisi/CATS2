project(video-grabber)
cmake_minimum_required(VERSION 3.0.0)
enable_testing()

#set(CMAKE_POSITION_INDEPENDENT_CODE ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_VERBOSE_MAKEFILE ON)
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/modules")

find_package(Qt5Core REQUIRED)
find_package(Qt5Widgets REQUIRED)
find_package(Qt5GStreamer REQUIRED)
find_package(Qt5GStreamer REQUIRED)
find_package(ReaderWriterQueue REQUIRED)
find_package(OpenCV 3 REQUIRED)
find_package(GStreamer 1.2.0 REQUIRED COMPONENTS base)
#find_package(GStreamerPluginsBase 1.2.0 COMPONENTS app audio video pbutils)
find_package(GLIB2 REQUIRED)

include_directories(${QT_INCLUDE_DIR})
include_directories(${QTGSTREAMER_INCLUDES})
include_directories(${GSTREAMER_INCLUDE_DIR})
include_directories(${GSTREAMER_CONFIG_INCLUDE_DIR})
include_directories(${GSTREAMER_PLUGIN_DIR})
include_directories(${GSTREAMER_BASE_INCLUDE_DIR})
include_directories(${GLIB2_INTERNAL_INCLUDE_DIR})
include_directories(${GLIB2_MAIN_INCLUDE_DIR})
include_directories(${OpenCV_INCLUDE_DIR})
include_directories(${READERWRITERQUEUE_INCLUDE_DIRS})

#add_definitions(${QTGSTREAMER_DEFINITIONS}) # defines QT_NO_KEYWORDS that demands replacing all "signals" by "Q_SIGNALS"
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${QTGSTREAMER_FLAGS}")


if (CMAKE_COMPILER_IS_GNUCXX)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -Wformat-security -Wundef -Wpointer-arith -fno-common")
endif ()

#message(${CMAKE_CXX_FLAGS})
#message(${QTGSTREAMER_DEFINITIONS})
#message(${OpenCV_INCLUDE_DIR})


add_subdirectory(source)
add_subdirectory(examples)
