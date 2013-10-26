/*******
edit_distance: STL and Boost compatible edit distance functions for C++

Copyright (c) 2013 Erik Erlandson

Author:  Erik Erlandson <erikerlandson@yahoo.com>

Distributed under the Boost Software License, Version 1.0.
See accompanying file LICENSE or copy at
http://www.boost.org/LICENSE_1_0.txt
*******/

#include <boost/test/unit_test.hpp>

#include "ut_common.h"

BOOST_AUTO_TEST_SUITE(edit_alignment_suite)

BOOST_AUTO_TEST_CASE(both_empty) {
    CHECK_EDIT_ALIGNMENT("", "", 0);
}

BOOST_AUTO_TEST_CASE(insertion) {
    CHECK_EDIT_ALIGNMENT("", "a", 1);
    CHECK_EDIT_ALIGNMENT("", "aa", 2);
    CHECK_EDIT_ALIGNMENT("", "aaa", 3);
}

BOOST_AUTO_TEST_CASE(deletion) {
    CHECK_EDIT_ALIGNMENT("a", "", 1);
    CHECK_EDIT_ALIGNMENT("aa", "", 2);
    CHECK_EDIT_ALIGNMENT("aaa", "", 3);
}

BOOST_AUTO_TEST_CASE(substitution) {
    CHECK_EDIT_ALIGNMENT("a", "x", 1);
    CHECK_EDIT_ALIGNMENT("ab", "xy", 2);
    CHECK_EDIT_ALIGNMENT("abc", "xyz", 3);
}

BOOST_AUTO_TEST_CASE(substitution_equal) {
    CHECK_EDIT_ALIGNMENT("a", "a", 0);
    CHECK_EDIT_ALIGNMENT("aa", "aa", 0);
    CHECK_EDIT_ALIGNMENT("aaa", "aaa", 0);
}

BOOST_AUTO_TEST_CASE(sequence_variations) {
    CHECK_EDIT_ALIGNMENT("abc", "axc", 1);
    CHECK_EDIT_ALIGNMENT(ASSTRING("abc"), ASSTRING("axc"), 1);
    CHECK_EDIT_ALIGNMENT(ASLIST("abc"), ASLIST("axc"), 1);
    CHECK_EDIT_ALIGNMENT(ASVECTOR("abc"), ASVECTOR("axc"), 1);
}

BOOST_AUTO_TEST_CASE(mixed_sequences) {
    CHECK_EDIT_ALIGNMENT("abc", "bcd", 2);

    CHECK_EDIT_ALIGNMENT("abc", ASSTRING("bcd"), 2);
    CHECK_EDIT_ALIGNMENT("abc", ASLIST("bcd"), 2);
    CHECK_EDIT_ALIGNMENT("abc", ASVECTOR("bcd"), 2);

    CHECK_EDIT_ALIGNMENT(ASSTRING("abc"), "bcd", 2);
    CHECK_EDIT_ALIGNMENT(ASLIST("abc"), "bcd", 2);
    CHECK_EDIT_ALIGNMENT(ASVECTOR("abc"), "bcd", 2);

    CHECK_EDIT_ALIGNMENT(ASSTRING("abc"), ASLIST("bcd"), 2);
    CHECK_EDIT_ALIGNMENT(ASVECTOR("abc"), ASLIST("bcd"), 2);
    CHECK_EDIT_ALIGNMENT(ASLIST("abc"), ASVECTOR("bcd"), 2);
}

BOOST_AUTO_TEST_CASE(range_adaptors) {
    CHECK_EDIT_ALIGNMENT("abc", ASLIST("abc") | boost::adaptors::reversed, 2);
}

BOOST_AUTO_TEST_CASE(mixed_ops) {
    CHECK_EDIT_ALIGNMENT("abcd", "bCde", 3);
}

BOOST_AUTO_TEST_CASE(custom_cost) {
    CHECK_EDIT_ALIGNMENT_COST("abcd", "bCde", cost_expensive_sub, 4);
    CHECK_EDIT_ALIGNMENT_COST("abcd", "aBCd", cost_expensive_sub, 4);

    CHECK_EDIT_ALIGNMENT_COST("aa", "axax", cost_expensive_ins, 4);
    CHECK_EDIT_ALIGNMENT_COST("axax", "aa", cost_expensive_ins, 2);
}

BOOST_AUTO_TEST_SUITE_END()
