/*******
edit_distance: STL and Boost compatible edit distance functions for C++

Copyright (c) 2013 Erik Erlandson

Author:  Erik Erlandson <erikerlandson@yahoo.com>

Distributed under the Boost Software License, Version 1.0.
See accompanying file LICENSE or copy at
http://www.boost.org/LICENSE_1_0.txt
*******/

#include "edit_distance_common.hpp"

// get the edit_alignment() function
#include <boost/algorithm/sequence/edit_distance.hpp>
using boost::algorithm::sequence::edit_distance;
using namespace boost::algorithm::sequence::parameter;
using boost::algorithm::sequence::unit_cost;

int main(int argc, char** argv) {
    char const* str1 = "Oh, hello world.";
    char const* str2 = "Hello world!!";

    // Generate the edit script from str1 to str2.
    // The output object 'out' processes the edit script operations in sequence
    // An output class must define types value_type and cost_type (similar to 
    // the cost function object for edit_distance), and the methods:
    //   insertion(v, c)        // element v inserted, with insertion cost c
    //   deletion(v, c)        // element v deleted, with deletion cost c
    //   substitution(v1, v2, c)   // v1 subsituted with v2, subst cost c
    //   equality(v1, v2)      // v1 == v2
    //
    // Defining substitution() is optional if substitution is compile-time disabled (the default)
    // To enable, pass the optional _substitution=boost::true_type(), or _substitution=<bool-value>
    stringstream_tuple_output<unit_cost, char const*> out;
    unsigned dist = edit_distance(str1, str2, _script = out);
    std::cout << "dist= " << dist << "   edit operations= " << out.ss.str() << "\n";

    return 0;
}
