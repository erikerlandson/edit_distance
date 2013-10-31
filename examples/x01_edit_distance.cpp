/*******
edit_distance: STL and Boost compatible edit distance functions for C++

Copyright (c) 2013 Erik Erlandson

Author:  Erik Erlandson <erikerlandson@yahoo.com>

Distributed under the Boost Software License, Version 1.0.
See accompanying file LICENSE or copy at
http://www.boost.org/LICENSE_1_0.txt
*******/

#include <iostream>

// get the edit_distance() function
#include <boost/algorithm/sequence_alignment/edit_distance.hpp>
using boost::algorithm::sequence_alignment::edit_distance;

#include <boost/function_types/result_type.hpp>
#include <boost/function_types/function_arity.hpp>
#include <boost/function_types/parameter_types.hpp>
#include <boost/function_types/is_member_function_pointer.hpp>
#include <boost/function_types/is_member_object_pointer.hpp>
#include <boost/type_traits.hpp>
#include <boost/type_traits/is_same.hpp>
#include <boost/utility/enable_if.hpp>
#include <boost/function.hpp>
#include <boost/typeof/std/utility.hpp>
#include <boost/mpl/has_xxx.hpp>
#include <boost/mpl/at.hpp>
#include <boost/mpl/if.hpp>
#include <boost/concept/requires.hpp>
#include <boost/mpl/assert.hpp>

#if 0
namespace introspection {
/*
    - Multiple inheritance forces ambiguity of member names.
    - SFINAE is used to make aliases to member names.
    - Expression SFINAE is used in just one generic has_member that can accept
      any alias we pass it.
*/

//Variadic to force ambiguity of class members.  C++11 and up.
//template <typename... Args> struct ambiguate : public Args... {};

//Non-variadic version of the line above.
template <typename A, typename B> struct ambiguate : public A, public B {};

template<typename A, typename = void>
struct got_type : false_type {};

template<typename A>
struct got_type<A> : true_type {
    typedef A type;
};

template<typename T, T>
struct sig_check : true_type {};

template<typename Alias, typename AmbiguitySeed>
struct has_member {
    template<typename C> static char ((   &f(  BOOST_TYPEOF(&C::value)  )   ))[1];
    template<typename C> static char ((&f(...)))[2];

#if 0
    //Make sure the member name is consistently spelled the same.
    static_assert(
        (sizeof(f<AmbiguitySeed>(0)) == 1)
        , "Member name specified in AmbiguitySeed is different from member name specified in Alias, or wrong Alias/AmbiguitySeed has been specified."
    );
#endif

    static bool const value = sizeof(f<Alias>(0)) == 2;
};

//Check for any member with given name, whether var, func, class, union, enum.
#define CREATE_MEMBER_CHECK(member)                                         \
                                                                            \
template<typename T, typename = true_type>                             \
struct Alias_##member;                                                      \
                                                                            \
template<typename T>                                                        \
struct Alias_##member <                                                     \
    T, std::integral_constant<bool, got_type<BOOST_TYPEOF(&T::member)>::value>  \
> { static const BOOST_TYPEOF(&T::member) value; };                             \
                                                                            \
struct AmbiguitySeed_##member { char member; };                             \
                                                                            \
template<typename T>                                                        \
struct has_member_##member {                                                \
    static const bool value                                                 \
        = has_member<                                                       \
            Alias_##member<ambiguate<T, AmbiguitySeed_##member> >            \
            , Alias_##member<AmbiguitySeed_##member>                        \
        >::value                                                            \
    ;                                                                       \
}

//Check for member variable with given name.
#define CREATE_MEMBER_VAR_CHECK(var_name)                                   \
                                                                            \
template<typename T, typename = true_type>                             \
struct has_member_var_##var_name : false_type {};                      \
                                                                            \
template<typename T>                                                        \
struct has_member_var_##var_name<                                           \
    T                                                                       \
    , std::integral_constant<                                               \
        bool                                                                \
        , !std::is_member_function_pointer<BOOST_TYPEOF(&T::var_name)>::value   \
    >                                                                       \
> : true_type {}


//Check for member function with given name AND signature.
#define CREATE_MEMBER_FUNC_SIG_CHECK(func_name, func_sig, templ_postfix)    \
                                                                            \
template<typename T, typename = true_type>                             \
struct has_member_func_##templ_postfix : false_type {};                \
                                                                            \
template<typename T>                                                        \
struct has_member_func_##templ_postfix<                                     \
    T, std::integral_constant<                                              \
        bool                                                                \
        , sig_check<func_sig, &T::func_name>::value                         \
    >                                                                       \
> : true_type {}


//Check for member class with given name.
#define CREATE_MEMBER_CLASS_CHECK(class_name)               \
                                                            \
template<typename T, typename = true_type>             \
struct has_member_class_##class_name : false_type {};  \
                                                            \
template<typename T>                                        \
struct has_member_class_##class_name<                       \
    T                                                       \
    , std::integral_constant<                               \
        bool                                                \
        , std::is_class<                                    \
            typename got_type<typename T::class_name>::type \
        >::value                                            \
    >                                                       \
> : true_type {}


//Check for member union with given name.
#define CREATE_MEMBER_UNION_CHECK(union_name)               \
                                                            \
template<typename T, typename = true_type>             \
struct has_member_union_##union_name : false_type {};  \
                                                            \
template<typename T>                                        \
struct has_member_union_##union_name<                       \
    T                                                       \
    , std::integral_constant<                               \
        bool                                                \
        , std::is_union<                                    \
            typename got_type<typename T::union_name>::type \
        >::value                                            \
    >                                                       \
> : true_type {}


//Check for member enum with given name.
#define CREATE_MEMBER_ENUM_CHECK(enum_name)                 \
                                                            \
template<typename T, typename = true_type>             \
struct has_member_enum_##enum_name : false_type {};    \
                                                            \
template<typename T>                                        \
struct has_member_enum_##enum_name<                         \
    T                                                       \
    , std::integral_constant<                               \
        bool                                                \
        , std::is_enum<                                     \
            typename got_type<typename T::enum_name>::type  \
        >::value                                            \
    >                                                       \
> : true_type {}


#define CREATE_MEMBER_FUNC_CHECK(func)          \
template<typename T>                            \
struct has_member_func_##func {                 \
    static const bool value                     \
        = has_member_##func<T>::value           \
        && !has_member_var_##func<T>::value     \
        && !has_member_class_##func<T>::value   \
        && !has_member_union_##func<T>::value   \
        && !has_member_enum_##func<T>::value    \
    ;                                           \
}


//Create all the checks for one member.  Does NOT include func sig checks.
#define CREATE_MEMBER_CHECKS(member)    \
CREATE_MEMBER_CHECK(member);            \
CREATE_MEMBER_VAR_CHECK(member);        \
CREATE_MEMBER_CLASS_CHECK(member);      \
CREATE_MEMBER_UNION_CHECK(member);      \
CREATE_MEMBER_ENUM_CHECK(member);       \
CREATE_MEMBER_FUNC_CHECK(member)

}
#endif

BOOST_MPL_HAS_XXX_TRAIT_DEF(cost_type)


#define CREATE_MEMBER_CHECK(member) \
namespace __has_member_##member { \
template <typename T, typename U> struct ambig : public T, public U {}; \
template <typename T, typename U, typename Enable = void> \
struct test { \
    static const bool value = true; \
}; \
template <typename T, typename U> \
struct test  <T, U, typename boost::enable_if<boost::is_same<BOOST_TYPEOF(&T:: member), BOOST_TYPEOF(&U:: member)> >::type> { \
    static const bool value = false; \
}; \
struct seed { char member ; }; \
} \
template <typename T> struct has_member_##member : public \
 boost::integral_constant<bool, __has_member_##member::test<__has_member_##member::ambig<T, __has_member_##member::seed>, __has_member_##member::seed>::value> {}

struct sss0 {};
struct sss1 { 
    int member;
    void foo() {}
};

template <typename Z, bool F> struct ct {
    typedef typename boost::function_types::result_type< BOOST_TYPEOF(&Z::m2) >::type type;
};
template <typename Z> struct ct<Z, true> {
    typedef typename Z::cost_type type;
};

template <typename X>
struct get_cost_type {

typedef typename ct<X, has_cost_type<X>::value >::type type;
};

template <typename X>
struct TTT {
    BOOST_MPL_ASSERT((boost::function_types::is_member_function_pointer<BOOST_TYPEOF(&X::m1)>));
    BOOST_MPL_ASSERT((boost::function_types::is_member_function_pointer<BOOST_TYPEOF(&X::m2)>));
    BOOST_MPL_ASSERT((boost::function_types::is_member_function_pointer<BOOST_TYPEOF(&X::m3)>));
    typedef BOOST_TYPEOF(&X::m2) m2_type;
    //typedef typename boost::function_types::result_type<m2_type>::type cost_type;
    typedef typename get_cost_type<X>::type cost_type;
    typedef typename boost::mpl::at_c<typename boost::function_types::parameter_types<m2_type>::type, 1>::type value_type;
        //BOOST_CONCEPT_ASSERT((Arithmetic<cost_type>));    
    BOOST_CONCEPT_USAGE(TTT) {
        c = x.m2(v);
    }
    X x;
    cost_type c;
    value_type v;
};

struct ttt {
    bool m1() { return false; }
    int m2(int v) { return 17+v; }
    bool m3(int v1, float v2) { return true; }
};

template <typename T>
BOOST_CONCEPT_REQUIRES(
    ((TTT<T>)),
(int))
fff(T t) {
    return t.m2(0);
}


CREATE_MEMBER_CHECK(member);
CREATE_MEMBER_CHECK(foo);

int main(int argc, char** argv) {
    std::cout << has_member_member<sss0>::value << "\n";
    std::cout << has_member_member<sss1>::value << "\n";
    std::cout << has_member_foo<sss0>::value << "\n";
    std::cout << has_member_foo<sss1>::value << "\n";
        //std::cout << has_member<ambiguate<sss0> >::value << "\n";
        //std::cout << has_member<ambiguate<sss1> >::value << "\n";
        //std::cout << typeid(BOOST_TYPEOF(&ambiguate<sss0>::member)).name() << "\n";
        //std::cout << typeid(BOOST_TYPEOF(&ambiguate<sss1>::member)).name() << "\n";

    return 0;

    std::cout << fff(ttt()) << "\n";
    std::cout << typeid(typename get_cost_type<ttt>::type).name() << "\n";

    typedef BOOST_TYPEOF(&ttt::m3) m3_type;
    // arity
    std::cout << boost::function_types::function_arity<m3_type>::value << "\n";
    // arguments (arg 0 is 'this' for member funcs)
    std::cout << typeid(boost::mpl::at_c<boost::function_types::parameter_types<m3_type>::type, 0>::type).name() << "\n";
    std::cout << typeid(boost::mpl::at_c<boost::function_types::parameter_types<m3_type>::type, 1>::type).name() << "\n";
    std::cout << typeid(boost::mpl::at_c<boost::function_types::parameter_types<m3_type>::type, 2>::type).name() << "\n";
    // return type
    std::cout << typeid(boost::function_types::result_type<m3_type>::type).name() << "\n";

    return 0;


    char const* str1 = "Oh, hello world.";
    char const* str2 = "Hello world!!";

    // Obtain the cost of minimal edit sequence to transform str1 --> str2.
    // The default cost function defines the cost of insertion, deletion and substitution to be 1.  Elements that are equal cost 0.
    // The distance should be 7: 
    // delete 4: "Oh, "
    // substitute 1: "h"->"H",
    // substitute 1: "." -> "!"
    // insert 1: "!"
    unsigned dist = edit_distance(str1, str2);
    std::cout << "The edit distance between \"" << str1 << "\" and \"" << str2 << "\" = " << dist << "\n";

    return 0;
}
