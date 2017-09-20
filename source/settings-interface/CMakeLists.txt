set(srcs
    Receiver.cpp
    SettingsInterface.cpp
)

set(hdrs
)

include_directories(${CMAKE_SOURCE_DIR}/source/grabber)
include_directories(${CMAKE_SOURCE_DIR}/source/viewer)
include_directories(${CMAKE_SOURCE_DIR}/source/common)
include_directories(${CMAKE_SOURCE_DIR}/source/hub)
include_directories(${CMAKE_SOURCE_DIR}/source/tracker)
include_directories(SYSTEM ${CMAKE_SOURCE_DIR}/source/messages)
include_directories(${CMAKE_SOURCE_DIR}/source/inter-species)
include_directories(${CMAKE_SOURCE_DIR}/source/robot-control)

add_library(settings-interface SHARED ${srcs} ${hdrs})

target_link_libraries(settings-interface common grabber tracker viewer hub
                        messages inter-species robot-control
                        Qt5::Core Qt5::Widgets Qt5::Gui Qt5::DBus
                        ${PROTOBUF_LIBRARIES} -fsanitize=address)

