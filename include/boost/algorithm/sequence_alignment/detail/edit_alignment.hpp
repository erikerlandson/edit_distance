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

#include <boost/heap/skew_heap.hpp>

#include <boost/range/metafunctions.hpp>

#include <boost/algorithm/sequence_alignment/detail/types.hpp>

namespace boost {
namespace algorithm {
namespace sequence_alignment {
namespace detail {

using boost::begin;
using boost::end;
using boost::range_iterator;


template <typename ForwardRange1, typename ForwardRange2, typename Output, typename Cost, typename EditBeam, typename AllowSub, typename CostBeam>
typename cost_type<Cost, typename boost::range_value<ForwardRange1>::type>::type
dijkstra_sssp_alignment(ForwardRange1 const& seq1, ForwardRange2 const& seq2, Output& output, const Cost& cost, const EditBeam& edit_beam, const AllowSub& allowsub, const CostBeam& cost_beam) {
    typedef typename cost_type<Cost, typename boost::range_value<ForwardRange1>::type>::type cost_t;
    typedef typename range_iterator<ForwardRange1 const>::type itr1_t;
    typedef typename range_iterator<ForwardRange2 const>::type itr2_t;
    typedef path_node<itr1_t, itr2_t, cost_t> head_t;
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
    boost::heap::skew_heap<head_t*, boost::heap::compare<heap_lessthan<pos1_t, pos2_t> > > heap(heap_lessthan<pos1_t, pos2_t>(beg1, beg2));

    sub_checker<AllowSub> allow_sub(allowsub);

    head_t* path_head = hnull;

    // keep track of nodes in the edit graph that have been visited
    typedef boost::unordered_set<head_t*, visited_hash<pos1_t,pos2_t>, visited_equal> visited_t;
    visited_t visited(31, visited_hash<pos1_t,pos2_t>(beg1,beg2));

    // support edit_beam-width pruning, if asked for
    edit_beam_checker<head_t, EditBeam> on_edit_beam(beg1, beg2, edit_beam);

    cost_beam_checker<head_t, Cost, cost_t, CostBeam> cost_beam_check(beg1, beg2, cost_beam);

    // kick off graph path frontier with initial node:
    heap.push(construct(pool, visited, beg1, beg2, cost_t(0), hnull));

    // update frontier from least-cost node at each iteration, until we hit sequence end
    while (true) {
        head_t* h = heap.top();
        heap.pop();
        if (!on_edit_beam(h)) {
            // prune all paths that move off the edit_beam
            // unless we are at the end of one of the sequences, in which
            // case going off-edit_beam is the only way to continue when
            // one string is longer than the other and difference > edit_beam
            if (h->pos1 != end1  &&  h->pos2 != end2) continue;
        }

        // compiles out if envelope pruning isn't requested
        if (cost_beam_check(h)) {
            // prune pathways inside the current envelope that do not look promising
            // this is a heuristic: it can result in non-minimum edit path
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
                if (csub > cost_t(0)  ||  p1 == end1  ||  p2 == end2) {
                    cost_beam_check.update(h->pos1, p1p, p2p, h->cost);
                    head_t* t;
                    if (allow_sub() || (csub <= 0)) {
                        t = construct(pool, visited, p1, p2, h->cost + csub, h);
                        if (t != hnull) heap.push(t);
                    }
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
