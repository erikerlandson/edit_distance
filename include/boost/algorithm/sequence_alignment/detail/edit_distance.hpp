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

#include <set>

// this requires me to link against boost system lib, which disappoints me,
// since it prevents my algorithms from being pure-template.
// maybe figure out how to use a pure-template pool allocator later
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


template <typename ForwardRange1, typename ForwardRange2, typename Cost, typename Beam, typename AllowSub, typename PruneBias>
typename cost_type<Cost, typename boost::range_value<ForwardRange1>::type>::type
dijkstra_sssp_cost(ForwardRange1 const& seq1, ForwardRange2 const& seq2, const Cost& cost, const Beam& beam, const AllowSub& allowsub, const PruneBias& prune_bias) {
    typedef typename cost_type<Cost, typename boost::range_value<ForwardRange1>::type>::type cost_t;
    typedef typename range_iterator<ForwardRange1 const>::type itr1_t;
    typedef typename range_iterator<ForwardRange2 const>::type itr2_t;
    typedef path_head<itr1_t, itr2_t, cost_t> head_t;
    typedef typename head_t::pos1_type pos1_t;
    typedef typename head_t::pos2_type pos2_t;

    head_t* const hnull = static_cast<head_t*>(NULL);

    const itr1_t end1 = end(seq1);
    const itr2_t end2 = end(seq2);
    pos1_t beg1;  beg1.beg(begin(seq1));
    pos2_t beg2;  beg2.beg(begin(seq2));

    // pool allocators are born for node allocations in graph algorithms
    boost::object_pool<head_t> pool;

    // is fibonacci heap best here?  O(1) insertion seems well suited.
    boost::heap::fibonacci_heap<head_t*, boost::heap::compare<heap_lessthan<pos1_t, pos2_t> > > heap(heap_lessthan<pos1_t, pos2_t>(beg1, beg2));

    sub_checker<AllowSub> allow_sub(allowsub);

    // keep track of nodes in the edit graph that have been visited
    typedef boost::unordered_set<head_t*, visited_hash<pos1_t,pos2_t>, visited_equal> visited_t;
    visited_t visited(31, visited_hash<pos1_t,pos2_t>(beg1,beg2));

    // support beam-width pruning, if asked for
    beam_checker<head_t, Beam> on_beam(beg1, beg2, beam);

    env_pruner<head_t, Cost, cost_t, PruneBias> env_prune(beg1, beg2, prune_bias);

    // kick off graph path frontier with initial node:
    heap.push(construct(pool, visited, beg1, beg2, cost_t(0)));

    // update frontier from least-cost node at each iteration, until we hit sequence end
    while (true) {
        head_t* h = heap.top();
        heap.pop();

        // compiles out if beam constraint is not requested
        if (!on_beam(h)) {
            // prune all paths that move off the beam
            // unless we are at the end of one of the sequences, in which
            // case going off-beam is the only way to continue when
            // one string is longer than the other and difference > beam
            if (h->pos1 != end1  &&  h->pos2 != end2) continue;
        }

        // compiles out if envelope pruning isn't requested
        if (env_prune(h)) {
            // prune pathways inside the current envelope that do not look promising
            // this is a heuristic: it can result in non-minimum edit path
            continue;
        }

        if (h->pos1 == end1) {
            // if we are at end of both sequences, then we have our final cost: 
            if (h->pos2 == end2) return h->cost;
            // sequence 1 is at end, so only consider insertion from seq2
            pos2_t p2 = h->pos2;
            head_t* t = construct(pool, visited, h->pos1, ++p2, h->cost + cost.cost_ins(*(h->pos2)));
            if (t != hnull) heap.push(t);
        } else if (h->pos2 == end2) {
            // sequence 2 is at end, so only consider deletion from seq1
            pos1_t p1 = h->pos1;
            head_t* t = construct(pool, visited, ++p1, h->pos2, h->cost + cost.cost_del(*(h->pos1)));
            if (t != hnull) heap.push(t);
        } else {
            // interior of both sequences: consider insertion deletion and sub/eql:
            pos1_t p1 = h->pos1;  ++p1;
            pos1_t p1p = h->pos1;
            pos2_t p2 = h->pos2;  ++p2;
            pos2_t p2p = h->pos2;
            while (true) {
                cost_t csub = cost.cost_sub(*p1p, *p2p);
                if (csub > cost_t(0)  ||  p1 == end1  ||  p2 == end2) {
                    env_prune.update(h->pos1, p1p, p2p, h->cost);
                    head_t* t;
                    if (allow_sub() || (csub <= 0)) {
                        t = construct(pool, visited, p1, p2, h->cost + csub);
                        if (t != hnull) heap.push(t);
                    }
                    t = construct(pool, visited, p1p, p2, h->cost + cost.cost_ins(*p2p));
                    if (t != hnull) heap.push(t);
                    t = construct(pool, visited, p1, p2p, h->cost + cost.cost_del(*p1p));
                    if (t != hnull) heap.push(t);
                    break;
                }
                ++p1;  ++p2;  ++p1p;  ++p2p;
            }
        }
    }

    // control should not reach here
    BOOST_ASSERT(false);
    return 0;
}

}}}}

#endif
