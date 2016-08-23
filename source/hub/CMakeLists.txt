set(srcs
    Multiplicator.cpp
    QueueHub.cpp
)

set(hdrs
    HubPointerTypes.hpp
)

include_directories(${CMAKE_SOURCE_DIR}/source/common)
add_library(hub SHARED ${srcs} ${hdrs})
target_link_libraries(hub common
                      Qt5::Core Qt5::Widgets)


