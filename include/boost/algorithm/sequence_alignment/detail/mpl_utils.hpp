/*******
edit_distance: STL and Boost compatible edit distance functions for C++

Copyright (c) 2013 Erik Erlandson

Author:  Erik Erlandson <erikerlandson@yahoo.com>

Distributed under the Boost Software License, Version 1.0.
See accompanying file LICENSE or copy at
http://www.boost.org/LICENSE_1_0.txt
*******/


#if !defined(BOOST_ALGORITHM_SEQUENCE_ALIGNMENT_DETAIL_MPL_UTILS_HPP)
#define BOOST_ALGORITHM_SEQUENCE_ALIGNMENT_DETAIL_MPL_UTILS_HPP

#include <boost/mpl/has_xxx.hpp>
#include <boost/mpl/if.hpp>
#include <boost/mpl/vector.hpp>
#include <boost/mpl/fold.hpp>
#include <boost/mpl/sort.hpp>
#include <boost/mpl/unique.hpp>

#include <boost/concept/assert.hpp>
#include <boost/concept/usage.hpp>

#include <boost/type_traits/is_arithmetic.hpp>
#include <boost/type_traits/is_same.hpp>

#include <boost/range/metafunctions.hpp>

namespace boost {
namespace algorithm {
namespace sequence_alignment {
namespace detail {

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

template <typename Vector, typename X>
struct append_to_vector {
    // error!
};
template <typename X>
struct append_to_vector<boost::mpl::vector<>, X> {
    typedef boost::mpl::vector<X> type;
};
template <typename T, typename X>
struct append_to_vector<boost::mpl::vector<T>, X> {
    typedef boost::mpl::vector<T, X> type;
};
template <typename T1, typename T2, typename X>
struct append_to_vector<boost::mpl::vector<T1, T2>, X> {
    typedef boost::mpl::vector<T1, T2, X> type;
};
template <typename T1, typename T2, typename T3, typename X>
struct append_to_vector<boost::mpl::vector<T1, T2, T3>, X> {
    typedef boost::mpl::vector<T1, T2, T3, X> type;
};


template <typename V, typename X>
struct append_sorted_unique {
    typedef typename append_to_vector<V, X>::type va;
    typedef typename boost::mpl::sort<va>::type vs;
    typedef typename boost::mpl::unique<vs, boost::is_same<boost::mpl::_1, boost::mpl::_2> >::type vu;
    typedef typename boost::mpl::fold<vu, boost::mpl::vector<>, append_to_vector<boost::mpl::_1, boost::mpl::_2> >::type type;
};

template <typename T>
struct zero {
    T operator()() { return T(0); }
};

// I created this for replacing '\0' with something printable for unit testing.
// Library users might also find uses for their own testing or output purposes.
#if defined(BOOST_CHAR_DEFAULT_OVERRIDE)
template<>
struct zero<char> {
    char operator()() { return BOOST_CHAR_DEFAULT_OVERRIDE; }
};
#endif

template <typename T>
struct default_ctor {
    T operator()() { return T(); }
};

template <typename T>
T default_value() {
    typename boost::mpl::if_<typename boost::is_arithmetic<T>::type, zero<T>, default_ctor<T> >::type dv;
    return dv();
}

}}}}

#endif
