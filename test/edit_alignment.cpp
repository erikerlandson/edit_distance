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
    CHECK_EDIT_ALIGNMENT(edit_alignment, "", "", char, 0, "");
}

BOOST_AUTO_TEST_CASE(insertion) {
    CHECK_EDIT_ALIGNMENT(edit_alignment, "", "a", char, 1, "+");
    CHECK_EDIT_ALIGNMENT(edit_alignment, "", "aa", char, 2, "++");
    CHECK_EDIT_ALIGNMENT(edit_alignment, "", "aaa", char, 3, "+++");
}

BOOST_AUTO_TEST_CASE(deletion) {
    CHECK_EDIT_ALIGNMENT(edit_alignment, "a", "", char, 1, "-");
    CHECK_EDIT_ALIGNMENT(edit_alignment, "aa", "", char, 2, "--");
    CHECK_EDIT_ALIGNMENT(edit_alignment, "aaa", "", char, 3, "---");
}

BOOST_AUTO_TEST_CASE(substitution) {
    CHECK_EDIT_ALIGNMENT(edit_alignment, "a", "x", char, 1, ":");
    CHECK_EDIT_ALIGNMENT(edit_alignment, "ab", "xy", char, 2, "::");
    CHECK_EDIT_ALIGNMENT(edit_alignment, "abc", "xyz", char, 3, ":::");
}

BOOST_AUTO_TEST_CASE(substitution_equal) {
    CHECK_EDIT_ALIGNMENT(edit_alignment, "a", "a", char, 0, "=");
    CHECK_EDIT_ALIGNMENT(edit_alignment, "aa", "aa", char, 0, "==");
    CHECK_EDIT_ALIGNMENT(edit_alignment, "aaa", "aaa", char, 0, "===");
}

BOOST_AUTO_TEST_CASE(sequence_variations) {
    CHECK_EDIT_ALIGNMENT(edit_alignment, "abc", "axc", char, 1, "=:=");
    CHECK_EDIT_ALIGNMENT(edit_alignment, ASSTRING("abc"), ASSTRING("axc"), char, 1, "=:=");
    CHECK_EDIT_ALIGNMENT(edit_alignment, ASLIST("abc"), ASLIST("axc"), char, 1, "=:=");
    CHECK_EDIT_ALIGNMENT(edit_alignment, ASVECTOR("abc"), ASVECTOR("axc"), char, 1, "=:=");
}

BOOST_AUTO_TEST_CASE(mixed_sequences) {
    CHECK_EDIT_ALIGNMENT(edit_alignment, "abc", "bcd", char, 2, "-==+");

    CHECK_EDIT_ALIGNMENT(edit_alignment, "abc", ASSTRING("bcd"), char, 2, "-==+");
    CHECK_EDIT_ALIGNMENT(edit_alignment, "abc", ASLIST("bcd"), char, 2, "-==+");
    CHECK_EDIT_ALIGNMENT(edit_alignment, "abc", ASVECTOR("bcd"), char, 2, "-==+");

    CHECK_EDIT_ALIGNMENT(edit_alignment, ASSTRING("abc"), "bcd", char, 2, "-==+");
    CHECK_EDIT_ALIGNMENT(edit_alignment, ASLIST("abc"), "bcd", char, 2, "-==+");
    CHECK_EDIT_ALIGNMENT(edit_alignment, ASVECTOR("abc"), "bcd", char, 2, "-==+");

    CHECK_EDIT_ALIGNMENT(edit_alignment, ASSTRING("abc"), ASLIST("bcd"), char, 2, "-==+");
    CHECK_EDIT_ALIGNMENT(edit_alignment, ASVECTOR("abc"), ASLIST("bcd"), char, 2, "-==+");
    CHECK_EDIT_ALIGNMENT(edit_alignment, ASLIST("abc"), ASVECTOR("bcd"), char, 2, "-==+");
}

BOOST_AUTO_TEST_CASE(range_adaptors) {
    CHECK_EDIT_ALIGNMENT(edit_alignment, "abc", ASLIST("abc") | boost::adaptors::reversed, char, 2, ":=:");
}

BOOST_AUTO_TEST_CASE(mixed_ops) {
    CHECK_EDIT_ALIGNMENT(edit_alignment, "abcd", "bCde", char, 3, "-=:=+");
}

BOOST_AUTO_TEST_CASE(custom_cost) {
    CHECK_EDIT_ALIGNMENT_COST(edit_alignment, "abcd", "bCde", cost_expensive_sub(), char, 4, "-=-+=+");
    CHECK_EDIT_ALIGNMENT_COST(edit_alignment, "abcd", "aBCd", cost_expensive_sub(), char, 4, "=--++=");

    CHECK_EDIT_ALIGNMENT_COST(edit_alignment, "aa", "axax", cost_expensive_ins(), char, 4, "=+=+");
    CHECK_EDIT_ALIGNMENT_COST(edit_alignment, "axax", "aa", cost_expensive_ins(), char, 2, "=-=-");
}

BOOST_AUTO_TEST_CASE(acquire_costs) {
    typedef boost::tuple<char, int> val_t;
    CHECK_EDIT_ALIGNMENT(acquire<costs>(edit_alignment), "abc", "axc", val_t, 1, "(=,0)(:,1)(=,0)");
    CHECK_EDIT_ALIGNMENT(acquire<costs>(edit_alignment), "abc", "bcd", val_t, 2, "(-,1)(=,0)(=,0)(+,1)");
}

BOOST_AUTO_TEST_CASE(acquire_indexes) {
    typedef boost::tuple<char, int, int> val_t;
    CHECK_EDIT_ALIGNMENT(acquire<indexes>(edit_alignment), "abc", "axc", val_t, 1, "(=,0,0)(:,1,1)(=,2,2)");
    CHECK_EDIT_ALIGNMENT(acquire<indexes>(edit_alignment), "abcd", "bCde", val_t, 3, "(-,0,0)(=,1,0)(:,2,1)(=,3,2)(+,3,0)");
}

BOOST_AUTO_TEST_CASE(acquire_elements) {
    typedef boost::tuple<char, char, char> val_t;
    CHECK_EDIT_ALIGNMENT(acquire<elements>(edit_alignment), "abc", "axc", val_t, 1, "(=,a,a)(:,b,x)(=,c,c)");
    CHECK_EDIT_ALIGNMENT(acquire<elements>(edit_alignment), "abcd", "bCde", val_t, 3, "(-,a,@)(=,b,b)(:,c,C)(=,d,d)(+,e,@)");
}

BOOST_AUTO_TEST_CASE(acquire_costs_indexes) {
    typedef boost::tuple<char, int, int, int> val_t;
    CHECK_EDIT_ALIGNMENT(acquire<costs>(acquire<indexes>(edit_alignment)), "abc", "axc", val_t, 1, "(=,0,0,0)(:,1,1,1)(=,0,2,2)");
    CHECK_EDIT_ALIGNMENT(acquire<costs>(acquire<indexes>(edit_alignment)), "abcd", "bCde", val_t, 3, "(-,1,0,0)(=,0,1,0)(:,1,2,1)(=,0,3,2)(+,1,3,0)");

    CHECK_EDIT_ALIGNMENT(acquire<indexes>(acquire<costs>(edit_alignment)), "abc", "axc", val_t, 1, "(=,0,0,0)(:,1,1,1)(=,0,2,2)");
    CHECK_EDIT_ALIGNMENT(acquire<indexes>(acquire<costs>(edit_alignment)), "abcd", "bCde", val_t, 3, "(-,1,0,0)(=,0,1,0)(:,1,2,1)(=,0,3,2)(+,1,3,0)");
}

BOOST_AUTO_TEST_CASE(acquire_costs_elements) {
    typedef boost::tuple<char, int, char, char> val_t;
    CHECK_EDIT_ALIGNMENT(acquire<costs>(acquire<elements>(edit_alignment)), "abc", "axc", val_t, 1, "(=,0,a,a)(:,1,b,x)(=,0,c,c)");
    CHECK_EDIT_ALIGNMENT(acquire<costs>(acquire<elements>(edit_alignment)), "abcd", "bCde", val_t, 3, "(-,1,a,@)(=,0,b,b)(:,1,c,C)(=,0,d,d)(+,1,e,@)");

    CHECK_EDIT_ALIGNMENT(acquire<elements>(acquire<costs>(edit_alignment)), "abc", "axc", val_t, 1, "(=,0,a,a)(:,1,b,x)(=,0,c,c)");
    CHECK_EDIT_ALIGNMENT(acquire<elements>(acquire<costs>(edit_alignment)), "abcd", "bCde", val_t, 3, "(-,1,a,@)(=,0,b,b)(:,1,c,C)(=,0,d,d)(+,1,e,@)");
}

BOOST_AUTO_TEST_SUITE_END()
