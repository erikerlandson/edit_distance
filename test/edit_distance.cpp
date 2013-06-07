#include <boost/test/unit_test.hpp>

#include "edit_distance.h"
#include <vector>

using std::vector;

BOOST_AUTO_TEST_SUITE(edit_distance_suite)

BOOST_AUTO_TEST_CASE(empty_sequence) {
    vector<int> t;
    unsigned long d = edit_distance(t, t);
    BOOST_CHECK_EQUAL(d, 0);
}

BOOST_AUTO_TEST_SUITE_END()
