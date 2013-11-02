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

#include <boost/type_traits.hpp>
#include <boost/type_traits/is_arithmetic.hpp>

#include <boost/utility/enable_if.hpp>
#include <boost/typeof/std/utility.hpp>

#include <boost/function_types/result_type.hpp>
#include <boost/function_types/function_arity.hpp>
#include <boost/function_types/parameter_types.hpp>

#include <boost/tti/has_type.hpp>
#include <boost/tti/has_member_function.hpp>

#include <boost/range/metafunctions.hpp>
#include <boost/range/as_literal.hpp>

#include <boost/mpl/at.hpp>
#include <boost/mpl/assert.hpp>
#include <boost/mpl/vector.hpp>
#include <boost/mpl/logical.hpp>
#include <boost/mpl/equal_to.hpp>

#include <boost/concept/assert.hpp>
#include <boost/concept/usage.hpp>

namespace boost {
namespace algorithm {
namespace sequence_alignment {
namespace detail {

using boost::is_same;
using boost::is_member_function_pointer;
using boost::integral_constant;
using boost::false_type;
using boost::true_type;
using boost::function_types::result_type;
using boost::function_types::parameter_types;
using boost::function_types::function_arity;
using boost::mpl::at_c;
using boost::mpl::and_;
using boost::mpl::not_;
using boost::mpl::equal_to;
using boost::enable_if;

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


#define TTI_HAS_MEMBER(name) \
namespace __has_member_##name { \
template <typename T, typename U> struct ambig : public T, public U {}; \
template <typename T, typename U, typename Enable = void> \
struct test : public true_type {}; \
template <typename T, typename U> \
struct test<T, U, typename enable_if<boost::is_same<BOOST_TYPEOF(&T:: name), BOOST_TYPEOF(&U:: name)> >::type> : public false_type {}; \
struct seed { char name ; }; \
} \
template <typename T> struct has_member_##name : public __has_member_##name::test<__has_member_##name::ambig<T, __has_member_##name::seed>, __has_member_##name::seed> {};


#define TTI_HAS_MEMBER_FUNCTION_ANYSIG(name) \
namespace __has_member_function_anysig_##name { \
TTI_HAS_MEMBER(name) \
BOOST_TTI_HAS_TYPE(name) \
template <typename T> \
struct fptest : public is_member_function_pointer<BOOST_TYPEOF(&T::name)> {}; \
template <typename T, typename Enable=void> struct test : public false_type {}; \
template <typename T> \
struct test<T, typename enable_if<and_<has_member_##name<T>, not_<has_type_##name<T> > >  >::type> : public fptest<T> {}; \
} \
template <typename T> struct has_member_function_anysig_##name : public __has_member_function_anysig_##name::test<T> {};


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
    BOOST_CONCEPT_USAGE(Arithmetic) {   
        BOOST_STATIC_ASSERT(boost::is_arithmetic<X>::value);
    }
    X x;
};

BOOST_TTI_HAS_TYPE(cost_type)

TTI_HAS_MEMBER_FUNCTION_ANYSIG(cost_ins)
BOOST_TTI_HAS_MEMBER_FUNCTION(cost_ins)
BOOST_TTI_HAS_MEMBER_FUNCTION(cost_del)
BOOST_TTI_HAS_MEMBER_FUNCTION(cost_sub)

template <typename X, typename Enable=void>
struct cost_type {
    typedef typename result_type< BOOST_TYPEOF(&X::cost_ins) >::type type;
};
template <typename X>
struct cost_type<X, typename enable_if<has_type_cost_type<X> >::type> {
    typedef typename X::cost_type type;
};

template <typename X>
struct value_type {
    typedef typename at_c<typename parameter_types< BOOST_TYPEOF(&X::cost_ins) >::type, 1>::type type;
};

template <typename X>
struct Satisfies {
    BOOST_CONCEPT_USAGE(Satisfies) {
        //  handles a form like either integral_constant<bool, true>, or bool_<true>:
        BOOST_STATIC_ASSERT(X::value == true);
    };
    X x;
};

template <typename X, typename Enabled=void>
struct cost_ins_arity : public integral_constant<int, 0> {};
template <typename X>
struct cost_ins_arity<X, typename enable_if<has_member_function_anysig_cost_ins<X> >::type> : public function_arity<BOOST_TYPEOF(&X::cost_ins)> {};

template <typename X> struct SequenceAlignmentCost {
    // check that some member function named 'cost_ins' exists, and it has arity of 1:
    BOOST_CONCEPT_ASSERT((Satisfies<has_member_function_anysig_cost_ins<X> >));
    BOOST_CONCEPT_ASSERT((Satisfies<equal_to<cost_ins_arity<X>, integral_constant<int, 2> > >));

    // we now know these will exist:
    typedef typename cost_type<X>::type cost_type;
    typedef typename result_type< BOOST_TYPEOF(&X::cost_ins) >::type result_type;
    typedef typename value_type<X>::type value_type;

    BOOST_CONCEPT_ASSERT((Arithmetic<cost_type>));

    // check for specific signatures of cost_ins(), cost_del() and cost_sub()
    BOOST_CONCEPT_ASSERT((Satisfies<typename has_member_function_cost_ins<result_type (X::*)(value_type) const>::type>));
    BOOST_CONCEPT_ASSERT((Satisfies<typename has_member_function_cost_del<result_type (X::*)(value_type) const>::type>));
    BOOST_CONCEPT_ASSERT((Satisfies<typename has_member_function_cost_sub<result_type (X::*)(value_type, value_type) const>::type>));

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
