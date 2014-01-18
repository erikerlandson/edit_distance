/*******
edit_distance: STL and Boost compatible edit distance functions for C++

Copyright (c) 2013 Erik Erlandson

Author:  Erik Erlandson <erikerlandson@yahoo.com>

Distributed under the Boost Software License, Version 1.0.
See accompanying file LICENSE or copy at
http://www.boost.org/LICENSE_1_0.txt
*******/

#if !defined(BOOST_ALGORITHM_SEQUENCE_EXAMPLE_EDIT_DISTANCE_COMMON_HPP)
#define BOOST_ALGORITHM_SEQUENCE_EXAMPLE_EDIT_DISTANCE_COMMON_HPP

#include <sstream>
#include <list>
#include <vector>
#include <string>

#include <boost/foreach.hpp>
#include <boost/range/as_literal.hpp>
#include <boost/range/adaptors.hpp>

#include <boost/tuple/tuple.hpp>
#include <boost/tuple/tuple_io.hpp>

#include <boost/algorithm/sequence/edit_distance.hpp>

std::list<char> as_list(char const* str) {
    std::list<char> r;
    BOOST_FOREACH(char e, boost::as_literal(str)) r.push_back(e);
    return r;
}
std::vector<char> as_vector(char const* str) {
    std::vector<char> r;
    BOOST_FOREACH(char e, boost::as_literal(str)) r.push_back(e);
    return r;
}

using boost::algorithm::sequence::detail::cost_type;

template <typename Cost, typename Sequence>
struct stringstream_tuple_output {
    typedef typename boost::range_value<Sequence>::type value_type;
    typedef typename cost_type<Cost, value_type>::type cost_type;

    stringstream_tuple_output() {
        ss << boost::tuples::set_delimiter(' ');
    }

    inline void insertion(const value_type v2, const cost_type c) {
        ss << boost::make_tuple('+', v2, c);
    }
    inline void deletion(const value_type v1, const cost_type c) {
        ss << boost::make_tuple('-', v1, c);
    }
    inline void substitution(const value_type v1, const value_type v2, const cost_type c) {
        ss << boost::make_tuple(':', v1, v2, c);
    }
    inline void equality(const value_type v1, const value_type v2) {
        ss << boost::make_tuple('=', v1, v2);
    }

    std::stringstream ss;
};


#endif
