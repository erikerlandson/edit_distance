/*******
edit_distance: STL and Boost compatible edit distance functions for C++

Copyright (c) 2013 Erik Erlandson

Author:  Erik Erlandson <erikerlandson@yahoo.com>

Distributed under the Boost Software License, Version 1.0.
See accompanying file LICENSE or copy at
http://www.boost.org/LICENSE_1_0.txt
*******/

#include <sstream>
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

// get the edit_alignment() function
#include <boost/algorithm/sequence_alignment/edit_alignment.hpp>
//using boost::algorithm::sequence_alignment::edit_alignment;
//using boost::algorithm::sequence_alignment::edit_opcode;

int main(int argc, char** argv) {
    char const* str1 = "abc";
    char const* str2 = "axc";

#if 0
    // Compare two null-terminated strings that differ by one substitution
    // (distance should be 1)
    std::stringstream ss;
    unsigned dist = edit_alignment(str1, str2, std::ostream_iterator<edit_opcode>(ss, "")).second;
    std::cout << "dist= " << dist << "   edit operations= " << ss.str() << "\n";

    // As with edit_distance(), any type of sequences or range adaptors can be 
    // applied as sequence arguments (here distance should be 3: "abc" -> "cxa")
    ss.str("");
    dist = edit_alignment(as_vector(str1), as_list(str2) | boost::adaptors::reversed, std::ostream_iterator<edit_opcode>(ss, "")).second;
    std::cout << "dist= " << dist << "   edit operations= " << ss.str() << "\n";
#endif

    return 0;
}
