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
using boost::distance;
using boost::range_iterator;

using boost::enable_if;
using boost::is_same;
using boost::mpl::and_;
using std::iterator_traits;
using std::random_access_iterator_tag;
using boost::make_tuple;

template <typename ForwardRange1, typename ForwardRange2, typename Output, typename Cost, typename Equal, typename AllowSub, typename MaxCost, typename EditBeam, typename CostBeam, typename Enabled = void>
struct edit_path_struct {

typename cost_type<Cost, typename boost::range_value<ForwardRange1>::type>::type
operator()(ForwardRange1 const& seq1, ForwardRange2 const& seq2, Output& output, const Cost& cost, const Equal& equal, const AllowSub& allowsub, const MaxCost& max_cost, const bool max_cost_exception, const EditBeam& edit_beam, const CostBeam& cost_beam) {
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

    // pool allocator for path nodes
    boost::object_pool<head_t> pool;

    // priority queue for path nodes
    boost::heap::skew_heap<head_t*, boost::heap::compare<heap_lessthan<pos1_t, pos2_t> > > heap(heap_lessthan<pos1_t, pos2_t>(beg1, beg2));

    sub_checker<AllowSub, Cost, cost_t, Output> allow_sub(allowsub);

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
                const bool eq = equal(*p1p, *p2p);
                if (!eq  ||  p1 == end1  ||  p2 == end2) {
                    cost_beam_check.update(h->pos1, p1p, p2p, h->cost);
                    head_t* t;
                    if (allow_sub() || eq) {
                        t = construct(pool, visited, p1, p2, h->cost + ((eq) ? 0 : allow_sub.cost_sub(cost, *p1p, *p2p)), h);
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

        itr1_t j1x = j1;  ++j1x;
        itr2_t j2x = j2;  ++j2x;

        while (j1x != j1end  &&  j2x != j2end) {
            // unpack any compressed runs of 'eql'
            output.output_eql(*j1, *j2);
            ++j1;  ++j2;  ++j1x;  ++j2x;
        }
        if (j1x == j1end) {
            if (j2x == j2end) {
                if (equal(*j1, *j2)) {
                    output.output_eql(*j1, *j2);
                } else {
                    allow_sub.output_sub(output, *j1, *j2, n->edge->cost - n->cost);
                }
            } else {
                output.output_eql(*j1, *j2);
                output.output_ins(*j2x, n->edge->cost - n->cost);
            }
        } else {
            output.output_eql(*j1, *j2);
            output.output_del(*j1x, n->edge->cost - n->cost);
        }
    }

    return edit_cost;
}

}; // edit_path_struct

template <typename Range1, typename Range2, typename Output, typename Equal, typename MaxCost>
struct edit_path_struct<Range1, Range2, Output, unit_cost, Equal, boost::false_type, MaxCost, none, none, 
                        typename enable_if<and_<is_same<typename iterator_traits<typename range_iterator<Range1>::type>::iterator_category, 
                                                        random_access_iterator_tag>, 
                                                is_same<typename iterator_traits<typename range_iterator<Range2>::type>::iterator_category, 
                                                        random_access_iterator_tag> > >::type> {

typedef typename range_iterator<Range1 const>::type itr1_t;
typedef typename range_iterator<Range2 const>::type itr2_t;

typedef std::vector<int>::difference_type diff_type;
typedef size_t size_type;

template <typename Vec, typename Itr> 
inline void expand(Vec& V_data, Itr& Vf, Itr& Vr, size_type& R, const diff_type& D, const diff_type& delta) const {
    size_type Rp = R + (R>>1);
    V_data.resize(2 + 4*Rp);
    Vf = V_data.begin() + R;
    Vr = V_data.begin() + (3*R+1) - delta;

    Itr Vp = V_data.begin() + (3*Rp+1) - delta;
    for (diff_type j=D+delta;  j >= -D+delta;  --j) Vp[j] = Vr[j];
    Vr = Vp;

    Vp = V_data.begin() + Rp;
    for (diff_type j=D;  j >= -D;  --j) Vp[j] = Vf[j];
    Vf = Vp;

    R = Rp;
}


typename cost_type<unit_cost, typename boost::range_value<Range1>::type>::type
path(const itr1_t& seq1, const size_type& len1, const itr2_t& seq2, const size_type& len2, const Equal& equal, const MaxCost& max_cost, const bool max_cost_exception, Output& output, std::vector<diff_type>& V_data) const {
    // identify any equal suffix and/or prefix
    diff_type eqb = 0;
    for (;  eqb < std::min(len1, len2);  ++eqb) if (!equal(seq1[eqb],seq2[eqb])) break;
    diff_type eqe = len1-1;
    for (diff_type j2 = len2-1;  eqe > eqb && j2 > eqb;  --eqe,--j2) if (!equal(seq1[eqe],seq2[j2])) break;
    eqe = len1-1-eqe;

    // sub-strings with equal suffix and/or prefix stripped
    const itr1_t S1 = seq1 + eqb;
    const diff_type L1 = len1-(eqb+eqe);
    const itr2_t S2 = seq2 + eqb;
    const diff_type L2 = len2-(eqb+eqe);

    // basis case: either or both strings are empty:
    if (L1 <= 0) {
        for (diff_type j = 0;  j < eqb;  ++j) output.output_eql(seq1[j], seq2[j]);
        for (diff_type j = 0;  j < L2;  ++j) output.output_ins(S2[j], 1);
        for (diff_type j1=len1-eqe, j2=len2-eqe; j1 < len1; ++j1,++j2) output.output_eql(seq1[j1], seq2[j2]);
        return L2;
    }
    if (L2 <= 0) {
        for (diff_type j = 0;  j < eqb;  ++j) output.output_eql(seq1[j], seq2[j]);
        for (diff_type j = 0;  j < L1;  ++j) output.output_del(S1[j], 1);
        for (diff_type j1=len1-eqe, j2=len2-eqe; j1 < len1; ++j1,++j2) output.output_eql(seq1[j1], seq2[j2]);
        return L1;
    }

    const diff_type delta = L1-L2;
    const bool delta_even = delta%2 == 0;

    // set up 'V' vectors for forward and reverse edit path diagonals
    // note, these are maintained to allow negative indexes
    if (V_data.size() <= 0) V_data.resize(2*(1+2*10));
    size_type R = V_data.size()/4;
    std::vector<diff_type>::iterator Vf = V_data.begin()+R;
    std::vector<diff_type>::iterator Vr = V_data.begin()+(3*R+1)-delta;

    // midpoint run of equal elements ("snake")
    diff_type r1b, r2b, r1e, r2e;

    diff_type D = 0;
    Vf[1] = 0;
    Vr[-1+delta] = L1;
    bool found = false;
    while (true) {
        // advance the forward-path diagonals:
        for (diff_type k = -D;  k <= D;  k += 2) {
            diff_type j1 = (k == -D  ||  (k != D  &&  Vf[k-1] < Vf[k+1]))  ?  Vf[k+1]  :  1+Vf[k-1];
            diff_type j2 = j1-k;
            r1b = j1;
            r2b = j2;
            while (j1 < L1  &&  j2 < L2  &&  equal(S1[j1], S2[j2])) { ++j1;  ++j2; }

            if (!delta_even  &&  (k-delta) >= -(D-1)  &&  (k-delta) <= (D-1)) {
                diff_type r1 = Vr[k];
                diff_type r2 = Vr[k]-k;
                if ((r1b-r2b) == (r1-r2)  &&  r1b >= r1) {
                    r1e = j1;
                    r2e = j2;
                    found = true;
                    break;
                }
            }

            Vf[k] = j1;
        }
        if (found) {
            D = 2*D - 1;
            break;
        }

        // advance the reverse-path diagonals:
        for (diff_type k = -D+delta;  k <= D+delta;  k += 2) {
            diff_type j1 = (k == D+delta  ||  (k != -D+delta  &&  Vr[k-1] < Vr[k+1]))  ?  Vr[k-1]  :  Vr[k+1]-1;
            diff_type j2 = j1-k;
            r1e = j1;
            r2e = j2;
            while (j1 > 0  &&  j2 > 0  &&  equal(S1[j1-1], S2[j2-1])) { --j1;  --j2; }

            if (delta_even  &&  k >= -D  &&  k <= D) {
                diff_type f1 = Vf[k];   
                diff_type f2 = Vf[k]-k;
                if ((r1e-r2e) == (f1-f2)  &&  f1 >= r1e) {
                    r1b = j1;
                    r2b = j2;
                    found = true;
                    break;
                }
            }

            Vr[k] = j1;
        }
        if (found) {
            D = 2*D;
            break;
        }

        // expand the working vectors as needed
        if (++D > R) expand(V_data, Vf, Vr, R, D, delta);
    }

    // output for equal prefix:
    for (diff_type j = 0;  j < eqb;  ++j) output.output_eql(seq1[j], seq2[j]);
    // output for path up to midpoint snake:
    path(S1, r1b, S2, r2b, equal, max_cost, max_cost_exception, output, V_data);
    // output for midpoint snake:
    for (diff_type j1=r1b,j2=r2b; j1 < r1e;  ++j1, ++j2) output.output_eql(S1[j1], S2[j2]);
    // output for path from midpoint to end:
    path(S1+r1e, L1-r1e, S2+r2e, L2-r2e, equal, max_cost, max_cost_exception, output, V_data);
    // output for equal suffix:
    for (diff_type j1=len1-eqe, j2=len2-eqe; j1 < len1; ++j1,++j2) output.output_eql(seq1[j1], seq2[j2]);

    return D;
}

inline
typename cost_type<unit_cost, typename boost::range_value<Range1>::type>::type
operator()(Range1 const& seq1, Range2 const& seq2, Output& output, const unit_cost&, const Equal& equal, const boost::false_type&, const MaxCost& max_cost, const bool max_cost_exception, const none&, const none&) const {
    typedef std::vector<int>::difference_type diff_type;
    std::vector<diff_type> V_data;
    return path(begin(seq1), distance(seq1), begin(seq2), distance(seq2), equal, max_cost, max_cost_exception, output, V_data);
}

}; // edit_path_struct


template <typename Range1, typename Range2, typename Output, typename Cost, typename Equal, typename AllowSub, typename MaxCost, typename EditBeam, typename CostBeam>
inline
typename cost_type<Cost, typename boost::range_value<Range1>::type>::type
edit_path_impl(Range1 const& seq1, Range2 const& seq2, Output& output, const Cost& cost, const Equal& equal, const AllowSub& allow_sub, const MaxCost& max_cost, const bool max_cost_exception, const EditBeam& edit_beam, const CostBeam& cost_beam) {
    // specialize the most appropriate implementation for the given parameters
    return edit_path_struct<Range1, Range2, Output, Cost, Equal, AllowSub, MaxCost, EditBeam, CostBeam>()(seq1, seq2, output, cost, equal, allow_sub, max_cost, max_cost_exception, edit_beam, cost_beam);
}


}}}}

#endif
