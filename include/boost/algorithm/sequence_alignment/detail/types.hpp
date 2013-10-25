/*******
edit_distance: STL and Boost compatible edit distance functions for C++

Copyright (c) 2013 Erik Erlandson

Author:  Erik Erlandson <erikerlandson@yahoo.com>

Distributed under the Boost Software License, Version 1.0.
See accompanying file LICENSE or copy at
http://www.boost.org/LICENSE_1_0.txt
*******/

#if !defined(BOOST_ALGORITHM_SEQUENCE_ALIGNMENT_DETAIL_TYPES_HPP)
#define BOOST_ALGORITHM_SEQUENCE_ALIGNMENT_DETAIL_TYPES_HPP

namespace boost {
namespace algorithm {
namespace sequence_alignment {
namespace detail {

template <typename Itr1, typename Itr2, typename Cost>
struct path_head {
    Itr1 j1;
    Itr2 j2;
    Cost cost;

    path_head() {}
    ~path_head() {}
    path_head(const path_head& src) : j1(src.j1), j2(src.j2), cost(src.cost) {}
    path_head(const Itr1& j1_, const Itr2& j2_, const Cost& cost_) : j1(j1_), j2(j2_), cost(cost_) {}
};

template <typename Itr1, typename Itr2, typename Cost>
struct path_node {
    Itr1 j1;
    Itr2 j2;
    Cost cost;
    struct path_node* edge;

    path_node() {}
    ~path_node() {}
    path_node(const path_node& src) : j1(src.j1), j2(src.j2), cost(src.cost), edge(src.edge) {}
    path_node(const Itr1& j1_, const Itr2& j2_, const Cost& cost_) : j1(j1_), j2(j2_), cost(cost_) {}
};

template <typename Pool, typename Itr1, typename Itr2, typename Cost>
inline
path_node<Itr1, Itr2, Cost>*
construct(Pool& pool, const Itr1& j1_, const Itr2& j2_, const Cost& cost_, path_node<Itr1, Itr2, Cost>* const& edge_) {
    // pool.construct evidently can't handle more than three parameters, hence this kludge:
    path_node<Itr1, Itr2, Cost>* r = pool.construct(j1_, j2_, cost_);
    r->edge = edge_;
    return r;
}

struct path_lessthan {
    template <typename T> bool operator()(T const* a, T const* b) const {
        return a->cost > b->cost;
    }
};

}}}}

#endif
