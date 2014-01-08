/*******
edit_distance: STL and Boost compatible edit distance functions for C++

Copyright (c) 2013 Erik Erlandson

Author:  Erik Erlandson <erikerlandson@yahoo.com>

Distributed under the Boost Software License, Version 1.0.
See accompanying file LICENSE or copy at
http://www.boost.org/LICENSE_1_0.txt
*******/


#if !defined(BOOST_ALGORITHM_SEQUENCE_EDIT_DISTANCE_HPP)
#define BOOST_ALGORITHM_SEQUENCE_EDIT_DISTANCE_HPP

#include <boost/concept/requires.hpp>

#include <boost/algorithm/sequence/detail/edit_distance.hpp>

namespace boost {
namespace algorithm {
namespace sequence {

using boost::as_literal;
using boost::parameter::value_type;
using boost::range_value;
using boost::false_type;

using detail::SequenceAlignmentCost;
using detail::ForwardRangeConvertible;
using detail::edit_cost_impl;
using detail::cost_type;
using detail::none;
using detail::nonconst_default;
using detail::default_equal;


template <typename Sequence1, typename Sequence2, typename Output, typename Cost, typename Equal, typename AllowSub, typename MaxCost>
inline
BOOST_CONCEPT_REQUIRES(
    ((ForwardRangeConvertible<Sequence1>))
    ((ForwardRangeConvertible<Sequence2>))
    ((SequenceAlignmentCost<Cost, Sequence1>)),
(typename cost_type<Cost, typename range_value<Sequence1>::type>::type))
edit_distance_check(Sequence1 const& seq1, Sequence2 const& seq2, Output& output, const Cost& cost, const Equal& equal, const AllowSub& allow_sub, const MaxCost& max_cost, const bool max_cost_exception) {
    return edit_cost_impl(as_literal(seq1), as_literal(seq2), output, cost, equal, allow_sub, max_cost, max_cost_exception);
}


BOOST_PARAMETER_FUNCTION(
(typename cost_type<typename value_type<Args, parameter::tag::cost, unit_cost>::type,
                    typename range_value<typename value_type<Args, parameter::tag::sequence1>::type>::type>::type),
    edit_distance,
    parameter::tag,
    (required
        (sequence1, *)
        (sequence2, *))
    (optional
        (cost, *, unit_cost())
        (equal, *, default_equal())
        (in_out(script), *, nonconst_default<none>())
        (substitution, *, false_type())
        (max_cost, *, none())
        (max_cost_exception, (bool), false))
)
{
    return edit_distance_check(sequence1, sequence2, script, cost, equal, substitution, max_cost, max_cost_exception);
}


}}} // boost::algorithm::sequence

#endif
