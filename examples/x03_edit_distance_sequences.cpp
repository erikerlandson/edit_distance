/*******
edit_distance: STL and Boost compatible edit distance functions for C++

Copyright (c) 2013 Erik Erlandson

Author:  Erik Erlandson <erikerlandson@yahoo.com>

Distributed under the Boost Software License, Version 1.0.
See accompanying file LICENSE or copy at
http://www.boost.org/LICENSE_1_0.txt
*******/

#include <iostream>

#include <list>
#include <vector>
#include <string>

#include <boost/foreach.hpp>
#include <boost/range/as_literal.hpp>
#include <boost/range/adaptors.hpp>

std::list<char> as_list(char const* str) {
    std::list<char> r;
    BOOST_FOREACH(char e, boost::as_literal(str)) r.push_back(e);
    return r;
}
std::vector<char> as_vector(char const* str) {
    std::vector<char> r;
    BOOST_FOREACH(char e, boost::as_literal(str)) r.push_back(e);
    return r;
}
std::string as_string(char const* str) {
    return std::string(str);
}

// get the edit_distance() function
#include <boost/algorithm/sequence/edit_distance.hpp>
using boost::algorithm::sequence::edit_distance;

int main(int argc, char** argv) {
    char const* str1 = "abc";
    char const* str2 = "axc";

    // Compare two null-terminated strings that differ by one insertion+deletion
    // (distance should be 2)
    std::cout << "dist= " << edit_distance(str1, str2) << "\n";

    // Any two sequences or ranges can be compared, even differing types, provided
    // that the sequence element types are compatible.  Sequence element types do not have
    // to be identical, as long as they can be accepted by the cost function.
    std::cout << "dist= " << edit_distance(str1, as_list(str2)) << "\n";
    std::cout << "dist= " << edit_distance(as_vector(str1), str2) << "\n";
    std::cout << "dist= " << edit_distance(as_string(str1), as_list(str2)) << "\n";    

    // Range adaptors work as expected (here distance should be 4: -a, -b, =c, +x, +a)
    std::cout << "dist= " << edit_distance(as_vector(str1), as_list(str2) | boost::adaptors::reversed) << "\n";

    return 0;
}
