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
    typedef long idx_t;

    Itr1 j1;
    Itr2 j2;
    idx_t idx1;
    idx_t idx2;
    Cost cost;

    path_head() {}
    ~path_head() {}
    path_head(const path_head& src) : j1(src.j1), j2(src.j2), cost(src.cost) {}
    path_head(const Itr1& j1_, const Itr2& j2_, const Cost& cost_) : j1(j1_), j2(j2_), cost(cost_) {}
};

template <typename Pool, typename Visited, typename Itr1, typename Itr2, typename Cost>
inline
path_head<Itr1, Itr2, Cost>*
construct(Pool& pool, Visited& visited, const Itr1& j1_, const Itr2& j2_, const Cost& cost_, 
          typename path_head<Itr1, Itr2, Cost>::idx_t idx1_, typename path_head<Itr1, Itr2, Cost>::idx_t idx2_) {
    // pool.construct evidently can't handle more than three parameters, hence this kludge:
    path_head<Itr1, Itr2, Cost>* r = pool.construct(j1_, j2_, cost_);
    r->idx1 = idx1_;
    r->idx2 = idx2_;
    typename Visited::iterator f(visited.find(r));
    if (visited.end() == f) {
        visited.insert(r);
        return r;
    }
    if (r->cost < (*f)->cost) {
        (*f)->cost = r->cost;
        return r;
    }
    pool.destroy(r);
    return static_cast<path_head<Itr1, Itr2, Cost>*>(NULL);
}

template <typename Itr1, typename Itr2, typename Cost>
struct path_node {
    typedef long idx_t;

    Itr1 j1;
    Itr2 j2;
    idx_t idx1;
    idx_t idx2;
    Cost cost;
    struct path_node* edge;

    path_node() {}
    ~path_node() {}
    path_node(const path_node& src) : j1(src.j1), j2(src.j2), cost(src.cost), edge(src.edge) {}
    path_node(const Itr1& j1_, const Itr2& j2_, const Cost& cost_) : j1(j1_), j2(j2_), cost(cost_) {}
};

template <typename Pool, typename Visited, typename Itr1, typename Itr2, typename Cost>
inline
path_node<Itr1, Itr2, Cost>*
construct(Pool& pool, Visited& visited, const Itr1& j1_, const Itr2& j2_, const Cost& cost_, path_node<Itr1, Itr2, Cost>* const& edge_, 
          typename path_node<Itr1, Itr2, Cost>::idx_t idx1_, typename path_node<Itr1, Itr2, Cost>::idx_t idx2_) {
    // pool.construct evidently can't handle more than three parameters, hence this kludge:
    path_node<Itr1, Itr2, Cost>* r = pool.construct(j1_, j2_, cost_);
    r->edge = edge_;
    r->idx1 = idx1_;
    r->idx2 = idx2_;
    typename Visited::iterator f(visited.find(r));
    if (visited.end() == f) {
        visited.insert(r);
        return r;
    }
    if (r->cost < (*f)->cost) {
        (*f)->cost = r->cost;
        return r;
    }
    pool.destroy(r);
    return static_cast<path_node<Itr1, Itr2, Cost>*>(NULL);
}

struct path_lessthan {
    template <typename T> bool operator()(T const* a, T const* b) const {
        return a->cost > b->cost;
    }
};

struct visited_lessthan {
    template <typename T> bool operator()(T const* a, T const* b) const {
        if (a->idx1 < b->idx1) return true;
        if (a->idx1 > b->idx1) return false;
        return a->idx2 < b->idx2;
    }
};


}}}}

#endif
