/*******
edit_distance: STL and Boost compatible edit distance functions for C++

Copyright (c) 2013 Erik Erlandson

Author:  Erik Erlandson <erikerlandson@yahoo.com>

Distributed under the Boost Software License, Version 1.0.
See accompanying file LICENSE or copy at
http://www.boost.org/LICENSE_1_0.txt
*******/

#if !defined(BOOST_ALGORITHM_SEQUENCE_ALIGNMENT_EDIT_ALIGNMENT_HPP)
#define BOOST_ALGORITHM_SEQUENCE_ALIGNMENT_EDIT_ALIGNMENT_HPP

#include <boost/concept/requires.hpp>

#include <boost/algorithm/sequence_alignment/edit_types.hpp>
#include <boost/algorithm/sequence_alignment/detail/types.hpp>
#include <boost/algorithm/sequence_alignment/detail/edit_alignment.hpp>

namespace boost {
namespace algorithm {
namespace sequence_alignment {

using detail::SequenceAlignmentCost;
using detail::ForwardRangeConvertible;
using detail::dijkstra_sssp_alignment;
using detail::cost_type;
using detail::none;

template <typename Sequence1, typename Sequence2, typename Output, typename Cost, typename Beam>
BOOST_CONCEPT_REQUIRES(
    ((ForwardRangeConvertible<Sequence1>))
    ((ForwardRangeConvertible<Sequence2>))
    ((SequenceAlignmentCost<Cost, Sequence1>)),
(typename cost_type<Cost, typename boost::range_value<Sequence1>::type>::type))
edit_alignment_check(Sequence1 const& seq1, Sequence2 const& seq2, Output& output, Cost cost, Beam beam) {
    // as_literal() appears to be idempotent, so I tentatively feel OK layering it in here to
    // handle char* transparently, which seems to be working correctly
    return dijkstra_sssp_alignment(boost::as_literal(seq1), boost::as_literal(seq2), output, cost, beam);
    // note to self - in the general case edit distance isn't a symmetric function, depending on
    // the cost matrix
}

namespace edit_alignment_params {
BOOST_PARAMETER_NAME(seq1)
BOOST_PARAMETER_NAME(seq2)
BOOST_PARAMETER_NAME(output)
BOOST_PARAMETER_NAME(cost)
BOOST_PARAMETER_NAME(beam)
}

BOOST_PARAMETER_FUNCTION(
(typename cost_type<typename boost::parameter::value_type<Args, edit_alignment_params::tag::cost, default_cost>::type, 
                    typename boost::range_value<typename boost::parameter::value_type<Args, edit_alignment_params::tag::seq1>::type>::type>::type),
    edit_alignment,
    edit_alignment_params::tag,
    (required
        (seq1, *)
        (seq2, *)
        (in_out(output), *))
    (optional
        (cost, *, default_cost())
        (beam, *, none()))
)
{
    return edit_alignment_check(seq1, seq2, output, cost, beam);
}


}}}

#endif
