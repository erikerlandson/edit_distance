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


BOOST_AUTO_TEST_CASE(beam_1) {
    // to find the equal run 'bcd', beam width has to be >= 1 
    CHECK_EDIT_ALIGNMENT("abcde", "bcdef", 2);
    CHECK_EDIT_ALIGNMENT_BEAM("abcde", "bcdef", 0, 5);
    CHECK_EDIT_ALIGNMENT_BEAM("abcde", "bcdef", 1, 2);

    // to find the equal run 'cd', beam width has to be >= 2
    CHECK_EDIT_ALIGNMENT("abcde", "cdefg", 4);
    CHECK_EDIT_ALIGNMENT_BEAM("abcde", "cdefg", 0, 5);
    CHECK_EDIT_ALIGNMENT_BEAM("abcde", "cdefg", 1, 5);
    CHECK_EDIT_ALIGNMENT_BEAM("abcde", "cdefg", 2, 4);

    // beam has to be >= 3 to discover the equal run 'abcd'
    CHECK_EDIT_ALIGNMENT("xxxabcd", "abcd", 3);
    CHECK_EDIT_ALIGNMENT_BEAM("xxxabcd", "abcd", 0, 7);
    CHECK_EDIT_ALIGNMENT_BEAM("xxxabcd", "abcd", 1, 7);
    CHECK_EDIT_ALIGNMENT_BEAM("xxxabcd", "abcd", 2, 7);
    CHECK_EDIT_ALIGNMENT_BEAM("xxxabcd", "abcd", 3, 3);

    CHECK_EDIT_ALIGNMENT("abcd", "xxxabcd", 3);
    CHECK_EDIT_ALIGNMENT_BEAM("abcd", "xxxabcd", 0, 7);
    CHECK_EDIT_ALIGNMENT_BEAM("abcd", "xxxabcd", 1, 7);
    CHECK_EDIT_ALIGNMENT_BEAM("abcd", "xxxabcd", 2, 7);
    CHECK_EDIT_ALIGNMENT_BEAM("abcd", "xxxabcd", 3, 3);

    // the equal run 'abcd' is at the beginning, and so always find-able
    CHECK_EDIT_ALIGNMENT("abcd", "abcdxxx", 3);
    CHECK_EDIT_ALIGNMENT_BEAM("abcd", "abcdxxx", 0, 3);
    CHECK_EDIT_ALIGNMENT_BEAM("abcd", "abcdxxx", 1, 3);
    CHECK_EDIT_ALIGNMENT_BEAM("abcd", "abcdxxx", 2, 3);
    CHECK_EDIT_ALIGNMENT_BEAM("abcd", "abcdxxx", 3, 3);

    CHECK_EDIT_ALIGNMENT("abcdxxx", "abcd", 3);
    CHECK_EDIT_ALIGNMENT_BEAM("abcdxxx", "abcd", 0, 3);
    CHECK_EDIT_ALIGNMENT_BEAM("abcdxxx", "abcd", 1, 3);
    CHECK_EDIT_ALIGNMENT_BEAM("abcdxxx", "abcd", 2, 3);
    CHECK_EDIT_ALIGNMENT_BEAM("abcdxxx", "abcd", 3, 3);
}


BOOST_AUTO_TEST_CASE(long_sequences) {
    CHECK_EDIT_ALIGNMENT("xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx", 
                         "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx", 
                         0);
    CHECK_EDIT_ALIGNMENT("xxxxxxxxxxxxxxxxxxxxxxxx*xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx", 
                         "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx*xxxxxxxxxxxxxxxxxxxxxxxxxx", 
                         2);
    CHECK_EDIT_ALIGNMENT("xxxxxxxxxxxxxxxxxxxxxxxx*xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx*xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx", 
                         "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx*xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx*xxxxxxxxxxxxxxxxxxxxxxxxxx", 
                         4);
    CHECK_EDIT_ALIGNMENT("xxxxxxxxxxxxxxxxxxxxxxxx**xxxxxxxxxxxxxxxxxxxxxxxxxxxxxx**xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx", 
                         "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx**xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx**xxxxxxxxxxxxxxxxxxxxxxxxx", 
                         8);
}

BOOST_AUTO_TEST_SUITE_END()
