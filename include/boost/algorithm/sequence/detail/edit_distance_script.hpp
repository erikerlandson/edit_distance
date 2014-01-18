/*******
edit_distance: STL and Boost compatible edit distance functions for C++

Copyright (c) 2013 Erik Erlandson

Author:  Erik Erlandson <erikerlandson@yahoo.com>

Distributed under the Boost Software License, Version 1.0.
See accompanying file LICENSE or copy at
http://www.boost.org/LICENSE_1_0.txt
*******/

#if !defined(BOOST_ALGORITHM_SEQUENCE_DETAIL_EDIT_DISTANCE_SCRIPT_HPP)
#define BOOST_ALGORITHM_SEQUENCE_DETAIL_EDIT_DISTANCE_SCRIPT_HPP

namespace boost {
namespace algorithm {
namespace sequence {
namespace detail {

using boost::distance;
using boost::range_iterator;

using boost::enable_if;
using boost::is_same;
using boost::mpl::and_;
using boost::mpl::not_;
using std::random_access_iterator_tag;
using boost::make_tuple;

template <typename ForwardRange1, typename ForwardRange2, typename Output, typename Cost, typename Equal, typename AllowSub, typename MaxCost, typename Enabled = void>
struct edit_cost_struct {
};

template <typename ForwardRange1, typename ForwardRange2, typename Output, typename Cost, typename Equal, typename AllowSub, typename MaxCost>
struct edit_cost_struct<ForwardRange1, ForwardRange2, Output, Cost, Equal, AllowSub, MaxCost, 
                        typename enable_if<and_<not_<is_same<Output, none> >,
                                                not_<and_<range_category<ForwardRange1, ForwardRange2, random_access_iterator_tag>,
                                                          is_same<Cost, unit_cost>,
                                                          is_same<AllowSub, boost::false_type> > > > >::type> {

typedef typename cost_type<Cost, typename boost::range_value<ForwardRange1>::type>::type cost_t;
typedef typename range_iterator<ForwardRange1 const>::type itr1_t;
typedef typename range_iterator<ForwardRange2 const>::type itr2_t;
typedef path_node<itr1_t, itr2_t, cost_t> head_t;
typedef typename head_t::pos1_type pos1_t;
typedef typename head_t::pos2_type pos2_t;


void traceback(head_t* path_head, const Equal& equal, sub_checker<AllowSub, Cost, cost_t, Output> const& allow_sub, Output& output) {
    head_t* const hnull = static_cast<head_t*>(NULL);

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
            output.insertion(*j2, n->edge->cost - n->cost);
            continue;
        }    
        if (j2 == j2end) {
            // seq2 didn't advance, this is a deletion from seq1
            output.deletion(*j1, n->edge->cost - n->cost);
            continue;
        }

        itr1_t j1x = j1;  ++j1x;
        itr2_t j2x = j2;  ++j2x;

        while (j1x != j1end  &&  j2x != j2end) {
            // unpack any compressed runs of 'eql'
            output.equality(*j1, *j2);
            ++j1;  ++j2;  ++j1x;  ++j2x;
        }
        if (j1x == j1end) {
            if (j2x == j2end) {
                if (equal(*j1, *j2)) {
                    output.equality(*j1, *j2);
                } else {
                    allow_sub.substitution(output, *j1, *j2, n->edge->cost - n->cost);
                }
            } else {
                output.equality(*j1, *j2);
                output.insertion(*j2x, n->edge->cost - n->cost);
            }
        } else {
            output.equality(*j1, *j2);
            output.deletion(*j1x, n->edge->cost - n->cost);
        }
    }
}

cost_t max_cost_fallback(max_cost_checker<MaxCost, cost_t, head_t>& max_cost_check, bool max_cost_exception, const itr1_t end1, const itr2_t end2, Output& output, const Cost& cost, const Equal& equal, sub_checker<AllowSub, Cost, cost_t, Output> const& allow_sub) {
    if (max_cost_exception) throw max_edit_cost_exception();

    head_t* h;
    max_cost_check.get(h);

    pos1_t j1 = h->pos1;
    pos2_t j2 = h->pos2;
    cost_t C = h->cost;

    // make use of any best-path work that was completed
    traceback(h, equal, allow_sub, output);

    // fast linear completion
    while (true) {
        if (j1 == end1) {
            if (j2 == end2) {
                return C;
            } else {
                cost_t c = cost.insertion(*j2);
                output.insertion(*j2, c);
                C += c;
                ++j2;
            }
        } else {
            if (j2 == end2) {
                cost_t c = cost.deletion(*j1);
                output.deletion(*j1, c);
                C += c;
                ++j1;
            } else {
                if (equal(*j1, *j2)) {
                    output.equality(*j1, *j2);
                } else {
                    cost_t cd = cost.deletion(*j1);
                    cost_t ci = cost.insertion(*j2);
                    if (!allow_sub()) {
                        output.deletion(*j1, cd);
                        output.insertion(*j2, ci);
                        C += cd+ci;
                    } else {
                        cost_t cs = allow_sub.substitution(cost, *j1, *j2);
                        if (cs <= cd+ci) {
                            allow_sub.substitution(output, *j1, *j2, cs);
                            C += cs;
                        } else {
                            output.deletion(*j1, cd);
                            output.insertion(*j2, ci);
                            C += cd+ci;
                        }
                    }
                }
                ++j1;  ++j2;
            }
        }
    }

    return C;
}

cost_t operator()(ForwardRange1 const& seq1, ForwardRange2 const& seq2, Output& output, const Cost& cost, const Equal& equal, const AllowSub& allowsub, const MaxCost& max_cost, const bool max_cost_exception) {
    head_t* const hnull = static_cast<head_t*>(NULL);

    const itr1_t end1 = boost::end(seq1);
    const itr2_t end2 = boost::end(seq2);
    pos1_t beg1;  beg1.beg(boost::begin(seq1));
    pos2_t beg2;  beg2.beg(boost::begin(seq2));

    // pool allocator for path nodes
    boost::object_pool<head_t> pool;

    // priority queue for path nodes
    boost::heap::skew_heap<head_t*, boost::heap::compare<heap_lessthan<pos1_t, pos2_t> > > heap(heap_lessthan<pos1_t, pos2_t>(beg1, beg2));

    sub_checker<AllowSub, Cost, cost_t, Output> allow_sub(allowsub);

    max_cost_checker<MaxCost, cost_t, head_t> max_cost_check(max_cost, beg1, beg2);

    head_t* path_head = hnull;

    // keep track of nodes in the edit graph that have been visited
    typedef boost::unordered_set<head_t*, visited_hash<pos1_t,pos2_t>, visited_equal> visited_t;
    visited_t visited(31, visited_hash<pos1_t,pos2_t>(beg1,beg2));

    // kick off graph path frontier with initial node:
    heap.push(construct(pool, visited, beg1, beg2, cost_t(0), hnull));

    // update frontier from least-cost node at each iteration, until we hit sequence end
    while (true) {
        head_t* h = heap.top();
        heap.pop();

        if (max_cost_check(h->cost)) {
            return max_cost_fallback(max_cost_check, max_cost_exception, end1, end2, output, cost, equal, allow_sub);
        }
        max_cost_check.update(h);

        if (h->pos1 == end1) {
            if (h->pos2 == end2) {
                // if we are at end of both sequences, then we have our final edit path:
                path_head = h;
                break;
            }
            // sequence 1 is at end, so only consider insertion from seq2
            pos2_t p2 = h->pos2;
            head_t* t = construct(pool, visited, h->pos1, ++p2, h->cost + cost.insertion(*(h->pos2)), h);
            if (t != hnull) heap.push(t);
       } else if (h->pos2 == end2) {
            // sequence 2 is at end, so only consider deletion from seq1
            pos1_t p1 = h->pos1;
            head_t* t = construct(pool, visited, ++p1, h->pos2, h->cost + cost.deletion(*(h->pos1)), h);
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
                        t = construct(pool, visited, p1, p2, h->cost + ((eq) ? 0 : allow_sub.substitution(cost, *p1p, *p2p)), h);
                        if (t != hnull) heap.push(t);
                    }
                    t = construct(pool, visited, p1p, p2, h->cost + cost.insertion(*p2p), h);
                    if (t != hnull) heap.push(t);
                    t = construct(pool, visited, p1, p2p, h->cost + cost.deletion(*p1p), h);
                    if (t != hnull) heap.push(t);
                    break;
                }
                ++p1;  ++p2;  ++p1p;  ++p2p;
            }
        }
    }

    const cost_t edit_cost = path_head->cost;

    traceback(path_head, equal, allow_sub, output);

    return edit_cost;
}

}; // edit_cost_struct


template <typename Range1, typename Range2, typename Output, typename Equal, typename MaxCost>
struct edit_cost_struct<Range1, Range2, Output, unit_cost, Equal, boost::false_type, MaxCost,
                        typename enable_if<and_<not_<is_same<Output, none> >,
                                                range_category<Range1, Range2, random_access_iterator_tag> > >::type> {

typedef typename range_iterator<Range1 const>::type itr1_t;
typedef typename range_iterator<Range2 const>::type itr2_t;

typedef std::vector<int>::difference_type diff_type;
typedef size_t size_type;

typedef std::vector<diff_type>::iterator itrv_t;

typedef max_cost_checker_myers<MaxCost, diff_type, diff_type> max_cost_type;


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

std::string dump(const itr1_t& S1, const size_type& len1) const {
    std::string r;
    for (int j = 0; j < len1;  ++j) r += S1[j];
    return r;
}

diff_type max_cost_fallback(max_cost_checker_myers<MaxCost, diff_type, diff_type>& max_cost_check, const bool max_cost_exception, 
                            const Equal& equal, const MaxCost& max_cost, Output& output,
                            const itr1_t& seq1, const size_type& len1, const itr2_t& seq2, const size_type& len2,
                            const diff_type& eqb, const diff_type& eqe,
                            const itr1_t& S1, const size_type& L1, const itr2_t& S2, const size_type& L2,
                            std::vector<diff_type>& V_data, const itrv_t& Vf, const itrv_t& Vr, const diff_type& delta, const diff_type& D) const {
    if (max_cost_exception) throw max_edit_cost_exception();

    for (diff_type k = -D;  k <= D;  k += 2) {
        max_cost_check.update(k, Vf, Vr, delta, L1, L2, D);
    }

    diff_type r1b, r2b, r1e, r2e;

    diff_type k;
    remainder::kind kind;
    max_cost_check.get(k, kind);
    switch (kind) {
        case remainder::forward: {
            r1b = Vf[k];
            r2b = r1b-k;
            r1e = L1;
            r2e = L2;
        }; break;

        case remainder::reverse: {
            r1b = 0;
            r2b = 0;
            r1e = Vr[k];
            r2e = r1e-k;
        }; break;

        case remainder::bidirectional: {
            r1b = Vf[k];
            r2b = r1b-k;
            r1e = Vr[k];
            r2e = r1e-k;
        }; break;

        default: BOOST_ASSERT(false);
    }

    // output equal prefix:
    for (diff_type j = 0;  j < eqb;  ++j) output.equality(seq1[j], seq2[j]);
    // output any known best-path in forward direction 
    diff_type C = path(S1, r1b, S2, r2b, equal, max_cost, max_cost_exception, output, V_data);

    // output the unknown subsequence: this is the part we bailed on due to hitting the maximum
    diff_type j1 = r1b;
    diff_type j2 = r2b;
    while (true) {
        if (j1 >= r1e) {
            if (j2 >= r2e) {
                break;
            } else {
                output.insertion(S2[j2], 1);
                C += 1;
                ++j2;
            }
        } else {
            if (j2 >= r2e) {
                output.deletion(S1[j1], 1);
                C += 1;
                ++j1;
            } else {
                if (equal(S1[j1], S2[j2])) {
                    output.equality(S1[j1], S2[j2]);
                } else {
                    output.deletion(S1[j1], 1);
                    output.insertion(S2[j2], 1);
                    C += 2;
                }
                ++j1;
                ++j2;
            }
        }
    }

    // output known subsequence from reverse direction
    C += path(S1+r1e, L1-r1e, S2+r2e, L2-r2e, equal, max_cost, max_cost_exception, output, V_data);
    // equal suffix
    for (diff_type jj1=len1-eqe, jj2=len2-eqe; jj1 < len1; ++jj1,++jj2) output.equality(seq1[jj1], seq2[jj2]);

    return C;
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
        for (diff_type j = 0;  j < eqb;  ++j) output.equality(seq1[j], seq2[j]);
        for (diff_type j = 0;  j < L2;  ++j) output.insertion(S2[j], 1);
        for (diff_type j1=len1-eqe, j2=len2-eqe; j1 < len1; ++j1,++j2) output.equality(seq1[j1], seq2[j2]);
        return L2;
    }
    if (L2 <= 0) {
        for (diff_type j = 0;  j < eqb;  ++j) output.equality(seq1[j], seq2[j]);
        for (diff_type j = 0;  j < L1;  ++j) output.deletion(S1[j], 1);
        for (diff_type j1=len1-eqe, j2=len2-eqe; j1 < len1; ++j1,++j2) output.equality(seq1[j1], seq2[j2]);
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

    max_cost_type max_cost_check(max_cost);

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

        if (max_cost_check((delta_even) ? (2*D+2) : (2*D+1))) {
            return max_cost_fallback(max_cost_check, max_cost_exception, 
                                     equal, max_cost, output,
                                     seq1, len1, seq2, len2,
                                     eqb, eqe,
                                     S1, L1, S2, L2,
                                     V_data, Vf, Vr, delta, D);
        }

        // expand the working vectors as needed
        if (D >= R) expand(V_data, Vf, Vr, R, D, delta);
        ++D;
    }

    // output for equal prefix:
    for (diff_type j = 0;  j < eqb;  ++j) output.equality(seq1[j], seq2[j]);
    // output for path up to midpoint snake:
    path(S1, r1b, S2, r2b, equal, max_cost, max_cost_exception, output, V_data);
    // output for midpoint snake:
    for (diff_type j1=r1b,j2=r2b; j1 < r1e;  ++j1, ++j2) output.equality(S1[j1], S2[j2]);
    // output for path from midpoint to end:
    path(S1+r1e, L1-r1e, S2+r2e, L2-r2e, equal, max_cost, max_cost_exception, output, V_data);
    // output for equal suffix:
    for (diff_type j1=len1-eqe, j2=len2-eqe; j1 < len1; ++j1,++j2) output.equality(seq1[j1], seq2[j2]);

    return D;
}

inline
typename cost_type<unit_cost, typename boost::range_value<Range1>::type>::type
operator()(Range1 const& seq1, Range2 const& seq2, Output& output, const unit_cost&, const Equal& equal, const boost::false_type&, const MaxCost& max_cost, const bool max_cost_exception) const {
    typedef std::vector<int>::difference_type diff_type;
    std::vector<diff_type> V_data;
    return path(boost::begin(seq1), distance(seq1), boost::begin(seq2), distance(seq2), equal, max_cost, max_cost_exception, output, V_data);
}

}; // edit_cost_struct


}}}}

#endif
