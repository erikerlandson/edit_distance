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
using boost::algorithm::sequence_alignment::unit_cost;

int main(int argc, char** argv) {
    char const* str1 = "Oh, hello world.";
    char const* str2 = "Hello world!!";

    // Generate the edit script from str1 to str2.
    // The output object 'out' processes the edit script operations in sequence
    // An output class must define types value_type and cost_type (similar to 
    // the cost function object for edit_distance), and the methods:
    //   output_ins(v, c)        // element v inserted, with insertion cost c
    //   output_del(v, c)        // element v deleted, with deletion cost c
    //   output_sub(v1, v2, c)   // v1 subsituted with v2, subst cost c
    //   output_eql(v1, v2)      // v1 equivalent to value v2
    stringstream_tuple_output<unit_cost, char const*> out;
    unsigned dist = edit_alignment(str1, str2, out);
    std::cout << "dist= " << dist << "   edit operations= " << out.ss.str() << "\n";

    return 0;
}
