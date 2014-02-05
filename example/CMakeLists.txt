include_directories(${edit_distance_SOURCE_DIR}/include)

macro(add_example name)
    add_executable(${name} ${name}.cpp)
    target_link_libraries(${name} ${Boost_LIBRARIES})
endmacro()

add_example(edit_distance_example)
add_example(edit_distance_cost_example)
add_example(edit_distance_range_example)
add_example(edit_script_example)
add_example(edit_script_cost_example)
add_example(edit_script_range_example)
add_example(edit_distance_diff_example)
