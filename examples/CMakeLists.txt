include_directories(${edit_distance_SOURCE_DIR}/include)

macro(add_example name)
    add_executable(${name} ${name}.cpp)
    target_link_libraries(${name} ${Boost_LIBRARIES})
endmacro()

add_example(x01_edit_distance)
add_example(x02_edit_distance_cost)
add_example(x03_edit_distance_sequences)
add_example(x04_edit_alignment)
add_example(x05_edit_alignment_cost)
add_example(x06_edit_alignment_sequences)
