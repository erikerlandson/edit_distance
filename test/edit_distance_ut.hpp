/*******
edit_distance: STL and Boost compatible edit distance functions for C++

Copyright (c) 2013 Erik Erlandson

Author:  Erik Erlandson <erikerlandson@yahoo.com>

Distributed under the Boost Software License, Version 1.0.
See accompanying file LICENSE or copy at
http://www.boost.org/LICENSE_1_0.txt
*******/

#if !defined(BOOST_ALGORITHM_SEQUENCE_TEST_EDIT_DISTANCE_UT_HPP)
#define BOOST_ALGORITHM_SEQUENCE_TEST_EDIT_DISTANCE_UT_HPP 1

#include <cstdlib>
#include <cmath>
#include <ctype.h>

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

using std::vector;
using std::list;
using std::string;

using boost::true_type;
using boost::false_type;

#include <boost/algorithm/sequence/edit_distance.hpp>

using boost::algorithm::sequence::edit_distance;
using namespace boost::algorithm::sequence::parameter;
using boost::algorithm::sequence::max_edit_cost_exception;

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

struct undef_sub_cost {
    typedef size_t cost_type;

    template <typename value_type> inline
    cost_type insertion(value_type const&) const {
        return cost_type(1);
    }

    template <typename value_type> inline
    cost_type deletion(value_type const&) const {
        return cost_type(1);
    }
};

// this is same as default unit_cost, but will trigger
// the generic algorithm because of its differing type
// useful for cross-checking
struct unit_cost_test {
    typedef size_t cost_type;

    template <typename value_type> inline
    cost_type insertion(value_type const&) const {
        return cost_type(1);
    }

    template <typename value_type> inline
    cost_type deletion(value_type const&) const {
        return cost_type(1);
    }

    template <typename value_type_1, typename value_type_2> inline
    cost_type substitution(value_type_1 const&, value_type_2 const&) const {
        return cost_type(1);
    }
};

struct cost_expensive_sub {
    typedef int cost_type;
    typedef char value_type;
    cost_type insertion(value_type) const { return 1; }
    cost_type deletion(value_type) const { return 1; }
    cost_type substitution(value_type, value_type) const { return 3; }
};

struct cost_expensive_ins {
    typedef int cost_type;
    typedef char value_type;
    cost_type insertion(value_type) const { return 2; }
    cost_type deletion(value_type) const { return 1; }
    cost_type substitution(value_type, value_type) const { return 1; }
};

struct cost_mixed_ops {
    typedef int cost_type;
    typedef char value_type;
    inline cost_type insertion(value_type const&) const { return 1; }
    inline cost_type deletion(value_type const&) const { return 1; }
    inline cost_type substitution(value_type const& c, value_type const& d) const {
        // allow substitution between alphabetics
        if (isalpha(c) && isalpha(d)) return 1;
        return 3;
    }
};

struct case_equal {
    inline bool operator()(char a, char b) const { return tolower(a) == tolower(b); }
};


template <typename ValueType, typename CostType = long, typename Equal=std::equal_to<ValueType> >
struct output_check_script {
    typedef ValueType value_type;
    typedef CostType cost_type;

    output_check_script(const std::vector<value_type>& seq1_, const std::vector<value_type>& seq2_) : seq1(ASVECTOR(seq1_)), seq2(ASVECTOR(seq2_)), correct(true), j1(-1), j2(-1), tcost(0) {
        ss << boost::tuples::set_delimiter(' ');
    }

    void insertion(const value_type& v2, cost_type c) { 
        ss << boost::make_tuple('+', v2, c);
        if (seq2[++j2] != v2) correct=false;
        tcost += c;
    }
    void deletion(const value_type& v1, cost_type c) { 
        ss << boost::make_tuple('-', v1, c); 
        if (seq1[++j1] != v1) correct=false; 
        tcost += c;
    }
    void substitution(const value_type& v1, const value_type& v2, cost_type c) { 
        ss << boost::make_tuple(':', v1, v2, c); 
        if (seq1[++j1] != v1  ||  seq2[++j2] != v2) correct=false;
        // cost should be > zero: otherwise we should be in equality()
        if (c <= cost_type(0)) correct=false;
        tcost += c;
    }
    void equality(const value_type& v1, const value_type& v2) { 
        ss << boost::make_tuple('=', v1, v2); 
        if (seq1[++j1] != v1  ||  seq2[++j2] != v2) correct=false;
        // this condition is not necessarily true in general, since a cost function
        // can define two elements as equivalent even if they are not identical, but
        // I expect it to be true for my testing examples, and it helps make the
        // test checking stronger
        if (!equal(seq1[j1],seq2[j2])) correct=false;
    }

    void finalize() {
        if (j1 != long(seq1.size())-1) correct = false;
        if (j2 != long(seq2.size())-1) correct = false;
    }

    void finalize(cost_type c) {
        finalize();
        if (tcost != c) correct = false;
    }

    std::stringstream ss;
    long j1;
    long j2;
    std::vector<value_type> seq1;
    std::vector<value_type> seq2;
    bool correct;
    Equal equal;
    cost_type tcost;
};

template <typename ValueType, typename CostType = long, typename Equal=std::equal_to<ValueType> >
struct undef_sub_output {
    typedef ValueType value_type;
    typedef CostType cost_type;

undef_sub_output(const std::vector<value_type>& seq1_, const std::vector<value_type>& seq2_) : seq1(ASVECTOR(seq1_)), seq2(ASVECTOR(seq2_)), correct(true), j1(-1), j2(-1), tcost(0) {
        ss << boost::tuples::set_delimiter(' ');
    }

    void insertion(const value_type& v2, cost_type c) { 
        ss << boost::make_tuple('+', v2, c);
        if (seq2[++j2] != v2) correct=false;
        tcost += c;
    }
    void deletion(const value_type& v1, cost_type c) { 
        ss << boost::make_tuple('-', v1, c); 
        if (seq1[++j1] != v1) correct=false; 
        tcost += c;
    }
    void equality(const value_type& v1, const value_type& v2) { 
        ss << boost::make_tuple('=', v1, v2); 
        if (seq1[++j1] != v1  ||  seq2[++j2] != v2) correct=false;
        if (!equal(seq1[j1],seq2[j2])) correct=false;
    }

    void finalize() {
        if (j1 != long(seq1.size())-1) correct = false;
        if (j2 != long(seq2.size())-1) correct = false;
    }

    void finalize(cost_type c) {
        finalize();
        if (tcost != c) correct = false;
    }

    std::stringstream ss;
    long j1;
    long j2;
    std::vector<value_type> seq1;
    std::vector<value_type> seq2;
    bool correct;
    Equal equal;
    cost_type tcost;
};


struct output_check_script_long_string {
    typedef char value_type;
    typedef long cost_type;

    output_check_script_long_string(const std::string& seq1_, const std::string& seq2_) : seq1(seq1_), seq2(seq2_), correct(true), j1(-1), j2(-1), tcost(0) {
    }

    void insertion(const value_type& v2, cost_type c) { 
        if (seq2[++j2] != v2) correct=false;
        tcost += c;
    }
    void deletion(const value_type& v1, cost_type c) { 
        if (seq1[++j1] != v1) correct=false; 
        tcost += c;
    }
    void substitution(const value_type& v1, const value_type& v2, cost_type c) { 
        if (seq1[++j1] != v1  ||  seq2[++j2] != v2) correct=false;
        // cost should be > zero: otherwise we should be in equality()
        if (c <= cost_type(0)) correct=false;
        tcost += c;
    }
    void equality(const value_type& v1, const value_type& v2) { 
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

    void finalize(cost_type c) {
        finalize();
        if (tcost != c) correct = false;
    }

    void reset() {
        j1 = -1;
        j2 = -1;
        correct = true;
        tcost = 0;
    }

    long j1;
    long j2;
    std::string seq1;
    std::string seq2;
    bool correct;
    cost_type tcost;
};


struct output_check_script_string {
    typedef char value_type;
    typedef long cost_type;

    output_check_script_string(const std::string& seq1_, const std::string& seq2_) : seq1(seq1_), seq2(seq2_), correct(true), j1(-1), j2(-1), tcost(0) {
    }

    void insertion(const value_type& v2, cost_type c) { 
        if (seq2[++j2] != v2) {
            std::cerr << "INS seq2["<<j2<<"]= " << seq2[j2] << "     != " << v2 << "\n";
            correct=false;
        }
        tcost += c;
    }
    void deletion(const value_type& v1, cost_type c) { 
        if (seq1[++j1] != v1) {
            std::cerr << "DEL seq1["<<j1<<"]= " << seq1[j1] << "     != " << v1 << "\n";
            correct=false; 
        }
        tcost += c;
    }
    void substitution(const value_type& v1, const value_type& v2, cost_type c) { 
        if (seq1[++j1] != v1  ||  seq2[++j2] != v2) {
            std::cerr << "SUB seq1["<<j1<<"]= " << seq1[j1] << "     != " << v1 <<   "   ||   seq2["<<j2<<"]= " << seq2[j2] << "     != " << v2 << "\n";
            correct=false;
        }
        // cost should be > zero: otherwise we should be in equality()
        if (c <= cost_type(0)) {
            std::cerr << "SUB cost= " << c << "   seq1["<<j1<<"]= " << seq1[j1] << "     != " << v1 <<   "   ||   seq2["<<j2<<"]= " << seq2[j2] << "     != " << v2 << "\n";
            correct=false;
        }
        tcost += c;
    }
    void equality(const value_type& v1, const value_type& v2) { 
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

    void finalize(cost_type c) {
        finalize();
        if (tcost != c) correct = false;
    }

    long j1;
    long j2;
    std::string seq1;
    std::string seq2;
    bool correct;
    cost_type tcost;
};


#define CHECK_EDIT_ALIGNMENT(seq1, seq2, dist) \
{ \
    BOOST_TEST_CHECKPOINT("testing seq1='" << ASSTRING(seq1) << "'  seq2= '" << ASSTRING(seq2) << "'"); \
    output_check_script<char> ob(ASVECTOR(seq1), ASVECTOR(seq2)); \
    long d = edit_distance(seq1, seq2, _script=ob); \
    ob.finalize(d); \
    BOOST_CHECK_MESSAGE(ob.correct, "incorrect edit script: '" << ob.ss.str() << "'  seq1='" << ASSTRING(seq1) << "'  seq2='" << ASSTRING(seq2) << "'"); \
    BOOST_CHECK_MESSAGE(d == dist, "incorrect edit distance " << d << "(expected " << dist << ")  seq1='" << ASSTRING(seq1) << "' seq2='" << ASSTRING(seq2) << "'  script='" << ob.ss.str() <<"'"); \
}

#define CHECK_EDIT_ALIGNMENT_ARG(seq1, seq2, arg, dist) \
{ \
    BOOST_TEST_CHECKPOINT("testing seq1='" << ASSTRING(seq1) << "'  seq2= '" << ASSTRING(seq2) << "'"); \
    output_check_script<char> ob(ASVECTOR(seq1), ASVECTOR(seq2)); \
    long d = edit_distance(seq1, seq2, _script=ob, arg); \
    ob.finalize(d); \
    BOOST_CHECK_MESSAGE(ob.correct, "incorrect edit script: '" << ob.ss.str() << "'  seq1='" << ASSTRING(seq1) << "'  seq2='" << ASSTRING(seq2) << "'"); \
    BOOST_CHECK_MESSAGE(d == dist, "incorrect edit distance " << d << "(expected " << dist << ")  seq1='" << ASSTRING(seq1) << "' seq2='" << ASSTRING(seq2) << "'  script='" << ob.ss.str() <<"'"); \
}

#define CHECK_EDIT_ALIGNMENT_2ARG(seq1, seq2, arg, arg2, dist)   \
{ \
    BOOST_TEST_CHECKPOINT("testing seq1='" << ASSTRING(seq1) << "'  seq2= '" << ASSTRING(seq2) << "'"); \
    output_check_script<char> ob(ASVECTOR(seq1), ASVECTOR(seq2)); \
    long d = edit_distance(seq1, seq2, _script=ob, arg, arg2); \
    ob.finalize(d); \
    BOOST_CHECK_MESSAGE(ob.correct, "incorrect edit script: '" << ob.ss.str() << "'  seq1='" << ASSTRING(seq1) << "'  seq2='" << ASSTRING(seq2) << "'"); \
    BOOST_CHECK_MESSAGE(d == dist, "incorrect edit distance " << d << "(expected " << dist << ")  seq1='" << ASSTRING(seq1) << "' seq2='" << ASSTRING(seq2) << "'  script='" << ob.ss.str() <<"'"); \
}

template <typename S>
void random_localized_deviations(vector<S>& seqdata, const int N, const int LEN, const int D, const int K, const int maxsfx = 100000) {
    char data[] = "abcdefghij0123456789";
    const unsigned int data_size = sizeof(data)-1;
    const unsigned int R = LEN/D;
    int sz = int(ceil((1 + sqrt(1+8*N))/2));
    int rsfx = std::min(maxsfx, std::max(2, LEN/10));
    seqdata.resize(sz);
    for (int i = 0;  i < seqdata.size();  ++i) {
        seqdata[i].resize(LEN + ((rand()%2) ? 0 : (rand()%rsfx)), 'x');
        for (int d = 0;  d < D;  ++d) {
            unsigned int b1 = d*R + (rand() % K);
            unsigned int l1 =  rand() % K;
            for (unsigned int j = b1;  j < b1+l1;  ++j) seqdata[i][j] = data[rand()%data_size];
        }
    }
}

#endif
