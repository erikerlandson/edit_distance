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

int main(int argc, char** argv) {
    char const* str1 = "Oh, hello world.";
    char const* str2 = "Hello world!!";

    // obtain the cost of minimal edit sequence to transform str1 --> str2
    // The distance should be 7: 
    // delete 4: "Oh, "
    // substitute 1: "h"->"H",
    // substitute 1: "." -> "!"
    // insert 1: "!"
    unsigned dist = edit_distance(str1, str2);
    std::cout << "The edit distance between \"" << str1 << "\" and \"" << str2 << "\" = " << dist << "\n";

    return 0;
}
