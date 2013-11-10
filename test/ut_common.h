/*******
edit_distance: STL and Boost compatible edit distance functions for C++

Copyright (c) 2013 Erik Erlandson

Author:  Erik Erlandson <erikerlandson@yahoo.com>

Distributed under the Boost Software License, Version 1.0.
See accompanying file LICENSE or copy at
http://www.boost.org/LICENSE_1_0.txt
*******/

#if !defined(__ut_common_h__)
#define __ut_common_h__ 1

#include <cstdlib>

#include <vector>
#include <list>
#include <string>
#include <iterator>
#include <algorithm>
#include <iostream>
#include <sstream>

#include <boost/foreach.hpp>

#include <boost/range/as_literal.hpp>
#include <boost/range/as_array.hpp>
#include <boost/range/functions.hpp>
#include <boost/range/metafunctions.hpp>
#include <boost/range/adaptors.hpp>

#include <boost/tuple/tuple.hpp>
#include <boost/tuple/tuple_io.hpp>

#include <boost/algorithm/sequence_alignment/edit_distance.hpp>
#include <boost/algorithm/sequence_alignment/edit_alignment.hpp>

using boost::algorithm::sequence_alignment::edit_distance;
using boost::algorithm::sequence_alignment::edit_alignment;
using namespace boost::algorithm::sequence_alignment::parameter;

template <typename Range>
std::list<typename boost::range_value<Range>::type>
_aslist(const Range& s) {
    typedef typename boost::range_value<Range>::type val_t;
    std::list<val_t> r;
    BOOST_FOREACH(val_t e, s) r.push_back(e);
    return r;
}

template <typename Range>
std::vector<typename boost::range_value<Range>::type>
_asvector(const Range& s) {
    typedef typename boost::range_value<Range>::type val_t;
    std::vector<val_t> r;
    BOOST_FOREACH(val_t e, s) r.push_back(e);
    return r;
}

template <typename Range>
std::string
_asstring(const Range& s) {
    typedef typename boost::range_value<Range>::type val_t;
    std::string r;
    BOOST_FOREACH(val_t e, s) r += e;
    return r;
}

#define ASLIST(seq) (_aslist(boost::as_literal(seq)))
#define ASVECTOR(seq) (_asvector(boost::as_literal(seq)))
#define ASSTRING(seq) (_asstring(boost::as_literal(seq)))

struct cost_expensive_sub {
    typedef int cost_type;
    typedef char value_type;
    cost_type cost_ins(value_type c) const { return 1; }
    cost_type cost_del(value_type c) const { return 1; }
    cost_type cost_sub(value_type c, value_type d) const { return (c == d) ? 0 : 3; }
};

struct cost_expensive_ins {
    typedef int cost_type;
    typedef char value_type;
    cost_type cost_ins(value_type c) const { return 2; }
    cost_type cost_del(value_type c) const { return 1; }
    cost_type cost_sub(value_type c, value_type d) const { return (c == d) ? 0 : 1; }
};

struct cost_mixed_ops {
    typedef int cost_type;
    typedef char value_type;
    inline cost_type cost_ins(value_type const& c) const { return 1; }
    inline cost_type cost_del(value_type const& c) const { return 1; }
    inline cost_type cost_sub(value_type const& c, value_type const& d) const {
        if (c == d) return 0;
        // allow substitution between alphabetics
        if (isalpha(c) && isalpha(d)) return 1;
        return 10;
    }
};


template <typename ValueType, typename CostType = long>
struct output_check_script {
    typedef ValueType value_type;
    typedef CostType cost_type;

    output_check_script(const std::vector<value_type>& seq1_, const std::vector<value_type>& seq2_) : seq1(ASVECTOR(seq1_)), seq2(ASVECTOR(seq2_)), correct(true), j1(-1), j2(-1) {
        ss << boost::tuples::set_delimiter(' ');
    }

    void output_ins(const value_type& v2, cost_type c) { 
        ss << boost::make_tuple('+', v2, c);
        if (seq2[++j2] != v2) correct=false;
    }
    void output_del(const value_type& v1, cost_type c) { 
        ss << boost::make_tuple('-', v1, c); 
        if (seq1[++j1] != v1) correct=false; 
    }
    void output_sub(const value_type& v1, const value_type& v2, cost_type c) { 
        ss << boost::make_tuple(':', v1, v2, c); 
        if (seq1[++j1] != v1  ||  seq2[++j2] != v2) correct=false;
        // cost should be > zero: otherwise we should be in output_eql()
        if (c <= cost_type(0)) correct=false;
    }
    void output_eql(const value_type& v1, const value_type& v2) { 
        ss << boost::make_tuple('=', v1, v2); 
        if (seq1[++j1] != v1  ||  seq2[++j2] != v2) correct=false;
        // this condition is not necessarily true in general, since a cost function
        // can define two elements as equivalent even if they are not identical, but
        // I expect it to be true for my testing examples, and it helps make the
        // test checking stronger
        if (seq1[j1] != seq2[j2]) correct=false;
    }

    void finalize() {
        if (j1 != long(seq1.size())-1) correct = false;
        if (j2 != long(seq2.size())-1) correct = false;
    }

    std::stringstream ss;
    long j1;
    long j2;
    std::vector<value_type> seq1;
    std::vector<value_type> seq2;
    bool correct;
};

struct output_check_script_long_string {
    typedef char value_type;
    typedef long cost_type;

    output_check_script_long_string(const std::string& seq1_, const std::string& seq2_) : seq1(seq1_), seq2(seq2_), correct(true), j1(-1), j2(-1) {
    }

    void output_ins(const value_type& v2, cost_type c) { 
        if (seq2[++j2] != v2) correct=false;
    }
    void output_del(const value_type& v1, cost_type c) { 
        if (seq1[++j1] != v1) correct=false; 
    }
    void output_sub(const value_type& v1, const value_type& v2, cost_type c) { 
        if (seq1[++j1] != v1  ||  seq2[++j2] != v2) correct=false;
        // cost should be > zero: otherwise we should be in output_eql()
        if (c <= cost_type(0)) correct=false;
    }
    void output_eql(const value_type& v1, const value_type& v2) { 
        if (seq1[++j1] != v1  ||  seq2[++j2] != v2) correct=false;
        // this condition is not necessarily true in general, since a cost function
        // can define two elements as equivalent even if they are not identical, but
        // I expect it to be true for my testing examples, and it helps make the
        // test checking stronger
        if (seq1[j1] != seq2[j2]) correct=false;
    }

    void finalize() {
        if (j1 != long(seq1.size())-1) correct = false;
        if (j2 != long(seq2.size())-1) correct = false;
    }

    long j1;
    long j2;
    std::string seq1;
    std::string seq2;
    bool correct;
};


struct output_check_script_string {
    typedef char value_type;
    typedef long cost_type;

    output_check_script_string(const std::string& seq1_, const std::string& seq2_) : seq1(seq1_), seq2(seq2_), correct(true), j1(-1), j2(-1) {
    }

    void output_ins(const value_type& v2, cost_type c) { 
        if (seq2[++j2] != v2) {
            std::cerr << "INS seq2["<<j2<<"]= " << seq2[j2] << "     != " << v2 << "\n";
            correct=false;
        }
    }
    void output_del(const value_type& v1, cost_type c) { 
        if (seq1[++j1] != v1) {
            std::cerr << "DEL seq1["<<j1<<"]= " << seq1[j1] << "     != " << v1 << "\n";
            correct=false; 
        }
    }
    void output_sub(const value_type& v1, const value_type& v2, cost_type c) { 
        if (seq1[++j1] != v1  ||  seq2[++j2] != v2) {
            std::cerr << "SUB seq1["<<j1<<"]= " << seq1[j1] << "     != " << v1 <<   "   ||   seq2["<<j2<<"]= " << seq2[j2] << "     != " << v2 << "\n";
            correct=false;
        }
        // cost should be > zero: otherwise we should be in output_eql()
        if (c <= cost_type(0)) {
            std::cerr << "SUB cost= " << c << "   seq1["<<j1<<"]= " << seq1[j1] << "     != " << v1 <<   "   ||   seq2["<<j2<<"]= " << seq2[j2] << "     != " << v2 << "\n";
            correct=false;
        }
    }
    void output_eql(const value_type& v1, const value_type& v2) { 
        if (seq1[++j1] != v1  ||  seq2[++j2] != v2) {
            std::cerr << "EQL seq1["<<j1<<"]= " << seq1[j1] << "     != " << v1 <<   "   ||   seq2["<<j2<<"]= " << seq2[j2] << "     != " << v2 << "\n";
            correct=false;
        }
        // this condition is not necessarily true in general, since a cost function
        // can define two elements as equivalent even if they are not identical, but
        // I expect it to be true for my testing examples, and it helps make the
        // test checking stronger
        if (seq1[j1] != seq2[j2]) {
            std::cerr << "EQL (2) seq1["<<j1<<"]= " << seq1[j1] << "     != " << v1 <<   "   ||   seq2["<<j2<<"]= " << seq2[j2] << "     != " << v2 << "\n";
            correct=false;
        }
    }

    void finalize() {
        if (j1 != long(seq1.size())-1) {
            std::cerr << "j1= " << j1  <<  "    != " << long(seq1.size())-1 << "\n";
            correct = false;
        }
        if (j2 != long(seq2.size())-1) {
            std::cerr << "j2= " << j2  <<  "    != " << long(seq2.size())-1 << "\n";
            correct = false;
        }
    }

    long j1;
    long j2;
    std::string seq1;
    std::string seq2;
    bool correct;
};


#define CHECK_EDIT_ALIGNMENT(seq1, seq2, dist) \
{ \
    BOOST_TEST_CHECKPOINT("testing seq1='" << ASSTRING(seq1) << "'  seq2= '" << ASSTRING(seq2) << "'"); \
    boost::unit_test::unit_test_log.set_threshold_level(boost::unit_test::log_messages); \
    output_check_script<char> ob(ASVECTOR(seq1), ASVECTOR(seq2)); \
    long d = edit_alignment(seq1, seq2, ob); \
    ob.finalize(); \
    BOOST_CHECK_MESSAGE(ob.correct, "incorrect edit script: '" << ob.ss.str() << "'  seq1='" << ASSTRING(seq1) << "'  seq2='" << ASSTRING(seq2) << "'"); \
    BOOST_CHECK_MESSAGE(d == dist, "incorrect edit distance " << d << "(expected " << dist << ")  seq1='" << ASSTRING(seq1) << "' seq2='" << ASSTRING(seq2) << "'  script='" << ob.ss.str() <<"'"); \
}

#define CHECK_EDIT_ALIGNMENT_COST(seq1, seq2, cost, dist) \
{ \
    BOOST_TEST_CHECKPOINT("testing seq1='" << ASSTRING(seq1) << "'  seq2= '" << ASSTRING(seq2) << "'"); \
    boost::unit_test::unit_test_log.set_threshold_level(boost::unit_test::log_messages); \
    output_check_script<char, cost::cost_type> ob(ASVECTOR(seq1), ASVECTOR(seq2)); \
    long d = edit_alignment(seq1, seq2, ob, cost ()); \
    ob.finalize(); \
    BOOST_CHECK_MESSAGE(ob.correct, "incorrect edit script: '" << ob.ss.str() << "'  seq1='" << ASSTRING(seq1) << "'  seq2='" << ASSTRING(seq2) << "'"); \
    BOOST_CHECK_MESSAGE(d == dist, "incorrect edit distance " << d << "(expected " << dist << ")  seq1='" << ASSTRING(seq1) << "' seq2='" << ASSTRING(seq2) << "'  script='" << ob.ss.str() <<"'"); \
}

#define CHECK_EDIT_ALIGNMENT_BEAM(seq1, seq2, B, dist) \
{ \
    BOOST_TEST_CHECKPOINT("testing seq1='" << ASSTRING(seq1) << "'  seq2= '" << ASSTRING(seq2) << "'"); \
    boost::unit_test::unit_test_log.set_threshold_level(boost::unit_test::log_messages); \
    output_check_script<char> ob(ASVECTOR(seq1), ASVECTOR(seq2)); \
    long d = edit_alignment(seq1, seq2, ob, _beam = B); \
    ob.finalize(); \
    BOOST_CHECK_MESSAGE(ob.correct, "incorrect edit script: '" << ob.ss.str() << "'  seq1='" << ASSTRING(seq1) << "'  seq2='" << ASSTRING(seq2) << "'"); \
    BOOST_CHECK_MESSAGE(d == dist, "incorrect edit distance " << d << "(expected " << dist << ")  seq1='" << ASSTRING(seq1) << "' seq2='" << ASSTRING(seq2) << "'  script='" << ob.ss.str() <<"'"); \
}

#define CHECK_EDIT_ALIGNMENT_ARG(seq1, seq2, arg, dist) \
{ \
    BOOST_TEST_CHECKPOINT("testing seq1='" << ASSTRING(seq1) << "'  seq2= '" << ASSTRING(seq2) << "'"); \
    boost::unit_test::unit_test_log.set_threshold_level(boost::unit_test::log_messages); \
    output_check_script<char> ob(ASVECTOR(seq1), ASVECTOR(seq2)); \
    long d = edit_alignment(seq1, seq2, ob, arg); \
    ob.finalize(); \
    BOOST_CHECK_MESSAGE(ob.correct, "incorrect edit script: '" << ob.ss.str() << "'  seq1='" << ASSTRING(seq1) << "'  seq2='" << ASSTRING(seq2) << "'"); \
    BOOST_CHECK_MESSAGE(d == dist, "incorrect edit distance " << d << "(expected " << dist << ")  seq1='" << ASSTRING(seq1) << "' seq2='" << ASSTRING(seq2) << "'  script='" << ob.ss.str() <<"'"); \
}

#endif
