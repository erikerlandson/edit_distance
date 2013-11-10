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

BOOST_AUTO_TEST_CASE(range_adaptors) {
    BOOST_CHECK_EQUAL(edit_distance("abc", ASLIST("abc") | boost::adaptors::reversed), 2);
}

BOOST_AUTO_TEST_CASE(custom_cost) {
    // make subsitution too expensive to use, so cheapest edit sequence
    // is to delete 'b' and insert 'x'
    BOOST_CHECK_EQUAL(edit_distance("abc", "axc", cost_expensive_sub()), 2);

    // insertion costs twice as much as deletion: an example of
    // an asymmetric cost function that causes edit distance to be
    // asymmetric
    BOOST_CHECK_EQUAL(edit_distance("aaaa", "aa", cost_expensive_ins()), 2);
    BOOST_CHECK_EQUAL(edit_distance("aa", "aaaa", cost_expensive_ins()), 4);
}


BOOST_AUTO_TEST_CASE(beam_1) {
    // to find the equal run 'bcd', beam width has to be >= 1 
    BOOST_CHECK_EQUAL(edit_distance("abcde", "bcdef"), 2);
    BOOST_CHECK_EQUAL(edit_distance("abcde", "bcdef", _beam=0), 5);
    BOOST_CHECK_EQUAL(edit_distance("abcde", "bcdef", _beam=1), 2);

    // to find the equal run 'cd', beam width has to be >= 2
    BOOST_CHECK_EQUAL(edit_distance("abcde", "cdefg"), 4);
    BOOST_CHECK_EQUAL(edit_distance("abcde", "cdefg", _beam=0), 5);
    BOOST_CHECK_EQUAL(edit_distance("abcde", "cdefg", _beam=1), 5);
    BOOST_CHECK_EQUAL(edit_distance("abcde", "cdefg", _beam=2), 4);

    // beam has to be >= 3 to discover the equal run 'abcd'
    BOOST_CHECK_EQUAL(edit_distance("xxxabcd", "abcd"), 3);
    BOOST_CHECK_EQUAL(edit_distance("xxxabcd", "abcd", _beam=0), 7);
    BOOST_CHECK_EQUAL(edit_distance("xxxabcd", "abcd", _beam=1), 7);
    BOOST_CHECK_EQUAL(edit_distance("xxxabcd", "abcd", _beam=2), 7);
    BOOST_CHECK_EQUAL(edit_distance("xxxabcd", "abcd", _beam=3), 3);

    BOOST_CHECK_EQUAL(edit_distance("abcd", "xxxabcd"), 3);
    BOOST_CHECK_EQUAL(edit_distance("abcd", "xxxabcd", _beam=0), 7);
    BOOST_CHECK_EQUAL(edit_distance("abcd", "xxxabcd", _beam=1), 7);
    BOOST_CHECK_EQUAL(edit_distance("abcd", "xxxabcd", _beam=2), 7);
    BOOST_CHECK_EQUAL(edit_distance("abcd", "xxxabcd", _beam=3), 3);

    // the equal run 'abcd' is at the beginning, and so always find-able
    BOOST_CHECK_EQUAL(edit_distance("abcd", "abcdxxx"), 3);
    BOOST_CHECK_EQUAL(edit_distance("abcd", "abcdxxx", _beam=0), 3);
    BOOST_CHECK_EQUAL(edit_distance("abcd", "abcdxxx", _beam=1), 3);
    BOOST_CHECK_EQUAL(edit_distance("abcd", "abcdxxx", _beam=2), 3);
    BOOST_CHECK_EQUAL(edit_distance("abcd", "abcdxxx", _beam=3), 3);

    BOOST_CHECK_EQUAL(edit_distance("abcdxxx", "abcd"), 3);
    BOOST_CHECK_EQUAL(edit_distance("abcdxxx", "abcd", _beam=0), 3);
    BOOST_CHECK_EQUAL(edit_distance("abcdxxx", "abcd", _beam=1), 3);
    BOOST_CHECK_EQUAL(edit_distance("abcdxxx", "abcd", _beam=2), 3);
    BOOST_CHECK_EQUAL(edit_distance("abcdxxx", "abcd", _beam=3), 3);
}

BOOST_AUTO_TEST_CASE(allow_sub_1) {
    BOOST_CHECK_EQUAL(edit_distance("abc", "xyz"), 3);

    // bool arg is run-time check
    BOOST_CHECK_EQUAL(edit_distance("abc", "xyz", _allow_sub=true), 3);
    BOOST_CHECK_EQUAL(edit_distance("abc", "xyz", _allow_sub=false), 6);

    // type arg gives compile-time value: check can be optimized out:
    BOOST_CHECK_EQUAL(edit_distance("abc", "xyz", _allow_sub=boost::true_type()), 3);
    BOOST_CHECK_EQUAL(edit_distance("abc", "xyz", _allow_sub=boost::false_type()), 6);

    BOOST_CHECK_EQUAL(edit_distance("aqc", "xqz", _allow_sub=boost::true_type()), 2);
    BOOST_CHECK_EQUAL(edit_distance("aqc", "xqz", _allow_sub=boost::false_type()), 4);

    BOOST_CHECK_EQUAL(edit_distance("aqcr", "xqzr", _allow_sub=boost::true_type()), 2);
    BOOST_CHECK_EQUAL(edit_distance("aqcr", "xqzr", _allow_sub=boost::false_type()), 4);

    BOOST_CHECK_EQUAL(edit_distance("raqc", "rxqz", _allow_sub=boost::true_type()), 2);
    BOOST_CHECK_EQUAL(edit_distance("raqc", "rxqz", _allow_sub=boost::false_type()), 4);
}


BOOST_AUTO_TEST_CASE(long_sequences) {
    BOOST_CHECK_EQUAL(edit_distance("xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx",
                                    "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx"),
                      0);
    BOOST_CHECK_EQUAL(edit_distance("xxxxxxxxxxxxxxxxxxxxxxxx*xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx",
                                    "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx*xxxxxxxxxxxxxxxxxxxxxxxxxx"),
                      2);
    BOOST_CHECK_EQUAL(edit_distance("xxxxxxxxxxxxxxxxxxxxxxxx*xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx*xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx",
                                    "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx*xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx*xxxxxxxxxxxxxxxxxxxxxxxxxx"),
                      4);
    BOOST_CHECK_EQUAL(edit_distance("xxxxxxxxxxxxxxxxxxxxxxxx**xxxxxxxxxxxxxxxxxxxxxxxxxxxxxx**xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx",
                                    "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx**xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx**xxxxxxxxxxxxxxxxxxxxxxxxx"),
                      8);
}


BOOST_AUTO_TEST_CASE(timing_1) {
    char data[] = "abcdefghij0123456789";
    const unsigned int data_size = sizeof(data)-1;
    srand(42);
    const unsigned int N = 100;
    const unsigned int LEN = 1000000;
    const unsigned int D = 10;
    const unsigned int R = LEN/D;
    const unsigned int K = 100;
    vector<std::string> seqdata(15);
    for (int i = 0;  i < seqdata.size();  ++i) {
        seqdata[i].resize(LEN, 'x');
        for (int d = 0;  d < D;  ++d) {
            unsigned int b1 = d*R + (rand() % K);
            unsigned int l1 =  rand() % K;
            for (unsigned int j = b1;  j < b1+l1;  ++j) seqdata[i][j] = data[rand()%data_size];
        }
    }
    int n = 0;
    double t0 = time(0);
    for (int i = 0;  i < seqdata.size();  ++i) {
        if (n > N) break;
        for (int j = 0;  j < i;  ++j) {
            if (++n > N) break;
            unsigned int d = edit_distance(seqdata[i], seqdata[j], _cost = cost_mixed_ops());
            BOOST_CHECK(d <= 2*LEN);
        }
    }
    n -= 1;
    double tt = time(0) - t0;
    BOOST_TEST_MESSAGE("time= " << tt << " sec   n= " << n << "   mean-time= " << tt/double(n) << "\n" );
}


BOOST_AUTO_TEST_CASE(timing_2) {
    char data[] = "abcdefghij0123456789";
    const unsigned int data_size = sizeof(data)-1;
    srand(42);
    const unsigned int N = 1000000;
    const unsigned int LEN = 100;
    const unsigned int D = 10;
    const unsigned int R = LEN/D;
    const unsigned int K = 2;
    vector<std::string> seqdata(1500);
    for (int i = 0;  i < seqdata.size();  ++i) {
        seqdata[i].resize(LEN, 'x');
        for (int d = 0;  d < D;  ++d) {
            unsigned int b1 = d*R + (rand() % K);
            unsigned int l1 =  rand() % K;
            for (unsigned int j = b1;  j < b1+l1;  ++j) seqdata[i][j] = data[rand()%data_size];
        }
    }
    int n = 0;
    double t0 = time(0);
    for (int i = 0;  i < seqdata.size();  ++i) {
        if (n > N) break;
        for (int j = 0;  j < i;  ++j) {
            if (++n > N) break;
            unsigned int d = edit_distance(seqdata[i], seqdata[j], _cost = cost_mixed_ops());
            BOOST_CHECK(d <= 2*LEN);
        }
    }
    n -= 1;
    double tt = time(0) - t0;
    BOOST_TEST_MESSAGE("time= " << tt << " sec   n= " << n << "   mean-time= " << tt/double(n) << "\n" );
}


BOOST_AUTO_TEST_CASE(timing_3) {
    char data[] = "abcdefghij0123456789";
    const unsigned int data_size = sizeof(data)-1;
    srand(42);
    const unsigned int N = 1000000;
    const unsigned int LEN = 10;
    const unsigned int D = 2;
    const unsigned int R = LEN/D;
    const unsigned int K = 2;
    vector<std::string> seqdata(1500);
    for (int i = 0;  i < seqdata.size();  ++i) {
        seqdata[i].resize(LEN, 'x');
        for (int d = 0;  d < D;  ++d) {
            unsigned int b1 = d*R + (rand() % K);
            unsigned int l1 =  rand() % K;
            for (unsigned int j = b1;  j < b1+l1;  ++j) seqdata[i][j] = data[rand()%data_size];
        }
    }
    int n = 0;
    double t0 = time(0);
    for (int i = 0;  i < seqdata.size();  ++i) {
        if (n > N) break;
        for (int j = 0;  j < i;  ++j) {
            if (++n > N) break;
            unsigned int d = edit_distance(seqdata[i], seqdata[j], _cost = cost_mixed_ops());
            BOOST_CHECK(d <= 2*LEN);
        }
    }
    n -= 1;
    double tt = time(0) - t0;
    BOOST_TEST_MESSAGE("time= " << tt << " sec   n= " << n << "   mean-time= " << tt/double(n) << "\n" );
}


BOOST_AUTO_TEST_SUITE_END()
