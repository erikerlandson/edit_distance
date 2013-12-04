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

// this requires me to link against boost system lib, which disappoints me,
// since it prevents my algorithms from being pure-template.
// maybe figure out how to use a pure-template pool allocator later
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
using boost::distance;
using boost::range_iterator;

using boost::enable_if;
using boost::is_same;
using boost::mpl::and_;
using std::iterator_traits;
using std::random_access_iterator_tag;

template <typename ForwardRange1, typename ForwardRange2, typename Cost, typename Equal, typename EditBeam, typename AllowSub, typename CostBeam, typename Enabled = void>
struct edit_cost_struct {

// Default is generic edit distance algorithm based on a Dijkstra Single Source Shortest Path approach
typename cost_type<Cost, typename boost::range_value<ForwardRange1>::type>::type
operator()(ForwardRange1 const& seq1, ForwardRange2 const& seq2, const Cost& cost, const Equal& equal, const EditBeam& edit_beam, const AllowSub& allowsub, const CostBeam& cost_beam) const {
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

    // pool allocator for path nodes
    boost::object_pool<head_t> pool;

    // priority queue for path nodes
    boost::heap::skew_heap<head_t*, boost::heap::compare<heap_lessthan<pos1_t, pos2_t> > > heap(heap_lessthan<pos1_t, pos2_t>(beg1, beg2));

    sub_checker<AllowSub, Cost, cost_t, int> allow_sub(allowsub);

    // keep track of nodes in the edit graph that have been visited
    typedef boost::unordered_set<head_t*, visited_hash<pos1_t,pos2_t>, visited_equal> visited_t;
    visited_t visited(31, visited_hash<pos1_t,pos2_t>(beg1,beg2));

    // support edit_beam-width pruning, if asked for
    edit_beam_checker<head_t, EditBeam> on_edit_beam(beg1, beg2, edit_beam);

    cost_beam_checker<head_t, Cost, cost_t, CostBeam> cost_beam_check(beg1, beg2, cost_beam);

    // kick off graph path frontier with initial node:
    heap.push(construct(pool, visited, beg1, beg2, cost_t(0)));

    // update frontier from least-cost node at each iteration, until we hit sequence end
    while (true) {
        head_t* h = heap.top();
        heap.pop();

        // compiles out if edit_beam constraint is not requested
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
                const bool eq = equal(*p1p, *p2p);
                if (!eq  ||  p1 == end1  ||  p2 == end2) {
                    cost_beam_check.update(h->pos1, p1p, p2p, h->cost);
                    head_t* t;
                    if (allow_sub() || eq) {
                        t = construct(pool, visited, p1, p2, h->cost + ((eq) ? 0 : allow_sub.cost_sub(cost, *p1p, *p2p)));
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

}; // edit_cost_struct


template <typename Range1, typename Range2, typename Equal>
struct edit_cost_struct<Range1, Range2, unit_cost, Equal, none, boost::false_type, none, 
                        typename enable_if<and_<is_same<typename iterator_traits<typename range_iterator<Range1>::type>::iterator_category, 
                                                        random_access_iterator_tag>, 
                                                is_same<typename iterator_traits<typename range_iterator<Range2>::type>::iterator_category, 
                                                        random_access_iterator_tag> > >::type> {

template <typename Vec, typename Itr, typename Size, typename Diff> 
inline void expand(Vec& V_data, Itr& V, Size& R, const Diff& D) const {
    Size Rp = R + (R>>1);
    V_data.resize(1 + 2*Rp);
    V = V_data.begin() + R;
    Itr Vp = V_data.begin() + Rp;
    for (Diff j=D;  j >= -D;  --j) Vp[j] = V[j];
    R = Rp;
    V = Vp;
}

// If we are using unit cost for ins/del, with no substitution,
// and if our sequences support random-access,
// and if no beam-searches were enabled,
// *then* we can invoke the efficient and elegant Myers algorithm:
//     An O(ND) Difference Algorithm and its Variations
//     by Eugene W. Myers
//     Dept of Computer Science, University of Arizona
typename cost_type<unit_cost, typename boost::range_value<Range1>::type>::type
operator()(Range1 const& seq1, Range2 const& seq2, const unit_cost&, const Equal& equal, const none&, const boost::false_type&, const none&) const {
    typedef typename range_iterator<Range1 const>::type itr1_t;
    typedef typename range_iterator<Range2 const>::type itr2_t;

    typedef std::vector<int>::difference_type difference_type;
    typedef std::vector<int>::size_type size_type;

    size_type len1 = distance(seq1);
    size_type len2 = distance(seq2);

    itr1_t S1 = begin(seq1);
    itr2_t S2 = begin(seq2);

    size_type R = 10;
    std::vector<size_type> V_data(1 + 2*R);
    std::vector<size_type>::iterator V = V_data.begin() + R;

    difference_type D = 0;
    V[1] = 0;
    while (true) {
        for (difference_type k = -D;  k <= D;  k += 2) {
            difference_type j1 = (k == -D  ||  (k != D  &&  V[k-1] < V[k+1]))  ?  V[k+1]  :  1+V[k-1];
            difference_type j2 = j1-k;
            while (j1 < len1  &&  j2 < len2  &&  equal(S1[j1], S2[j2])) { ++j1;  ++j2; }
            if (j1 >= len1  &&  j2 >= len2) return D;
            V[k] = j1;
        }
        // expand the working vector as needed
        if (++D > R) expand(V_data, V, R, D);
    }

    // control should not reach here
    BOOST_ASSERT(false);
    return 0;
}

}; // edit_cost_struct


template <typename Range1, typename Range2, typename Cost, typename Equal, typename EditBeam, typename AllowSub, typename CostBeam>
inline
typename cost_type<Cost, typename boost::range_value<Range1>::type>::type
edit_cost_impl(Range1 const& seq1, Range2 const& seq2, const Cost& cost, const Equal& equal, const EditBeam& edit_beam, const AllowSub& allowsub, const CostBeam& cost_beam) {
    // specialize the most appropriate implementation for the given parameters
    return edit_cost_struct<Range1, Range2, Cost, Equal, EditBeam, AllowSub, CostBeam>()(seq1, seq2, cost, equal, edit_beam, allowsub, cost_beam);
}


}}}}

#endif
