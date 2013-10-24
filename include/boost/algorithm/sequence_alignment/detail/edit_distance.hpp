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

using boost::algorithm::sequence_alignment::detail::path_head;
using boost::algorithm::sequence_alignment::detail::path_lessthan;

template <typename ForwardRange1, typename ForwardRange2, typename Cost>
typename Cost::cost_type 
dijkstra_sssp_cost(ForwardRange1 const& seq1, ForwardRange2 const& seq2, Cost& cost) {
    typedef typename Cost::cost_type cost_t;
    typedef typename range_iterator<ForwardRange1 const>::type itr1_t;
    typedef typename range_iterator<ForwardRange2 const>::type itr2_t;
    typedef path_head<itr1_t, itr2_t, cost_t> head_t;

    const itr1_t end1 = end(seq1);
    const itr2_t end2 = end(seq2);

    boost::object_pool<head_t> pool;
    boost::heap::fibonacci_heap<head_t*, boost::heap::compare<path_lessthan> > heap;

    heap.push(pool.construct(begin(seq1), begin(seq2), cost_t(0)));
    while (true) {
        head_t* h = heap.top();
        heap.pop();
        if (h->j1 == end1) {
            if (h->j2 == end2) return h->cost;
            itr2_t j2 = h->j2;  ++(h->j2);
            heap.push(pool.construct(h->j1, h->j2, h->cost + cost.cost_ins(*j2)));
        }  else {
            itr1_t j1 = h->j1;  ++(h->j1);
            heap.push(pool.construct(h->j1, h->j2, h->cost + cost.cost_del(*j1)));
            if (h->j2 != end2) {
                itr2_t j2 = h->j2;  ++(h->j2);
                heap.push(pool.construct(j1, h->j2, h->cost + cost.cost_ins(*j2)));
                heap.push(pool.construct(h->j1, h->j2, h->cost + cost.cost_sub(*j1, *j2)));
            }
        }
        // this node can be reused
        pool.destroy(h);
    }

    // control should not reach here
    return 0;
}

}}}}

#endif
