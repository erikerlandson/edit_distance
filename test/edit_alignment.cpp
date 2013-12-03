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
    CHECK_EDIT_ALIGNMENT_ARG("abcd", "bCde", cost_expensive_sub(), 4);
    CHECK_EDIT_ALIGNMENT_ARG("abcd", "aBCd", cost_expensive_sub(), 4);

    CHECK_EDIT_ALIGNMENT_ARG("aa", "axax", cost_expensive_ins(), 4);
    CHECK_EDIT_ALIGNMENT_ARG("axax", "aa", cost_expensive_ins(), 2);

    CHECK_EDIT_ALIGNMENT_ARG("aa", "axax", _cost = cost_expensive_ins(), 4);
    CHECK_EDIT_ALIGNMENT_ARG("axax", "aa", _cost = cost_expensive_ins(), 2);
}


BOOST_AUTO_TEST_CASE(edit_beam_1) {
    // to find the equal run 'bcd', edit_beam width has to be >= 1 
    CHECK_EDIT_ALIGNMENT("abcde", "bcdef", 2);
    CHECK_EDIT_ALIGNMENT_ARG("abcde", "bcdef", _edit_beam = 0, 5);
    CHECK_EDIT_ALIGNMENT_ARG("abcde", "bcdef", _edit_beam = 1, 2);

    // to find the equal run 'cd', edit_beam width has to be >= 2
    CHECK_EDIT_ALIGNMENT("abcde", "cdefg", 4);
    CHECK_EDIT_ALIGNMENT_ARG("abcde", "cdefg", _edit_beam = 0, 5);
    CHECK_EDIT_ALIGNMENT_ARG("abcde", "cdefg", _edit_beam = 1, 5);
    CHECK_EDIT_ALIGNMENT_ARG("abcde", "cdefg", _edit_beam = 2, 4);

    // edit_beam has to be >= 3 to discover the equal run 'abcd'
    CHECK_EDIT_ALIGNMENT("xxxabcd", "abcd", 3);
    CHECK_EDIT_ALIGNMENT_ARG("xxxabcd", "abcd", _edit_beam = 0, 7);
    CHECK_EDIT_ALIGNMENT_ARG("xxxabcd", "abcd", _edit_beam = 1, 7);
    CHECK_EDIT_ALIGNMENT_ARG("xxxabcd", "abcd", _edit_beam = 2, 7);
    CHECK_EDIT_ALIGNMENT_ARG("xxxabcd", "abcd", _edit_beam = 3, 3);

    CHECK_EDIT_ALIGNMENT("abcd", "xxxabcd", 3);
    CHECK_EDIT_ALIGNMENT_ARG("abcd", "xxxabcd", _edit_beam = 0, 7);
    CHECK_EDIT_ALIGNMENT_ARG("abcd", "xxxabcd", _edit_beam = 1, 7);
    CHECK_EDIT_ALIGNMENT_ARG("abcd", "xxxabcd", _edit_beam = 2, 7);
    CHECK_EDIT_ALIGNMENT_ARG("abcd", "xxxabcd", _edit_beam = 3, 3);

    // the equal run 'abcd' is at the beginning, and so always find-able
    CHECK_EDIT_ALIGNMENT("abcd", "abcdxxx", 3);
    CHECK_EDIT_ALIGNMENT_ARG("abcd", "abcdxxx", _edit_beam = 0, 3);
    CHECK_EDIT_ALIGNMENT_ARG("abcd", "abcdxxx", _edit_beam = 1, 3);
    CHECK_EDIT_ALIGNMENT_ARG("abcd", "abcdxxx", _edit_beam = 2, 3);
    CHECK_EDIT_ALIGNMENT_ARG("abcd", "abcdxxx", _edit_beam = 3, 3);

    CHECK_EDIT_ALIGNMENT("abcdxxx", "abcd", 3);
    CHECK_EDIT_ALIGNMENT_ARG("abcdxxx", "abcd", _edit_beam = 0, 3);
    CHECK_EDIT_ALIGNMENT_ARG("abcdxxx", "abcd", _edit_beam = 1, 3);
    CHECK_EDIT_ALIGNMENT_ARG("abcdxxx", "abcd", _edit_beam = 2, 3);
    CHECK_EDIT_ALIGNMENT_ARG("abcdxxx", "abcd", _edit_beam = 3, 3);
}

BOOST_AUTO_TEST_CASE(myers_empty) {
    CHECK_EDIT_ALIGNMENT_ARG("", "", _allow_sub=boost::false_type(), 0);
}

BOOST_AUTO_TEST_CASE(myers_equal) {
    CHECK_EDIT_ALIGNMENT_ARG("a", "a", _allow_sub=boost::false_type(), 0);
    CHECK_EDIT_ALIGNMENT_ARG("aa", "aa", _allow_sub=boost::false_type(), 0);
    CHECK_EDIT_ALIGNMENT_ARG("aaa", "aaa", _allow_sub=boost::false_type(), 0);
}

BOOST_AUTO_TEST_CASE(myers_basis) {
    CHECK_EDIT_ALIGNMENT_ARG("", "a", _allow_sub=boost::false_type(), 1);
    CHECK_EDIT_ALIGNMENT_ARG("", "aa", _allow_sub=boost::false_type(), 2);
    CHECK_EDIT_ALIGNMENT_ARG("", "aaa", _allow_sub=boost::false_type(), 3);

    CHECK_EDIT_ALIGNMENT_ARG("a", "", _allow_sub=boost::false_type(), 1);
    CHECK_EDIT_ALIGNMENT_ARG("aa", "", _allow_sub=boost::false_type(), 2);
    CHECK_EDIT_ALIGNMENT_ARG("aaa", "", _allow_sub=boost::false_type(), 3);
}

BOOST_AUTO_TEST_CASE(myers_prefix_suffix) {
    CHECK_EDIT_ALIGNMENT_ARG("a", "aa", _allow_sub=boost::false_type(), 1);
    CHECK_EDIT_ALIGNMENT_ARG("a", "aaa", _allow_sub=boost::false_type(), 2);
    CHECK_EDIT_ALIGNMENT_ARG("a", "aaaa", _allow_sub=boost::false_type(), 3);
    CHECK_EDIT_ALIGNMENT_ARG("a", "aaaaa", _allow_sub=boost::false_type(), 4);

    CHECK_EDIT_ALIGNMENT_ARG("aa", "a", _allow_sub=boost::false_type(), 1);
    CHECK_EDIT_ALIGNMENT_ARG("aaa", "a", _allow_sub=boost::false_type(), 2);
    CHECK_EDIT_ALIGNMENT_ARG("aaaa", "a", _allow_sub=boost::false_type(), 3);
    CHECK_EDIT_ALIGNMENT_ARG("aaaaa", "a", _allow_sub=boost::false_type(), 4);
}

BOOST_AUTO_TEST_CASE(myers_no_equal) {
    CHECK_EDIT_ALIGNMENT_ARG("a", "x", _allow_sub=boost::false_type(), 2);
    CHECK_EDIT_ALIGNMENT_ARG("ab", "xy", _allow_sub=boost::false_type(), 4);
    CHECK_EDIT_ALIGNMENT_ARG("abc", "xyz", _allow_sub=boost::false_type(), 6);

    CHECK_EDIT_ALIGNMENT_ARG("a", "xy", _allow_sub=boost::false_type(), 3);
    CHECK_EDIT_ALIGNMENT_ARG("a", "xyz", _allow_sub=boost::false_type(), 4);

    CHECK_EDIT_ALIGNMENT_ARG("ab", "x", _allow_sub=boost::false_type(), 3);
    CHECK_EDIT_ALIGNMENT_ARG("abc", "x", _allow_sub=boost::false_type(), 4);
}

BOOST_AUTO_TEST_CASE(myers_equal_runs) {
    CHECK_EDIT_ALIGNMENT_ARG("aqc", "arc", _allow_sub=boost::false_type(), 2);
    CHECK_EDIT_ALIGNMENT_ARG("aqc", "xqz", _allow_sub=boost::false_type(), 4);

    CHECK_EDIT_ALIGNMENT_ARG("aqqc", "arrc", _allow_sub=boost::false_type(), 4);
    CHECK_EDIT_ALIGNMENT_ARG("aqqc", "xqqz", _allow_sub=boost::false_type(), 4);

    CHECK_EDIT_ALIGNMENT_ARG("ax", "abx", _allow_sub=boost::false_type(), 1);
    CHECK_EDIT_ALIGNMENT_ARG("abx", "ax", _allow_sub=boost::false_type(), 1);

    CHECK_EDIT_ALIGNMENT_ARG("ax", "abbx", _allow_sub=boost::false_type(), 2);
    CHECK_EDIT_ALIGNMENT_ARG("abx", "ax", _allow_sub=boost::false_type(), 1);
}

BOOST_AUTO_TEST_CASE(allow_sub_1) {
    CHECK_EDIT_ALIGNMENT("abc", "xyz", 3);

    CHECK_EDIT_ALIGNMENT_ARG("abc", "xyz", _allow_sub=true, 3);
    CHECK_EDIT_ALIGNMENT_ARG("abc", "xyz", _allow_sub=false, 6);

    CHECK_EDIT_ALIGNMENT_ARG("abc", "xyz", _allow_sub=boost::true_type(), 3);
    CHECK_EDIT_ALIGNMENT_ARG("abc", "xyz", _allow_sub=boost::false_type(), 6);

    CHECK_EDIT_ALIGNMENT_ARG("aqc", "xqz", _allow_sub=boost::true_type(), 2);
    CHECK_EDIT_ALIGNMENT_ARG("aqc", "xqz", _allow_sub=boost::false_type(), 4);

    CHECK_EDIT_ALIGNMENT_ARG("aqcr", "xqzr", _allow_sub=boost::true_type(), 2);
    CHECK_EDIT_ALIGNMENT_ARG("aqcr", "xqzr", _allow_sub=boost::false_type(), 4);

    CHECK_EDIT_ALIGNMENT_ARG("raqc", "rxqz", _allow_sub=boost::true_type(), 2);
    CHECK_EDIT_ALIGNMENT_ARG("raqc", "rxqz", _allow_sub=boost::false_type(), 4);
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


BOOST_AUTO_TEST_CASE(failure_1) {
    std::string seq1 = "xxxxxxxxx3d07a05d385h77xxxxxxxxxxxx";
    std::string seq2 = "xbd9a3d2gjf6b7a77hjcxxxxxxxxxxxxxxx";
    output_check_script_string out(seq1, seq2);
    unsigned int d = edit_alignment(seq1, seq2, _output = out, _cost = cost_mixed_ops());
    out.finalize();
    BOOST_CHECK_MESSAGE(out.correct, "\n\nseq1= '" << seq1 << "'\nseq2= '"<< seq2 <<"'\n\n");
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
            output_check_script_long_string out(seqdata[i], seqdata[j]);
            unsigned int d = edit_alignment(seqdata[i], seqdata[j], _output = out, _cost = cost_mixed_ops());
            out.finalize();
            BOOST_CHECK(out.correct);
            BOOST_CHECK(d <= 2*seqdata[i].size());
            if (++n >= N) break;
        }
    }
    double tt = time(0) - t0;
    BOOST_TEST_MESSAGE("time= " << tt << " sec   n= " << n << "   mean-time= " << tt/double(n) << "\n" );
}

BOOST_AUTO_TEST_CASE(crosscheck_1) {
    srand(time(0));
    vector<std::string> seqdata;
    const int N = 1000;
    random_localized_deviations(seqdata, N, 100, 2, 25);
    int n = 0;
    double t0 = time(0);
    for (int i = 0;  i < seqdata.size();  ++i) {
        if (n >= N) break;
        for (int j = 0;  j < i;  ++j) {
            output_check_script_long_string out(seqdata[i], seqdata[j]);
            unsigned int d1 = edit_alignment(seqdata[i], seqdata[j], _output = out, _cost = cost_mixed_ops());
            unsigned int d2 = edit_distance(seqdata[i], seqdata[j], _cost = cost_mixed_ops());
            out.finalize();
            // verify that the edit script is a correct one: it transforms seq1 into seq2
            BOOST_CHECK_MESSAGE(out.correct, "\n\nseq1= '" << seqdata[i] << "'\nseq2= '"<< seqdata[j] <<"'\n\n");
            // cross-check that edit_alignment() and edit_distance() compute the same distance
            BOOST_CHECK_MESSAGE(d1==d2, "\n\nseq1= '" << seqdata[i] << "'\nseq2= '"<< seqdata[j] <<"'\n\n");
            if (++n >= N) break;
        }
    }
    double tt = time(0) - t0;
    BOOST_TEST_MESSAGE("time= " << tt << " sec   n= " << n << "   mean-time= " << tt/double(n) << "\n" );
}


BOOST_AUTO_TEST_CASE(myers_sssp_crosscheck_1) {
    srand(time(0));
    vector<std::string> seqdata;
    const int N = 1000;
    random_localized_deviations(seqdata, N, 100, 2, 25);
    int n = 0;
    double t0 = time(0);
    for (int i = 0;  i < seqdata.size();  ++i) {
        if (n >= N) break;
        for (int j = 0;  j < i;  ++j) {
            output_check_script_long_string out1(seqdata[i], seqdata[j]);
            output_check_script_long_string out2(seqdata[i], seqdata[j]);
            // Myers algorithm
            unsigned int d1 = edit_alignment(seqdata[i], seqdata[j], out1, _allow_sub=boost::false_type());
            out1.finalize();
            // SSSP algorithm
            unsigned int d2 = edit_alignment(seqdata[i], seqdata[j], out2, _allow_sub=boost::false_type(), _cost = unit_cost_test());
            out2.finalize();
            // verify that the edit script is a correct one: it transforms seq1 into seq2
            BOOST_CHECK_MESSAGE(out1.correct, "\n\nseq1= '" << seqdata[i] << "'\nseq2= '"<< seqdata[j] <<"'\n\n");
            BOOST_CHECK_MESSAGE(out2.correct, "\n\nseq1= '" << seqdata[i] << "'\nseq2= '"<< seqdata[j] <<"'\n\n");
            // cross-check that edit_alignment() and edit_distance() compute the same distance
            BOOST_CHECK_MESSAGE(d1==d2, "\n\nseq1= '" << seqdata[i] << "'\nseq2= '"<< seqdata[j] <<"'\n\n");
            if (++n >= N) break;
        }
    }
    double tt = time(0) - t0;
    BOOST_TEST_MESSAGE("time= " << tt << " sec   n= " << n << "   mean-time= " << tt/double(n) << "\n" );
}


BOOST_AUTO_TEST_CASE(myers_dist_path_crosscheck_1) {
    srand(time(0));
    vector<std::string> seqdata;
    const int N = 100000;
    random_localized_deviations(seqdata, N, 100, 5, 10);
    int n = 0;
    double t0 = time(0);
    for (int i = 0;  i < seqdata.size();  ++i) {
        if (n >= N) break;
        for (int j = 0;  j < i;  ++j) {
            BOOST_TEST_CHECKPOINT("n= " << n << "   i= " << i << "   j= " << j);
            output_check_script_long_string out(seqdata[i], seqdata[j]);
            unsigned int d2 = edit_distance(seqdata[i], seqdata[j], _allow_sub=boost::false_type());
                //BOOST_TEST_CHECKPOINT("aaa");
            unsigned int d1 = edit_alignment(seqdata[i], seqdata[j], _output = out, _allow_sub=boost::false_type());
                //BOOST_TEST_CHECKPOINT("bbb");
            out.finalize();
            // verify that the edit script is a correct one: it transforms seq1 into seq2
            BOOST_CHECK_MESSAGE(out.correct, "\n\nseq1= '" << seqdata[i] << "'\nseq2= '"<< seqdata[j] <<"'\n\n");
            // cross-check that edit_alignment() and edit_distance() compute the same distance
            BOOST_CHECK_MESSAGE(d1==d2, "\n\nseq1= '" << seqdata[i] << "'\nseq2= '"<< seqdata[j] <<"'\n\n");
            if (++n >= N) break;
        }
    }
    double tt = time(0) - t0;
    BOOST_TEST_MESSAGE("time= " << tt << " sec   n= " << n << "   mean-time= " << tt/double(n) << "\n");
}



BOOST_AUTO_TEST_SUITE_END()
