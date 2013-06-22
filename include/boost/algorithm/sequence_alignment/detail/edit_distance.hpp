/*******
edit_distance: STL and Boost compatible edit distance functions for C++

Copyright (c) 2013 Erik Erlandson

Author:  Erik Erlandson <erikerlandson@yahoo.com>

Distributed under the Boost Software License, Version 1.0.
See accompanying file LICENSE or copy at
http://www.boost.org/LICENSE_1_0.txt
*******/


#if !defined(BOOST_ALGORITHM_SEQUENCE_ALIGNMENT_DETAIL_EDIT_DISTANCE_HPP)
#define BOOST_ALGORITHM_SEQUENCE_ALIGNMENT_DETAIL_EDIT_DISTANCE_HPP

#include <boost/mpl/has_xxx.hpp>
#include <boost/mpl/if.hpp>
#include <boost/mpl/vector.hpp>
#include <boost/mpl/int.hpp>
#include <boost/mpl/fold.hpp>
#include <boost/mpl/sort.hpp>
#include <boost/mpl/unique.hpp>

#include <boost/concept/requires.hpp>
#include <boost/concept/usage.hpp>
#include <boost/concept/assert.hpp>
#include <boost/type_traits/is_arithmetic.hpp>
#include <boost/type_traits/is_same.hpp>

#include <boost/range/as_literal.hpp>
#include <boost/range/as_array.hpp>
#include <boost/range/functions.hpp>
#include <boost/range/metafunctions.hpp>

#include <boost/multi_array.hpp>

#include <boost/algorithm/sequence_alignment/detail/mpl_utils.hpp>

namespace boost {
namespace algorithm {
namespace sequence_alignment {
namespace detail {

using boost::distance;
using boost::begin;
using boost::end;
using boost::range_iterator;
using boost::range_value;
using boost::range_reference;

template <typename ForwardRange1, typename ForwardRange2, typename Cost>
typename Cost::cost_type 
needleman_wunsch_distance(ForwardRange1 const& seq1, ForwardRange2 const& seq2, Cost& cost) {
    typedef typename Cost::cost_type cost_t;
    typedef typename range_iterator<ForwardRange1 const>::type itr1_t;
    typedef typename range_iterator<ForwardRange2 const>::type itr2_t;
    typedef boost::multi_array<cost_t, 1> cost_array_t;
    typedef typename cost_array_t::iterator itrc_t;
    typename cost_array_t::size_type len2 = distance(seq2);
    cost_array_t cv1(boost::extents[1+len2]);
    cost_array_t cv2(boost::extents[1+len2]);
    cost_array_t* cur = &cv1;
    cost_array_t* prv = &cv2;
    itr2_t j2 = begin(seq2);
    itrc_t c00 = cur->begin();
    itrc_t c01 = c00;
    *c01 = 0;
    for (++c00;  c00 != cur->end();  ++c00, ++c01, ++j2) {
        *c00 = *c01 + cost.cost_ins(*j2);
    }
    for (itr1_t j1 = begin(seq1);  j1 != end(seq1);  ++j1) {
        std::swap(cur, prv);
        j2 = begin(seq2);
        c00 = cur->begin();
        c01 = c00;
        itrc_t c10 = prv->begin();
        itrc_t c11 = c10;
        *c01 = *c11 + cost.cost_del(*j1);
        for (++c00, ++c10;  c00 != cur->end();  ++c00, ++c01, ++c10, ++c11, ++j2) {
            cost_t c = *c01 + cost.cost_ins(*j2);
            c = std::min(c, *c10 + cost.cost_del(*j1));
            c = std::min(c, *c11 + cost.cost_sub(*j1, *j2));
            *c00 = c;
        }
    }
    return *c01;
}

}}}}

#endif
