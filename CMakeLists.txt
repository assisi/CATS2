project(CATS2)
cmake_minimum_required(VERSION 3.2.0)
enable_testing()

#set(CMAKE_POSITION_INDEPENDENT_CODE ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_VERBOSE_MAKEFILE ON)
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/modules")
set(ENV{CATS2_DIR} ${CMAKE_SOURCE_DIR})
# set the install prefix
if(CMAKE_BUILD_TYPE MATCHES Debug)
    set (CMAKE_INSTALL_PREFIX ${CMAKE_SOURCE_DIR}/bind)
else()
    set (CMAKE_INSTALL_PREFIX ${CMAKE_SOURCE_DIR}/bin)
endif()

find_package(Qt5 REQUIRED Core Widgets DBus Xml Test)
# SYSTEM is used to suppress GCC warnings in libraries headers
include_directories(SYSTEM ${QT_INCLUDE_DIR})

find_package(ReaderWriterQueue REQUIRED)
include_directories(SYSTEM ${READERWRITERQUEUE_INCLUDE_DIRS})

find_package(OpenCV 3 REQUIRED)
include_directories(SYSTEM ${OpenCV_INCLUDE_DIR})

find_package(LibXml2 2.6.0 REQUIRED)
include_directories(SYSTEM ${LIBXML2_INCLUDE_DIR})

find_package(GStreamer REQUIRED COMPONENTS base)
include_directories(SYSTEM ${GSTREAMER_INCLUDE_DIRS})

find_package(Qt5GStreamer REQUIRED)
include_directories(SYSTEM ${QTGSTREAMER_INCLUDES})

find_package(GLIB2 REQUIRED)
include_directories(SYSTEM ${GLIB2_INTERNAL_INCLUDE_DIR})
include_directories(SYSTEM ${GLIB2_MAIN_INCLUDE_DIR})

find_package(Protobuf REQUIRED)
include_directories(SYSTEM ${PROTOBUF_INCLUDE_DIRS})

find_package(Boost COMPONENTS program_options REQUIRED )
include_directories(SYSTEM ${Boost_INCLUDE_DIR} )

find_package(dashel REQUIRED)
include_directories(SYSTEM ${dashel_INCLUDE_DIRS})

find_package(Aseba REQUIRED)
include_directories(SYSTEM ${Aseba_INCLUDE_DIRS})

#add_definitions(${QTGSTREAMER_DEFINITIONS}) # defines QT_NO_KEYWORDS that demands replacing all "signals" by "Q_SIGNALS"
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${QTGSTREAMER_FLAGS}")
add_definitions(-DQT_MESSAGELOGCONTEXT)

if("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")
    # require at least gcc 4.4
    if (CMAKE_CXX_COMPILER_VERSION VERSION_LESS 4.4)
        message(FATAL_ERROR "GCC version must be at least 4.4")
    else()
        if (CMAKE_CXX_COMPILER_VERSION VERSION_GREATER 5.0)
            message("GCC version is later than 5.0")
            set(CATS2_CXX_FLAGS "-std=c++11 -Wall -Wextra -Wcast-align -Wcast-qual -Wdisabled-optimization -Wformat=2 \
            -Wlogical-op -Wmissing-declarations -Wmissing-include-dirs -Wnoexcept -Woverloaded-virtual \
            -Wredundant-decls -Wstrict-null-sentinel -Wstrict-overflow=5 -Wswitch-default \
            -Wno-unused-parameter -Wold-style-cast -Wfloat-equal -Wundef -Wsuggest-override -Wsuggest-final-methods")
        else()
            message("GCC version is earlier than 5.0")
            set(CATS2_CXX_FLAGS "-std=c++11 -Wall -Wextra -Wcast-align -Wcast-qual -Wdisabled-optimization -Wformat=2 \
            -Wlogical-op -Wmissing-declarations -Wmissing-include-dirs -Wnoexcept -Woverloaded-virtual \
            -Wredundant-decls -Wstrict-null-sentinel -Wstrict-overflow=5 -Wswitch-default \
            -Wno-unused-parameter -Wold-style-cast -Wfloat-equal -Wundef")
        endif()
    endif()
#elseif ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang")
#    # require at least clang 3.2
#    if (CMAKE_CXX_COMPILER_VERSION VERSION_LESS 3.2)
#        message(FATAL_ERROR "Clang version must be at least 3.2!")
#    endif()
else()
    message(WARNING "You are using an unsupported compiler! Compilation has only been tested with Clang and GCC.")
endif()

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
