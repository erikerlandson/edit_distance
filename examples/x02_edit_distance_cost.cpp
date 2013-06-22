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


// define a custom cost function where insertion or deletion of space costs nothing
struct cost_free_space {
    // note, these types are required for a cost function:
    typedef unsigned cost_type;  // edit_distance uses cost_type to store cost values internally
                                 // you can define it smaller to save space, or define it as
                                 // floating point to support non-integer costs, etc.
    typedef char value_type;     // sequence elements must be assignable to value_type

    // inserting or deleting a space is free:
    cost_type cost_ins(value_type c) { return (c == ' ') ? 0 : 1; }
    cost_type cost_del(value_type c) { return (c == ' ') ? 0 : 1; }
    // note that substitution cost also encompasses the definition of equality
    cost_type cost_sub(value_type c, value_type d) { return (c == d) ? 0 : 1; }
};


int main(int argc, char** argv) {
    char const* str1 = " so   many spaces     ";
    char const* str2 = "    so many   spaces ";

    // with custom "free space" cost function, the distance should be zero:
    unsigned dist = edit_distance(str1, str2, cost_free_space());
    std::cout << "The edit distance between \"" << str1 << "\" and \"" << str2 << "\" = " << dist << "\n";    

    return 0;
}
