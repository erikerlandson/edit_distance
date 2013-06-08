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

using std::vector;
using std::list;

BOOST_AUTO_TEST_SUITE(edit_distance_suite)

BOOST_AUTO_TEST_CASE(both_empty) {
    BOOST_CHECK_EQUAL(edit_distance("", ""), 0);
}

BOOST_AUTO_TEST_CASE(one_empty) {
    BOOST_CHECK_EQUAL(edit_distance("", "abc"), 3);
    BOOST_CHECK_EQUAL(edit_distance("abc", ""), 3);
}

BOOST_AUTO_TEST_CASE(length_1) {
    // some boundary conditions for sequence length
    BOOST_CHECK_EQUAL(edit_distance("a", ""), 1);
    BOOST_CHECK_EQUAL(edit_distance("ab", "a"), 1);
    BOOST_CHECK_EQUAL(edit_distance("", "a"), 1);
    BOOST_CHECK_EQUAL(edit_distance("a", "ab"), 1);
}

BOOST_AUTO_TEST_CASE(equal_nonempty) {
    BOOST_CHECK_EQUAL(edit_distance("a", "a"), 0);
    BOOST_CHECK_EQUAL(edit_distance("ab", "ab"), 0);
    BOOST_CHECK_EQUAL(edit_distance("abc", "abc"), 0);
}

BOOST_AUTO_TEST_CASE(insertion) {
    // insertion occurs wrt seq2
    BOOST_CHECK_EQUAL(edit_distance("abc", "abcx"), 1);
    BOOST_CHECK_EQUAL(edit_distance("abc", "abxc"), 1);
    BOOST_CHECK_EQUAL(edit_distance("abc", "axbc"), 1);
    BOOST_CHECK_EQUAL(edit_distance("abc", "xabc"), 1);

    BOOST_CHECK_EQUAL(edit_distance("abc", "abcxx"), 2);
    BOOST_CHECK_EQUAL(edit_distance("abc", "abxxc"), 2);
    BOOST_CHECK_EQUAL(edit_distance("abc", "axxbc"), 2);
    BOOST_CHECK_EQUAL(edit_distance("abc", "xxabc"), 2);
    BOOST_CHECK_EQUAL(edit_distance("abc", "axbxc"), 2);
    BOOST_CHECK_EQUAL(edit_distance("abc", "xabcx"), 2);
}

BOOST_AUTO_TEST_CASE(deletion) {
    // deletion occurs wrt seq1
    BOOST_CHECK_EQUAL(edit_distance("abcx", "abc"), 1);
    BOOST_CHECK_EQUAL(edit_distance("abxc", "abc"), 1);
    BOOST_CHECK_EQUAL(edit_distance("axbc", "abc"), 1);
    BOOST_CHECK_EQUAL(edit_distance("xabc", "abc"), 1);

    BOOST_CHECK_EQUAL(edit_distance("abcxx", "abc"), 2);
    BOOST_CHECK_EQUAL(edit_distance("abxxc", "abc"), 2);
    BOOST_CHECK_EQUAL(edit_distance("axxbc", "abc"), 2);
    BOOST_CHECK_EQUAL(edit_distance("xxabc", "abc"), 2);
    BOOST_CHECK_EQUAL(edit_distance("axbxc", "abc"), 2);
    BOOST_CHECK_EQUAL(edit_distance("xabcx", "abc"), 2);
}

BOOST_AUTO_TEST_CASE(substitution) {
    BOOST_CHECK_EQUAL(edit_distance("abc", "axc"), 1);
    BOOST_CHECK_EQUAL(edit_distance("axc", "abc"), 1);
}

BOOST_AUTO_TEST_CASE(sequence_variations) {
    BOOST_CHECK_EQUAL(edit_distance("abc", "axc"), 1);
    BOOST_CHECK_EQUAL(edit_distance(ASSTRING("abc"), ASSTRING("axc")), 1);
    BOOST_CHECK_EQUAL(edit_distance(ASLIST("abc"), ASLIST("axc")), 1);
    BOOST_CHECK_EQUAL(edit_distance(ASVECTOR("abc"), ASVECTOR("axc")), 1);
}

BOOST_AUTO_TEST_CASE(mixed_sequences) {
    BOOST_CHECK_EQUAL(edit_distance("abc", "bcd"), 2);

    BOOST_CHECK_EQUAL(edit_distance("abc", ASSTRING("bcd")), 2);
    BOOST_CHECK_EQUAL(edit_distance("abc", ASLIST("bcd")), 2);
    BOOST_CHECK_EQUAL(edit_distance("abc", ASVECTOR("bcd")), 2);

    BOOST_CHECK_EQUAL(edit_distance(ASSTRING("abc"), "bcd"), 2);
    BOOST_CHECK_EQUAL(edit_distance(ASLIST("abc"), "bcd"), 2);
    BOOST_CHECK_EQUAL(edit_distance(ASVECTOR("abc"), "bcd"), 2);

    BOOST_CHECK_EQUAL(edit_distance(ASSTRING("abc"), ASLIST("bcd")), 2);
    BOOST_CHECK_EQUAL(edit_distance(ASVECTOR("abc"), ASLIST("bcd")), 2);
    BOOST_CHECK_EQUAL(edit_distance(ASLIST("abc"), ASVECTOR("bcd")), 2);
}

BOOST_AUTO_TEST_SUITE_END()
