/*******
edit_distance: STL and Boost compatible edit distance functions for C++

Copyright (c) 2013 Erik Erlandson

Author:  Erik Erlandson <erikerlandson@yahoo.com>

Distributed under the Boost Software License, Version 1.0.
See accompanying file LICENSE or copy at
http://www.boost.org/LICENSE_1_0.txt
*******/


#if !defined(BOOST_ALGORITHM_SEQUENCE_DETAIL_EDIT_DISTANCE_HPP)
#define BOOST_ALGORITHM_SEQUENCE_DETAIL_EDIT_DISTANCE_HPP

// this requires me to link against boost system lib, which disappoints me,
// since it prevents my algorithms from being pure-template.
// maybe figure out how to use a pure-template pool allocator later
#include <boost/pool/object_pool.hpp>

#include <boost/heap/skew_heap.hpp>

#include <boost/range/metafunctions.hpp>

#include <boost/algorithm/sequence/detail/edit_distance_types.hpp>
#include <boost/algorithm/sequence/detail/edit_distance_script.hpp>

namespace boost {
namespace algorithm {
namespace sequence {
namespace detail {

using boost::distance;
using boost::range_iterator;

using boost::make_tuple;
using boost::enable_if;
using boost::is_same;
using boost::mpl::and_;
using std::iterator_traits;
using std::random_access_iterator_tag;

template <typename ForwardRange1, typename ForwardRange2, typename Output, typename Cost, typename Equal, typename AllowSub, typename MaxCost>
struct edit_cost_struct<ForwardRange1, ForwardRange2, Output, Cost, Equal, AllowSub, MaxCost,
                        typename enable_if<and_<is_same<Output, none>,
                                                not_<and_<range_category<ForwardRange1, ForwardRange2, random_access_iterator_tag>,
                                                          is_same<Cost, unit_cost>,
                                                          is_same<AllowSub, boost::false_type> > > > >::type> {
typedef typename cost_type<Cost, typename boost::range_value<ForwardRange1>::type>::type cost_t;
typedef typename range_iterator<ForwardRange1 const>::type itr1_t;
typedef typename range_iterator<ForwardRange2 const>::type itr2_t;
typedef path_head<itr1_t, itr2_t, cost_t> head_t;
typedef typename head_t::pos1_type pos1_t;
typedef typename head_t::pos2_type pos2_t;

cost_t max_cost_fallback(max_cost_checker<MaxCost, cost_t, head_t>& max_cost_check, bool max_cost_exception, const itr1_t end1, const itr2_t end2, const Cost& cost, const Equal& equal, sub_checker<AllowSub, Cost, cost_t, int> const& allow_sub) const {
    if (max_cost_exception) throw max_edit_cost_exception();

    head_t* h;
    max_cost_check.get(h);

    pos1_t j1 = h->pos1;
    pos2_t j2 = h->pos2;
    cost_t C = h->cost;
    while (true) {
        if (j1 == end1) {
            if (j2 == end2) {
                return C;
            } else {
                C += cost.insertion(*j2);
                ++j2;
            }
        } else {
            if (j2 == end2) {
                C += cost.deletion(*j1);
                ++j1;
            } else {
                C += (equal(*j1, *j2)) ? 0 
                                       : ((allow_sub()) ? std::min(allow_sub.substitution(cost, *j1, *j2), (cost.deletion(*j1)+cost.insertion(*j2))) 
                                                        : (cost.deletion(*j1)+cost.insertion(*j2))) ;
                ++j1;  ++j2;
            }
        }
    }
    return C;
}

// Default is generic edit distance algorithm based on a Dijkstra Single Source Shortest Path approach
typename cost_type<Cost, typename boost::range_value<ForwardRange1>::type>::type
operator()(ForwardRange1 const& seq1, ForwardRange2 const& seq2, none&, const Cost& cost, const Equal& equal, const AllowSub& allowsub, const MaxCost& max_cost, const bool max_cost_exception) const {

    head_t* const hnull = static_cast<head_t*>(NULL);

    const itr1_t end1 = boost::end(seq1);
    const itr2_t end2 = boost::end(seq2);
    pos1_t beg1;  beg1.beg(boost::begin(seq1));
    pos2_t beg2;  beg2.beg(boost::begin(seq2));

    // pool allocator for path nodes
    boost::object_pool<head_t> pool;

    // priority queue for path nodes
    boost::heap::skew_heap<head_t*, boost::heap::compare<heap_lessthan<pos1_t, pos2_t> > > heap(heap_lessthan<pos1_t, pos2_t>(beg1, beg2));

    sub_checker<AllowSub, Cost, cost_t, int> allow_sub(allowsub);

    max_cost_checker<MaxCost, cost_t, head_t> max_cost_check(max_cost, beg1, beg2);

    // keep track of nodes in the edit graph that have been visited
    typedef boost::unordered_set<head_t*, visited_hash<pos1_t,pos2_t>, visited_equal> visited_t;
    visited_t visited(31, visited_hash<pos1_t,pos2_t>(beg1,beg2));

    // kick off graph path frontier with initial node:
    heap.push(construct(pool, visited, beg1, beg2, cost_t(0)));

    // update frontier from least-cost node at each iteration, until we hit sequence end
    while (true) {
        head_t* h = heap.top();
        heap.pop();

        if (max_cost_check(h->cost)) {
            return max_cost_fallback(max_cost_check, max_cost_exception, end1, end2, cost, equal, allow_sub);
        }
        max_cost_check.update(h);

        if (h->pos1 == end1) {
            // if we are at end of both sequences, then we have our final cost: 
            if (h->pos2 == end2) return h->cost;
            // sequence 1 is at end, so only consider insertion from seq2
            pos2_t p2 = h->pos2;
            head_t* t = construct(pool, visited, h->pos1, ++p2, h->cost + cost.insertion(*(h->pos2)));
            if (t != hnull) heap.push(t);
        } else if (h->pos2 == end2) {
            // sequence 2 is at end, so only consider deletion from seq1
            pos1_t p1 = h->pos1;
            head_t* t = construct(pool, visited, ++p1, h->pos2, h->cost + cost.deletion(*(h->pos1)));
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
                    head_t* t;
                    if (allow_sub() || eq) {
                        t = construct(pool, visited, p1, p2, h->cost + ((eq) ? 0 : allow_sub.substitution(cost, *p1p, *p2p)));
                        if (t != hnull) heap.push(t);
                    }
                    t = construct(pool, visited, p1p, p2, h->cost + cost.insertion(*p2p));
                    if (t != hnull) heap.push(t);
                    t = construct(pool, visited, p1, p2p, h->cost + cost.deletion(*p1p));
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


template <typename Range1, typename Range2, typename Output, typename Equal, typename MaxCost>
struct edit_cost_struct<Range1, Range2, Output, unit_cost, Equal, boost::false_type, MaxCost,
                        typename enable_if<and_<is_same<Output, none>,
                                                range_category<Range1, Range2, random_access_iterator_tag> > >::type> {

typedef typename range_iterator<Range1 const>::type itr1_t;
typedef typename range_iterator<Range2 const>::type itr2_t;

typedef std::vector<int>::difference_type diff_type;
typedef std::vector<int>::size_type size_type;

typedef typename std::vector<diff_type>::iterator itrv_t;
typedef max_cost_checker_myers<MaxCost, diff_type, diff_type> max_cost_type;

std::string dump(const itr1_t& S1, const size_type& len1) const {
    std::string r;
    for (int j = 0; j < len1;  ++j) r += S1[j];
    return r;
}

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

diff_type max_cost_fallback(max_cost_type& max_cost_check, const bool max_cost_exception, const Equal& equal,
                            const itr1_t& S1, const size_type& L1, const itr2_t& S2, const size_type& L2,
                            const itrv_t& Vf, const itrv_t& Vr, const diff_type& delta, const diff_type& D) const {
    if (max_cost_exception) throw max_edit_cost_exception();

    for (diff_type k = -D;  k <= D;  k += 2) {
        max_cost_check.update(k, Vf, Vr, delta, L1, L2, D);
    }

    diff_type r1b, r2b, r1e, r2e;

    diff_type C = 0;
    diff_type k;
    remainder::kind kind;
    max_cost_check.get(k, kind);
    switch (kind) {
    case remainder::forward: {
            r1b = Vf[k];
            r2b = r1b-k;
            r1e = L1;
            r2e = L2;
            C = D;
        }; break;

        case remainder::reverse: {
            r1b = 0;
            r2b = 0;
            r1e = Vr[k];
            r2e = r1e-k;
            C = D;
        }; break;

        case remainder::bidirectional: {
            r1b = Vf[k];
            r2b = r1b-k;
            r1e = Vr[k];
            r2e = r1e-k;
            C = 2*D;
        }; break;

        default: BOOST_ASSERT(false);
    }

    // this is the part we bailed on due to hitting the maximum
    diff_type j1 = r1b;
    diff_type j2 = r2b;
    while (true) {
        if (j1 >= r1e) {
            if (j2 >= r2e) {
                break;
            } else {
                C += 1;
                ++j2;
            }
        } else {
            if (j2 >= r2e) {
                C += 1;
                ++j1;
            } else {
                if (!equal(S1[j1], S2[j2])) C += 2;
                ++j1;
                ++j2;
            }
        }
    }

    return C;
}

// If we are using unit cost for ins/del, with no substitution,
// and if our sequences support random-access,
// *then* we can invoke the efficient and elegant Myers algorithm:
//     An O(ND) Difference Algorithm and its Variations
//     by Eugene W. Myers
//     Dept of Computer Science, University of Arizona
typename cost_type<unit_cost, typename boost::range_value<Range1>::type>::type
operator()(Range1 const& seq1_, Range2 const& seq2_, none&, const unit_cost&, const Equal& equal, const boost::false_type&, const MaxCost& max_cost, const bool max_cost_exception) const {
    itr1_t seq1 = boost::begin(seq1_);
    itr2_t seq2 = boost::begin(seq2_);
    size_type len1 = distance(seq1_);
    size_type len2 = distance(seq2_);

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

    // either or both strings are empty:
    if (L1 <= 0) return L2;
    if (L2 <= 0) return L1;

    const diff_type delta = L1-L2;
    const bool delta_even = delta%2 == 0;

    size_type R = 10;
    std::vector<diff_type> V_data(2*(1 + 2*R));
    itrv_t Vf = V_data.begin()+R;
    itrv_t Vr = V_data.begin()+(3*R+1)-delta;

    max_cost_type max_cost_check(max_cost);

    diff_type D = 0;
    Vf[1] = 0;
    Vr[-1+delta] = L1;
    while (true) {
        // advance forward-path diagonals:
        for (diff_type k = -D;  k <= D;  k += 2) {
            diff_type j1 = (k == -D  ||  (k != D  &&  Vf[k-1] < Vf[k+1]))  ?  Vf[k+1]  :  1+Vf[k-1];
            diff_type j2 = j1-k;
            if (!delta_even  &&  (k-delta) >= -(D-1)  &&  (k-delta) <= (D-1)) {
                diff_type r1 = Vr[k];
                diff_type r2 = Vr[k]-k;
                if ((j1-j2) == (r1-r2)  &&  j1 >= r1) return 2*D-1;
            }
            while (j1 < L1  &&  j2 < L2  &&  equal(S1[j1], S2[j2])) { ++j1;  ++j2; }
            Vf[k] = j1;
        }

        // advance the reverse-path diagonals:
        for (diff_type k = -D+delta;  k <= D+delta;  k += 2) {
            diff_type j1 = (k == D+delta  ||  (k != -D+delta  &&  Vr[k-1] < Vr[k+1]))  ?  Vr[k-1]  :  Vr[k+1]-1;
            diff_type j2 = j1-k;
            if (delta_even  &&  k >= -D  &&  k <= D) {
                diff_type f1 = Vf[k];
                diff_type f2 = Vf[k]-k;
                if ((j1-j2) == (f1-f2)  &&  f1 >= j1) return 2*D;
            }
            while (j1 > 0  &&  j2 > 0  &&  equal(S1[j1-1], S2[j2-1])) { --j1;  --j2; }
            Vr[k] = j1;
        }

        if (max_cost_check((delta_even) ? (2*D+2) : (2*D+1))) {
            return max_cost_fallback(max_cost_check, max_cost_exception, equal,
                                     S1, L1, S2, L2,
                                     Vf, Vr, delta, D);
        }

        // expand the working vector as needed
        if (D >= R) expand(V_data, Vf, Vr, R, D, delta);
        ++D;
    }

    // control should not reach here
    BOOST_ASSERT(false);
    return 0;
}

}; // edit_cost_struct


template <typename Range1, typename Range2, typename Output, typename Cost, typename Equal, typename AllowSub, typename MaxCost>
inline
typename cost_type<Cost, typename boost::range_value<Range1>::type>::type
edit_cost_impl(Range1 const& seq1, Range2 const& seq2, Output& output, const Cost& cost, const Equal& equal, const AllowSub& allow_sub, const MaxCost& max_cost, const bool max_cost_exception) {
    // specialize the most appropriate implementation for the given parameters
    return edit_cost_struct<Range1, Range2, Output, Cost, Equal, AllowSub, MaxCost>()(seq1, seq2, output, cost, equal, allow_sub, max_cost, max_cost_exception);
}


}}}}

#endif
