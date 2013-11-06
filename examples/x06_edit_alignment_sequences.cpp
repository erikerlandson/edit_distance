/*******
edit_distance: STL and Boost compatible edit distance functions for C++

Copyright (c) 2013 Erik Erlandson

Author:  Erik Erlandson <erikerlandson@yahoo.com>

Distributed under the Boost Software License, Version 1.0.
See accompanying file LICENSE or copy at
http://www.boost.org/LICENSE_1_0.txt
*******/

#include "x_common.hpp"

// get the edit_alignment() function
#include <boost/algorithm/sequence_alignment/edit_alignment.hpp>
using boost::algorithm::sequence_alignment::edit_alignment;
using boost::algorithm::sequence_alignment::default_cost;

int main(int argc, char** argv) {
    char const* str1 = "abc";
    char const* str2 = "axc";

    // Compare two null-terminated strings that differ by one substitution
    // (distance should be 1)
    stringstream_tuple_output<default_cost, char const*> out;
    unsigned dist = edit_alignment(str1, str2, out);
    std::cout << "dist= " << dist << "   edit operations= " << out.ss.str() << "\n";

    // As with edit_distance(), any type of sequences or range adaptors can be 
    // applied as sequence arguments (here distance should be 3: "abc" -> "cxa")
    out.ss.str("");
    dist = edit_alignment(as_vector(str1), as_list(str2) | boost::adaptors::reversed, out);
    std::cout << "dist= " << dist << "   edit operations= " << out.ss.str() << "\n";

    return 0;
}
