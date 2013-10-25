/*******
edit_distance: STL and Boost compatible edit distance functions for C++

Copyright (c) 2013 Erik Erlandson

Author:  Erik Erlandson <erikerlandson@yahoo.com>

Distributed under the Boost Software License, Version 1.0.
See accompanying file LICENSE or copy at
http://www.boost.org/LICENSE_1_0.txt
*******/

#if !defined(BOOST_ALGORITHM_SEQUENCE_ALIGNMENT_DETAIL_EDIT_ALIGNMENT_HPP)
#define BOOST_ALGORITHM_SEQUENCE_ALIGNMENT_DETAIL_EDIT_ALIGNMENT_HPP

#include <boost/pool/object_pool.hpp>

#include <boost/heap/fibonacci_heap.hpp>

#include <boost/range/metafunctions.hpp>

#include <boost/algorithm/sequence_alignment/detail/types.hpp>

namespace boost {
namespace algorithm {
namespace sequence_alignment {
namespace detail {

using boost::begin;
using boost::end;
using boost::range_iterator;

using detail::ForwardRangeConvertible;

using boost::algorithm::sequence_alignment::detail::path_node;
using boost::algorithm::sequence_alignment::detail::construct;
using boost::algorithm::sequence_alignment::detail::path_lessthan;

template <typename ForwardRange1, typename ForwardRange2, typename Output, typename Cost>
typename Cost::cost_type
dijkstra_sssp_alignment(ForwardRange1 const& seq1, ForwardRange2 const& seq2, Output& output, Cost& cost) {
    typedef typename Cost::cost_type cost_t;
    typedef typename range_iterator<ForwardRange1 const>::type itr1_t;
    typedef typename range_iterator<ForwardRange2 const>::type itr2_t;
    typedef path_node<itr1_t, itr2_t, cost_t> head_t;
    head_t* const hnull = static_cast<head_t*>(NULL);

    const itr1_t end1 = end(seq1);
    const itr2_t end2 = end(seq2);

    // pool allocators are born for node allocations in graph algorithms
    boost::object_pool<head_t> pool;

    // is fibonacci heap best here?  O(1) insertion seems well suited.
    boost::heap::fibonacci_heap<head_t*, boost::heap::compare<path_lessthan> > heap;

    // kick off graph path frontier with initial node:
    heap.push(construct(pool, begin(seq1), begin(seq2), cost_t(0), hnull));

    // update frontier from least-cost node at each iteration, until we hit sequence end
    while (true) {
        head_t* h = heap.top();
        heap.pop();
        if (h->j1 == end1) {
            // if we are at end of both sequences, then we have our final edit path:
            if (h->j2 == end2) break;
            // sequence 1 is at end, so only consider insertion from seq2
            itr2_t j2 = h->j2;  ++j2;
            heap.push(construct(pool, h->j1, j2, h->cost + cost.cost_ins(*(h->j2)), h));
       } else if (h->j2 == end2) {
            // sequence 2 is at end, so only consider deletion from seq1
            itr1_t j1 = h->j1;  ++j1;
            heap.push(construct(pool, j1, h->j2, h->cost + cost.cost_del(*(h->j1)), h));            
        } else {
            // interior of both sequences: consider insertion deletion and sub/eql:
            itr1_t j1p = h->j1;
            itr1_t j1 = h->j1;  ++j1;
            itr2_t j2p = h->j2;
            itr2_t j2 = h->j2;  ++j2;
            while (true) {
                cost_t csub = cost.cost_sub(*j1p, *j2p);
                if (csub > cost_t(0)  ||  j1 == end1  ||  j2 == end2) {
                    // Only push end-points of long runs of 'equal'
                    // On sequences where most most elements are same, this saves a ton
                    // of cost from 'pool' and 'heap'.
                    // This clever trick is adapted from:
                    // "An O(ND) Difference Algorithm and Its Variations"
                    // by Eugene W. Myers
                    // Dept of Computer Science, University of Arizona, Tucscon
                    // NSF Grant MCS82-10096
                    heap.push(construct(pool, j1, j2, h->cost + csub, h));
                    heap.push(construct(pool, j1p, j2, h->cost + cost.cost_ins(*j2p), h));
                    heap.push(construct(pool, j1, j2p, h->cost + cost.cost_del(*j1p), h));      
                    break;
                }
                ++j1;  ++j2;  ++j1p;  ++j2p;
            }
        }
    }

    return cost_t(0);
}

}}}}

#endif
