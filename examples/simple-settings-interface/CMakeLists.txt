set(srcs
   main.cpp
)

include_directories(${CMAKE_SOURCE_DIR}/source/robot-control)
include_directories(${CMAKE_SOURCE_DIR}/source/settings-interface)
include_directories(${CMAKE_SOURCE_DIR}/source/common)
include_directories(${CMAKE_SOURCE_DIR}/source/inter-species)

add_executable(simple-settings-interface ${srcs})

target_link_libraries(simple-settings-interface common robot-control
                        settings-interface inter-species
                        Qt5::Core Qt5::Widgets Qt5::Gui Qt5::DBus)
