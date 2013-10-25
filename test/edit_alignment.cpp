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
    CHECK_EDIT_ALIGNMENT("", "", 0, "");
}

BOOST_AUTO_TEST_CASE(insertion) {
    CHECK_EDIT_ALIGNMENT("", "a", 1, "(+ a)");
    CHECK_EDIT_ALIGNMENT("", "aa", 2, "(+ a)(+ a)");
    CHECK_EDIT_ALIGNMENT("", "aaa", 3, "(+ a)(+ a)(+ a)");
}

BOOST_AUTO_TEST_CASE(deletion) {
    CHECK_EDIT_ALIGNMENT("a", "", 1, "(- a)");
    CHECK_EDIT_ALIGNMENT("aa", "", 2, "(- a)(- a)");
    CHECK_EDIT_ALIGNMENT("aaa", "", 3, "(- a)(- a)(- a)");
}

BOOST_AUTO_TEST_CASE(substitution) {
    CHECK_EDIT_ALIGNMENT("a", "x", 1, "(a : x)");
    CHECK_EDIT_ALIGNMENT("ab", "xy", 2, "(a : x)(b : y)");
    CHECK_EDIT_ALIGNMENT("abc", "xyz", 3, "(a : x)(b : y)(c : z)");
}

BOOST_AUTO_TEST_CASE(substitution_equal) {
    CHECK_EDIT_ALIGNMENT("a", "a", 0, "(a = a)");
    CHECK_EDIT_ALIGNMENT("aa", "aa", 0, "(a = a)(a = a)");
    CHECK_EDIT_ALIGNMENT("aaa", "aaa", 0, "(a = a)(a = a)(a = a)");
}

BOOST_AUTO_TEST_CASE(sequence_variations) {
    CHECK_EDIT_ALIGNMENT("abc", "axc", 1, "(a = a)(b : x)(c = c)");
    CHECK_EDIT_ALIGNMENT(ASSTRING("abc"), ASSTRING("axc"), 1, "(a = a)(b : x)(c = c)");
    CHECK_EDIT_ALIGNMENT(ASLIST("abc"), ASLIST("axc"), 1, "(a = a)(b : x)(c = c)");
    CHECK_EDIT_ALIGNMENT(ASVECTOR("abc"), ASVECTOR("axc"), 1, "(a = a)(b : x)(c = c)");
}

BOOST_AUTO_TEST_CASE(mixed_sequences) {
    CHECK_EDIT_ALIGNMENT("abc", "bcd", 2, "(- a)(b = b)(c = c)(+ d)");

    CHECK_EDIT_ALIGNMENT("abc", ASSTRING("bcd"), 2, "(- a)(b = b)(c = c)(+ d)");
    CHECK_EDIT_ALIGNMENT("abc", ASLIST("bcd"), 2, "(- a)(b = b)(c = c)(+ d)");
    CHECK_EDIT_ALIGNMENT("abc", ASVECTOR("bcd"), 2, "(- a)(b = b)(c = c)(+ d)");

    CHECK_EDIT_ALIGNMENT(ASSTRING("abc"), "bcd", 2, "(- a)(b = b)(c = c)(+ d)");
    CHECK_EDIT_ALIGNMENT(ASLIST("abc"), "bcd", 2, "(- a)(b = b)(c = c)(+ d)");
    CHECK_EDIT_ALIGNMENT(ASVECTOR("abc"), "bcd", 2, "(- a)(b = b)(c = c)(+ d)");

    CHECK_EDIT_ALIGNMENT(ASSTRING("abc"), ASLIST("bcd"), 2, "(- a)(b = b)(c = c)(+ d)");
    CHECK_EDIT_ALIGNMENT(ASVECTOR("abc"), ASLIST("bcd"), 2, "(- a)(b = b)(c = c)(+ d)");
    CHECK_EDIT_ALIGNMENT(ASLIST("abc"), ASVECTOR("bcd"), 2, "(- a)(b = b)(c = c)(+ d)");
}

BOOST_AUTO_TEST_CASE(range_adaptors) {
    CHECK_EDIT_ALIGNMENT("abc", ASLIST("abc") | boost::adaptors::reversed, 2, "(a : c)(b = b)(c : a)");
}

BOOST_AUTO_TEST_CASE(mixed_ops) {
    CHECK_EDIT_ALIGNMENT("abcd", "bCde", 3, "(- a)(b = b)(c : C)(d = d)(+ e)");
}

#if 0
BOOST_AUTO_TEST_CASE(custom_cost) {
    CHECK_EDIT_ALIGNMENT_COST("abcd", "bCde", cost_expensive_sub, 4, "(- a 1)(= b b)(- c 1)(+ C 1)(= d d)(+ e 1)");
    CHECK_EDIT_ALIGNMENT_COST("abcd", "aBCd", cost_expensive_sub, 4, "(= a a)(- b 1)(- c 1)(+ B 1)(+ C 1)(= d d)");

        //CHECK_EDIT_ALIGNMENT_COST("aa", "axax", cost_expensive_ins(), 4, "=+=+");
        //CHECK_EDIT_ALIGNMENT_COST("axax", "aa", cost_expensive_ins(), 2, "=-=-");
}

BOOST_AUTO_TEST_CASE(acquire_costs) {
    typedef boost::tuple<char, int> val_t;
    CHECK_EDIT_ALIGNMENT(acquire<costs>(edit_alignment), "abc", "axc", val_t, 1, "(=,0)(:,1)(=,0)");
    CHECK_EDIT_ALIGNMENT(acquire<costs>(edit_alignment), "abc", "bcd", val_t, 2, "(-,1)(=,0)(=,0)(+,1)");
}

BOOST_AUTO_TEST_CASE(acquire_indexes) {
    typedef boost::tuple<char, int, int> val_t;
    CHECK_EDIT_ALIGNMENT(acquire<indexes>(edit_alignment), "abc", "axc", val_t, 1, "(=,0,0)(:,1,1)(=,2,2)");
    CHECK_EDIT_ALIGNMENT(acquire<indexes>(edit_alignment), "abcd", "bCde", val_t, 3, "(-,0,0)(=,1,0)(:,2,1)(=,3,2)(+,0,3)");
}

BOOST_AUTO_TEST_CASE(acquire_elements) {
    typedef boost::tuple<char> val_t;
    CHECK_EDIT_ALIGNMENT(acquire<elements>(edit_alignment), "abc", "axc", val_t, 1, "(=,a,a)(:,b,x)(=,c,c)");
    CHECK_EDIT_ALIGNMENT(acquire<elements>(edit_alignment), "abcd", "bCde", val_t, 3, "(-,a,@)(=,b,b)(:,c,C)(=,d,d)(+,@,e)");
}

BOOST_AUTO_TEST_CASE(acquire_costs_indexes) {
    typedef boost::tuple<char, int, int, int> val_t;
    CHECK_EDIT_ALIGNMENT(acquire<costs>(acquire<indexes>(edit_alignment)), "abc", "axc", val_t, 1, "(=,0,0,0)(:,1,1,1)(=,0,2,2)");
    CHECK_EDIT_ALIGNMENT(acquire<costs>(acquire<indexes>(edit_alignment)), "abcd", "bCde", val_t, 3, "(-,1,0,0)(=,0,1,0)(:,1,2,1)(=,0,3,2)(+,1,0,3)");

    CHECK_EDIT_ALIGNMENT(acquire<indexes>(acquire<costs>(edit_alignment)), "abc", "axc", val_t, 1, "(=,0,0,0)(:,1,1,1)(=,0,2,2)");
    CHECK_EDIT_ALIGNMENT(acquire<indexes>(acquire<costs>(edit_alignment)), "abcd", "bCde", val_t, 3, "(-,1,0,0)(=,0,1,0)(:,1,2,1)(=,0,3,2)(+,1,0,3)");
}

BOOST_AUTO_TEST_CASE(acquire_costs_elements) {
    typedef boost::tuple<char, int> val_t;
    CHECK_EDIT_ALIGNMENT(acquire<costs>(acquire<elements>(edit_alignment)), "abc", "axc", val_t, 1, "(=,0,a,a)(:,1,b,x)(=,0,c,c)");
    CHECK_EDIT_ALIGNMENT(acquire<costs>(acquire<elements>(edit_alignment)), "abcd", "bCde", val_t, 3, "(-,1,a,@)(=,0,b,b)(:,1,c,C)(=,0,d,d)(+,1,@,e)");

    CHECK_EDIT_ALIGNMENT(acquire<elements>(acquire<costs>(edit_alignment)), "abc", "axc", val_t, 1, "(=,0,a,a)(:,1,b,x)(=,0,c,c)");
    CHECK_EDIT_ALIGNMENT(acquire<elements>(acquire<costs>(edit_alignment)), "abcd", "bCde", val_t, 3, "(-,1,a,@)(=,0,b,b)(:,1,c,C)(=,0,d,d)(+,1,@,e)");
}

#endif

BOOST_AUTO_TEST_SUITE_END()
