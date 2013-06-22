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

#include <boost/range/metafunctions.hpp>

namespace boost {
namespace algorithm {
namespace sequence_alignment {


template <typename Range>
struct default_cost {
    // interesting that these appear to work correctly when Range = char*
    typedef typename boost::range_difference<Range>::type cost_type;
    typedef typename boost::range_value<Range>::type value_type;
    cost_type cost_ins(value_type const& a) const {
        return cost_type(1);
    }
    cost_type cost_del(value_type const& a) const {
        return cost_type(1);
    }
    cost_type cost_sub(value_type const& a, value_type const& b) const {
        return (a == b) ? cost_type(0) : cost_type(1);
    }
};

typedef char edit_opcode;
const edit_opcode ins_op = '+';
const edit_opcode del_op = '-';
const edit_opcode sub_op = ':';
const edit_opcode eql_op = '=';

typedef boost::mpl::int_<1> costs;
typedef boost::mpl::int_<2> indexes;
typedef boost::mpl::int_<3> elements;

struct parameter_list_is_unimplemented {};

}}}

#endif
