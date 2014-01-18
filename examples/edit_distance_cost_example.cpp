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
#include <boost/algorithm/sequence/edit_distance.hpp>
using boost::algorithm::sequence::edit_distance;
using namespace boost::algorithm::sequence::parameter;


// define a custom cost function where insertion or deletion of space costs nothing
struct cost_free_space {
    // edit_distance uses cost_type to store cost values internally
    // you can define it smaller to save space, or define it as
    // floating point to support non-integer costs, etc.

    // cost_type is inferred from the return values of cost functions if not defined explicitly
    typedef unsigned cost_type;

    // inserting or deleting a space is free:
    unsigned insertion(char c) const { return (c == ' ') ? 0 : 1; }
    unsigned deletion(char c) const { return (c == ' ') ? 0 : 1; }

    // replacing one char with another costs 1
    // (equal elements always incur zero cost)
    unsigned substitution(char c, char d) const { return 1; }

    // Defining substitution() is optional if substitution is compile-time disabled (the default).
    // To enable, pass the optional _substitution=boost::true_type(), or _substitution=<bool-value>
};

int main(int argc, char** argv) {
    char const* str1 = " so   many spaces     ";
    char const* str2 = "    so many   spaces ";

    // with custom "free space" cost function, the distance should be zero:
    // here we also enable substitution
    unsigned dist = edit_distance(str1, str2, _cost = cost_free_space(), _substitution = true);
    std::cout << "The edit distance between \"" << str1 << "\" and \"" << str2 << "\" = " << dist << "\n";    

    return 0;
}
