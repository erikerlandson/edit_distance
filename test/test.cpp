/*******
edit_distance: STL and Boost compatible edit distance functions for C++

Copyright (c) 2013 Erik Erlandson

Author:  Erik Erlandson <erikerlandson@yahoo.com>

Distributed under the Boost Software License, Version 1.0.
See accompanying file LICENSE or copy at
http://www.boost.org/LICENSE_1_0.txt
*******/

#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE edit_distance
#include <boost/test/unit_test.hpp>

#include "ut_common.h"

#include <boost/mpl/set.hpp>
#include <boost/mpl/vector.hpp>
#include <boost/mpl/insert.hpp>
#include <boost/mpl/int.hpp>
#include <boost/mpl/next_prior.hpp>
#include <boost/mpl/plus.hpp>
#include <boost/mpl/arithmetic.hpp>
#include <boost/mpl/push_back.hpp>
#include <boost/mpl/fold.hpp>

using boost::mpl::int_;
using boost::mpl::insert;


template <typename MPL_Seq> 
struct mpl_seq_sum {
    typedef typename boost::mpl::fold<MPL_Seq, int_<0>, boost::mpl::plus<boost::mpl::_1, boost::mpl::_2> >::type type;
};

template <typename Set, typename Arg>
struct union_with {
    typedef typename boost::mpl::insert<Set, Arg>::type type;
};

void f_impl(int& v) { v = 3; }

int f() {
    int v;
    f_impl(v);
    return v;
}

template <typename ParamList>
int f_adaptor_impl(int v) {
    return v + mpl_seq_sum<ParamList>::type::value;
}

template <typename Param>
struct f_adaptor_type_basis {
    // param list basis case:
    typedef boost::mpl::vector<Param> param_list;
    // note, if this adaptor is composed with another, this operator is ignored, only the accumulated param_list matters
    int operator()() {
        int v;
        f_impl(v);
        return f_adaptor_impl<param_list>(v);
    }
};

template <typename F, typename Param>
struct f_adaptor_type {
    // accumulating param_list from below:
    typedef typename boost::mpl::push_back<typename F::param_list, Param>::type param_list;
    // note, if this adaptor is composed with another, this operator is ignored, only the accumulated param_list matters
    int operator()() {
        int v;
        f_impl(v);
        return f_adaptor_impl<param_list>(v);
    }
    F ff;
};

template <typename Param, typename F>
f_adaptor_type<F, Param>
f_adaptor(F func) {
    f_adaptor_type<F, Param> ff;
    ff.ff = func;
    return ff;
}

template <typename Param>
f_adaptor_type_basis<Param>
f_adaptor(int (*func)()) {
    f_adaptor_type_basis<Param> ff;
    return ff;
}

BOOST_AUTO_TEST_CASE(adaptor) {
    std::cout << f() << "\n";
        //typedef boost::mpl::int_<1> n;
        //std::cout << n::value << "\n";
        //std::cout << boost::mpl::plus<n, boost::mpl::int_<1> >::value << "\n";
    std::cout << f_adaptor<int_<7> >(f)() << "\n";
    std::cout << f_adaptor<int_<7> >(f_adaptor<int_<7> >(f))() << "\n";
    std::cout << boost::mpl::plus<int_<1>,int_<2> >::value << "\n";
    typedef typename mpl_seq_sum<boost::mpl::vector<int_<1>,int_<2>,int_<3> > >::type sum_t; 
    std::cout << sum_t::value << "\n";
}
