/*******
edit_distance: STL and Boost compatible edit distance functions for C++

Copyright (c) 2013 Erik Erlandson

Author:  Erik Erlandson <erikerlandson@yahoo.com>

Distributed under the Boost Software License, Version 1.0.
See accompanying file LICENSE or copy at
http://www.boost.org/LICENSE_1_0.txt
*******/

#if !defined(BOOST_ALGORITHM_SEQUENCE_ALIGNMENT_DETAIL_TYPES_HPP)
#define BOOST_ALGORITHM_SEQUENCE_ALIGNMENT_DETAIL_TYPES_HPP

#include <boost/algorithm/sequence_alignment/edit_types.hpp>

namespace boost {
namespace algorithm {
namespace sequence_alignment {
namespace detail {

using boost::algorithm::sequence_alignment::edit_opcode;

template <typename Itr1, typename Itr2, typename Cost>
struct path_head {
    edit_opcode op;
    Itr1 j1;
    Itr2 j2;
    Cost cost;

    path_head() {}
    ~path_head() {}
    path_head(const path_head& src) : op(src.op), j1(src.j1), j2(src.j2), cost(src.cost) {}
};

}}}}

#endif
