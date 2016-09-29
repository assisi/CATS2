# run protobuf to generate messages
file(GLOB ProtoFiles RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} "definitions/*.proto")

file(MAKE_DIRECTORY "msg")
execute_process(COMMAND ${PROTOBUF_PROTOC_EXECUTABLE} --cpp_out "msg" -I "definitions" ${ProtoFiles}
                WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})

# make the list of files to compile
set(hdrs
    MessageTypes.hpp
)
file(GLOB messages_source_files "msg/*.h" "msg/*.cc")

add_library(messages SHARED ${messages_source_files} ${srcs} ${hdrs})
target_link_libraries(messages Qt5::Core ${PROTOBUF_LIBRARIES})

# set specific compile flags for this project, because otherwise the protobuf generated files trigger to many warnings
set_target_properties(messages PROPERTIES COMPILE_FLAGS "-Wno-suggest-override -Wno-float-equal -Wno-undef")
