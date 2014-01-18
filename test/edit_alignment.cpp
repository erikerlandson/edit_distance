/*******
edit_distance: STL and Boost compatible edit distance functions for C++

Copyright (c) 2013 Erik Erlandson

Author:  Erik Erlandson <erikerlandson@yahoo.com>

Distributed under the Boost Software License, Version 1.0.
See accompanying file LICENSE or copy at
http://www.boost.org/LICENSE_1_0.txt
*******/

#include <boost/test/unit_test.hpp>

#include "edit_distance_ut.hpp"

BOOST_AUTO_TEST_SUITE(edit_alignment_suite)

BOOST_AUTO_TEST_CASE(both_empty) {
    CHECK_EDIT_ALIGNMENT_ARG("", "", _substitution=true_type(), 0);
}

BOOST_AUTO_TEST_CASE(insertion) {
    CHECK_EDIT_ALIGNMENT_ARG("", "a", _substitution=true_type(), 1);
    CHECK_EDIT_ALIGNMENT_ARG("", "aa", _substitution=true_type(), 2);
    CHECK_EDIT_ALIGNMENT_ARG("", "aaa", _substitution=true_type(), 3);
}

BOOST_AUTO_TEST_CASE(deletion) {
    CHECK_EDIT_ALIGNMENT_ARG("a", "", _substitution=true_type(), 1);
    CHECK_EDIT_ALIGNMENT_ARG("aa", "", _substitution=true_type(), 2);
    CHECK_EDIT_ALIGNMENT_ARG("aaa", "", _substitution=true_type(), 3);
}

BOOST_AUTO_TEST_CASE(substitution) {
    CHECK_EDIT_ALIGNMENT_ARG("a", "x", _substitution=true_type(), 1);
    CHECK_EDIT_ALIGNMENT_ARG("ab", "xy", _substitution=true_type(), 2);
    CHECK_EDIT_ALIGNMENT_ARG("abc", "xyz", _substitution=true_type(), 3);
}

BOOST_AUTO_TEST_CASE(substitution_equal) {
    CHECK_EDIT_ALIGNMENT_ARG("a", "a", _substitution=true_type(), 0);
    CHECK_EDIT_ALIGNMENT_ARG("aa", "aa", _substitution=true_type(), 0);
    CHECK_EDIT_ALIGNMENT_ARG("aaa", "aaa", _substitution=true_type(), 0);
}

BOOST_AUTO_TEST_CASE(sequence_variations) {
    CHECK_EDIT_ALIGNMENT_ARG("abc", "axc", _substitution=true_type(), 1);
    CHECK_EDIT_ALIGNMENT_ARG(ASSTRING("abc"), ASSTRING("axc"), _substitution=true_type(), 1);
    CHECK_EDIT_ALIGNMENT_ARG(ASLIST("abc"), ASLIST("axc"), _substitution=true_type(), 1);
    CHECK_EDIT_ALIGNMENT_ARG(ASVECTOR("abc"), ASVECTOR("axc"), _substitution=true_type(), 1);
}

BOOST_AUTO_TEST_CASE(mixed_sequences) {
    CHECK_EDIT_ALIGNMENT_ARG("abc", "bcd", _substitution=true_type(), 2);

    CHECK_EDIT_ALIGNMENT_ARG("abc", ASSTRING("bcd"), _substitution=true_type(), 2);
    CHECK_EDIT_ALIGNMENT_ARG("abc", ASLIST("bcd"), _substitution=true_type(), 2);
    CHECK_EDIT_ALIGNMENT_ARG("abc", ASVECTOR("bcd"), _substitution=true_type(), 2);

    CHECK_EDIT_ALIGNMENT_ARG(ASSTRING("abc"), "bcd", _substitution=true_type(), 2);
    CHECK_EDIT_ALIGNMENT_ARG(ASLIST("abc"), "bcd", _substitution=true_type(), 2);
    CHECK_EDIT_ALIGNMENT_ARG(ASVECTOR("abc"), "bcd", _substitution=true_type(), 2);

    CHECK_EDIT_ALIGNMENT_ARG(ASSTRING("abc"), ASLIST("bcd"), _substitution=true_type(), 2);
    CHECK_EDIT_ALIGNMENT_ARG(ASVECTOR("abc"), ASLIST("bcd"), _substitution=true_type(), 2);
    CHECK_EDIT_ALIGNMENT_ARG(ASLIST("abc"), ASVECTOR("bcd"), _substitution=true_type(), 2);
}

BOOST_AUTO_TEST_CASE(range_adaptors) {
    CHECK_EDIT_ALIGNMENT_ARG("abc", ASLIST("abc") | boost::adaptors::reversed, _substitution=true_type(), 2);
}

BOOST_AUTO_TEST_CASE(mixed_ops) {
    CHECK_EDIT_ALIGNMENT_ARG("abcd", "bCde", _substitution=true_type(), 3);
}

BOOST_AUTO_TEST_CASE(custom_cost) {
    CHECK_EDIT_ALIGNMENT_2ARG("abcd", "bCde", _substitution=true_type(), _cost = cost_expensive_sub(), 4);
    CHECK_EDIT_ALIGNMENT_2ARG("abcd", "aBCd", _substitution=true_type(), _cost = cost_expensive_sub(), 4);

    CHECK_EDIT_ALIGNMENT_2ARG("aa", "axax", _substitution=true_type(), _cost = cost_expensive_ins(), 4);
    CHECK_EDIT_ALIGNMENT_2ARG("axax", "aa", _substitution=true_type(), _cost = cost_expensive_ins(), 2);

    CHECK_EDIT_ALIGNMENT_2ARG("aa", "axax", _substitution=true_type(), _cost = cost_expensive_ins(), 4);
    CHECK_EDIT_ALIGNMENT_2ARG("axax", "aa", _substitution=true_type(), _cost = cost_expensive_ins(), 2);
}

BOOST_AUTO_TEST_CASE(undefined_sub) {
    // verify that substitution() and substitution() can be undefined when substitution is compile-time disabled:
    const std::string seq1 = "abc";
    const std::string seq2 = "axc";

    {
        // test sssp implementation
        undef_sub_output<char, unsigned> ob(ASVECTOR(seq1), ASVECTOR(seq2));
        BOOST_CHECK_EQUAL(edit_distance(seq1, seq2, _script=ob, _cost=undef_sub_cost()), 2);
        ob.finalize(2);
        BOOST_CHECK(ob.correct);
    }

    {
        // test invoking myers specialization
        undef_sub_output<char, unsigned> ob(ASVECTOR(seq1), ASVECTOR(seq2));
        BOOST_CHECK_EQUAL(edit_distance(seq1, seq2, _script=ob), 2);
        ob.finalize(2);
        BOOST_CHECK(ob.correct);
    }
}


BOOST_AUTO_TEST_CASE(custom_equal) {
    {
        const std::string seq1 = "abc";
        const std::string seq2 = "aBc";
        const unsigned dist = 2;
        output_check_script<char, unsigned> ob(ASVECTOR(seq1), ASVECTOR(seq2));
        unsigned d = edit_distance(seq1, seq2, _script=ob);
        ob.finalize(dist);
        BOOST_CHECK_MESSAGE(ob.correct, "incorrect edit script: '" << ob.ss.str() << "'  seq1='" << seq1 << "'  seq2='" << seq2 << "'");
        BOOST_CHECK_MESSAGE(d == dist, "incorrect edit distance " << d << "(expected " << dist << ")  seq1='" << seq1 << "' seq2='" << seq2 << "'  script='" << ob.ss.str() <<"'");
    }
    {
        const std::string seq1 = "abc";
        const std::string seq2 = "aBc";
        const unsigned dist = 0;
        output_check_script<char, unsigned, case_equal> ob(ASVECTOR(seq1), ASVECTOR(seq2));
        unsigned d = edit_distance(seq1, seq2, _script=ob, _equal=case_equal());
        ob.finalize(dist);
        BOOST_CHECK_MESSAGE(ob.correct, "incorrect edit script: '" << ob.ss.str() << "'  seq1='" << seq1 << "'  seq2='" << seq2 << "'");
        BOOST_CHECK_MESSAGE(d == dist, "incorrect edit distance " << d << "(expected " << dist << ")  seq1='" << seq1 << "' seq2='" << seq2 << "'  script='" << ob.ss.str() <<"'");
    }
    {
        const std::string seq1 = "abc";
        const std::string seq2 = "aBc";
        const unsigned dist = 1;
        output_check_script<char, unsigned> ob(ASVECTOR(seq1), ASVECTOR(seq2));
        unsigned d = edit_distance(seq1, seq2, _script=ob, _substitution=true_type());
        ob.finalize(dist);
        BOOST_CHECK_MESSAGE(ob.correct, "incorrect edit script: '" << ob.ss.str() << "'  seq1='" << seq1 << "'  seq2='" << seq2 << "'");
        BOOST_CHECK_MESSAGE(d == dist, "incorrect edit distance " << d << "(expected " << dist << ")  seq1='" << seq1 << "' seq2='" << seq2 << "'  script='" << ob.ss.str() <<"'");
    }
    {
        const std::string seq1 = "abc";
        const std::string seq2 = "aBc";
        const unsigned dist = 0;
        output_check_script<char, unsigned, case_equal> ob(ASVECTOR(seq1), ASVECTOR(seq2));
        unsigned d = edit_distance(seq1, seq2, _script=ob, _substitution=true_type(), _equal=case_equal());
        ob.finalize(dist);
        BOOST_CHECK_MESSAGE(ob.correct, "incorrect edit script: '" << ob.ss.str() << "'  seq1='" << seq1 << "'  seq2='" << seq2 << "'");
        BOOST_CHECK_MESSAGE(d == dist, "incorrect edit distance " << d << "(expected " << dist << ")  seq1='" << seq1 << "' seq2='" << seq2 << "'  script='" << ob.ss.str() <<"'");
    }
}

BOOST_AUTO_TEST_CASE(myers_empty) {
    CHECK_EDIT_ALIGNMENT_ARG("", "", _substitution=boost::false_type(), 0);
}

BOOST_AUTO_TEST_CASE(myers_equal) {
    CHECK_EDIT_ALIGNMENT_ARG("a", "a", _substitution=boost::false_type(), 0);
    CHECK_EDIT_ALIGNMENT_ARG("aa", "aa", _substitution=boost::false_type(), 0);
    CHECK_EDIT_ALIGNMENT_ARG("aaa", "aaa", _substitution=boost::false_type(), 0);
}

BOOST_AUTO_TEST_CASE(myers_basis) {
    CHECK_EDIT_ALIGNMENT_ARG("", "a", _substitution=boost::false_type(), 1);
    CHECK_EDIT_ALIGNMENT_ARG("", "aa", _substitution=boost::false_type(), 2);
    CHECK_EDIT_ALIGNMENT_ARG("", "aaa", _substitution=boost::false_type(), 3);

    CHECK_EDIT_ALIGNMENT_ARG("a", "", _substitution=boost::false_type(), 1);
    CHECK_EDIT_ALIGNMENT_ARG("aa", "", _substitution=boost::false_type(), 2);
    CHECK_EDIT_ALIGNMENT_ARG("aaa", "", _substitution=boost::false_type(), 3);
}

BOOST_AUTO_TEST_CASE(myers_prefix_suffix) {
    CHECK_EDIT_ALIGNMENT_ARG("a", "aa", _substitution=boost::false_type(), 1);
    CHECK_EDIT_ALIGNMENT_ARG("a", "aaa", _substitution=boost::false_type(), 2);
    CHECK_EDIT_ALIGNMENT_ARG("a", "aaaa", _substitution=boost::false_type(), 3);
    CHECK_EDIT_ALIGNMENT_ARG("a", "aaaaa", _substitution=boost::false_type(), 4);

    CHECK_EDIT_ALIGNMENT_ARG("aa", "a", _substitution=boost::false_type(), 1);
    CHECK_EDIT_ALIGNMENT_ARG("aaa", "a", _substitution=boost::false_type(), 2);
    CHECK_EDIT_ALIGNMENT_ARG("aaaa", "a", _substitution=boost::false_type(), 3);
    CHECK_EDIT_ALIGNMENT_ARG("aaaaa", "a", _substitution=boost::false_type(), 4);
}

BOOST_AUTO_TEST_CASE(myers_no_equal) {
    CHECK_EDIT_ALIGNMENT_ARG("a", "x", _substitution=boost::false_type(), 2);
    CHECK_EDIT_ALIGNMENT_ARG("ab", "xy", _substitution=boost::false_type(), 4);
    CHECK_EDIT_ALIGNMENT_ARG("abc", "xyz", _substitution=boost::false_type(), 6);

    CHECK_EDIT_ALIGNMENT_ARG("a", "xy", _substitution=boost::false_type(), 3);
    CHECK_EDIT_ALIGNMENT_ARG("a", "xyz", _substitution=boost::false_type(), 4);

    CHECK_EDIT_ALIGNMENT_ARG("ab", "x", _substitution=boost::false_type(), 3);
    CHECK_EDIT_ALIGNMENT_ARG("abc", "x", _substitution=boost::false_type(), 4);
}

BOOST_AUTO_TEST_CASE(myers_equal_runs) {
    CHECK_EDIT_ALIGNMENT_ARG("aqc", "arc", _substitution=boost::false_type(), 2);
    CHECK_EDIT_ALIGNMENT_ARG("aqc", "xqz", _substitution=boost::false_type(), 4);

    CHECK_EDIT_ALIGNMENT_ARG("aqqc", "arrc", _substitution=boost::false_type(), 4);
    CHECK_EDIT_ALIGNMENT_ARG("aqqc", "xqqz", _substitution=boost::false_type(), 4);

    CHECK_EDIT_ALIGNMENT_ARG("ax", "abx", _substitution=boost::false_type(), 1);
    CHECK_EDIT_ALIGNMENT_ARG("abx", "ax", _substitution=boost::false_type(), 1);

    CHECK_EDIT_ALIGNMENT_ARG("ax", "abbx", _substitution=boost::false_type(), 2);
    CHECK_EDIT_ALIGNMENT_ARG("abx", "ax", _substitution=boost::false_type(), 1);
}

BOOST_AUTO_TEST_CASE(allow_sub_1) {
    CHECK_EDIT_ALIGNMENT_ARG("abc", "xyz", _substitution=true, 3);
    CHECK_EDIT_ALIGNMENT_ARG("abc", "xyz", _substitution=false, 6);

    CHECK_EDIT_ALIGNMENT_ARG("abc", "xyz", _substitution=boost::true_type(), 3);
    CHECK_EDIT_ALIGNMENT_ARG("abc", "xyz", _substitution=boost::false_type(), 6);

    CHECK_EDIT_ALIGNMENT_ARG("aqc", "xqz", _substitution=boost::true_type(), 2);
    CHECK_EDIT_ALIGNMENT_ARG("aqc", "xqz", _substitution=boost::false_type(), 4);

    CHECK_EDIT_ALIGNMENT_ARG("aqcr", "xqzr", _substitution=boost::true_type(), 2);
    CHECK_EDIT_ALIGNMENT_ARG("aqcr", "xqzr", _substitution=boost::false_type(), 4);

    CHECK_EDIT_ALIGNMENT_ARG("raqc", "rxqz", _substitution=boost::true_type(), 2);
    CHECK_EDIT_ALIGNMENT_ARG("raqc", "rxqz", _substitution=boost::false_type(), 4);
}


BOOST_AUTO_TEST_CASE(max_cost_0) {
    CHECK_EDIT_ALIGNMENT_2ARG("abc", "xyz", _substitution=true_type(), _max_cost=0, 3);
    CHECK_EDIT_ALIGNMENT_2ARG("abc", "xyz", _substitution=true_type(), _max_cost=1, 3);
    CHECK_EDIT_ALIGNMENT_2ARG("abc", "xyz", _substitution=true_type(), _max_cost=2, 3);
    CHECK_EDIT_ALIGNMENT_2ARG("abc", "xyz", _substitution=true_type(), _max_cost=3, 3);
    CHECK_EDIT_ALIGNMENT_2ARG("abc", "xyz", _substitution=true_type(), _max_cost=4, 3);

    CHECK_EDIT_ALIGNMENT_2ARG("abc", "xyz", _cost=unit_cost_test(), _max_cost=0, 6);
    CHECK_EDIT_ALIGNMENT_2ARG("abc", "xyz", _cost=unit_cost_test(), _max_cost=1, 6);
    CHECK_EDIT_ALIGNMENT_2ARG("abc", "xyz", _cost=unit_cost_test(), _max_cost=2, 6);
    CHECK_EDIT_ALIGNMENT_2ARG("abc", "xyz", _cost=unit_cost_test(), _max_cost=3, 6);
    CHECK_EDIT_ALIGNMENT_2ARG("abc", "xyz", _cost=unit_cost_test(), _max_cost=4, 6);

    CHECK_EDIT_ALIGNMENT_ARG("abc", "xyz", _max_cost=0, 6);
    CHECK_EDIT_ALIGNMENT_ARG("abc", "xyz", _max_cost=1, 6);
    CHECK_EDIT_ALIGNMENT_ARG("abc", "xyz", _max_cost=2, 6);
    CHECK_EDIT_ALIGNMENT_ARG("abc", "xyz", _max_cost=3, 6);
    CHECK_EDIT_ALIGNMENT_ARG("abc", "xyz", _max_cost=4, 6);
}

BOOST_AUTO_TEST_CASE(max_cost_1) {
    srand(time(0));
    vector<std::string> seqdata;
    const int N = 100;
    random_localized_deviations(seqdata, N, 100, 5, 10);
    int n = 0;
    double t0 = time(0);
    for (int i = 0;  i < seqdata.size();  ++i) {
        if (n >= N) break;
        for (int j = 0;  j < i;  ++j) {
            output_check_script_long_string out(seqdata[i], seqdata[j]);

            unsigned int d = edit_distance(seqdata[i], seqdata[j], _script=out);
            if (d < 2) continue;

            unsigned int dub = seqdata[i].size() + seqdata[j].size();
            unsigned int dt;

            out.reset();
            dt = edit_distance(seqdata[i], seqdata[j], _script=out, _max_cost=d/2);
            out.finalize(dt);
            BOOST_CHECK_GT(dt, d/2);
            BOOST_CHECK_GE(dt, d);
            BOOST_CHECK_LE(dt, dub);
            BOOST_CHECK_MESSAGE(out.correct, "\n\nseq1= '" << seqdata[i] << "'\nseq2= '"<< seqdata[j] <<"'\n\n");

            out.reset();
            dt = edit_distance(seqdata[i], seqdata[j], _script=out, _max_cost=d-1);
            out.finalize(dt);
            BOOST_CHECK_GT(dt, d-1);
            BOOST_CHECK_GE(dt, d);
            BOOST_CHECK_LE(dt, dub);
            BOOST_CHECK_MESSAGE(out.correct, "\n\nseq1= '" << seqdata[i] << "'\nseq2= '"<< seqdata[j] <<"'\n\n");

            out.reset();
            dt = edit_distance(seqdata[i], seqdata[j], _script=out, _max_cost=d);
            out.finalize(dt);
            BOOST_CHECK_EQUAL(dt, d);
            BOOST_CHECK(out.correct);

            out.reset();
            dt = edit_distance(seqdata[i], seqdata[j], _script=out, _max_cost=d+1);
            out.finalize(dt);
            BOOST_CHECK_EQUAL(dt, d);
            BOOST_CHECK(out.correct);

            out.reset();
            dt = edit_distance(seqdata[i], seqdata[j], _script=out, _max_cost=d+2);
            out.finalize(dt);
            BOOST_CHECK_EQUAL(dt, d);
            BOOST_CHECK(out.correct);

            out.reset();
            BOOST_CHECK_THROW(edit_distance(seqdata[i], seqdata[j], _script=out, _max_cost=d-1, _max_cost_exception=true), max_edit_cost_exception);

            if (++n >= N) break;
        }
    }
    double tt = time(0) - t0;
    BOOST_TEST_MESSAGE("time= " << tt << " sec   n= " << n << "   mean-time= " << tt/double(n) << "\n");
}


BOOST_AUTO_TEST_CASE(max_cost_error_1) {
    std::string seq1 = "xxxxxjja0xxxxxxxxxxxxb544b0xxxxxxxxxxxxxxxxx2xxxxxxxxxxxxxxxxxxxxxxxicff76xxxxxxxxxxxxxxxdxxxxxxxxxx";
    std::string seq2 = "xxxxxxx16g7xxxxxxxxxxxxxxx8f5c5xxxxxxxxxxxxxah9xxxxxxxxxxxxxxbxxxxxxxxxxxxxxxxxxxxxxxxxxxj5432539cxxxxxxx";
    output_check_script_long_string out(seq1, seq2);

    int d = edit_distance(seq1, seq2, _script=out, _max_cost=20);
    out.finalize(d);
    BOOST_CHECK_MESSAGE(out.correct, "\n\nseq1= '" << seq1 << "'\nseq2= '"<< seq2 <<"'\n\n");
}


BOOST_AUTO_TEST_CASE(max_cost_2) {
    srand(time(0));
    vector<std::string> seqdata;
    const int N = 100;
    random_localized_deviations(seqdata, N, 100, 5, 10);
    int n = 0;
    double t0 = time(0);
    for (int i = 0;  i < seqdata.size();  ++i) {
        if (n >= N) break;
        for (int j = 0;  j < i;  ++j) {
            output_check_script_long_string out(seqdata[i], seqdata[j]);

            unsigned int d = edit_distance(seqdata[i], seqdata[j], _script=out, _substitution=true_type());
            if (d < 2) continue;

            unsigned int dub = std::max(seqdata[i].size(), seqdata[j].size());
            unsigned int dt;

            out.reset();
            dt = edit_distance(seqdata[i], seqdata[j], _script=out, _substitution=true_type(), _max_cost=d/2);
            out.finalize(dt);
            BOOST_CHECK_GT(dt, d/2);
            BOOST_CHECK_GE(dt, d);
            BOOST_CHECK_LE(dt, dub);
            BOOST_CHECK(out.correct);

            out.reset();
            dt = edit_distance(seqdata[i], seqdata[j], _script=out, _substitution=true_type(), _max_cost=d-1);
            out.finalize(dt);
            BOOST_CHECK_GT(dt, d-1);
            BOOST_CHECK_GE(dt, d);
            BOOST_CHECK_LE(dt, dub);
            BOOST_CHECK(out.correct);

            out.reset();
            dt = edit_distance(seqdata[i], seqdata[j], _script=out, _substitution=true_type(), _max_cost=d);
            out.finalize(dt);
            BOOST_CHECK_EQUAL(dt, d);
            BOOST_CHECK(out.correct);

            out.reset();
            dt = edit_distance(seqdata[i], seqdata[j], _script=out, _substitution=true_type(), _max_cost=d+1);
            out.finalize(dt);
            BOOST_CHECK_EQUAL(dt, d);
            BOOST_CHECK(out.correct);

            out.reset();
            dt = edit_distance(seqdata[i], seqdata[j], _script=out, _substitution=true_type(), _max_cost=d+2);
            out.finalize(dt);
            BOOST_CHECK_EQUAL(dt, d);
            BOOST_CHECK(out.correct);

            out.reset();
            BOOST_CHECK_THROW(edit_distance(seqdata[i], seqdata[j], _script=out, _substitution=true_type(), _max_cost=d-1, _max_cost_exception=true), max_edit_cost_exception);

            if (++n >= N) break;
        }
    }
    double tt = time(0) - t0;
    BOOST_TEST_MESSAGE("time= " << tt << " sec   n= " << n << "   mean-time= " << tt/double(n) << "\n");
}


BOOST_AUTO_TEST_CASE(long_sequences) {
    CHECK_EDIT_ALIGNMENT_ARG("xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx", 
                         "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx", _substitution=true_type(), 
                         0);
    CHECK_EDIT_ALIGNMENT_ARG("xxxxxxxxxxxxxxxxxxxxxxxx*xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx", 
                         "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx*xxxxxxxxxxxxxxxxxxxxxxxxxx", _substitution=true_type(), 
                         2);
    CHECK_EDIT_ALIGNMENT_ARG("xxxxxxxxxxxxxxxxxxxxxxxx*xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx*xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx", 
                         "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx*xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx*xxxxxxxxxxxxxxxxxxxxxxxxxx", _substitution=true_type(), 
                         4);
    CHECK_EDIT_ALIGNMENT_ARG("xxxxxxxxxxxxxxxxxxxxxxxx**xxxxxxxxxxxxxxxxxxxxxxxxxxxxxx**xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx", 
                         "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx**xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx**xxxxxxxxxxxxxxxxxxxxxxxxx", _substitution=true_type(), 
                         8);
}


BOOST_AUTO_TEST_CASE(failure_1) {
    std::string seq1 = "xxxxxxxxx3d07a05d385h77xxxxxxxxxxxx";
    std::string seq2 = "xbd9a3d2gjf6b7a77hjcxxxxxxxxxxxxxxx";
    output_check_script_string out(seq1, seq2);
    unsigned int d = edit_distance(seq1, seq2, _script = out, _substitution=true_type(), _cost = cost_mixed_ops());
    out.finalize();
    BOOST_CHECK_MESSAGE(out.correct, "\n\nseq1= '" << seq1 << "'\nseq2= '"<< seq2 <<"'\n\n");
}


BOOST_AUTO_TEST_CASE(timing_1) {
    srand(time(0));
    vector<std::string> seqdata;
    const int N = 100;
    random_localized_deviations(seqdata, N, 100000, 5, 20, 100);
    int n = 0;
    double t0 = time(0);
    for (int i = 0;  i < seqdata.size();  ++i) {
        if (n >= N) break;
        for (int j = 0;  j < i;  ++j) {
            output_check_script_long_string out(seqdata[i], seqdata[j]);
            unsigned int d = edit_distance(seqdata[i], seqdata[j], _script = out, _substitution=true_type(), _cost = cost_mixed_ops());
            out.finalize(d);
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
            unsigned int d1 = edit_distance(seqdata[i], seqdata[j], _script = out, _substitution=true_type(), _cost = cost_mixed_ops());
            unsigned int d2 = edit_distance(seqdata[i], seqdata[j], _substitution=true_type(), _cost = cost_mixed_ops());
            out.finalize(d2);
            // verify that the edit script is a correct one: it transforms seq1 into seq2
            BOOST_CHECK_MESSAGE(out.correct, "\n\nseq1= '" << seqdata[i] << "'\nseq2= '"<< seqdata[j] <<"'\n\n");
            // cross-check 
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
            unsigned int d1 = edit_distance(seqdata[i], seqdata[j], _script=out1, _substitution=boost::false_type());
            out1.finalize(d1);
            // SSSP algorithm
            unsigned int d2 = edit_distance(seqdata[i], seqdata[j], _script=out2, _substitution=boost::false_type(), _cost = unit_cost_test());
            out2.finalize(d2);
            // verify that the edit script is a correct one: it transforms seq1 into seq2
            BOOST_CHECK_MESSAGE(out1.correct, "\n\nseq1= '" << seqdata[i] << "'\nseq2= '"<< seqdata[j] <<"'\n\n");
            BOOST_CHECK_MESSAGE(out2.correct, "\n\nseq1= '" << seqdata[i] << "'\nseq2= '"<< seqdata[j] <<"'\n\n");
            // cross-check 
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
            unsigned int d2 = edit_distance(seqdata[i], seqdata[j], _substitution=boost::false_type());
                //BOOST_TEST_CHECKPOINT("aaa");
            unsigned int d1 = edit_distance(seqdata[i], seqdata[j], _script = out, _substitution=boost::false_type());
                //BOOST_TEST_CHECKPOINT("bbb");
            out.finalize(d2);
            // verify that the edit script is a correct one: it transforms seq1 into seq2
            BOOST_CHECK_MESSAGE(out.correct, "\n\nseq1= '" << seqdata[i] << "'\nseq2= '"<< seqdata[j] <<"'\n\n");
            // cross-check 
            BOOST_CHECK_MESSAGE(d1==d2, "\n\nseq1= '" << seqdata[i] << "'\nseq2= '"<< seqdata[j] <<"'\n\n");
            if (++n >= N) break;
        }
    }
    double tt = time(0) - t0;
    BOOST_TEST_MESSAGE("time= " << tt << " sec   n= " << n << "   mean-time= " << tt/double(n) << "\n");
}



BOOST_AUTO_TEST_SUITE_END()
