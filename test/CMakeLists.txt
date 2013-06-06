# unit testing is built on the boost unit testing package
find_package(Boost 1.50 COMPONENTS unit_test_framework)

# include path
include_directories(${edit_distance_SOURCE_DIR}/include)

#if (Boost_FOUND)
#    add_executable(test
#                   test.cpp
#                   ut_src1.cpp
#                   ut_src2.cpp
#                  )
#    include_directories(${Boost_INCLUDE_DIR})
#    target_link_libraries(test ${Boost_LIBRARIES})
#endif ()
