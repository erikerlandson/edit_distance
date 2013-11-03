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

#include <set>

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
using boost::algorithm::sequence_alignment::detail::visited_lessthan;
using boost::algorithm::sequence_alignment::detail::cost_type;

template <typename ForwardRange1, typename ForwardRange2, typename Output, typename Cost>
typename cost_type<Cost, typename boost::range_value<ForwardRange1>::type>::type
dijkstra_sssp_alignment(ForwardRange1 const& seq1, ForwardRange2 const& seq2, Output& output, Cost& cost) {
    typedef typename cost_type<Cost, typename boost::range_value<ForwardRange1>::type>::type cost_t;
    typedef typename range_iterator<ForwardRange1 const>::type itr1_t;
    typedef typename range_iterator<ForwardRange2 const>::type itr2_t;
    typedef path_node<itr1_t, itr2_t, cost_t> head_t;
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

    head_t* path_head = hnull;

    // maintain an envelope where we have a known-best cost that
    // offers strong path pruning potential.  Runs of 'equal' 
    // provide this kind of opportunity.
    pos1_t env1; env1.beg(begin(seq1));
    pos2_t env2; env2.beg(begin(seq2));
    cost_t env_best_cost = 0;

    // kick off graph path frontier with initial node:
    heap.push(construct(pool, visited, env1, env2, cost_t(0), hnull));

    // update frontier from least-cost node at each iteration, until we hit sequence end
    while (true) {
        head_t* h = heap.top();
        heap.pop();
        if (h->pos1 < env1  &&  h->pos2 < env2  &&  h->cost >= env_best_cost) {
            // no edit path from this node can do better than the current
            // known best path, so we can drop this line of exploration
            continue;
        }
        if (h->pos1 == end1) {
            if (h->pos2 == end2) {
                // if we are at end of both sequences, then we have our final edit path:
                path_head = h;
                break;
            }
            // sequence 1 is at end, so only consider insertion from seq2
            pos2_t p2 = h->pos2;
            head_t* t = construct(pool, visited, h->pos1, ++p2, h->cost + cost.cost_ins(*(h->pos2)), h);
            if (t != hnull) heap.push(t);
       } else if (h->pos2 == end2) {
            // sequence 2 is at end, so only consider deletion from seq1
            pos1_t p1 = h->pos1;
            head_t* t = construct(pool, visited, ++p1, h->pos2, h->cost + cost.cost_del(*(h->pos1)), h);
            if (t != hnull) heap.push(t);
        } else {
            // interior of both sequences: consider insertion deletion and sub/eql:
            pos1_t p1 = h->pos1;  ++p1;
            pos1_t p1p = h->pos1;
            pos2_t p2 = h->pos2;  ++p2;
            pos2_t p2p = h->pos2;
            while (true) {
                cost_t csub = cost.cost_sub(*p1p, *p2p);
                head_t* t = construct(pool, visited, p1, p2, h->cost + csub, h);
                if (t != hnull  &&  csub <= 0) {
                    // on a run of 'eql', updating the 'best path' envelope will help prune
                    // edit paths that cannot improve on it, for potentially big savings
                    if (env1 < t->pos1) {
                        env1 = t->pos1;
                        env_best_cost = t->cost;
                    }
                    if (env2 < t->pos2) {
                        env2 = t->pos2;
                        env_best_cost = t->cost;
                    }
                }
                if (csub > cost_t(0)  ||  p1 == end1  ||  p2 == end2) {
                    if (t != hnull) heap.push(t);
                    t = construct(pool, visited, p1p, p2, h->cost + cost.cost_ins(*p2p), h);
                    if (t != hnull) heap.push(t);
                    t = construct(pool, visited, p1, p2p, h->cost + cost.cost_del(*p1p), h);
                    if (t != hnull) heap.push(t);
                    break;
                }
                ++p1;  ++p2;  ++p1p;  ++p2p;
            }
        }
    }

    const cost_t edit_cost = path_head->cost;

    // trace back from the head, reversing as we go
    head_t* ncur = path_head;
    head_t* nprv = hnull;
    while (true) {
        head_t* nnxt = ncur->edge;
        ncur->edge = nprv;
        if (nnxt == hnull) {
            // now path head points to edit sequence beginning
            path_head = ncur;
            break;
        }
        nprv = ncur;
        ncur = nnxt;
    }

    // now traverse the edit path, from the beginning forward
    for (head_t* n = path_head;  n->edge != hnull;  n = n->edge) {
        itr1_t j1 = n->pos1.j;
        itr1_t j1end = n->edge->pos1.j;
        itr2_t j2 = n->pos2.j;
        itr2_t j2end = n->edge->pos2.j;

        if (j1 == j1end) {
            // seq1 didn't advance, this is an insertion from seq2
            output.output_ins(*j2, n->edge->cost - n->cost);
            continue;
        }    
        if (j2 == j2end) {
            // seq2 didn't advance, this is a deletion from seq1
            output.output_del(*j1, n->edge->cost - n->cost);
            continue;
        }

        // if we arrived here, this is either:
        // 1) a subsitution
        // 2) 1 or more 'equal'
        // 3) 1 or more 'equal', followed by a sub, ins or del
        
        itr1_t j1x = j1;  ++j1x;
        itr2_t j2x = j2;  ++j2x;

        while (j1x != j1end  &&  j2x != j2end) {
            // unpack any compressed runs of 'eql'
            output.output_eql(*j1, *j2);
            ++j1;  ++j2;  ++j1x;  ++j2x;
        }
        if (j1x == j1end) {
            if (j2x == j2end) {
                cost_t c = n->edge->cost - n->cost;
                if (c > cost_t(0)) {
                    output.output_sub(*j1, *j2, c);
                } else {
                    output.output_eql(*j1, *j2);
                }
            } else {
                output.output_eql(*j1, *j2);
                output.output_ins(*j2x, n->edge->cost - n->cost);
            }
        } else {
            // j1x != j1end  and  j2x == j2end
            output.output_eql(*j1, *j2);
            output.output_del(*j1x, n->edge->cost - n->cost);
        }
    }

    return edit_cost;
}

}}}}

#endif
