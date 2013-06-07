/*******
edit_distance: STL and Boost compatible edit distance functions for C++

Copyright (c) 2013 Erik Erlandson

Author:  Erik Erlandson <erikerlandson@yahoo.com>

Distributed under the Boost Software License, Version 1.0.
See accompanying file LICENSE or copy at
http://www.boost.org/LICENSE_1_0.txt
*******/


#if !defined(__edit_distance_h__)
#define __edit_distance_h__ 1

#include <boost/range/functions.hpp>
#include <boost/range/metafunctions.hpp>

template <typename Range>
struct default_cost {
    typedef typename boost::range_difference<Range>::type cost_type;
    typedef typename boost::range_value<Range>::type value_type;
    cost_type cost_ins(value_type const& a, value_type const& b) const {
        return cost_type(1);
    }
    cost_type cost_del(value_type const& a, value_type const& b) const {
        return cost_type(1);
    }
    cost_type cost_sub(value_type const& a, value_type const& b) const {
        return (a == b) ? cost_type(0) : cost_type(1);
    }
};


template <typename ForwardRange1, typename ForwardRange2, typename Cost>
unsigned int edit_distance(ForwardRange1 const& seq1, ForwardRange2 const& seq2, Cost& cost) {
    return 0;
}

template <typename ForwardRange1, typename ForwardRange2>
inline
unsigned int edit_distance(ForwardRange1 const& seq1, ForwardRange2 const& seq2) {
    default_cost<ForwardRange1> cost;
    return edit_distance(seq1, seq2, cost);
}



#endif
