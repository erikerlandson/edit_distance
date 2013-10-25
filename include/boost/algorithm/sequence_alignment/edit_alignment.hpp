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
#include <boost/algorithm/sequence_alignment/detail/mpl_utils.hpp>
#include <boost/algorithm/sequence_alignment/detail/edit_alignment.hpp>

namespace boost {
namespace algorithm {
namespace sequence_alignment {

using detail::SequenceAlignmentCost;
using detail::ForwardRangeConvertible;
using detail::dijkstra_sssp_alignment;

template <typename Sequence1, typename Sequence2, typename Output, typename Cost>
BOOST_CONCEPT_REQUIRES(
    ((ForwardRangeConvertible<Sequence1>))
    ((ForwardRangeConvertible<Sequence2>))
    ((SequenceAlignmentCost<Cost>)),
(typename Cost::cost_type))
edit_alignment(Sequence1 const& seq1, Sequence2 const& seq2, Output output, Cost cost) {
    // as_literal() appears to be idempotent, so I tentatively feel OK layering it in here to
    // handle char* transparently, which seems to be working correctly
    return dijkstra_sssp_alignment(boost::as_literal(seq1), boost::as_literal(seq2), output, cost);
    // note to self - in the general case edit distance isn't a symmetric function, depending on
    // the cost matrix
}

template <typename Sequence1, typename Sequence2, typename Output>
inline 
BOOST_CONCEPT_REQUIRES(
    ((ForwardRangeConvertible<Sequence1>))
    ((ForwardRangeConvertible<Sequence2>)),
(typename default_cost<Sequence1>::cost_type))
edit_alignment(Sequence1 const& seq1, Sequence2 const& seq2, Output output) {
    return edit_alignment(seq1, seq2, output, default_cost<Sequence1>());
}

}}}

#endif
