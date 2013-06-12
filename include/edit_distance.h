/*******
edit_distance: STL and Boost compatible edit distance functions for C++

Copyright (c) 2013 Erik Erlandson

Author:  Erik Erlandson <erikerlandson@yahoo.com>

Distributed under the Boost Software License, Version 1.0.
See accompanying file LICENSE or copy at
http://www.boost.org/LICENSE_1_0.txt
*******/


#if !defined(__edit_distance_h__)
#define __edit_distance_h__ 1

#include <vector>

#include <boost/concept/requires.hpp>
#include <boost/concept/usage.hpp>
#include <boost/concept/assert.hpp>
#include <boost/type_traits/is_arithmetic.hpp>

#include <boost/range/as_literal.hpp>
#include <boost/range/as_array.hpp>
#include <boost/range/functions.hpp>
#include <boost/range/metafunctions.hpp>

#include <boost/mpl/has_xxx.hpp>
#include <boost/mpl/if.hpp>

#include <boost/multi_array.hpp>

using std::vector;
using boost::distance;
using boost::begin;
using boost::end;
using boost::range_iterator;
using boost::range_value;
using boost::range_reference;


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


template <typename Range>
struct default_cost {
    // interesting that these appear to work correctly when Range = char*
    typedef typename boost::range_difference<Range>::type cost_type;
    typedef typename boost::range_value<Range>::type value_type;
    cost_type cost_ins(value_type const& a) const {
        return cost_type(1);
    }
    cost_type cost_del(value_type const& a) const {
        return cost_type(1);
    }
    cost_type cost_sub(value_type const& a, value_type const& b) const {
        return (a == b) ? cost_type(0) : cost_type(1);
    }
};


template <typename ForwardRange1, typename ForwardRange2, typename Cost>
typename Cost::cost_type 
needleman_wunsch_distance(ForwardRange1 const& seq1, ForwardRange2 const& seq2, Cost& cost) {
    typedef typename Cost::cost_type cost_t;
    typedef typename range_iterator<ForwardRange1 const>::type itr1_t;
    typedef typename range_iterator<ForwardRange2 const>::type itr2_t;
    typedef boost::multi_array<cost_t, 1> cost_array_t;
    typedef typename cost_array_t::iterator itrc_t;
    typename vector<cost_t>::size_type len2 = distance(seq2);
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


template <typename Sequence1, typename Sequence2, typename Cost>
BOOST_CONCEPT_REQUIRES(
    ((ForwardRangeConvertible<Sequence1>))
    ((ForwardRangeConvertible<Sequence2>))
    ((SequenceAlignmentCost<Cost>)),
(typename Cost::cost_type))
edit_distance(Sequence1 const& seq1, Sequence2 const& seq2, Cost cost) {
    // as_literal() appears to be idempotent, so I tentatively feel OK layering it in here to
    // handle char* transparently, which seems to be working correctly
    return needleman_wunsch_distance(boost::as_literal(seq1), boost::as_literal(seq2), cost);
    // note to self - in the general case edit distance isn't a symmetric function, depending on
    // the cost matrix
}


template <typename Sequence1, typename Sequence2>
inline 
BOOST_CONCEPT_REQUIRES(
    ((ForwardRangeConvertible<Sequence1>))
    ((ForwardRangeConvertible<Sequence2>)),
(typename default_cost<Sequence1>::cost_type))
edit_distance(Sequence1 const& seq1, Sequence2 const& seq2) {
    return edit_distance(seq1, seq2, default_cost<Sequence1>());
}

#endif
