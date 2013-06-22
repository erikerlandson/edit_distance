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

#include <boost/mpl/has_xxx.hpp>
#include <boost/mpl/if.hpp>
#include <boost/mpl/vector.hpp>
#include <boost/mpl/int.hpp>
#include <boost/mpl/fold.hpp>
#include <boost/mpl/sort.hpp>
#include <boost/mpl/unique.hpp>

#include <boost/concept/requires.hpp>
#include <boost/concept/usage.hpp>
#include <boost/concept/assert.hpp>
#include <boost/type_traits/is_arithmetic.hpp>
#include <boost/type_traits/is_same.hpp>

#include <boost/range/as_literal.hpp>
#include <boost/range/as_array.hpp>
#include <boost/range/functions.hpp>
#include <boost/range/metafunctions.hpp>

#include <boost/multi_array.hpp>

using boost::distance;
using boost::begin;
using boost::end;
using boost::range_iterator;
using boost::range_value;
using boost::range_reference;

typedef char edit_opcode;
const edit_opcode ins_op = '+';
const edit_opcode del_op = '-';
const edit_opcode sub_op = ':';
const edit_opcode eql_op = '=';

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


template <typename T>
void dump_matrix(const boost::multi_array<T, 2>& a) {
    std::cout << "\n";
    for (int r = 0;  r < a.size();  ++r) {
        for (int c = 0;  c < a[0].size();  ++c) {
            std::cout << "  " << a[r][c];
        }
        std::cout << "\n";
    }
}


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
void
needleman_wunsch_alignment_impl(ForwardRange1 const& seq1, ForwardRange2 const& seq2, Cost& cost, boost::multi_array<typename Cost::cost_type, 2>& ca, boost::multi_array<edit_opcode, 1>& ops, boost::multi_array<edit_opcode, 1>::iterator& ops_begin) {
    typedef typename range_iterator<ForwardRange1 const>::type itr1_t;
    typedef typename range_iterator<ForwardRange2 const>::type itr2_t;
    typedef typename Cost::cost_type cost_t;
    typedef boost::multi_array<cost_t, 2> cost_array_t;
    typedef typename cost_array_t::element* itrc_t;
    typedef typename cost_array_t::size_type size_type;
    size_type len1 = distance(seq1);
    size_type len2 = distance(seq2);
    ca.resize(boost::extents[1+len1][1+len2]);
    boost::multi_array<edit_opcode, 2> opa(boost::extents[1+len1][1+len2]);
    itr1_t beg1 = begin(seq1);
    itr1_t end1 = end(seq1);
    itr2_t beg2 = begin(seq2);
    itr2_t end2 = end(seq2);
    itrc_t c01 = ca.data();
    itrc_t c00 = c01+1;
    itrc_t c11 = c01;
    itrc_t c10 = c00;
    edit_opcode* op00 = opa.data();
    *c01 = 0;
    *op00++ = eql_op;
    for (itr2_t j2 = beg2;  j2 != end2;  ++j2, ++c00, ++c01, ++op00) {
        *c00 = *c01 + cost.cost_ins(*j2);
        *op00 = ins_op;
    }
    ++c00; ++c01;
    for (itr1_t j1 = beg1;  j1 != end1;  ++j1) {
        *c01 = *c11 + cost.cost_del(*j1);
        *op00++ = del_op;
        for (itr2_t j2 = beg2;  j2 != end2;  ++j2, ++c00, ++c01, ++c10, ++c11, ++op00) {
            *c00 = *c11 + cost.cost_sub(*j1, *j2);
            *op00 = (*c00 == *c11) ? eql_op : sub_op;
            cost_t t;
            t = *c01 + cost.cost_ins(*j2);
            if (t < *c00) {
                *c00 = t;
                *op00 = ins_op;
            }
            t = *c10 + cost.cost_del(*j1);
            if (t < *c00) {
                *c00 = t;
                *op00 = del_op;
            }
        }
        ++c00, ++c01, ++c10, ++c11;
    }

    // backtrace the cost matrix to obtain sequence of edit operations
    //dump_matrix(ca);
    //dump_matrix(opa);
    ops.resize(boost::extents[len1+len2]);
    boost::multi_array<edit_opcode, 1>::iterator opbeg = ops.end();
    size_type k1 = len1;
    size_type k2 = len2;
    while (k1 > 0 || k2 > 0) {
        --opbeg;
        *opbeg = opa[k1][k2];
        switch (*opbeg) {
            case ins_op: --k2; break;
            case del_op: --k1; break;
            case sub_op:
            case eql_op: --k1; --k2; break;
        }
    }
    ops_begin = opbeg;
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

typedef boost::mpl::int_<1> costs;
typedef boost::mpl::int_<2> indexes;
typedef boost::mpl::int_<3> elements;

typedef boost::mpl::int_<4> split_eql_sub;

struct parameter_list_is_unimplemented {};

template <typename ParamList>
struct edit_alignment_adaptor_impl {
    // ideally, I use some boost magic to induce an informative compiler error
    // that says "edit alignment for <ParamList> not implemented"
    BOOST_MPL_ASSERT((boost::is_same<ParamList, parameter_list_is_unimplemented>));
};


template <>
struct edit_alignment_adaptor_impl<boost::mpl::vector<> > {
    template <typename ForwardRange1, typename ForwardRange2, typename OutputIterator, typename Cost>
    std::pair<OutputIterator, typename Cost::cost_type>
    operator()(ForwardRange1 const& seq1, ForwardRange2 const& seq2, OutputIterator outi, Cost& cost) {
        boost::multi_array<typename Cost::cost_type, 2> ca;
        boost::multi_array<edit_opcode, 1> ops;
        boost::multi_array<edit_opcode, 1>::iterator ops_begin;
        needleman_wunsch_alignment_impl(seq1, seq2, cost, ca, ops, ops_begin);
        return std::pair<OutputIterator, typename Cost::cost_type>(std::copy(ops_begin, ops.end(), outi), ca[ca.size()-1][ca[0].size()-1]);
    }
};


template <>
struct edit_alignment_adaptor_impl<boost::mpl::vector<costs> > {
    template <typename ForwardRange1, typename ForwardRange2, typename OutputIterator, typename Cost>
    std::pair<OutputIterator, typename Cost::cost_type>
    operator()(ForwardRange1 const& seq1, ForwardRange2 const& seq2, OutputIterator outi, Cost& cost) {
        boost::multi_array<typename Cost::cost_type, 2> ca;
        boost::multi_array<edit_opcode, 1> ops;
        boost::multi_array<edit_opcode, 1>::iterator ops_begin;
        needleman_wunsch_alignment_impl(seq1, seq2, cost, ca, ops, ops_begin);
        boost::multi_array<edit_opcode, 1>::size_type k1=0, k2=0;
        for (boost::multi_array<edit_opcode, 1>::iterator jo = ops_begin;  jo != ops.end();  ++jo) {
            typename Cost::cost_type c = ca[k1][k2];
            switch (*jo) {
                case ins_op: ++k2; break;
                case del_op: ++k1; break;
                case sub_op: case eql_op: ++k1; ++k2; break;
            }
            *outi++ = boost::make_tuple(*jo, ca[k1][k2]-c);
        }
        return std::pair<OutputIterator, typename Cost::cost_type>(outi, ca[k1][k2]);
    }
};

template <>
struct edit_alignment_adaptor_impl<boost::mpl::vector<indexes> > {
    template <typename ForwardRange1, typename ForwardRange2, typename OutputIterator, typename Cost>
    std::pair<OutputIterator, typename Cost::cost_type>
    operator()(ForwardRange1 const& seq1, ForwardRange2 const& seq2, OutputIterator outi, Cost& cost) {
        boost::multi_array<typename Cost::cost_type, 2> ca;
        boost::multi_array<edit_opcode, 1> ops;
        boost::multi_array<edit_opcode, 1>::iterator ops_begin;
        needleman_wunsch_alignment_impl(seq1, seq2, cost, ca, ops, ops_begin);
        boost::multi_array<edit_opcode, 1>::size_type k1=0, k2=0;
        for (boost::multi_array<edit_opcode, 1>::iterator jo = ops_begin;  jo != ops.end();  ++jo) {
            switch (*jo) {
                case ins_op: *outi++ = boost::make_tuple(*jo, k2, 0); ++k2; break;
                case del_op: *outi++ = boost::make_tuple(*jo, k1, 0); ++k1; break;
                case sub_op: case eql_op: *outi++ = boost::make_tuple(*jo, k1, k2); ++k1; ++k2; break;
            }
        }
        return std::pair<OutputIterator, typename Cost::cost_type>(outi, ca[k1][k2]);
    }
};

template <>
struct edit_alignment_adaptor_impl<boost::mpl::vector<elements> > {
    template <typename ForwardRange1, typename ForwardRange2, typename OutputIterator, typename Cost>
    std::pair<OutputIterator, typename Cost::cost_type>
    operator()(ForwardRange1 const& seq1, ForwardRange2 const& seq2, OutputIterator outi, Cost& cost) {
        boost::multi_array<typename Cost::cost_type, 2> ca;
        boost::multi_array<edit_opcode, 1> ops;
        boost::multi_array<edit_opcode, 1>::iterator ops_begin;
        needleman_wunsch_alignment_impl(seq1, seq2, cost, ca, ops, ops_begin);
        typename range_iterator<ForwardRange1 const>::type e1 = begin(seq1);
        typename range_iterator<ForwardRange2 const>::type e2 = begin(seq2);
        typedef typename boost::range_value<ForwardRange1 const>::type vtype1;
        typedef typename boost::range_value<ForwardRange2 const>::type vtype2;
        boost::multi_array<edit_opcode, 1>::size_type k1=0, k2=0;
        for (boost::multi_array<edit_opcode, 1>::iterator jo = ops_begin;  jo != ops.end();  ++jo) {
            switch (*jo) {
                case ins_op: *outi++ = boost::make_tuple(*jo, *e2, default_value<vtype2>()); ++e2; break;
                case del_op: *outi++ = boost::make_tuple(*jo, *e1, default_value<vtype1>()); ++e1; break;
                case sub_op: case eql_op: *outi++ = boost::make_tuple(*jo, *e1, *e2); ++e1; ++e2; break;
            }
        }
        return std::pair<OutputIterator, typename Cost::cost_type>(outi, ca[ca.size()-1][ca[0].size()-1]);
    }
};

template <>
struct edit_alignment_adaptor_impl<boost::mpl::vector<costs, indexes> > {
    template <typename ForwardRange1, typename ForwardRange2, typename OutputIterator, typename Cost>
    std::pair<OutputIterator, typename Cost::cost_type>
    operator()(ForwardRange1 const& seq1, ForwardRange2 const& seq2, OutputIterator outi, Cost& cost) {
        boost::multi_array<typename Cost::cost_type, 2> ca;
        boost::multi_array<edit_opcode, 1> ops;
        boost::multi_array<edit_opcode, 1>::iterator ops_begin;
        needleman_wunsch_alignment_impl(seq1, seq2, cost, ca, ops, ops_begin);
        boost::multi_array<edit_opcode, 1>::size_type k1=0, k2=0;
        for (boost::multi_array<edit_opcode, 1>::iterator jo = ops_begin;  jo != ops.end();  ++jo) {
            typename Cost::cost_type c = ca[k1][k2];
            switch (*jo) {
                case ins_op: *outi++ = boost::make_tuple(*jo, ca[k1][k2+1]-c, k2, 0); ++k2; break;
                case del_op: *outi++ = boost::make_tuple(*jo, ca[k1+1][k2]-c, k1, 0); ++k1; break;
                case sub_op: case eql_op: *outi++ = boost::make_tuple(*jo, ca[k1+1][k2+1]-c, k1, k2); ++k1; ++k2; break;
            }
        }
        return std::pair<OutputIterator, typename Cost::cost_type>(outi, ca[k1][k2]);
    }
};



struct edit_alignment_adaptor_basis_type {
    // param list basis case:
    typedef boost::mpl::vector<> param_list;
    // note, if this adaptor is composed with another, this operator is ignored, only the accumulated param_list matters
    template <typename Sequence1, typename Sequence2, typename OutputIterator, typename Cost>
    std::pair<OutputIterator, typename Cost::cost_type>
    operator()(Sequence1 const& seq1, Sequence2 const& seq2, OutputIterator outi, Cost cost) {
        return edit_alignment_adaptor_impl<param_list>()(boost::as_literal(seq1), boost::as_literal(seq2), outi, cost);
    }
    template <typename Sequence1, typename Sequence2, typename OutputIterator>
    std::pair<OutputIterator, typename default_cost<Sequence1>::cost_type>
    operator()(Sequence1 const& seq1, Sequence2 const& seq2, OutputIterator outi) {
        return (*this)(seq1, seq2, outi, default_cost<Sequence1>());
    }
};

static edit_alignment_adaptor_basis_type edit_alignment;

template <typename F, typename Param>
struct edit_alignment_adaptor_type {
    typedef typename append_sorted_unique<typename F::param_list, Param>::type param_list;
    // note, if this adaptor is composed with another, this operator is ignored, only the accumulated param_list matters
    template <typename Sequence1, typename Sequence2, typename OutputIterator, typename Cost>
    std::pair<OutputIterator, typename Cost::cost_type>
    operator()(Sequence1 const& seq1, Sequence2 const& seq2, OutputIterator outi, Cost cost) {
        return edit_alignment_adaptor_impl<param_list>()(boost::as_literal(seq1), boost::as_literal(seq2), outi, cost);
    }
    template <typename Sequence1, typename Sequence2, typename OutputIterator>
    std::pair<OutputIterator, typename default_cost<Sequence1>::cost_type>
    operator()(Sequence1 const& seq1, Sequence2 const& seq2, OutputIterator outi) {
        return (*this)(seq1, seq2, outi, default_cost<Sequence1>());
    }
};

template <typename Param>
edit_alignment_adaptor_type<edit_alignment_adaptor_basis_type, Param>
acquire(edit_alignment_adaptor_basis_type) {
    return edit_alignment_adaptor_type<edit_alignment_adaptor_basis_type, Param>();
}

template <typename Param, typename F>
edit_alignment_adaptor_type<F, Param>
acquire(F) {
    return edit_alignment_adaptor_type<F, Param>();
}


#endif
