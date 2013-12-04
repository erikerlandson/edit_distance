/*******
edit_distance: STL and Boost compatible edit distance functions for C++

Copyright (c) 2013 Erik Erlandson

Author:  Erik Erlandson <erikerlandson@yahoo.com>

Distributed under the Boost Software License, Version 1.0.
See accompanying file LICENSE or copy at
http://www.boost.org/LICENSE_1_0.txt
*******/


#if !defined(BOOST_ALGORITHM_SEQUENCE_ALIGNMENT_EDIT_DISTANCE_HPP)
#define BOOST_ALGORITHM_SEQUENCE_ALIGNMENT_EDIT_DISTANCE_HPP

#include <boost/concept/requires.hpp>

#include <boost/algorithm/sequence_alignment/detail/types.hpp>
#include <boost/algorithm/sequence_alignment/detail/edit_distance.hpp>

namespace boost {
namespace algorithm {
namespace sequence_alignment {

using boost::as_literal;
using boost::parameter::value_type;
using boost::range_value;
using boost::false_type;

using detail::SequenceAlignmentCost;
using detail::ForwardRangeConvertible;
using detail::edit_cost_impl;
using detail::cost_type;
using detail::none;
using detail::default_equal;


template <typename Sequence1, typename Sequence2, typename Cost, typename Equal, typename EditBeam, typename AllowSub, typename CostBeam>
inline
BOOST_CONCEPT_REQUIRES(
    ((ForwardRangeConvertible<Sequence1>))
    ((ForwardRangeConvertible<Sequence2>))
    ((SequenceAlignmentCost<Cost, Sequence1>)),
(typename cost_type<Cost, typename range_value<Sequence1>::type>::type))
edit_distance_check(Sequence1 const& seq1, Sequence2 const& seq2, const Cost& cost, const Equal& equal, const EditBeam& edit_beam, const AllowSub& allow_sub, const CostBeam& cost_beam) {
    return edit_cost_impl(as_literal(seq1), as_literal(seq2), cost, equal, edit_beam, allow_sub, cost_beam);
}

BOOST_PARAMETER_FUNCTION(
(typename cost_type<typename value_type<Args, parameter::tag::cost, unit_cost>::type,
                    typename range_value<typename value_type<Args, parameter::tag::seq1>::type>::type>::type),
    edit_distance,
    parameter::tag,
    (required
        (seq1, *)
        (seq2, *))
    (optional
        (cost, *, unit_cost())
        (equal, *, default_equal())
        (allow_sub, *, false_type())
        (edit_beam, *, none())
        (cost_beam, *, none()))
)
{
    return edit_distance_check(seq1, seq2, cost, equal, edit_beam, allow_sub, cost_beam);
}


}}} // boost::algorithm::sequence_alignment

#endif
