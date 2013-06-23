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
#include <boost/algorithm/sequence_alignment/edit_distance.hpp>
using boost::algorithm::sequence_alignment::edit_distance;

int main(int argc, char** argv) {
    char const* str1 = "abc";
    char const* str2 = "axc";
    unsigned dist;

    // compare two null-terminated strings that differ by one substitution
    // (distance should be 1)
    std::cout << "dist= " << edit_distance(str1, str2) << "\n";

    // however, any two sequences or ranges can be compared, even differing types, provided
    // that the sequence element types are compatible
    std::cout << "dist= " << edit_distance(str1, as_list(str2)) << "\n";
    std::cout << "dist= " << edit_distance(as_vector(str1), str2) << "\n";
    std::cout << "dist= " << edit_distance(as_string(str1), as_list(str2)) << "\n";    

    // range adaptors work as expected (here distance should be 3)
    std::cout << "dist= " << edit_distance(as_vector(str1), as_list(str2) | boost::adaptors::reversed) << "\n";

    return 0;
}
