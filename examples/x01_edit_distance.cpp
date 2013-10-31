/*******
edit_distance: STL and Boost compatible edit distance functions for C++

Copyright (c) 2013 Erik Erlandson

Author:  Erik Erlandson <erikerlandson@yahoo.com>

Distributed under the Boost Software License, Version 1.0.
See accompanying file LICENSE or copy at
http://www.boost.org/LICENSE_1_0.txt
*******/

#include <iostream>

#include <boost/type_traits.hpp>
#include <boost/type_traits/is_same.hpp>
#include <boost/utility/enable_if.hpp>
#include <boost/typeof/std/utility.hpp>


// put working definitions inside a namespace \
// when T and U both have a member 'member', this will induce an ambiguity failure: \
// struct 'test' uses SFINAE to resolve to 'true' when T defines 'member' \
// this specialization of 'test' matches when T does not define 'member' \
// 'seed' exists to define a 'member', that will induce an ambiguity error when  \
// the class of interest also defines 'member' \

// this macro defines a predicate template to check a class for a member 'member'
#define CREATE_MEMBER_CHECK(member) \
namespace __has_member_##member { \
 \
template <typename T, typename U> struct ambig : public T, public U {}; \
 \
template <typename T, typename U, typename Enable = void> \
struct test { \
    static const bool value = true; \
}; \
template <typename T, typename U> \
struct test  <T, U, typename boost::enable_if<boost::is_same<BOOST_TYPEOF(&T:: member), BOOST_TYPEOF(&U:: member)> >::type> { \
    static const bool value = false; \
}; \
 \
struct seed { char member ; }; \
} \
template <typename T> struct has_member_##member : public \
boost::integral_constant<bool, __has_member_##member::test<__has_member_##member::ambig<T, __has_member_##member::seed>, __has_member_##member::seed>::value> {}

// this test class has no members
struct sss0 {};

// this one has all the members
struct sss1 { 
    void foo() {}
    int bar;
};

// define member-checking templates for 'foo' and 'bar'
CREATE_MEMBER_CHECK(foo);
CREATE_MEMBER_CHECK(bar);

int main(int argc, char** argv) {
    // outputs 0 (false)
    std::cout << has_member_foo<sss0>::value << "\n";
    // outputs 1 (true)
    std::cout << has_member_foo<sss1>::value << "\n";

    // outputs 0 (false)
    std::cout << has_member_bar<sss0>::value << "\n";
    // outputs 1 (true)
    std::cout << has_member_bar<sss1>::value << "\n";

    return 0;
}
