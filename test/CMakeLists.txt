include_directories(${edit_distance_SOURCE_DIR}/include)

add_executable(test
               test.cpp
               edit_distance.cpp
               edit_alignment.cpp
              )
include_directories(${Boost_INCLUDE_DIR})
target_link_libraries(test ${Boost_LIBRARIES})
