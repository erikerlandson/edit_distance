/*******
edit_distance: STL and Boost compatible edit distance functions for C++

Copyright (c) 2013 Erik Erlandson

Author:  Erik Erlandson <erikerlandson@yahoo.com>

Distributed under the Boost Software License, Version 1.0.
See accompanying file LICENSE or copy at
http://www.boost.org/LICENSE_1_0.txt
*******/

#if !defined(BOOST_ALGORITHM_SEQUENCE_ALIGNMENT_DETAIL_EDIT_ALIGNMENT_HPP)
#define BOOST_ALGORITHM_SEQUENCE_ALIGNMENT_DETAIL_EDIT_ALIGNMENT_HPP

#include <boost/mpl/vector.hpp>

#include <boost/concept/requires.hpp>

#include <boost/range/as_literal.hpp>
#include <boost/range/metafunctions.hpp>

#include <boost/multi_array.hpp>

#include <boost/algorithm/sequence_alignment/edit_types.hpp>
#include <boost/algorithm/sequence_alignment/detail/mpl_utils.hpp>

#include <boost/tuple/tuple.hpp>
#include <boost/tuple/tuple_io.hpp>

namespace boost {
namespace algorithm {
namespace sequence_alignment {
namespace detail {

using boost::distance;
using boost::begin;
using boost::end;
using boost::range_iterator;

using detail::SequenceAlignmentCost;
using detail::ForwardRangeConvertible;

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
            cost_t t;
            *c00 = *c01 + cost.cost_ins(*j2);
            *op00 = ins_op;
            t = *c11 + cost.cost_sub(*j1, *j2);
            if (t < *c00) {
                *c00 = t;
                *op00 = (*c00 == *c11) ? eql_op : sub_op;
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
                case ins_op: *outi++ = boost::make_tuple(*jo, 0, k2); ++k2; break;
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
        for (boost::multi_array<edit_opcode, 1>::iterator jo = ops_begin;  jo != ops.end();  ++jo) {
            switch (*jo) {
                case ins_op: *outi++ = boost::make_tuple(*jo, default_value<vtype1>(), *e2); ++e2; break;
                case del_op: *outi++ = boost::make_tuple(*jo, *e1, default_value<vtype2>()); ++e1; break;
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
                case ins_op: *outi++ = boost::make_tuple(*jo, ca[k1][k2+1]-c, 0, k2); ++k2; break;
                case del_op: *outi++ = boost::make_tuple(*jo, ca[k1+1][k2]-c, k1, 0); ++k1; break;
                case sub_op: case eql_op: *outi++ = boost::make_tuple(*jo, ca[k1+1][k2+1]-c, k1, k2); ++k1; ++k2; break;
            }
        }
        return std::pair<OutputIterator, typename Cost::cost_type>(outi, ca[k1][k2]);
    }
};

template <>
struct edit_alignment_adaptor_impl<boost::mpl::vector<costs, elements> > {
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
            typename Cost::cost_type c = ca[k1][k2];
            switch (*jo) {
                case ins_op: *outi++ = boost::make_tuple(*jo, ca[k1][k2+1]-c, default_value<vtype1>(), *e2); ++e2; ++k2; break;
                case del_op: *outi++ = boost::make_tuple(*jo, ca[k1+1][k2]-c, *e1, default_value<vtype2>()); ++e1; ++k1; break;
                case sub_op: case eql_op: *outi++ = boost::make_tuple(*jo, ca[k1+1][k2+1]-c, *e1, *e2); ++e1; ++e2; ++k1; ++k2; break;
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

}}}}

#endif
