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
#include <iterator>

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

#include <boost/unordered_set.hpp>
#include <boost/functional/hash.hpp>

#include <boost/parameter/name.hpp>
#include <boost/parameter/preprocessor.hpp>

namespace boost {
namespace algorithm {
namespace sequence_alignment {

struct unit_cost {
    typedef size_t cost_type;

    template <typename value_type> inline
    cost_type cost_ins(value_type const&) const {
        return cost_type(1);
    }

    template <typename value_type> inline
    cost_type cost_del(value_type const&) const {
        return cost_type(1);
    }

    template <typename value_type_1, typename value_type_2> inline
    cost_type cost_sub(value_type_1 const&, value_type_2 const&) const {
        return cost_type(1);
    }
};


namespace parameter {
    BOOST_PARAMETER_NAME(seq1)
    BOOST_PARAMETER_NAME(seq2)
    BOOST_PARAMETER_NAME(output)
    BOOST_PARAMETER_NAME(cost)
    BOOST_PARAMETER_NAME(equal)
    BOOST_PARAMETER_NAME(allow_sub)
    BOOST_PARAMETER_NAME(edit_beam)
    BOOST_PARAMETER_NAME(cost_beam)
}

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

template <typename X> struct invoke : public true_type {};

struct none {};

struct default_equal {
    template <typename T1, typename T2>
    inline bool operator()(const T1& a, const T2& b) const { return a == b; }
};

template <typename Node, typename EditBeam, typename Enabled=void>
struct edit_beam_checker {
    // some kind of compile-time error here
};

// default edit_beam checker is no-op: no edit_beam checking at all
template <typename Node, typename EditBeam>
struct edit_beam_checker<Node, EditBeam, typename enable_if<is_same<EditBeam, none> >::type> {
    edit_beam_checker(typename Node::pos1_type const& beg1_, typename Node::pos2_type const& beg2_, EditBeam const& edit_beam_) {}
    inline bool operator()(Node*) const { return true; }
};

template <typename Node, typename EditBeam>
struct edit_beam_checker<Node, EditBeam, typename enable_if<is_integral<EditBeam> >::type> {
    typename Node::pos1_type beg1;
    typename Node::pos2_type beg2;
    EditBeam edit_beam;
    edit_beam_checker(typename Node::pos1_type const& beg1_, typename Node::pos2_type const& beg2_, EditBeam const& edit_beam_) : beg1(beg1_), beg2(beg2_), edit_beam(std::abs(edit_beam_)) {}
    inline bool operator()(Node* n) const {
        return std::abs((n->pos1 - beg1) - (n->pos2 - beg2)) <= edit_beam;
    }
};

template <typename Node, typename Cost, typename CostT, typename Beam, typename Enabled=void>
struct cost_beam_checker {
    // some kind of compile-time error here
};

// default env pruner is no-op: no pruning at all
template <typename Node, typename Cost, typename CostT, typename Beam>
struct cost_beam_checker<Node, Cost, CostT, Beam, typename enable_if<is_same<Beam, none> >::type> {
    typedef typename Node::pos1_type pos1_type;
    typedef typename Node::pos2_type pos2_type;

    cost_beam_checker(const pos1_type&, const pos2_type&, const Beam&) {}

    inline bool operator()(Node*) const { return false; }

    inline void update(const pos1_type&, const pos1_type&, const pos2_type&, const CostT&) const {}
};

template <typename Node, typename Cost, typename CostT, typename Beam>
struct cost_beam_checker<Node, Cost, CostT, Beam, typename enable_if<is_arithmetic<Beam> >::type> {
    typedef typename Node::pos1_type pos1_type;
    typedef typename Node::pos2_type pos2_type;

    pos1_type env1;
    pos2_type env2;
    CostT beam;
    CostT beam_ub;

    cost_beam_checker(const pos1_type& pos1_, const pos2_type& pos2_, const Beam& beam_) : 
        env1(pos1_), env2(pos2_), beam(CostT(std::abs(beam_))), beam_ub(0) {}

    inline bool operator()(Node* n) const {
        return (n->cost >= beam_ub)  &&  (n->pos1 < env1)  &&  (n->pos2 < env2);
    }

    inline void update(typename Node::pos1_type const& ref1, typename Node::pos1_type const& pos1, typename Node::pos2_type const& pos2, const CostT& cost) {
        // I cannot decide if this should be exposed as a parameter or not.
        // 4 is a magic number, but appears to be consistently a bit better than
        // other magic numbers in my testing.  Given the uncertainty I will lean in 
        // favor of not exposing more free parameters than necessary.
        // The basic idea in play here is that it's advantageous to only update the
        // cost beam envelope for 'real' runs of equality, not spurious ones that
        // occur purely by chance.
        const size_t run_min = 4;

        if ((env1 < pos1  ||  env2 < pos2)  &&  (pos1-ref1) >= run_min) {
            env1 = pos1;
            env2 = pos2;
            beam_ub = cost + beam;
        }
    }
};

template <typename AllowSub, typename Cost, typename CostT, typename Output, typename Enable=void>
struct sub_checker {
    // informative compile error here 
};

template <typename AllowSub, typename Cost, typename CostT, typename Output>
struct sub_checker<AllowSub, Cost, CostT, Output, typename enable_if<is_same<AllowSub, bool> >::type> {
    bool allow;
    sub_checker(const bool& allow_) : allow(allow_) {}
    inline bool operator()() const { return allow; }
    template <typename V1, typename V2> inline CostT cost_sub(const Cost& cost, const V1& v1, const V2& v2) const { return cost.cost_sub(v1, v2); }
    template <typename V1, typename V2> inline void output_sub(Output& out, const V1& v1, const V2& v2, const CostT& csub) const { out.output_sub(v1, v2, csub); }
};

template <typename AllowSub, typename Cost, typename CostT, typename Output>
struct sub_checker<AllowSub, Cost, CostT, Output, typename enable_if<is_same<AllowSub, true_type> >::type> {
    sub_checker(const AllowSub&) {}
    inline bool operator()() const { return true; }
    template <typename V1, typename V2> inline CostT cost_sub(const Cost& cost, const V1& v1, const V2& v2) const { return cost.cost_sub(v1, v2); }
    template <typename V1, typename V2> inline void output_sub(Output& out, const V1& v1, const V2& v2, const CostT& csub) const { out.output_sub(v1, v2, csub); }
};

template <typename AllowSub, typename Cost, typename CostT, typename Output>
struct sub_checker<AllowSub, Cost, CostT, Output, typename enable_if<is_same<AllowSub, false_type> >::type> {
    sub_checker(const AllowSub&) {}
    inline bool operator()() const { return false; }
    template <typename V1, typename V2> inline CostT cost_sub(const Cost&, const V1&, const V2&) const { return 0; }
    template <typename V1, typename V2> inline void output_sub(Output&, const V1&, const V2&, const CostT&) const {}
};


template <typename Itr, typename Enabled=void>
struct position {
    typedef Itr itr_type;
    typedef typename std::iterator_traits<Itr>::difference_type idx_type;
    typedef typename std::iterator_traits<Itr>::difference_type difference_type;
    typedef typename std::iterator_traits<Itr>::value_type value_type;
    typedef typename std::iterator_traits<Itr>::reference reference;

    itr_type j;
    idx_type idx;

    inline void beg(const itr_type& src) { j = src;  idx = 0; }

    inline reference operator*() { return *j; }
    inline bool operator==(const itr_type& rhs) const { return j == rhs; }
    inline bool operator!=(const itr_type& rhs) const { return j != rhs; }
    inline bool operator<(const position& rhs) const { return idx < rhs.idx; }
    inline position& operator++() { ++j; ++idx; return *this; }
    inline difference_type operator-(const position& rhs) const { return idx - rhs.idx; }
};

template <typename Itr>
struct position<Itr, typename enable_if<is_same<typename std::iterator_traits<Itr>::iterator_category, std::random_access_iterator_tag> >::type> {
    typedef Itr itr_type;
    typedef typename std::iterator_traits<Itr>::difference_type idx_type;
    typedef typename std::iterator_traits<Itr>::difference_type difference_type;
    typedef typename std::iterator_traits<Itr>::value_type value_type;
    typedef typename std::iterator_traits<Itr>::reference reference;

    itr_type j;

    inline void beg(const itr_type& src) { j = src; }

    inline reference operator*() { return *j; }
    inline bool operator==(const itr_type& rhs) const { return j == rhs; }
    inline bool operator!=(const itr_type& rhs) const { return j != rhs; }
    inline bool operator<(const position& rhs) const { return j < rhs.j; }
    inline position& operator++() { ++j; return *this; }    
    inline difference_type operator-(const position& rhs) const { return j - rhs.j; }
};

template <typename Itr1, typename Itr2, typename Cost>
struct path_head {
    typedef position<Itr1> pos1_type;
    typedef position<Itr2> pos2_type;

    pos1_type pos1;
    pos2_type pos2;
    Cost cost;
};

template <typename Itr1, typename Itr2, typename Cost>
struct path_node : public path_head<Itr1, Itr2, Cost> {
    struct path_node* edge;
};

template <typename Pool, typename Visited, typename Pos1, typename Pos2, typename Cost>
inline
path_head<typename Pos1::itr_type, typename Pos2::itr_type, Cost>*
construct(Pool& pool, Visited& visited, const Pos1& pos1_, const Pos2& pos2_, const Cost& cost_) {
    typedef path_head<typename Pos1::itr_type, typename Pos2::itr_type, Cost> head_t;
    head_t w;
    w.pos1 = pos1_;
    w.pos2 = pos2_;
    typename Visited::iterator f(visited.find(&w));
    if (visited.end() != f   &&   cost_ >= (*f)->cost) return static_cast<head_t*>(NULL);
    head_t* r = pool.construct();
    r->pos1 = pos1_;
    r->pos2 = pos2_;
    r->cost = cost_;
    if (visited.end() != f) visited.erase(f);
    visited.insert(r);
    return r;
}

template <typename Pool, typename Visited, typename Pos1, typename Pos2, typename Cost>
inline
path_node<typename Pos1::itr_type, typename Pos2::itr_type, Cost>*
construct(Pool& pool, Visited& visited, const Pos1& pos1_, const Pos2& pos2_, const Cost& cost_, path_node<typename Pos1::itr_type, typename Pos2::itr_type, Cost>* const& edge_) {
    typedef path_node<typename Pos1::itr_type, typename Pos2::itr_type, Cost> head_t;
    head_t w;
    w.pos1 = pos1_;
    w.pos2 = pos2_;
    typename Visited::iterator f(visited.find(&w));
    if (visited.end() != f   &&   cost_ >= (*f)->cost) return static_cast<head_t*>(NULL);
    head_t* r = pool.construct();
    r->pos1 = pos1_;
    r->pos2 = pos2_;
    r->cost = cost_;
    r->edge = edge_;
    if (visited.end() != f) visited.erase(f);
    visited.insert(r);
    return r;
}

template <typename Pos1, typename Pos2>
struct heap_lessthan {
    Pos1 beg1;
    Pos2 beg2;
    heap_lessthan(const Pos1& pos1_, const Pos2& pos2_) : beg1(pos1_), beg2(pos2_) {}
    template <typename T> inline bool operator()(T const* a, T const* b) const {
        return a->cost > b->cost;
    }
};

template <typename Pos1, typename Pos2>
struct visited_hash {
    Pos1 beg1;
    Pos2 beg2;
    visited_hash(const Pos1& pos1_, const Pos2& pos2_) : beg1(pos1_), beg2(pos2_) {}
    template<typename T> inline
    size_t operator()(T const* e) const {
        size_t h = 0;
        boost::hash_combine(h, e->pos1-beg1);
        boost::hash_combine(h, e->pos2-beg2);        
        return h;
    }
};

struct visited_equal {
    template<typename T> inline
    bool operator()(T const* a, T const* b) const {
        return (a->pos1.j == b->pos1.j) && (a->pos2.j == b->pos2.j);
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
    BOOST_CONCEPT_USAGE(Arithmetic) {   
        BOOST_STATIC_ASSERT(boost::is_arithmetic<X>::value);
    }
    X x;
};

BOOST_TTI_HAS_TYPE(cost_type)

template <typename X, typename V, typename Enable=void>
struct cost_type {
    X x;
    V v;
    // by default, we infer cost_type from the return value of cost functions
    typedef BOOST_TYPEOF(x.cost_ins(v)) type;
};

template <typename X, typename V>
struct cost_type<X, V, typename enable_if<has_type_cost_type<X> >::type> {
    // if the class explicitly defines a cost_type, then that is what we use
    typedef typename X::cost_type type;
};


template <typename X, typename Sequence, typename Enabled=void> struct TestCostSub {};

template <typename X, typename Sequence>
struct TestCostSub<X, Sequence, typename enable_if<invoke<BOOST_TYPEOF_TPL(&X::cost_sub)> >::type> {
    typedef typename boost::range_value<Sequence>::type value_type;
    typedef typename cost_type<X, value_type>::type cost_type;
    BOOST_CONCEPT_USAGE(TestCostSub) {
        c = x.cost_sub(v,v);
    }

    X x;
    cost_type c;
    value_type v;
};


template <typename X, typename Sequence> struct SequenceAlignmentCost {
    BOOST_CONCEPT_ASSERT((ForwardRangeConvertible<Sequence>));

    typedef typename boost::range_value<Sequence>::type value_type;
    typedef typename cost_type<X, value_type>::type cost_type;
    BOOST_CONCEPT_ASSERT((Arithmetic<cost_type>));

    BOOST_CONCEPT_USAGE(SequenceAlignmentCost) {
        c = x.cost_ins(v);
        c = x.cost_del(v);
    }

    // test x.cost_sub() method, if it is defined 
    BOOST_CONCEPT_ASSERT((TestCostSub<X, Sequence>));

    X x;
    cost_type c;
    value_type v;
};


}}}}

#endif
