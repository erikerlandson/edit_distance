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

using namespace boost::algorithm::sequence_alignment;

struct cost_expensive_sub {
    typedef int cost_type;
    typedef char value_type;
    cost_type cost_ins(value_type c) { return 1; }
    cost_type cost_del(value_type c) { return 1; }
    cost_type cost_sub(value_type c, value_type d) { return (c == d) ? 0 : 3; }
};

struct cost_expensive_ins {
    typedef int cost_type;
    typedef char value_type;
    cost_type cost_ins(value_type c) { return 2; }
    cost_type cost_del(value_type c) { return 1; }
    cost_type cost_sub(value_type c, value_type d) { return (c == d) ? 0 : 1; }
};


template <typename ValueType, typename CostType = long>
struct output_basic {
    typedef ValueType value_type;
    typedef CostType cost_type;

    output_basic(std::stringstream& ss_) : ss(&ss_) {
        *ss << boost::tuples::set_delimiter(' ');
    }

    void output_ins(const value_type& c, cost_type s) { *ss << boost::make_tuple('+', c); }
    void output_del(const value_type& c, cost_type s) { *ss << boost::make_tuple('-', c); }
    void output_sub(const value_type& c, const value_type& d, cost_type s) { *ss << boost::make_tuple(c, ':', d); }
    void output_eql(const value_type& c, const value_type& d) { *ss << boost::make_tuple(c, '=', d); }

    std::stringstream* ss;
};


template <typename ValueType, typename CostType = long>
struct output_with_cost {
    typedef ValueType value_type;
    typedef CostType cost_type;

    output_with_cost(std::stringstream& ss_) : ss(&ss_) {
        *ss << boost::tuples::set_delimiter(' ');
    }

    void output_ins(const value_type& c, cost_type s) { *ss << boost::make_tuple('+', c, s); }
    void output_del(const value_type& c, cost_type s) { *ss << boost::make_tuple('-', c, s); }
    void output_sub(const value_type& c, const value_type& d, cost_type s) { *ss << boost::make_tuple(':', c, d, s); }
    void output_eql(const value_type& c, const value_type& d) { *ss << boost::make_tuple('=', c, d); }

    std::stringstream* ss;
};


#define CHECK_EDIT_ALIGNMENT(seq1, seq2, dist, output) \
{ \
    std::stringstream ss; \
    output_basic<char> ob(ss); \
    BOOST_CHECK_EQUAL(edit_alignment(seq1, seq2, ob), dist); \
    BOOST_CHECK_EQUAL(ss.str(), output); \
}

#define CHECK_EDIT_ALIGNMENT_COST(seq1, seq2, cost, dist, output) \
{ \
    std::stringstream ss; \
    output_with_cost<char, cost::cost_type> ob(ss); \
    BOOST_CHECK_EQUAL(edit_alignment(seq1, seq2, ob, cost ()), dist);   \
    BOOST_CHECK_EQUAL(ss.str(), output); \
}


#define ASLIST(seq) (_aslist(boost::as_literal(seq)))
#define ASVECTOR(seq) (_asvector(boost::as_literal(seq)))
#define ASSTRING(seq) (_asstring(boost::as_literal(seq)))

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

#endif
