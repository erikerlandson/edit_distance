/*******
edit_distance: STL and Boost compatible edit distance functions for C++

Copyright (c) 2013 Erik Erlandson

Author:  Erik Erlandson <erikerlandson@yahoo.com>

Distributed under the Boost Software License, Version 1.0.
See accompanying file LICENSE or copy at
http://www.boost.org/LICENSE_1_0.txt
*******/

#if !defined(BOOST_ALGORITHM_SEQUENCE_ALIGNMENT_EDIT_TYPES_HPP)
#define BOOST_ALGORITHM_SEQUENCE_ALIGNMENT_EDIT_TYPES_HPP

#include <cstddef>

#include <boost/range/metafunctions.hpp>

#include <boost/parameter/name.hpp>
#include <boost/parameter/preprocessor.hpp>

namespace boost {
namespace algorithm {
namespace sequence_alignment {


struct default_cost {
    typedef size_t cost_type;

    template <typename value_type> inline
    cost_type cost_ins(value_type const& a) const {
        return cost_type(1);
    }

    template <typename value_type> inline
    cost_type cost_del(value_type const& a) const {
        return cost_type(1);
    }

    template <typename value_type> inline
    cost_type cost_sub(value_type const& a, value_type const& b) const {
        return (a == b) ? cost_type(0) : cost_type(1);
    }
};


}}}

#endif
