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

// Define this flag to make char values default to '@' instead of zero '\0'
// So default values are printable, which is much easier to work with in testing.
#define BOOST_CHAR_DEFAULT_OVERRIDE '@'
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


#define CHECK_EDIT_ALIGNMENT_COST(fun, seq1, seq2, cost, vtype, dist, output) \
{ \
    std::stringstream ss; \
    ss << boost::tuples::set_delimiter(','); \
    BOOST_CHECK_EQUAL(fun(seq1, seq2, std::ostream_iterator<vtype>(ss, ""), cost).second, dist); \
    BOOST_CHECK_EQUAL(ss.str(), output); \
}

#define CHECK_EDIT_ALIGNMENT(fun, seq1, seq2, vtype, dist, output) \
{ \
    std::stringstream ss; \
    ss << boost::tuples::set_delimiter(','); \
    BOOST_CHECK_EQUAL(fun(seq1, seq2, std::ostream_iterator<vtype>(ss, "")).second, dist); \
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
