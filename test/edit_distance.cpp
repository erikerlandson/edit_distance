/*******
edit_distance: STL and Boost compatible edit distance functions for C++

Copyright (c) 2013 Erik Erlandson

Author:  Erik Erlandson <erikerlandson@yahoo.com>

Distributed under the Boost Software License, Version 1.0.
See accompanying file LICENSE or copy at
http://www.boost.org/LICENSE_1_0.txt
*******/

#include <boost/test/unit_test.hpp>
#include <boost/test/floating_point_comparison.hpp>

#include "ut_common.h"

BOOST_AUTO_TEST_SUITE(edit_distance_suite)

BOOST_AUTO_TEST_CASE(both_empty) {
    BOOST_CHECK_EQUAL(edit_distance("", "", _allow_sub=true_type()), 0);
}

BOOST_AUTO_TEST_CASE(one_empty) {
    BOOST_CHECK_EQUAL(edit_distance("", "abc", _allow_sub=true_type()), 3);
    BOOST_CHECK_EQUAL(edit_distance("abc", "", _allow_sub=true_type()), 3);
}

BOOST_AUTO_TEST_CASE(length_1) {
    // some boundary conditions for sequence length
    BOOST_CHECK_EQUAL(edit_distance("a", "", _allow_sub=true_type()), 1);
    BOOST_CHECK_EQUAL(edit_distance("ab", "a", _allow_sub=true_type()), 1);
    BOOST_CHECK_EQUAL(edit_distance("", "a", _allow_sub=true_type()), 1);
    BOOST_CHECK_EQUAL(edit_distance("a", "ab", _allow_sub=true_type()), 1);
}

BOOST_AUTO_TEST_CASE(equal_nonempty) {
    BOOST_CHECK_EQUAL(edit_distance("a", "a", _allow_sub=true_type()), 0);
    BOOST_CHECK_EQUAL(edit_distance("ab", "ab", _allow_sub=true_type()), 0);
    BOOST_CHECK_EQUAL(edit_distance("abc", "abc", _allow_sub=true_type()), 0);
}

BOOST_AUTO_TEST_CASE(insertion) {
    // insertion occurs wrt seq2
    BOOST_CHECK_EQUAL(edit_distance("abc", "abcx", _allow_sub=true_type()), 1);
    BOOST_CHECK_EQUAL(edit_distance("abc", "abxc", _allow_sub=true_type()), 1);
    BOOST_CHECK_EQUAL(edit_distance("abc", "axbc", _allow_sub=true_type()), 1);
    BOOST_CHECK_EQUAL(edit_distance("abc", "xabc", _allow_sub=true_type()), 1);

    BOOST_CHECK_EQUAL(edit_distance("abc", "abcxx", _allow_sub=true_type()), 2);
    BOOST_CHECK_EQUAL(edit_distance("abc", "abxxc", _allow_sub=true_type()), 2);
    BOOST_CHECK_EQUAL(edit_distance("abc", "axxbc", _allow_sub=true_type()), 2);
    BOOST_CHECK_EQUAL(edit_distance("abc", "xxabc", _allow_sub=true_type()), 2);
    BOOST_CHECK_EQUAL(edit_distance("abc", "axbxc", _allow_sub=true_type()), 2);
    BOOST_CHECK_EQUAL(edit_distance("abc", "xabcx", _allow_sub=true_type()), 2);
}

BOOST_AUTO_TEST_CASE(deletion) {
    // deletion occurs wrt seq1
    BOOST_CHECK_EQUAL(edit_distance("abcx", "abc", _allow_sub=true_type()), 1);
    BOOST_CHECK_EQUAL(edit_distance("abxc", "abc", _allow_sub=true_type()), 1);
    BOOST_CHECK_EQUAL(edit_distance("axbc", "abc", _allow_sub=true_type()), 1);
    BOOST_CHECK_EQUAL(edit_distance("xabc", "abc", _allow_sub=true_type()), 1);

    BOOST_CHECK_EQUAL(edit_distance("abcxx", "abc", _allow_sub=true_type()), 2);
    BOOST_CHECK_EQUAL(edit_distance("abxxc", "abc", _allow_sub=true_type()), 2);
    BOOST_CHECK_EQUAL(edit_distance("axxbc", "abc", _allow_sub=true_type()), 2);
    BOOST_CHECK_EQUAL(edit_distance("xxabc", "abc", _allow_sub=true_type()), 2);
    BOOST_CHECK_EQUAL(edit_distance("axbxc", "abc", _allow_sub=true_type()), 2);
    BOOST_CHECK_EQUAL(edit_distance("xabcx", "abc", _allow_sub=true_type()), 2);
}

BOOST_AUTO_TEST_CASE(substitution) {
    BOOST_CHECK_EQUAL(edit_distance("abc", "axc", _allow_sub=true_type()), 1);
    BOOST_CHECK_EQUAL(edit_distance("axc", "abc", _allow_sub=true_type()), 1);
}

BOOST_AUTO_TEST_CASE(sequence_variations) {
    BOOST_CHECK_EQUAL(edit_distance("abc", "axc", _allow_sub=true_type()), 1);
    BOOST_CHECK_EQUAL(edit_distance(ASSTRING("abc"), ASSTRING("axc"), _allow_sub=true_type()), 1);
    BOOST_CHECK_EQUAL(edit_distance(ASLIST("abc"), ASLIST("axc"), _allow_sub=true_type()), 1);
    BOOST_CHECK_EQUAL(edit_distance(ASVECTOR("abc"), ASVECTOR("axc"), _allow_sub=true_type()), 1);
}

BOOST_AUTO_TEST_CASE(mixed_sequences) {
    BOOST_CHECK_EQUAL(edit_distance("abc", "bcd", _allow_sub=true_type()), 2);

    BOOST_CHECK_EQUAL(edit_distance("abc", ASSTRING("bcd"), _allow_sub=true_type()), 2);
    BOOST_CHECK_EQUAL(edit_distance("abc", ASLIST("bcd"), _allow_sub=true_type()), 2);
    BOOST_CHECK_EQUAL(edit_distance("abc", ASVECTOR("bcd"), _allow_sub=true_type()), 2);

    BOOST_CHECK_EQUAL(edit_distance(ASSTRING("abc"), "bcd", _allow_sub=true_type()), 2);
    BOOST_CHECK_EQUAL(edit_distance(ASLIST("abc"), "bcd", _allow_sub=true_type()), 2);
    BOOST_CHECK_EQUAL(edit_distance(ASVECTOR("abc"), "bcd", _allow_sub=true_type()), 2);

    BOOST_CHECK_EQUAL(edit_distance(ASSTRING("abc"), ASLIST("bcd"), _allow_sub=true_type()), 2);
    BOOST_CHECK_EQUAL(edit_distance(ASVECTOR("abc"), ASLIST("bcd"), _allow_sub=true_type()), 2);
    BOOST_CHECK_EQUAL(edit_distance(ASLIST("abc"), ASVECTOR("bcd"), _allow_sub=true_type()), 2);
}

BOOST_AUTO_TEST_CASE(range_adaptors) {
    BOOST_CHECK_EQUAL(edit_distance("abc", ASLIST("abc") | boost::adaptors::reversed, _allow_sub=true_type()), 2);
}

BOOST_AUTO_TEST_CASE(custom_cost) {
    // make subsitution too expensive to use, so cheapest edit sequence
    // is to delete 'b' and insert 'x'
    BOOST_CHECK_EQUAL(edit_distance("abc", "axc", cost_expensive_sub(), _allow_sub=true_type()), 2);

    // insertion costs twice as much as deletion: an example of
    // an asymmetric cost function that causes edit distance to be
    // asymmetric
    BOOST_CHECK_EQUAL(edit_distance("aaaa", "aa", cost_expensive_ins(), _allow_sub=true_type()), 2);
    BOOST_CHECK_EQUAL(edit_distance("aa", "aaaa", cost_expensive_ins(), _allow_sub=true_type()), 4);
}

BOOST_AUTO_TEST_CASE(undefined_sub) {
    // verify that cost_sub() method can be undefined when substitution is compile-time disabled:
    BOOST_CHECK_EQUAL(edit_distance("abc", "axc", _cost=undef_sub_cost()), 2);
}

BOOST_AUTO_TEST_CASE(custom_equal) {
    BOOST_CHECK_EQUAL(edit_distance("abc", "aBc"), 2);
    BOOST_CHECK_EQUAL(edit_distance("abc", "aBc", _equal=case_equal()), 0);

    BOOST_CHECK_EQUAL(edit_distance("abc", "aBc", _allow_sub=true_type()), 1);
    BOOST_CHECK_EQUAL(edit_distance("abc", "aBc", _allow_sub=true_type(), _equal=case_equal()), 0);
}

BOOST_AUTO_TEST_CASE(edit_beam_1) {
    // to find the equal run 'bcd', edit_beam width has to be >= 1 
    BOOST_CHECK_EQUAL(edit_distance("abcde", "bcdef", _allow_sub=true_type()), 2);
    BOOST_CHECK_EQUAL(edit_distance("abcde", "bcdef", _edit_beam=0, _allow_sub=true_type()), 5);
    BOOST_CHECK_EQUAL(edit_distance("abcde", "bcdef", _edit_beam=1, _allow_sub=true_type()), 2);

    // to find the equal run 'cd', edit_beam width has to be >= 2
    BOOST_CHECK_EQUAL(edit_distance("abcde", "cdefg", _allow_sub=true_type()), 4);
    BOOST_CHECK_EQUAL(edit_distance("abcde", "cdefg", _edit_beam=0, _allow_sub=true_type()), 5);
    BOOST_CHECK_EQUAL(edit_distance("abcde", "cdefg", _edit_beam=1, _allow_sub=true_type()), 5);
    BOOST_CHECK_EQUAL(edit_distance("abcde", "cdefg", _edit_beam=2, _allow_sub=true_type()), 4);

    // edit_beam has to be >= 3 to discover the equal run 'abcd'
    BOOST_CHECK_EQUAL(edit_distance("xxxabcd", "abcd", _allow_sub=true_type()), 3);
    BOOST_CHECK_EQUAL(edit_distance("xxxabcd", "abcd", _edit_beam=0, _allow_sub=true_type()), 7);
    BOOST_CHECK_EQUAL(edit_distance("xxxabcd", "abcd", _edit_beam=1, _allow_sub=true_type()), 7);
    BOOST_CHECK_EQUAL(edit_distance("xxxabcd", "abcd", _edit_beam=2, _allow_sub=true_type()), 7);
    BOOST_CHECK_EQUAL(edit_distance("xxxabcd", "abcd", _edit_beam=3, _allow_sub=true_type()), 3);

    BOOST_CHECK_EQUAL(edit_distance("abcd", "xxxabcd", _allow_sub=true_type()), 3);
    BOOST_CHECK_EQUAL(edit_distance("abcd", "xxxabcd", _edit_beam=0, _allow_sub=true_type()), 7);
    BOOST_CHECK_EQUAL(edit_distance("abcd", "xxxabcd", _edit_beam=1, _allow_sub=true_type()), 7);
    BOOST_CHECK_EQUAL(edit_distance("abcd", "xxxabcd", _edit_beam=2, _allow_sub=true_type()), 7);
    BOOST_CHECK_EQUAL(edit_distance("abcd", "xxxabcd", _edit_beam=3, _allow_sub=true_type()), 3);

    // the equal run 'abcd' is at the beginning, and so always find-able
    BOOST_CHECK_EQUAL(edit_distance("abcd", "abcdxxx", _allow_sub=true_type()), 3);
    BOOST_CHECK_EQUAL(edit_distance("abcd", "abcdxxx", _edit_beam=0, _allow_sub=true_type()), 3);
    BOOST_CHECK_EQUAL(edit_distance("abcd", "abcdxxx", _edit_beam=1, _allow_sub=true_type()), 3);
    BOOST_CHECK_EQUAL(edit_distance("abcd", "abcdxxx", _edit_beam=2, _allow_sub=true_type()), 3);
    BOOST_CHECK_EQUAL(edit_distance("abcd", "abcdxxx", _edit_beam=3, _allow_sub=true_type()), 3);

    BOOST_CHECK_EQUAL(edit_distance("abcdxxx", "abcd", _allow_sub=true_type()), 3);
    BOOST_CHECK_EQUAL(edit_distance("abcdxxx", "abcd", _edit_beam=0, _allow_sub=true_type()), 3);
    BOOST_CHECK_EQUAL(edit_distance("abcdxxx", "abcd", _edit_beam=1, _allow_sub=true_type()), 3);
    BOOST_CHECK_EQUAL(edit_distance("abcdxxx", "abcd", _edit_beam=2, _allow_sub=true_type()), 3);
    BOOST_CHECK_EQUAL(edit_distance("abcdxxx", "abcd", _edit_beam=3, _allow_sub=true_type()), 3);
}

BOOST_AUTO_TEST_CASE(allow_sub_1) {
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


BOOST_AUTO_TEST_CASE(max_cost_1) {
    srand(time(0));
    vector<std::string> seqdata;
    const int N = 10000;
    random_localized_deviations(seqdata, N, 100, 5, 10);
    int n = 0;
    double t0 = time(0);
    for (int i = 0;  i < seqdata.size();  ++i) {
        if (n >= N) break;
        for (int j = 0;  j < i;  ++j) {
            unsigned int d = edit_distance(seqdata[i], seqdata[j]);
            if (d < 2) continue;

            unsigned int dub = seqdata[i].size()+seqdata[j].size();
            unsigned int dt;

            dt = edit_distance(seqdata[i], seqdata[j], _max_cost=d-2);
            BOOST_CHECK_GT(dt, d-2);
            BOOST_CHECK_GE(dt, d);
            BOOST_CHECK_LE(dt, dub);

            dt = edit_distance(seqdata[i], seqdata[j], _max_cost=d-1);
            BOOST_CHECK_GT(dt, d-1);
            BOOST_CHECK_GE(dt, d);
            BOOST_CHECK_LE(dt, dub);
            
            dt = edit_distance(seqdata[i], seqdata[j], _max_cost=d);
            BOOST_CHECK_EQUAL(dt, d);

            dt = edit_distance(seqdata[i], seqdata[j], _max_cost=d+1);
            BOOST_CHECK_EQUAL(dt, d);

            dt = edit_distance(seqdata[i], seqdata[j], _max_cost=d+2);
            BOOST_CHECK_EQUAL(dt, d);

            BOOST_CHECK_THROW(edit_distance(seqdata[i], seqdata[j], _max_cost=d-1, _max_cost_exception=true), max_edit_cost_exception);

            if (++n >= N) break;
        }
    }
    double tt = time(0) - t0;
    BOOST_TEST_MESSAGE("time= " << tt << " sec   n= " << n << "   mean-time= " << tt/double(n) << "\n");
}


BOOST_AUTO_TEST_CASE(max_cost_2) {
    srand(time(0));
    vector<std::string> seqdata;
    const int N = 200;
    random_localized_deviations(seqdata, N, 100, 5, 10);
    int n = 0;
    double t0 = time(0);
    for (int i = 0;  i < seqdata.size();  ++i) {
        if (n >= N) break;
        for (int j = 0;  j < i;  ++j) {
            unsigned int d = edit_distance(seqdata[i], seqdata[j], _allow_sub=true_type());
            if (d < 2) continue;

            unsigned int dub = std::max(seqdata[i].size(), seqdata[j].size());
            unsigned int dt;

            dt = edit_distance(seqdata[i], seqdata[j], _allow_sub=true_type(), _max_cost=d-2);
            BOOST_CHECK_GT(dt, d-2);
            BOOST_CHECK_GE(dt, d);
            BOOST_CHECK_LE(dt, dub);

            dt = edit_distance(seqdata[i], seqdata[j], _allow_sub=true_type(), _max_cost=d-1);
            BOOST_CHECK_GT(dt, d-1);
            BOOST_CHECK_GE(dt, d);
            BOOST_CHECK_LE(dt, dub);
            
            dt = edit_distance(seqdata[i], seqdata[j], _allow_sub=true_type(), _max_cost=d);
            BOOST_CHECK_EQUAL(dt, d);

            dt = edit_distance(seqdata[i], seqdata[j], _allow_sub=true_type(), _max_cost=d+1);
            BOOST_CHECK_EQUAL(dt, d);

            dt = edit_distance(seqdata[i], seqdata[j], _allow_sub=true_type(), _max_cost=d+2);
            BOOST_CHECK_EQUAL(dt, d);

            BOOST_CHECK_THROW(edit_distance(seqdata[i], seqdata[j], _allow_sub=true_type(), _max_cost=d-1, _max_cost_exception=true), max_edit_cost_exception);

            if (++n >= N) break;
        }
    }
    double tt = time(0) - t0;
    BOOST_TEST_MESSAGE("time= " << tt << " sec   n= " << n << "   mean-time= " << tt/double(n) << "\n");
}


BOOST_AUTO_TEST_CASE(long_sequences) {
    BOOST_CHECK_EQUAL(edit_distance("xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx",
                                    "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx", _allow_sub=true_type()),
                      0);
    BOOST_CHECK_EQUAL(edit_distance("xxxxxxxxxxxxxxxxxxxxxxxx*xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx",
                                    "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx*xxxxxxxxxxxxxxxxxxxxxxxxxx", _allow_sub=true_type()),
                      2);
    BOOST_CHECK_EQUAL(edit_distance("xxxxxxxxxxxxxxxxxxxxxxxx*xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx*xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx",
                                    "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx*xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx*xxxxxxxxxxxxxxxxxxxxxxxxxx", _allow_sub=true_type()),
                      4);
    BOOST_CHECK_EQUAL(edit_distance("xxxxxxxxxxxxxxxxxxxxxxxx**xxxxxxxxxxxxxxxxxxxxxxxxxxxxxx**xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx",
                                    "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx**xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx**xxxxxxxxxxxxxxxxxxxxxxxxx", _allow_sub=true_type()),
                      8);
}


BOOST_AUTO_TEST_CASE(cost_beam_crosscheck_1) {
    srand(time(0));
    vector<std::string> seqdata;
    const int N = 100;
    const int beam = 2;
    random_localized_deviations(seqdata, N, 100000, 5, 50);
    int n = 0;
    double t0 = time(0);
    double sum = 0;
    double sumdiff = 0;
    for (int i = 0;  i < seqdata.size();  ++i) {
        if (n >= N) break;
        for (int j = 0;  j < i;  ++j) {
            unsigned int d1 = edit_distance(seqdata[i], seqdata[j], _allow_sub=true_type());
            unsigned int d2 = edit_distance(seqdata[i], seqdata[j], _allow_sub=true_type(), _cost_beam=beam);
            BOOST_CHECK_LE(d1, d2);
            sum += d1;
            sumdiff += d2 - d1;
            if (++n >= N) break;
        }
    }
    double tt = time(0) - t0;
    BOOST_TEST_MESSAGE("time= " << tt << " sec   n= " << n << "   mean-time= " << tt/double(n));
    BOOST_TEST_MESSAGE("sum= " << sum << "   sumdiff= " << sumdiff << "   ratio= " << sumdiff/sum);
    BOOST_CHECK_LE(sumdiff/sum, 0.01);

    n = 0;
    t0 = time(0);
    for (int i = 0;  i < seqdata.size();  ++i) {
        if (n >= N) break;
        for (int j = 0;  j < i;  ++j) {
            unsigned int d2 = edit_distance(seqdata[i], seqdata[j], _allow_sub=true_type(), _cost_beam=beam);
            if (++n >= N) break;
        }
    }
    double t2 = time(0) - t0;
    BOOST_TEST_MESSAGE("time= " << t2 << " sec   n= " << n << "   mean-time= " << t2/double(n) << "\n");
    BOOST_CHECK_LT(t2, tt);
}


BOOST_AUTO_TEST_CASE(myers_sssp_crosscheck_1) {
    srand(time(0));
    vector<std::string> seqdata;
    const int N = 100;
    random_localized_deviations(seqdata, N, 100000, 5, 50);
    int n = 0;
    double t0 = time(0);
    for (int i = 0;  i < seqdata.size();  ++i) {
        if (n >= N) break;
        for (int j = 0;  j < i;  ++j) {
            unsigned int d1 = edit_distance(seqdata[i], seqdata[j], _allow_sub=boost::false_type());
            unsigned int d2 = edit_distance(seqdata[i], seqdata[j], _allow_sub=boost::false_type(), _cost=unit_cost_test());
            BOOST_CHECK_EQUAL(d1, d2);
            if (++n >= N) break;
        }
    }
    double tt = time(0) - t0;
    BOOST_TEST_MESSAGE("time= " << tt << " sec   n= " << n << "   mean-time= " << tt/double(n) << "\n");
}


BOOST_AUTO_TEST_CASE(myers_sssp_crosscheck_2) {
    srand(time(0));
    vector<std::string> seqdata;
    const int N = 4000;
    random_localized_deviations(seqdata, N, 100, 5, 10);
    int n = 0;
    double t0 = time(0);
    for (int i = 0;  i < seqdata.size();  ++i) {
        if (n >= N) break;
        for (int j = 0;  j < i;  ++j) {
            unsigned int d1 = edit_distance(seqdata[i], seqdata[j], _allow_sub=boost::false_type());
            unsigned int d2 = edit_distance(seqdata[i], seqdata[j], _allow_sub=boost::false_type(), _cost=unit_cost_test());
            BOOST_CHECK_MESSAGE(d1==d2, "\n\nseq1= '" << seqdata[i] << "'\nseq2= '"<< seqdata[j]);
            BOOST_CHECK_EQUAL(d1, d2);
            if (++n >= N) break;
        }
    }
    double tt = time(0) - t0;
    BOOST_TEST_MESSAGE("time= " << tt << " sec   n= " << n << "   mean-time= " << tt/double(n) << "\n");
}


BOOST_AUTO_TEST_CASE(timing_1) {
    srand(time(0));
    vector<std::string> seqdata;
    const int N = 100;
    random_localized_deviations(seqdata, N, 100000, 5, 20);
    int n = 0;
    double t0 = time(0);
    for (int i = 0;  i < seqdata.size();  ++i) {
        if (n >= N) break;
        for (int j = 0;  j < i;  ++j) {
            unsigned int d = edit_distance(seqdata[i], seqdata[j], _allow_sub=true_type(), _cost_beam=2);
            BOOST_CHECK(d <= 2 * seqdata[i].size());
            if (++n >= N) break;
        }
    }
    double tt = time(0) - t0;
    BOOST_TEST_MESSAGE("time= " << tt << " sec   n= " << n << "   mean-time= " << tt/double(n) << "\n" );
}


BOOST_AUTO_TEST_CASE(timing_2) {
    srand(time(0));
    vector<std::string> seqdata;
    const int N = 100000;
    random_localized_deviations(seqdata, N, 100, 2, 5);
    int n = 0;
    double t0 = time(0);
    for (int i = 0;  i < seqdata.size();  ++i) {
        if (n >= N) break;
        for (int j = 0;  j < i;  ++j) {
            unsigned int d = edit_distance(seqdata[i], seqdata[j], _allow_sub=true_type(), _cost_beam=2);
            BOOST_CHECK(d <= 2 * seqdata[i].size());
            if (++n >= N) break;
        }
    }
    double tt = time(0) - t0;
    BOOST_TEST_MESSAGE("time= " << tt << " sec   n= " << n << "   mean-time= " << tt/double(n) << "\n" );
}


BOOST_AUTO_TEST_CASE(timing_3) {
    srand(time(0));
    vector<std::string> seqdata;
    const int N = 1000000;
    random_localized_deviations(seqdata, N, 10, 2, 2);
    int n = 0;
    double t0 = time(0);
    for (int i = 0;  i < seqdata.size();  ++i) {
        if (n >= N) break;
        for (int j = 0;  j < i;  ++j) {
            unsigned int d = edit_distance(seqdata[i], seqdata[j], _allow_sub=true_type(), _cost_beam=2);
            BOOST_CHECK(d <= 2 * seqdata[i].size());
            if (++n >= N) break;
        }
    }
    double tt = time(0) - t0;
    BOOST_TEST_MESSAGE("time= " << tt << " sec   n= " << n << "   mean-time= " << tt/double(n) << "\n" );
}




BOOST_AUTO_TEST_SUITE_END()
