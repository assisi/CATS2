enable_testing(true)
set(CMAKE_INCLUDE_CURRENT_DIR ON)
include_directories(${CMAKE_SOURCE_DIR}/source/common)

add_executable(agent-state-test TestAgentState.cpp)
target_link_libraries(agent-state-test common Qt5::Test)

add_test(agent-state-test agent-state-test)
