/*******
edit_distance: STL and Boost compatible edit distance functions for C++

Copyright (c) 2013 Erik Erlandson

Author:  Erik Erlandson <erikerlandson@yahoo.com>

Distributed under the Boost Software License, Version 1.0.
See accompanying file LICENSE or copy at
http://www.boost.org/LICENSE_1_0.txt
*******/

#if !defined(X_COMMON_HPP)
#define X_COMMON_HPP

#include <sstream>
#include <list>
#include <vector>
#include <string>

#include <boost/foreach.hpp>
#include <boost/range/as_literal.hpp>
#include <boost/range/adaptors.hpp>

#include <boost/tuple/tuple.hpp>
#include <boost/tuple/tuple_io.hpp>


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


template <typename Cost>
struct stringstream_tuple_output {
    typedef typename Cost::cost_type cost_type;
    typedef typename Cost::value_type value_type;

    stringstream_tuple_output() {
        ss << boost::tuples::set_delimiter(' ');
    }

    inline void output_ins(const value_type& v2, const cost_type& c) {
        ss << boost::make_tuple('+', v2, c);
    }
    inline void output_del(const value_type& v1, const cost_type& c) {
        ss << boost::make_tuple('-', v1, c);
    }
    inline void output_sub(const value_type& v1, const value_type& v2, const cost_type& c) {
        ss << boost::make_tuple(':', v1, v2);
    }
    inline void output_eql(const value_type& v1, const value_type& v2) {
        ss << boost::make_tuple('=', v1, v2);
    }

    std::stringstream ss;
};


#endif
