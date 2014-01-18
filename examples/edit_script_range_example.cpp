/*******
edit_distance: STL and Boost compatible edit distance functions for C++

Copyright (c) 2013 Erik Erlandson

Author:  Erik Erlandson <erikerlandson@yahoo.com>

Distributed under the Boost Software License, Version 1.0.
See accompanying file LICENSE or copy at
http://www.boost.org/LICENSE_1_0.txt
*******/

#include "edit_distance_common.hpp"

#include <boost/algorithm/sequence/edit_distance.hpp>
using namespace boost::algorithm::sequence::parameter;
using boost::algorithm::sequence::edit_distance;
using boost::algorithm::sequence::unit_cost;

int main(int argc, char** argv) {
    char const* str1 = "abc";
    char const* str2 = "axc";

    // Compare two null-terminated strings that differ by one substitution
    // (distance should be 2)
    stringstream_tuple_output<unit_cost, char const*> out;
    unsigned dist = edit_distance(str1, str2, _script = out);
    std::cout << "dist= " << dist << "   edit operations= " << out.ss.str() << "\n";

    // Any type of sequences or range adaptors can be 
    // applied as sequence arguments (here distance should be 4: "abc" -> "cxa" (-a, -b, =c, +x, +a)
    out.ss.str("");
    dist = edit_distance(as_vector(str1), as_list(str2) | boost::adaptors::reversed, _script = out);
    std::cout << "dist= " << dist << "   edit operations= " << out.ss.str() << "\n";

    return 0;
}
