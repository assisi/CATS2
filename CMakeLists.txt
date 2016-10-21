project(CATS2)
cmake_minimum_required(VERSION 3.2.0)
enable_testing()

#set(CMAKE_POSITION_INDEPENDENT_CODE ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_VERBOSE_MAKEFILE ON)
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/modules")
set(ENV{CATS2_DIR} ${CMAKE_SOURCE_DIR}) 

find_package(Qt5 REQUIRED Core Widgets DBus)
find_package(ReaderWriterQueue REQUIRED)
find_package(OpenCV 3 REQUIRED)
find_package(LibXml2 2.6.0 REQUIRED)
find_package(GStreamer REQUIRED COMPONENTS base)
find_package(Qt5GStreamer REQUIRED)
find_package(GLIB2 REQUIRED)
#find_package(Libtsai REQUIRED)
find_package(Protobuf REQUIRED)

# SYSTEM is used to suppress GCC warnings in libraries headers
include_directories(SYSTEM ${QT_INCLUDE_DIR})
include_directories(SYSTEM ${LIBXML2_INCLUDE_DIR})
include_directories(SYSTEM ${GSTREAMER_INCLUDE_DIRS})
include_directories(SYSTEM ${QTGSTREAMER_INCLUDES})
include_directories(SYSTEM ${GLIB2_INTERNAL_INCLUDE_DIR})
include_directories(SYSTEM ${GLIB2_MAIN_INCLUDE_DIR})
include_directories(SYSTEM ${OpenCV_INCLUDE_DIR})
include_directories(SYSTEM ${READERWRITERQUEUE_INCLUDE_DIRS})
#include_directories(SYSTEM ${LIBTSAI_INCLUDE_DIRS})
include_directories(SYSTEM ${PROTOBUF_INCLUDE_DIRS})

#add_definitions(${QTGSTREAMER_DEFINITIONS}) # defines QT_NO_KEYWORDS that demands replacing all "signals" by "Q_SIGNALS"
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${QTGSTREAMER_FLAGS}")

set(CATS2_CXX_FLAGS "-std=c++11 -Wall -Wextra -Wcast-align -Wcast-qual -Wdisabled-optimization -Wformat=2 \
-Wlogical-op -Wmissing-declarations -Wmissing-include-dirs -Wnoexcept -Woverloaded-virtual \
-Wredundant-decls -Wstrict-null-sentinel -Wstrict-overflow=5 -Wswitch-default \
-Wno-unused-parameter -Wold-style-cast  -Wsuggest-override \
-Wsuggest-final-methods -Wfloat-equal -Wundef -Wshadow")

if (CMAKE_COMPILER_IS_GNUCXX)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${CATS2_CXX_FLAGS}")
endif ()

if ("${CMAKE_CXX_COMPILER_ID}" MATCHES "Clang")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${CATS2_CXX_FLAGS} -Wnull-dereference")
elseif ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${CATS2_CXX_FLAGS}")
#elseif ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "MSVC")
endif()

add_subdirectory(source)
add_subdirectory(examples)
