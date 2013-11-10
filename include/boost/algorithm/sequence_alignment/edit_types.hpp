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


struct unit_cost {
    typedef size_t cost_type;

    template <typename value_type> inline
    cost_type cost_ins(value_type const& a) const {
        return cost_type(1);
    }

    template <typename value_type> inline
    cost_type cost_del(value_type const& a) const {
        return cost_type(1);
    }

    template <typename value_type_1, typename value_type_2> inline
    cost_type cost_sub(value_type_1 const& a, value_type_2 const& b) const {
        return (a == b) ? cost_type(0) : cost_type(1);
    }
};


typedef unit_cost default_cost;


namespace parameter {
    BOOST_PARAMETER_NAME(seq1)
    BOOST_PARAMETER_NAME(seq2)
    BOOST_PARAMETER_NAME(output)
    BOOST_PARAMETER_NAME(cost)
    BOOST_PARAMETER_NAME(beam)
    BOOST_PARAMETER_NAME(allow_sub)
}

}}}

#endif
