/*******
Demonstrate a Boost/MPL style macro to create predicate templates for class member checking

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


// This macro defines a predicate template to check a class for a member 'member'
// 'member' can be any definition: member variable, member function, a nested type, etc
// 
// It works like this: first, it declares a namespace to cache working typedefs.
// When T and U both have a member 'member', struct 'ambig' will induce an ambiguity failure
// The struct 'test' uses SFINAE to resolve to 'true' when T defines 'member' \
// The specialization of 'test' matches when T does *not* define 'member' \
// Struct 'seed' exists to define a 'member', that will induce an ambiguity error when \
// the class of interest also defines 'member' \
// 
#define CREATE_MEMBER_CHECK(member) \
namespace __has_member_##member { \
template <typename T, typename U> struct ambig : public T, public U {}; \
\
template <typename T, typename U, typename Enable = void> \
struct test : public boost::true_type {}; \
template <typename T, typename U> \
struct test<T, U, typename boost::enable_if<boost::is_same<BOOST_TYPEOF(&T:: member), BOOST_TYPEOF(&U:: member)> >::type> : public boost::false_type {}; \
\
struct seed { char member ; }; \
} \
template <typename T> struct has_member_##member : public __has_member_##member::test<__has_member_##member::ambig<T, __has_member_##member::seed>, __has_member_##member::seed> {}


// define a couple test classes
// this test class has no members
struct sss0 {};

// this one has all the members
struct sss1 { 
    void foo() {}
    int bar;
    typedef int baz;
};

// define member-checking templates for 'foo', 'bar' and 'baz:
CREATE_MEMBER_CHECK(foo);
CREATE_MEMBER_CHECK(bar);
CREATE_MEMBER_CHECK(baz);

// Now demonstrate the checking templates in action.  Note that the template
// inherits from either boost::true_type or boost::false_type, so it can be used in
// all the same kind of MPL contexts 
int main(int argc, char** argv) {
    // outputs 0 (false)
    std::cout << has_member_foo<sss0>::value << "\n";
    // outputs 1 (true)
    std::cout << has_member_foo<sss1>::value << "\n";

    // outputs 0 (false)
    std::cout << has_member_bar<sss0>::value << "\n";
    // outputs 1 (true)
    std::cout << has_member_bar<sss1>::value << "\n";

    // outputs 0 (false)
    std::cout << has_member_baz<sss0>::value << "\n";
    // outputs 1 (true)
    std::cout << has_member_baz<sss1>::value << "\n";

    return 0;
}
