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

# make the list of files to compile
set(srcs
    InterSpeciesDataManager.cpp
    Subscriber.cpp
    GenericSubscriber.cpp
)

set(hdrs
    zmqHelpers.hpp
    settings/InterSpeciesSettings.cpp
)

include_directories(${CMAKE_SOURCE_DIR}/source/common)
include_directories(SYSTEM ${CMAKE_SOURCE_DIR}/source/messages)

add_library(inter-species SHARED ${inter-species_source_files} ${srcs} ${hdrs})
target_link_libraries(inter-species common messages Qt5::Core ${ZeroMQ_LIBRARY} ${PROTOBUF_LIBRARIES})

#install(TARGETS inter-species DESTINATION .)
#install(FILES
#         DESTINATION include/inter-species)
