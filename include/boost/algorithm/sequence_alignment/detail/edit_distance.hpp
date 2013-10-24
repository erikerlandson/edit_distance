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

#include <boost/heap/priority_queue.hpp>
#include <boost/heap/fibonacci_heap.hpp>

#include <boost/range/metafunctions.hpp>

#include <boost/multi_array.hpp>

#include <boost/algorithm/sequence_alignment/detail/types.hpp>

namespace boost {
namespace algorithm {
namespace sequence_alignment {
namespace detail {

using boost::distance;
using boost::begin;
using boost::end;
using boost::range_iterator;
using boost::range_value;
using boost::range_reference;

using boost::algorithm::sequence_alignment::detail::path_head;
using boost::algorithm::sequence_alignment::detail::path_lessthan;

#if 0
using boost::algorithm::sequence_alignment::detail::edit_opcode;
using boost::algorithm::sequence_alignment::detail::ins_op;
using boost::algorithm::sequence_alignment::detail::del_op;
using boost::algorithm::sequence_alignment::detail::sub_op;
using boost::algorithm::sequence_alignment::detail::eql_op;
#endif

template <typename ForwardRange1, typename ForwardRange2, typename Cost>
typename Cost::cost_type 
needleman_wunsch_distance(ForwardRange1 const& seq1, ForwardRange2 const& seq2, Cost& cost) {
    typedef typename Cost::cost_type cost_t;
    typedef typename range_iterator<ForwardRange1 const>::type itr1_t;
    typedef typename range_iterator<ForwardRange2 const>::type itr2_t;
    typedef boost::multi_array<cost_t, 1> cost_array_t;
    typedef typename cost_array_t::iterator itrc_t;
    typename cost_array_t::size_type len2 = distance(seq2);
    cost_array_t cv1(boost::extents[1+len2]);
    cost_array_t cv2(boost::extents[1+len2]);
    cost_array_t* cur = &cv1;
    cost_array_t* prv = &cv2;
    itr2_t j2 = begin(seq2);
    itrc_t c00 = cur->begin();
    itrc_t c01 = c00;
    *c01 = 0;
    for (++c00;  c00 != cur->end();  ++c00, ++c01, ++j2) {
        *c00 = *c01 + cost.cost_ins(*j2);
    }
    for (itr1_t j1 = begin(seq1);  j1 != end(seq1);  ++j1) {
        std::swap(cur, prv);
        j2 = begin(seq2);
        c00 = cur->begin();
        c01 = c00;
        itrc_t c10 = prv->begin();
        itrc_t c11 = c10;
        *c01 = *c11 + cost.cost_del(*j1);
        for (++c00, ++c10;  c00 != cur->end();  ++c00, ++c01, ++c10, ++c11, ++j2) {
            cost_t c = *c01 + cost.cost_ins(*j2);
            c = std::min(c, *c10 + cost.cost_del(*j1));
            c = std::min(c, *c11 + cost.cost_sub(*j1, *j2));
            *c00 = c;
        }
    }
    return *c01;
}

template <typename ForwardRange1, typename ForwardRange2, typename Cost>
typename Cost::cost_type 
dijkstra_sssp_cost(ForwardRange1 const& seq1, ForwardRange2 const& seq2, Cost& cost) {
    typedef typename Cost::cost_type cost_t;
    typedef typename range_iterator<ForwardRange1 const>::type itr1_t;
    typedef typename range_iterator<ForwardRange2 const>::type itr2_t;
    typedef path_head<itr1_t, itr2_t, cost_t> head_t;

    const itr1_t beg1 = begin(seq1);
    const itr1_t end1 = end(seq1);
    itr1_t tt1 = end1; ++tt1;
    const itr1_t z1 = tt1;
    const itr2_t beg2 = begin(seq2);
    const itr2_t end2 = end(seq2);
    itr2_t tt2 = end2; ++tt2;
    const itr2_t z2 = tt2;

    boost::object_pool<head_t> pool;
    boost::heap::fibonacci_heap<head_t*, boost::heap::compare<path_lessthan> > heap;

    heap.push(pool.construct(z1, z2, cost_t(0)));
    while (true) {
        head_t* h = heap.top();
        heap.pop();
        if (h->j1 == end1) {
            if (h->j2 == end2) break;
            itr2_t j2 = (h->j2 == z2) ? beg2 : (++(h->j2));
            heap.push(pool.construct(h->j1, j2, h->cost + cost.cost_ins(*j2)));
        }  else {
            itr1_t j1 = h->j1;
            j1 = (j1 == z1) ? beg1 : (++j1);
            heap.push(pool.construct(j1, h->j2, h->cost + cost.cost_del(*j1)));
            if (h->j2 == end2) continue;
            itr2_t j2 = (h->j2 == z2) ? beg2 : (++(h->j2));
            heap.push(pool.construct(h->j1, j2, h->cost + cost.cost_ins(*j2)));     
            heap.push(pool.construct(j1, j2, h->cost + cost.cost_sub(*j1, *j2)));            
        }
    }

    return heap.top()->cost;
}

}}}}

#endif
