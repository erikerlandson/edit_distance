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


template <typename ForwardRange1, typename ForwardRange2, typename Cost, typename Beam>
typename cost_type<Cost, typename boost::range_value<ForwardRange1>::type>::type
dijkstra_sssp_cost(ForwardRange1 const& seq1, ForwardRange2 const& seq2, Cost& cost, Beam beam) {
    typedef typename cost_type<Cost, typename boost::range_value<ForwardRange1>::type>::type cost_t;
    typedef typename range_iterator<ForwardRange1 const>::type itr1_t;
    typedef typename range_iterator<ForwardRange2 const>::type itr2_t;
    typedef path_head<itr1_t, itr2_t, cost_t> head_t;
    typedef typename head_t::pos1_type pos1_t;
    typedef typename head_t::pos2_type pos2_t;

    head_t* const hnull = static_cast<head_t*>(NULL);

    const itr1_t end1 = end(seq1);
    const itr2_t end2 = end(seq2);

    std::set<head_t*, visited_lessthan> visited;

    // pool allocators are born for node allocations in graph algorithms
    boost::object_pool<head_t> pool;

    // is fibonacci heap best here?  O(1) insertion seems well suited.
    boost::heap::fibonacci_heap<head_t*, boost::heap::compare<path_lessthan> > heap;

    // maintain an envelope where we have a known-best cost that
    // offers strong path pruning potential.  Runs of 'equal' 
    // provide this kind of opportunity.
    pos1_t env1;  env1.beg(begin(seq1));
    pos2_t env2;  env2.beg(begin(seq2));
    cost_t env_best_cost = 0;

    // support beam-width pruning, if asked for
    beam_checker<head_t, Beam> on_beam(env1, env2, beam);

    // kick off graph path frontier with initial node:
    heap.push(construct(pool, visited, env1, env2, cost_t(0)));

    // update frontier from least-cost node at each iteration, until we hit sequence end
    while (true) {
        head_t* h = heap.top();
        heap.pop();
        if (!on_beam(h)) {
            // prune all paths that move off the beam
            // unless we are at the end of one of the sequences, in which
            // case going off-beam is the only way to continue when
            // one string is longer than the other and difference > beam
            if (h->pos1 != end1  &&  h->pos2 != end2) continue;
        }
        if (h->pos1 < env1  &&  h->pos2 < env2  &&  h->cost >= env_best_cost) {
            // no edit path from this node can do better than the current
            // known best path, so we can drop this line of exploration
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
                if (csub <= 0) {
                    // on a run of 'eql', updating the 'best path' envelope will help prune
                    // edit paths that cannot improve on it, for potentially big savings
                    if (env1 < p1) {
                        env1 = p1;
                        env_best_cost = h->cost;
                    }
                    if (env2 < p2) {
                        env2 = p2;
                        env_best_cost = h->cost;
                    }
                }
                if (csub > cost_t(0)  ||  p1 == end1  ||  p2 == end2) {
                    head_t* t = construct(pool, visited, p1, p2, h->cost + csub);
                    if (t != hnull) heap.push(t);
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
    return 0;
}

}}}}

#endif
