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

#include <cstddef>

#include <boost/concept/assert.hpp>
#include <boost/concept/usage.hpp>

#include <boost/type_traits/is_arithmetic.hpp>
#include <boost/type_traits/is_same.hpp>

#include <boost/range/metafunctions.hpp>
#include <boost/range/as_literal.hpp>

namespace boost {
namespace algorithm {
namespace sequence_alignment {
namespace detail {

template <typename Itr1, typename Itr2, typename Cost>
struct path_head {
    typedef size_t idx_t;

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
    typedef path_head<Itr1, Itr2, Cost> head_t;
    head_t w;
    w.idx1 = idx1_;
    w.idx2 = idx2_;
    typename Visited::iterator f(visited.find(&w));
    if (visited.end() != f   &&   cost_ >= (*f)->cost) return static_cast<head_t*>(NULL);
    head_t* r = pool.construct(j1_, j2_, cost_);
    r->idx1 = idx1_;
    r->idx2 = idx2_;
    if (visited.end() == f) {
        visited.insert(r);
    } else if (cost_ < (*f)->cost) {
        (*f)->cost = cost_;
    }
    return r;
}

template <typename Itr1, typename Itr2, typename Cost>
struct path_node {
    typedef size_t idx_t;

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
    typedef path_node<Itr1, Itr2, Cost> head_t;
    head_t w;
    w.idx1 = idx1_;
    w.idx2 = idx2_;
    typename Visited::iterator f(visited.find(&w));
    if (visited.end() != f   &&   cost_ >= (*f)->cost) return static_cast<head_t*>(NULL);
    head_t* r = pool.construct(j1_, j2_, cost_);
    r->edge = edge_;
    r->idx1 = idx1_;
    r->idx2 = idx2_;
    if (visited.end() == f) {
        visited.insert(r);
    } else if (cost_ < (*f)->cost) {
        (*f)->cost = cost_;
    }
    return r;
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

template <typename X>
struct ForwardRangeConvertible {
    BOOST_CONCEPT_USAGE(ForwardRangeConvertible) {
        // all I really want to capture here is that any sequence argument to edit_distance()
        // and friends can be treated as a ForwardRange -- currently I'm doing this by
        // applying as_literal() to all incoming arguments, which seems to allow me to send in
        // null-terminated strings, ranges, sequence containers, etc, which is what I want.
        boost::as_literal(x);
    }
    X x;
};

// I'm a little surprised this doesn't exist already
template <typename X>
struct Arithmetic {
    typedef typename boost::is_arithmetic<X>::type type;
};

template <typename X>
struct SequenceAlignmentCost {
    typedef typename X::cost_type cost_type;
    typedef typename X::value_type value_type;
    BOOST_CONCEPT_ASSERT((Arithmetic<cost_type>));    
    BOOST_CONCEPT_USAGE(SequenceAlignmentCost) {
        c = x.cost_ins(v);
        c = x.cost_del(v);
        c = x.cost_sub(v,v);
    }
    X x;
    cost_type c;
    value_type v;
};


}}}}

#endif
