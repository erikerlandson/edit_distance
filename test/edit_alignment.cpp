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
    std::string s;
    BOOST_CHECK_EQUAL(edit_alignment("", "", std::back_inserter(s)).second, 0);
    BOOST_CHECK_EQUAL(s, "");
}

BOOST_AUTO_TEST_CASE(insertion) {
    std::string s;
    BOOST_CHECK_EQUAL(edit_alignment("", "a", std::back_inserter(s)).second, 1);
    BOOST_CHECK_EQUAL(s, "+");
    s = "";
    BOOST_CHECK_EQUAL(edit_alignment("", "ab", std::back_inserter(s)).second, 2);
    BOOST_CHECK_EQUAL(s, "++");
    s = "";
    BOOST_CHECK_EQUAL(edit_alignment("", "abc", std::back_inserter(s)).second, 3);
    BOOST_CHECK_EQUAL(s, "+++");
}

BOOST_AUTO_TEST_CASE(deletion) {
    std::string s;
    BOOST_CHECK_EQUAL(edit_alignment("a", "", std::back_inserter(s)).second, 1);
    BOOST_CHECK_EQUAL(s, "-");
    s = "";
    BOOST_CHECK_EQUAL(edit_alignment("ab", "", std::back_inserter(s)).second, 2);
    BOOST_CHECK_EQUAL(s, "--");
    s = "";
    BOOST_CHECK_EQUAL(edit_alignment("abc", "", std::back_inserter(s)).second, 3);
    BOOST_CHECK_EQUAL(s, "---");
}

BOOST_AUTO_TEST_CASE(substitution) {
    std::string s;
    BOOST_CHECK_EQUAL(edit_alignment("a", "x", std::back_inserter(s)).second, 1);
    BOOST_CHECK_EQUAL(s, ":");
    s = "";
    BOOST_CHECK_EQUAL(edit_alignment("ab", "xy", std::back_inserter(s)).second, 2);
    BOOST_CHECK_EQUAL(s, "::");
    s = "";
    BOOST_CHECK_EQUAL(edit_alignment("abc", "xyz", std::back_inserter(s)).second, 3);
    BOOST_CHECK_EQUAL(s, ":::");
}

BOOST_AUTO_TEST_CASE(substitution_equal) {
    std::string s;
    BOOST_CHECK_EQUAL(edit_alignment("a", "a", std::back_inserter(s)).second, 0);
    BOOST_CHECK_EQUAL(s, "=");
    s = "";
    BOOST_CHECK_EQUAL(edit_alignment("ab", "ab", std::back_inserter(s)).second, 0);
    BOOST_CHECK_EQUAL(s, "==");
    s = "";
    BOOST_CHECK_EQUAL(edit_alignment("abc", "abc", std::back_inserter(s)).second, 0);
    BOOST_CHECK_EQUAL(s, "===");
}

BOOST_AUTO_TEST_CASE(sequence_variations) {
    std::string s;
    BOOST_CHECK_EQUAL(edit_alignment("abc", "axc", std::back_inserter(s)).second, 1);
    BOOST_CHECK_EQUAL(s, "=:=");
    s = "";
    BOOST_CHECK_EQUAL(edit_alignment(ASSTRING("abc"), ASSTRING("axc"), std::back_inserter(s)).second, 1);
    BOOST_CHECK_EQUAL(s, "=:=");
    s = "";
    BOOST_CHECK_EQUAL(edit_alignment(ASLIST("abc"), ASLIST("axc"), std::back_inserter(s)).second, 1);
    BOOST_CHECK_EQUAL(s, "=:=");
    s = "";
    BOOST_CHECK_EQUAL(edit_alignment(ASVECTOR("abc"), ASVECTOR("axc"), std::back_inserter(s)).second, 1);
    BOOST_CHECK_EQUAL(s, "=:=");
}

BOOST_AUTO_TEST_CASE(mixed_sequences) {
    std::string s;
    BOOST_CHECK_EQUAL(edit_alignment("abc", "bcd", std::back_inserter(s)).second, 2);
    BOOST_CHECK_EQUAL(s, "-==+");

    s = "";
    BOOST_CHECK_EQUAL(edit_alignment("abc", ASSTRING("bcd"), std::back_inserter(s)).second, 2);
    BOOST_CHECK_EQUAL(s, "-==+");
    s = "";
    BOOST_CHECK_EQUAL(edit_alignment("abc", ASLIST("bcd"), std::back_inserter(s)).second, 2);
    BOOST_CHECK_EQUAL(s, "-==+");
    s = "";
    BOOST_CHECK_EQUAL(edit_alignment("abc", ASVECTOR("bcd"), std::back_inserter(s)).second, 2);
    BOOST_CHECK_EQUAL(s, "-==+");

    s = "";
    BOOST_CHECK_EQUAL(edit_alignment(ASSTRING("abc"), "bcd", std::back_inserter(s)).second, 2);
    BOOST_CHECK_EQUAL(s, "-==+");
    s = "";
    BOOST_CHECK_EQUAL(edit_alignment(ASLIST("abc"), "bcd", std::back_inserter(s)).second, 2);
    BOOST_CHECK_EQUAL(s, "-==+");
    s = "";
    BOOST_CHECK_EQUAL(edit_alignment(ASVECTOR("abc"), "bcd", std::back_inserter(s)).second, 2);
    BOOST_CHECK_EQUAL(s, "-==+");

    s = "";
    BOOST_CHECK_EQUAL(edit_alignment(ASSTRING("abc"), ASLIST("bcd"), std::back_inserter(s)).second, 2);
    BOOST_CHECK_EQUAL(s, "-==+");
    s = "";
    BOOST_CHECK_EQUAL(edit_alignment(ASVECTOR("abc"), ASLIST("bcd"), std::back_inserter(s)).second, 2);
    BOOST_CHECK_EQUAL(s, "-==+");
    s = "";
    BOOST_CHECK_EQUAL(edit_alignment(ASLIST("abc"), ASVECTOR("bcd"), std::back_inserter(s)).second, 2);
    BOOST_CHECK_EQUAL(s, "-==+");
}

BOOST_AUTO_TEST_CASE(range_adaptors) {
    std::string s;
    BOOST_CHECK_EQUAL(edit_alignment("abc", ASLIST("abc") | boost::adaptors::reversed, std::back_inserter(s)).second, 2);
    BOOST_CHECK_EQUAL(s, ":=:");
}

BOOST_AUTO_TEST_CASE(mixed_ops) {
    std::string s;
    BOOST_CHECK_EQUAL(edit_alignment("abcd", "bCde", std::back_inserter(s)).second, 3);
    BOOST_CHECK_EQUAL(s, "-=:=+");
}

BOOST_AUTO_TEST_CASE(custom_cost) {
    std::string s;
    BOOST_CHECK_EQUAL(edit_alignment("abcd", "bCde", std::back_inserter(s), cost_expensive_sub()).second, 4);
    BOOST_CHECK_EQUAL(s, "-=-+=+");
    s = "";
    BOOST_CHECK_EQUAL(edit_alignment("abcd", "aBCd", std::back_inserter(s), cost_expensive_sub()).second, 4);
    BOOST_CHECK_EQUAL(s, "=--++=");

    s = "";
    BOOST_CHECK_EQUAL(edit_alignment("aa", "axax", std::back_inserter(s), cost_expensive_ins()).second, 4);
    BOOST_CHECK_EQUAL(s, "=+=+");
    s = "";
    BOOST_CHECK_EQUAL(edit_alignment("axax", "aa", std::back_inserter(s), cost_expensive_ins()).second, 2);
    BOOST_CHECK_EQUAL(s, "=-=-");
}

BOOST_AUTO_TEST_SUITE_END()
