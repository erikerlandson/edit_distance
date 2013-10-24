/*******
edit_distance: STL and Boost compatible edit distance functions for C++

Copyright (c) 2013 Erik Erlandson

Author:  Erik Erlandson <erikerlandson@yahoo.com>

Distributed under the Boost Software License, Version 1.0.
See accompanying file LICENSE or copy at
http://www.boost.org/LICENSE_1_0.txt
*******/

#if !defined(BOOST_ALGORITHM_SEQUENCE_ALIGNMENT_EDIT_ALIGNMENT_HPP)
#define BOOST_ALGORITHM_SEQUENCE_ALIGNMENT_EDIT_ALIGNMENT_HPP

#include <boost/algorithm/sequence_alignment/edit_types.hpp>
#include <boost/algorithm/sequence_alignment/detail/edit_alignment.hpp>

namespace boost {
namespace algorithm {
namespace sequence_alignment {

#if 0
using detail::edit_alignment_adaptor_basis_type;
using detail::edit_alignment_adaptor_type;

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


static edit_alignment_adaptor_basis_type edit_alignment;

#endif

}}}

#endif
