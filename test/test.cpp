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

#include <boost/mpl/int.hpp>
#include <boost/mpl/plus.hpp>
#include <boost/mpl/arithmetic.hpp>
#include <boost/type_traits/is_same.hpp>

using boost::mpl::int_;

template <typename MPL_Seq> 
struct mpl_seq_sum {
    typedef typename boost::mpl::fold<MPL_Seq, int_<0>, boost::mpl::plus<boost::mpl::_1, boost::mpl::_2> >::type type;
};

void f_impl(int w, int& v) { v = 3+w; }

int f(int w) {
    int v;
    f_impl(w,v);
    return v; 
}

int f() {
    return f(0);
}

template <typename ParamList>
int f_adaptor_impl(int v) {
    return v + mpl_seq_sum<ParamList>::type::value;
}

template <>
int f_adaptor_impl<boost::mpl::vector<int_<1> > >(int v) {
    std::cout << "<1> specialization!\n";
    return 111;
}
template <>
int f_adaptor_impl<boost::mpl::vector<int_<2> > >(int v) {
    std::cout << "<2> specialization!\n";
    return 222;
}
template <>
int f_adaptor_impl<boost::mpl::vector<int_<1>, int_<2> > >(int v) {
    std::cout << "<1,2> specialization!\n";
    return 111222;
}

template <typename Param>
struct f_adaptor_type_basis {
    // param list basis case:
    typedef boost::mpl::vector<Param> param_list;
    // note, if this adaptor is composed with another, this operator is ignored, only the accumulated param_list matters
    int operator()(int w) {
        int v;
        f_impl(w, v);
        return f_adaptor_impl<param_list>(v);
    }
    int operator()() {
        return (*this)(0);
    }
};

template <typename F, typename Param>
struct f_adaptor_type {
    // accumulating param_list from below:
    typedef typename append<typename F::param_list, Param>::type param_list;
    // note, if this adaptor is composed with another, this operator is ignored, only the accumulated param_list matters
    int operator()(int w) {
        int v;
        f_impl(w,v);
        return f_adaptor_impl<param_list>(v);
    }
    int operator()() {
        return (*this)(0);
    }
};

template <typename Param, typename F>
f_adaptor_type<F, Param>
f_adaptor(F func) {
    f_adaptor_type<F, Param>();
}

template <typename Param>
f_adaptor_type_basis<Param>
f_adaptor(int (*func)(int)) {
    f_adaptor_type_basis<Param>();
}

BOOST_AUTO_TEST_CASE(adaptor) {
    std::cout << f() << " " << f(1000) <<"\n";
    std::cout << f_adaptor<int_<7> >(f)() << " " <<  f_adaptor<int_<7> >(f)(1000)  << "\n";
    std::cout << f_adaptor<int_<7> >(f_adaptor<int_<7> >(f))() << " " <<  f_adaptor<int_<7> >(f_adaptor<int_<7> >(f))(1000)  << "\n";
    std::cout << f_adaptor<int_<1> >(f)() << "\n";
    std::cout << f_adaptor<int_<2> >(f)() << "\n";
    std::cout << f_adaptor<int_<2> >(f_adaptor<int_<1> >(f))() << "\n";
    typedef append<boost::mpl::vector<int_<1> >, int_<2> >::type result;
    BOOST_MPL_ASSERT((boost::is_same<result, boost::mpl::vector<int_<1>, int_<2> > >));

    typedef boost::mpl::vector<int_<3>, int_<1>, int_<2>, int_<1>, int_<2>, int_<3> > vv;
    typedef typename boost::mpl::sort<vv>::type vvs;
    typedef typename boost::mpl::unique<vvs, boost::is_same<boost::mpl::_1, boost::mpl::_2> >::type vvu;
    //BOOST_MPL_ASSERT((boost::is_same<vvu, boost::mpl::vector<int_<2>, int_<1>, int_<3> > >));
    typedef append<boost::mpl::vector<>, int_<1> >::type rr;
    BOOST_MPL_ASSERT((boost::is_same<rr, boost::mpl::vector<int_<1> > >));
    typedef typename boost::mpl::fold<vvu, boost::mpl::vector<>, append<boost::mpl::_1, boost::mpl::_2> >::type vvuf;
    BOOST_MPL_ASSERT((boost::is_same<vvuf, boost::mpl::vector<int_<1>, int_<2>, int_<3> > >));
    typedef boost::mpl::vector<> v0;
    typedef typename append_sorted_unique<v0, int_<3> >::type v1;
    typedef typename append_sorted_unique<v1, int_<2> >::type v2;
    typedef typename append_sorted_unique<v2, int_<1> >::type v3;
    typedef typename append_sorted_unique<v3, int_<3> >::type v4;
    typedef typename append_sorted_unique<v4, int_<2> >::type v5;
    typedef typename append_sorted_unique<v5, int_<1> >::type v6;
    BOOST_MPL_ASSERT((boost::is_same<v6, boost::mpl::vector<int_<1>, int_<2>, int_<3> > >));
}
